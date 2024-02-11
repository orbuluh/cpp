# Add this line to find the system-wide Google Test On Ubuntu/Debian-based
# systems, through `sudo apt-get install libgtest-dev`

find_package(GTest REQUIRED)

add_executable(test_${PROJECT_NAME} test_utility.cpp)

target_include_directories(test_${PROJECT_NAME} PRIVATE ../util)
target_include_directories(test_${PROJECT_NAME} PRIVATE ${GTEST_INCLUDE_DIRS}
)# Add this line to include Google Test headers

target_link_libraries(test_${PROJECT_NAME} PRIVATE util ${GTEST_BOTH_LIBRARIES}
)# Link against system-wide Google Test libraries

add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})
