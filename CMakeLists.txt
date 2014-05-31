
cmake_minimum_required(VERSION 2.8)
project("closure")

add_subdirectory(gtest)

include_directories(
  "${gtest_SOURCE_DIR}"
  "${gtest_SOURCE_DIR}/include"
)
add_executable(closure_test closure_test.cc)
target_link_libraries(closure_test glog gtest gtest_main)
