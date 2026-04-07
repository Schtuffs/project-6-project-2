cmake_minimum_required(VERSION 3.25)
project(Client CXX)

set(CMAKE_CXX_STANDARD 23)
set(SRC ${CMAKE_CURRENT_LIST_DIR}/src)
set(SOCKET ${SRC}/sockets)
set(INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(LIB ${CMAKE_CURRENT_LIST_DIR}/lib)
  
add_library(SOCKET_LIB SHARED
    ${SOCKET}/ClientSocket.cpp
    ${SOCKET}/Packet.cpp
    ${SOCKET}/ServerSocket.cpp
    ${SOCKET}/Socket.cpp
)

add_executable(${PROJECT_NAME}
    ${SRC}/main.cpp
)

target_include_directories(SOCKET_LIB PUBLIC ${INCLUDE})
target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE})

if (WIN32 OR MSYS)
  target_link_libraries(SOCKET_LIB PUBLIC Ws2_32)
endif()

target_link_libraries(SOCKET_LIB PUBLIC stdc++exp)
target_link_libraries(${PROJECT_NAME} PRIVATE SOCKET_LIB)


if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
)
    target_compile_options(SOCKET_LIB PRIVATE -Wall -Wextra -Werror -Wpedantic)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()