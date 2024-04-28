find_package(GTest REQUIRED)

add_executable(test_${PROJECT_NAME} test_typelists.cpp)

target_include_directories(test_${PROJECT_NAME} PRIVATE ${GTEST_INCLUDE_DIRS}
                                                        ${PROJECT_SOURCE_DIR})

target_link_libraries(test_${PROJECT_NAME} PRIVATE ${GTEST_BOTH_LIBRARIES} util
                                                   ch3)

add_test(NAME test_${PROJECT_NAME} COMMAND test_${PROJECT_NAME})
