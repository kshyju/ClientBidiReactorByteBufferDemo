# CMakeList.txt : Top-level CMake project file, do global configuration
# and include sub-projects here.
#
cmake_minimum_required (VERSION 3.18)


include(FetchContent)
FetchContent_Declare(vcpkg
    GIT_REPOSITORY https://github.com/microsoft/vcpkg/
    GIT_TAG 2022.10.19
)
FetchContent_MakeAvailable(vcpkg)

# NOTE: This must be defined before the first project call
set(CMAKE_TOOLCHAIN_FILE "${vcpkg_SOURCE_DIR}/scripts/buildsystems/vcpkg.cmake" CACHE FILEPATH "")


project ("GreeterServiceClient")

# Include sub-projects.
add_subdirectory ("GreeterServiceClient")
add_subdirectory("Protos")


