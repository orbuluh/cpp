set(PROJECT_NAME modern_cpp_design)

add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ..)

target_link_libraries(${PROJECT_NAME} PRIVATE ch3)
