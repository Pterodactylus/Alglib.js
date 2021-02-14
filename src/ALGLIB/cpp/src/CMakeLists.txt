cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 17)
project(ALGLIB)

add_library(alglib SHARED
    ./alglibinternal.cpp
	./alglibmisc.cpp
	./ap.cpp
	./dataanalysis.cpp
	./diffequations.cpp
	./fasttransforms.cpp
	./integration.cpp
	./interpolation.cpp
	./linalg.cpp
	./optimization.cpp
	./solvers.cpp
	./specialfunctions.cpp
	./statistics.cpp
	./alglibinternal.h
	./alglibmisc.h
	./ap.h
	./dataanalysis.h
	./diffequations.h
	./fasttransforms.h
	./integration.h
	./interpolation.h
	./linalg.h
	./optimization.h
	./solvers.h
	./specialfunctions.h
	./statistics.h
	./stdafx.h
)

SET(CMAKE_C_COMPILER emcc)
SET(CMAKE_CPP_COMPILER em++)
#SET(CMAKE_C_COMPILER gcc)
#SET(CMAKE_CPP_COMPILER g++)
set( CMAKE_CXX_FLAGS "-O3" )