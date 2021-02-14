cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(ALGLIB-js)

set(ALGLIB_HELPER ${CMAKE_CURRENT_SOURCE_DIR}/src/ALGLIB.cpp)
set(ALGLIB_LIB ${CMAKE_CURRENT_SOURCE_DIR}/build/alglib)
set(ALGLIB_HEADERS ${CMAKE_CURRENT_SOURCE_DIR}/src/ALGLIB/cpp/src)

#Include the header files here.
include_directories(${ALGLIB_HEADERS})

#The first parameter is the name of the executable and the second is the variable with the file names the third the source file directory.
add_executable(alglibjs ${ALGLIB_HELPER})


# Links to the complied library files here.
target_link_libraries(alglibjs ${ALGLIB_LIB}/libalglib.a)

# set compiler and flags
SET(CMAKE_C_COMPILER emcc)
SET(CMAKE_CPP_COMPILER em++)
#SET_TARGET_PROPERTIES(ALGLIB PROPERTIES LINK_FLAGS -g)
string(CONCAT HELPERJS "" "")
#string(CONCAT HELPERJS "--post-js " "${CMAKE_CURRENT_SOURCE_DIR}/CeresHelper.js")

set( CMAKE_CXX_FLAGS "-O3 -s --bind -s MODULARIZE=2 -s ALLOW_TABLE_GROWTH=1 -s EXPORT_NAME=ALGLIBModule -s FILESYSTEM=0 -s SINGLE_FILE=1 ${HELPERJS}" )



#set( CMAKE_CXX_FLAGS "-O1 -g4 --source-map-base -s --bind -s ALLOW_TABLE_GROWTH=1 -s SINGLE_FILE=1 ${HELPERJS}" )