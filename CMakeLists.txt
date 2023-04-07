cmake_minimum_required(VERSION 3.2)

project(depthify)

set(CMAKE_CXX_STANDARD 17)

add_executable(depthify
  main.cpp
  manager.h
  manager.cpp
  order_book.h
  order_book.cpp
  "message.h"
)

target_compile_options(depthify PRIVATE -Wall -Wextra -Wpedantic -Werror -Wconversion -Wsign-conversion)

# add_executable(orderbook_test
#   util/orderbook_test.cpp
#   manager.h
#   manager.cpp
#   order_book.h
#   order_book.cpp
#   "message.h"
# )
