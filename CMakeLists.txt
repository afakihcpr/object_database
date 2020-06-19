cmake_minimum_required(VERSION 3.5)
project(object_database)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(Boost COMPONENTS iostreams system filesystem REQUIRED)


add_library(${CMAKE_PROJECT_NAME} SHARED
  src/shared_types.cpp
  src/object_database.cpp
  src/type_introspection.cpp
)
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE include)
target_link_libraries(${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES} Threads::Threads)

add_executable(odb_cli src/cli.cpp)
target_include_directories(odb_cli PRIVATE include)
target_link_libraries(odb_cli ${CMAKE_PROJECT_NAME} ${Boost_LIBRARIES} Threads::Threads)
