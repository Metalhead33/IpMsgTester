cmake_minimum_required(VERSION 3.13.0)
project(IpMsgTester)
set(Boost_USE_STATIC_LIBS ON)
add_executable(${PROJECT_NAME} main TcpClient TcpServer)
