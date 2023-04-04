add_executable(${PROJECT_NAME} main.cpp)

target_include_directories(${PROJECT_NAME} PRIVATE ../util)

target_link_libraries(${PROJECT_NAME} PRIVATE util)