cmake_minimum_required(VERSION 3.14)
project(plus)

add_subdirectory(Vendor/volk)
add_subdirectory(Vendor/glfw)

add_subdirectory(Videoland)

add_subdirectory(DS)
target_link_libraries(DS Videoland)
