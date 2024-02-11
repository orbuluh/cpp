# Note that headers are optional, and do not affect add_library, but they will
# not show up in IDEs unless they are listed in add_library.

# for CMake 3.12 or later:
file(GLOB HEADER_LIST CONFIGURE_DEPENDS
     "${ModernCMakeExample_SOURCE_DIR}/include/util/*.h")
# the other syntax set(HEADER_LIST
# "${ModernCMakeExample_SOURCE_DIR}/include/some_lib/lib.h")

# Make an automatic library - will be static or dynamic based on user setting
add_library(util STATIC utility.cpp ${HEADER_LIST})

# We need this directory, and users of our library will need it too
target_include_directories(util PUBLIC ../include)
