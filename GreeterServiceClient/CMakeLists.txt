# CMakeList.txt : CMake project for GreeterServiceClient, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.18)

find_package(protobuf CONFIG REQUIRED)
find_package(gRPC CONFIG REQUIRED)
find_package(Threads)

# Add source to this project's executable.
add_executable (GreeterServiceClient "main.cpp" "my_async_worker.cpp" "my_async_worker2..cpp" "my_async_worker3.cpp")

if (CMAKE_VERSION VERSION_GREATER 3.12)
  set_property(TARGET GreeterServiceClient PROPERTY CXX_STANDARD 20)
endif()

target_link_libraries(GreeterServiceClient PUBLIC func_protos)

