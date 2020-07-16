cmake_minimum_required(VERSION 3.13.0)
project(IpMsgTester)
add_executable(${PROJECT_NAME} main TcpClient TcpServer)
set(Boost_USE_STATIC_LIBS ON)

find_package(Threads REQUIRED)
find_package(Boost REQUIRED)

target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_LIST_DIR})

target_link_libraries(${PROJECT_NAME}
        Threads::Threads
        ${Boost_LIBRARIES})
if(WIN32)
    target_link_libraries(${PROJECT_NAME}
            wsock32
            ws2_32)
endif()
