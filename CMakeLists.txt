cmake_minimum_required(VERSION 2.6)
project(BG C)

include_directories(
  ${PROJECT_SOURCE_DIR}/src
  ${PROJECT_SOURCE_DIR}/include
)

add_library(palloc
  src/palloc/palloc.c
  src/palloc/vector.c
  src/palloc/sstream.c
)

add_library(bg
  #src/bg/mongoose.c
  #To be replaced by Karsten's single-file replacement
  
  #JSON Stuff
  src/bg/parson.c
  
  src/bg/Document.c
  src/bg/Collection.c
  src/bg/State.c
)

target_link_libraries(bg palloc)

add_executable(example
  src/example/main.c
)

target_link_libraries(example bg)

