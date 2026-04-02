cmake_minimum_required(VERSION 3.28)
project(http-event-server VERSION 0.1.0 LANGUAGES CXX)

# Estándar C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# GoogleTest via FetchContent
include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
FetchContent_MakeAvailable(googletest)

# Archivos fuente del servidor
set(SERVER_SOURCES
  src/thread_pool.cpp
  src/http_parser.cpp
  src/router.cpp
  src/event_store.cpp
  src/server.cpp
)

# Ejecutable principal
add_executable(http-event-server
  src/main.cpp
  ${SERVER_SOURCES}
)
target_include_directories(http-event-server PRIVATE include)

# Ejecutable de tests
add_executable(http-server-tests
  test/test_parser.cpp
  test/test_router.cpp
  ${SERVER_SOURCES}
)
target_include_directories(http-server-tests PRIVATE include)
target_link_libraries(http-server-tests
  GTest::gtest_main
  pthread
)

# Habilita CTest
enable_testing()
add_test(NAME all_tests COMMAND http-server-tests)