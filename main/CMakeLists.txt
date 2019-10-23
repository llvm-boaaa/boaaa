cmake_minimum_required(VERSION 3.12)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB HeaderFilesMain) (
	"hdr/*.h"
)

file(GLOB SourceFilesMain) (
	"src/*.cpp"
)

add_executable(boaaa ${HeaderFilesMain} ${SourceFilesMain})