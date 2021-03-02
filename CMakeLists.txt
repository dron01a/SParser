cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(SXML VERSION 0.2.1 LANGUAGES C CXX)

include_directories(include) # add include dir
include_directories(libs/FSTool/include)
include_directories(libs/WPTool/include)

# set C++ flags 
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED on)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

# add sources
set( __SOURCES__ ../src/SXML.cpp )

add_subdirectory(libs/FSTool/CMake)
add_subdirectory(libs/WPTool/cmake)
                 
# make lib 
add_library( SXML STATIC ${__SOURCES__})

# add libs 
target_link_libraries( SXML FSTLib WPTLib )

# set name and prefix
set_target_properties(SXML PROPERTIES OUTPUT_NAME "SXMLlib")
set_target_properties(SXML PROPERTIES PREFIX "")