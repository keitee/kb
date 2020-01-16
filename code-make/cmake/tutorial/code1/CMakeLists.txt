cmake_minimum_required (VERSION 2.6)
project (code1)

message("PROJECT_NAME      : ${PROJECT_NAME}")
message("CMAKE_PROJECT_NAME: ${CMAKE_PROJECT_NAME}")

message("Regular variable (before): ${abc}")
message("Cache variable (before): ${xyz}")

set(abc "123")
set(xyz "321" CACHE STRING "???")

message("Regular variable (after): ${abc}")
message("Cache variable (after): ${xyz}")
