#include "http.h"

#include <palloc/palloc.h>
#include <palloc/vector.h>
#include <palloc/sstream.h>

#ifdef _WIN32
  #define USE_WINSOCK
  #define USE_WINAPI
#else
  #define USE_POSIX
#endif

#ifdef USE_WINSOCK
  #define WIN32_LEAN_AND_MEAN
  #include <windows.h>
  #include <winsock2.h>
  #include <ws2tcpip.h>
#endif

#ifdef USE_POSIX
  #include <sys/socket.h>
  #include <sys/types.h>
  #include <netinet/in.h>
  #include <arpa/inet.h>
  #include <netdb.h>
  #include <fcntl.h>
  #include <errno.h>
#endif

#include <stdio.h>
#include <string.h>

#define HTTP_CONNECTING 1
#define HTTP_RECEIVING 2
#define HTTP_COMPLETE 3

#define BUFFER_SIZE 1024

#ifdef USE_POSIX
  #define NULL_SOCKET -1
#endif
#ifdef USE_WINSOCK
  #define NULL_SOCKET INVALID_SOCKET
#endif

static int winsockInitialized;

struct CustomHeader
{
  sstream *variable;
  sstream *value;
};

struct Http
{
  sstream *host;
  sstream *path;
  sstream *query;
  sstream *post;
  vector(char) *raw;
  vector(int) *socks;
  sstream *rawHeaders;
  sstream *rawContent;
  vector(struct CustomHeader) *customHeaders;
  int sock;
  int status;
};

void HttpAddCustomHeader(struct Http *ctx, char *variable, char *value)
{
  struct CustomHeader ch = {0};

  ch.variable = sstream_new();
  sstream_push_cstr(ch.variable, variable);
  ch.value = sstream_new();
  sstream_push_cstr(ch.value, value);
  vector_push_back(ctx->customHeaders, ch);
}

void _HttpClearSocks(struct Http *ctx)
{
  size_t i = 0;

  for(i = 0; i < vector_size(ctx->socks); i++)
  {
#ifdef USE_POSIX
    close(vector_at(ctx->socks, i));
#endif
#ifdef USE_WINSOCK
    shutdown(vector_at(ctx->socks, i), SD_BOTH);
    closesocket(vector_at(ctx->socks, i));
#endif
  }

  vector_clear(ctx->socks);
}

int HttpState(struct Http *ctx)
{
  if(vector_size(ctx->socks) > 0)
  {
    return HTTP_CONNECTING;
  }
  else if(ctx->sock != NULL_SOCKET)
  {
    return HTTP_RECEIVING;
  }

  return HTTP_COMPLETE;
}

#ifdef USE_WINSOCK
void _HttpShutdownWinsock()
{
  WSACleanup();
}
#endif

struct Http *HttpCreate()
{
  struct Http *rtn = NULL;
#ifdef USE_WINSOCK
  if(!winsockInitialized)
  {
    WSADATA wsaData = {0};
    int result = 0;

    result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if(result != 0)
    {
      printf("WSAStartup failed with error: %d\n", result);
      return NULL;
    }

    winsockInitialized = 1;
    atexit(_HttpShutdownWinsock);
  }
#endif

  rtn = palloc(struct Http);
  rtn->raw = vector_new(char);
  rtn->rawContent = sstream_new();
  rtn->rawHeaders = sstream_new();
  rtn->socks = vector_new(int);
  rtn->customHeaders = vector_new(struct CustomHeader);
  rtn->sock = NULL_SOCKET;
  rtn->host = sstream_new();
  rtn->path = sstream_new();
  rtn->query = sstream_new();
  rtn->post = sstream_new();

  return rtn;
}

void HttpDestroy(struct Http *ctx)
{
  size_t i = 0;

  _HttpClearSocks(ctx);
  vector_delete(ctx->socks);
  sstream_delete(ctx->rawHeaders);
  sstream_delete(ctx->rawContent);
  vector_delete(ctx->raw);
  sstream_delete(ctx->host);
  sstream_delete(ctx->path);
  sstream_delete(ctx->query);
  sstream_delete(ctx->post);

  for(i = 0; i < vector_size(ctx->customHeaders); i++)
  {
    sstream_delete(vector_at(ctx->customHeaders, i).variable);
    sstream_delete(vector_at(ctx->customHeaders, i).value);
  }

  vector_delete(ctx->customHeaders);

  pfree(ctx);
}

void _HttpPollConnect(struct Http *ctx)
{
  size_t i = 0;
  sstream *content = NULL;

  //printf("polling connect\n");

  for(i = 0; i < vector_size(ctx->socks); i++)
  {
    fd_set write_fds = {0};
    struct timeval tv = {0};
    int err = 0;
    int sock = vector_at(ctx->socks, i);

    FD_SET(sock, &write_fds);
    err = select(sock + 1, NULL, &write_fds, NULL, &tv);

    if(err == 0)
    {
      continue;
    }
    else if(err == -1)
    {
#ifdef USE_POSIX
      close(sock);
#endif
#ifdef USE_WINSOCK
      shutdown(sock, SD_BOTH);
      closesocket(sock);
#endif
      vector_erase(ctx->socks, i);
      i--;
      continue;
    }
    else
    {
      int optval = 0;
      int optlen = sizeof(optval);
#ifdef USE_POSIX
      if(getsockopt(sock, SOL_SOCKET, SO_ERROR, &optval, &optlen) == -1)
#endif
#ifdef USE_WINSOCK
      if(getsockopt(sock, SOL_SOCKET, SO_ERROR, (char*)&optval, &optlen) == -1)
#endif
      {
#ifdef USE_POSIX
        close(sock);
#endif
#ifdef USE_WINSOCK
        shutdown(sock, SD_BOTH);
        closesocket(sock);
#endif
        vector_erase(ctx->socks, i);
        i--;
        continue;
      }

      if(optval != 0)
      {
#ifdef USE_POSIX
        close(sock);
#endif
#ifdef USE_WINSOCK
        shutdown(sock, SD_BOTH);
        closesocket(sock);
#endif
        vector_erase(ctx->socks, i);
        i--;
        continue;
      }
    }

    ctx->sock = sock;
    vector_erase(ctx->socks, i);
    _HttpClearSocks(ctx);
    break;
  }

  if(ctx->sock == NULL_SOCKET && vector_size(ctx->socks) < 1)
  {
    ctx->status = -1;
    return;
  }

  if(ctx->sock == NULL_SOCKET)
  {
    return;
  }

  content = sstream_new();

  if(sstream_length(ctx->post) > 0)
  {
    sstream_push_cstr(content, "POST ");
  }
  else
  {
    sstream_push_cstr(content, "GET ");
  }

  sstream_push_cstr(content, sstream_cstr(ctx->path));
  sstream_push_char(content, '?');
  sstream_push_cstr(content, sstream_cstr(ctx->query));
  sstream_push_cstr(content, " HTTP/1.0\r\n");
  sstream_push_cstr(content, "Host: ");
  sstream_push_cstr(content, sstream_cstr(ctx->host));
  sstream_push_cstr(content, "\r\n");

  for(i = 0; i < vector_size(ctx->customHeaders); i++)
  {
    sstream_push_cstr(content,
      sstream_cstr(vector_at(ctx->customHeaders, i).variable));

    sstream_push_cstr(content, ": ");

    sstream_push_cstr(content,
      sstream_cstr(vector_at(ctx->customHeaders, i).value));

    sstream_push_cstr(content, "\r\n");
  }

  if(sstream_length(ctx->post) > 0)
  {
    sstream_push_cstr(content, "Content-Length: ");
    sstream_push_int(content, sstream_length(ctx->post));
    sstream_push_cstr(content, "\r\n");
  }

  sstream_push_cstr(content, "\r\n");

  if(sstream_length(ctx->post) > 0)
  {
    sstream_push_cstr(content, sstream_cstr(ctx->post));
  }

#ifdef USE_POSIX
  send(ctx->sock, sstream_cstr(content), sstream_length(content), MSG_NOSIGNAL);
#endif
#ifdef USE_WINSOCK
  send(ctx->sock, sstream_cstr(content), sstream_length(content), 0);
#endif

  sstream_delete(content);
}

void _HttpProcessHeaders(struct Http *ctx)
{
  vector(sstream *) *lines = NULL;
  vector(sstream *) *line = NULL;
  size_t i = 0;
  size_t j = 0;

  lines = vector_new(sstream *);
  line = vector_new(sstream *);
  sstream_split(ctx->rawHeaders, '\n', lines);

  for(i = 0; i < vector_size(lines); i++)
  {
    sstream_split(vector_at(lines, i), ' ', line);

    if(vector_size(line) >= 2)
    {
      if(strcmp(sstream_cstr(vector_at(line, 0)), "HTTP/1.1") == 0 ||
        strcmp(sstream_cstr(vector_at(line, 0)), "HTTP/1.0") == 0)
      {
        //printf("Status: %s\n", sstream_cstr(vector_at(line, 1)));
        ctx->status = atoi(sstream_cstr(vector_at(line, 1)));
      }
    }

    for(j = 0; j < vector_size(line); j++)
    {
      sstream_delete(vector_at(line, j));
    }

    vector_clear(line);
  }

  //printf("%i %s\n", (int)vector_size(lines), sstream_cstr(ctx->rawHeaders));

  for(i = 0; i < vector_size(lines); i++)
  {
    sstream_delete(vector_at(lines, i));
  }

  vector_delete(lines);
  vector_delete(line);
}

void _HttpProcessRaw(struct Http *ctx)
{
  if(sstream_length(ctx->rawHeaders) == 0)
  {
    size_t i = 0;
    char last[5] = {0};

    for(i = 0; i < vector_size(ctx->raw); i++)
    {
      last[0] = last[1];
      last[1] = last[2];
      last[2] = last[3];
      last[3] = vector_at(ctx->raw, i);

      if(strcmp(last, "\r\n\r\n") == 0)
      {
        size_t c = 0;

        for(c = 0; c < i-3; c++)
        {
          sstream_push_char(ctx->rawHeaders, vector_at(ctx->raw, c));
        }

        _HttpProcessHeaders(ctx);
        break;
      }
    }
  }

  if(sstream_length(ctx->rawHeaders) == 0) return;

  if(ctx->sock == NULL_SOCKET)
  {
    size_t i = 0;

    for(i = sstream_length(ctx->rawHeaders) + 4; i < vector_size(ctx->raw); i++)
    {
      sstream_push_char(ctx->rawContent, vector_at(ctx->raw, i));
    }

    //printf("Content: %s\n", sstream_cstr(ctx->rawContent));
  }
}

void _HttpPollReceive(struct Http *ctx)
{
  fd_set read_fds = {0};
  struct timeval tv = {0};
  int err = 0;

  //printf("polling receive\n");

  FD_SET(ctx->sock, &read_fds);
  err = select(ctx->sock + 1, &read_fds, NULL, NULL, &tv);

  if(err == 0)
  {
    return;
  }
  else if(err == -1)
  {
    ctx->status = -1;
#ifdef USE_POSIX
    close(ctx->sock);
#endif
#ifdef USE_WINSOCK
    shutdown(ctx->sock, SD_BOTH);
    closesocket(ctx->sock);
#endif
    ctx->sock = NULL_SOCKET;
    return;
  }
  else
  {
    char buff[BUFFER_SIZE] = {0};
    size_t i = 0;
#ifdef USE_POSIX
    ssize_t n = 0;

    n = read(ctx->sock, buff, BUFFER_SIZE - 1);

    if(n > 0)
#endif
#ifdef USE_WINSOCK
    int n = 0;
    n = recv(ctx->sock, buff, BUFFER_SIZE - 1, 0);

    if(n != SOCKET_ERROR && n != 0)
#endif
    {
      for(i = 0; i < n; i++)
      {
        vector_push_back(ctx->raw, buff[i]);
      }

      //printf("Data waiting: %s\n", buff);
    }
    else
    {
#ifdef USE_POSIX
      close(ctx->sock);
#endif
#ifdef USE_WINSOCK
      shutdown(ctx->sock, SD_BOTH);
      closesocket(ctx->sock);
#endif
      ctx->sock = NULL_SOCKET;
    }

    _HttpProcessRaw(ctx);
  }
}

void _HttpPoll(struct Http *ctx)
{
  if(HttpState(ctx) == HTTP_CONNECTING)
  {
    _HttpPollConnect(ctx);
  }
  else if(HttpState(ctx) == HTTP_RECEIVING)
  {
    _HttpPollReceive(ctx);
  }
}

void _HttpParseRequest(struct Http *ctx, char *url)
{
  size_t i = 0;
  size_t len = 0;

  sstream_clear(ctx->host);
  sstream_clear(ctx->path);
  sstream_clear(ctx->query);

  len = strlen(url);

  for(; i < len - 1; i++)
  {
    if(url[i] == '/' && url[i+1] == '/')
    {
      i+=2;
      break;
    }
  }

  for(; i < len; i++)
  {
    if(url[i] == '/')
    {
      break;
    }

    sstream_push_char(ctx->host, url[i]);
  }

  for(; i < len; i++)
  {
    if(url[i] == '?')
    {
      i++;
      break;
    }

    sstream_push_char(ctx->path, url[i]);
  }

  for(; i < len; i++)
  {
    sstream_push_char(ctx->query, url[i]);
  }

  //printf("Host: %s\n", sstream_cstr(ctx->host));
  //printf("Path: %s\n", sstream_cstr(ctx->path));
  //printf("Query: %s\n", sstream_cstr(ctx->query));
}

void HttpRequest(struct Http *ctx, char *url, char *post)
{
  struct addrinfo hints = {0};
  struct addrinfo *res = NULL;
  struct addrinfo *ent = NULL;
  int err = 0;
  int flags = 0;

  if(HttpState(ctx) != HTTP_COMPLETE) return;

  sstream_clear(ctx->post);

  if(post)
  {
    sstream_push_cstr(ctx->post, post);
  }

  _HttpParseRequest(ctx, url);

  sstream_clear(ctx->rawHeaders);
  sstream_clear(ctx->rawContent);
  vector_clear(ctx->raw);
  ctx->status = 0;

  hints.ai_family = AF_UNSPEC;
  hints.ai_socktype = SOCK_STREAM;

  err = getaddrinfo(sstream_cstr(ctx->host), "80", &hints, &res);

  if(err)
  {
    ctx->status = -1;
    return;
  }

  for(ent = res; ent != NULL; ent = ent->ai_next)
  {
    int sock = NULL_SOCKET;

    if(ent->ai_family != AF_INET && ent->ai_family != AF_INET6)
    {
      continue;
    }

    sock = socket(ent->ai_family, ent->ai_socktype, 0);

    if(sock == NULL_SOCKET)
    {
      continue;
    }

#ifdef USE_POSIX
    flags = fcntl(sock, F_GETFL);

    if(fcntl(sock, F_SETFL, flags | O_NONBLOCK) == -1)
    {

      close(sock);
      continue;
    }
#endif
#ifdef USE_WINSOCK
    {
    unsigned nonblocking = 1;
    ioctlsocket(sock, FIONBIO, &nonblocking);
    }
#endif

    err = connect(sock, (struct sockaddr *)ent->ai_addr, ent->ai_addrlen);

#ifdef USE_POSIX
    if(err == -1)
#endif
#ifdef USE_WINSOCK
    if(err == SOCKET_ERROR)
#endif
    {
#ifdef USE_POSIX
      if(errno == EINPROGRESS)
#endif
#ifdef USE_WINSOCK
      if(WSAGetLastError() == WSAEWOULDBLOCK)
#endif
      {
        vector_push_back(ctx->socks, sock);
      }
      else
      {
#ifdef USE_POSIX
        close(sock);
#endif
#ifdef USE_WINSOCK
        shutdown(sock, SD_BOTH);
        closesocket(sock);
#endif
      }

      continue;
    }

    ctx->sock = sock;
    _HttpClearSocks(ctx);
  }

  if(ctx->sock == NULL_SOCKET && vector_size(ctx->socks) < 1)
  {
    ctx->status = -1;
  }

  freeaddrinfo(res);
}

int HttpRequestComplete(struct Http *ctx)
{
  _HttpPoll(ctx);

  if(HttpState(ctx) == HTTP_COMPLETE)
  {
    return 1;
  }

  return 0;
}

int HttpResponseStatus(struct Http *ctx)
{
  return ctx->status;
}

char *HttpResponseContent(struct Http *ctx)
{
  return sstream_cstr(ctx->rawContent);
}
