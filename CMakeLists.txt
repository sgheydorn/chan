cmake_minimum_required(VERSION 3.23)

project(chan VERSION 0.1.0)

option(CHAN_BUILD_TESTS "Build chan tests." OFF)

add_library(chan INTERFACE)
target_include_directories(chan INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)
target_compile_features(chan INTERFACE cxx_std_23)

install(DIRECTORY include/chan DESTINATION include)
install(TARGETS chan EXPORT chan-config)
install(EXPORT chan-config DESTINATION lib/cmake/chan NAMESPACE chan::)

if(CHAN_BUILD_TESTS)
  enable_testing()
  add_subdirectory(tests)
endif()
