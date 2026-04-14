cmake_minimum_required(VERSION 3.25)
project(server CXX)

set(CMAKE_CXX_STANDARD 23)
set(SRC ${CMAKE_CURRENT_LIST_DIR}/src)
set(SOCKET src/sockets)
set(SHARE ${CMAKE_CURRENT_LIST_DIR}/../shared)
set(INCLUDE ${CMAKE_CURRENT_LIST_DIR}/include)
set(LIB ${CMAKE_CURRENT_LIST_DIR}/lib)
  
add_executable(${PROJECT_NAME}
    ${SHARE}/${SOCKET}/ClientSocket.cpp
    ${SHARE}/${SOCKET}/Packet.cpp
    ${SHARE}/${SOCKET}/ServerSocket.cpp
    ${SHARE}/${SOCKET}/Socket.cpp
    ${SHARE}/src/DateTime.cpp
    ${SRC}/main.cpp
)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDE} ${SHARE}/include)
if (WIN32 OR MSYS)
  target_link_libraries(${PROJECT_NAME} PRIVATE Ws2_32)
endif()

if (${CMAKE_CXX_COMPILER_ID} STREQUAL "GNU" OR
    ${CMAKE_CXX_COMPILER_ID} STREQUAL "Clang"
)
    target_link_libraries(${PROJECT_NAME} PRIVATE stdc++exp)
    target_compile_options(${PROJECT_NAME} PRIVATE -Wall -Wextra -Werror -Wpedantic)
endif()

