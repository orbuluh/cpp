add_library(cppcon_2016_asio_talks STATIC snippets.cpp snippets.h)

target_link_libraries(cppcon_2016_asio_talks PRIVATE Boost::system utility_lib)
target_include_directories(cppcon_2016_asio_talks
                           PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/util)
