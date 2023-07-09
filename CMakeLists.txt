cmake_minimum_required(VERSION 3.5.0)
project(espx23)

add_executable(espx23 main.c queue.c prod-con.c timer.c)

target_link_libraries(espx23 m)