cmake_minimum_required(VERSION 2.8.12)

file(GLOB_RECURSE SRCS *.cpp *.h)

find_package (glog REQUIRED)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
#link_libraries("/usr/local/lib/libevpp.so" glog  Threads::Threads)
#link_libraries(evpp glog  Threads::Threads)


#add_executable(evpp_demo main.cpp)
add_executable(evpp_demo_server server.cc)
add_executable(evpp_demo_client client.cc)

target_link_libraries(evpp_demo_server "/usr/local/lib/libevpp.so" glog  Threads::Threads)
target_link_libraries(evpp_demo_client "/usr/local/lib/libevpp.so" glog  Threads::Threads)
