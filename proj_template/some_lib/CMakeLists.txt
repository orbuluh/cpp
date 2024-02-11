# Note that headers are optional, and do not affect add_library, but they will
# not show up in IDEs unless they are listed in add_library.

# for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS
     "${ModernCMakeExample_SOURCE_DIR}/include/some_lib/*.h")
# the other syntax set(HEADER_LIST
# "${ModernCMakeExample_SOURCE_DIR}/include/some_lib/lib.h")

# Make an automatic library - will be static or dynamic based on user setting
add_library(some_lib some_lib.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(some_lib PUBLIC ../include)

# This depends on (header only) boost
target_link_libraries(some_lib PRIVATE Boost::boost util)

# All users of this library will need at least C++11
target_compile_features(some_lib PUBLIC cxx_std_11)

# IDEs should put the headers in a nice place
source_group(
  TREE "${PROJECT_SOURCE_DIR}/include"
  PREFIX "Header Files"
  FILES ${HEADER_LIST})

# set(LIB_NAME template_lib)

# add_library(${LIB_NAME} template_lib.cpp)

# target_include_directories(${LIB_NAME} PRIVATE ${CMAKE_SOURCE_DIR}/util)
# target_include_directories(${LIB_NAME} PUBLIC ${CMAKE_SOURCE_DIR}/include)

# target_link_libraries(${LIB_NAME} PRIVATE util)
