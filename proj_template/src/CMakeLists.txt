set(LIB_NAME template_lib)

add_library(${LIB_NAME} template_lib.cpp)

target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/util)
target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

target_link_libraries(${LIB_NAME} PRIVATE util)