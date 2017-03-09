cmake_minimum_required(VERSION 2.8)

project(Lab2d)

include_directories(./units)

add_definitions(-DNOMINMAX -D_WINDOWS -D_CRT_SECURE_NO_WARNINGS -D_SCL_SECURE_NO_WARNINGS)

file(GLOB_RECURSE SRC_LIST  *.cpp *.h)

add_executable(${PROJECT_NAME} ${SRC_LIST})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "/SUBSYSTEM:WINDOWS")

target_link_libraries(${PROJECT_NAME} Winmm.lib)
