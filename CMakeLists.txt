cmake_minimum_required(VERSION 3.10)

if(DEFINED ENV{VCPKG_ROOT} AND NOT DEFINED CMAKE_TOOLCHAIN_FILE)
    set(CMAKE_TOOLCHAIN_FILE "$ENV{VCPKG_ROOT}/scripts/buildsystems/vcpkg.cmake" CACHE STRING "")
endif()

project(FCT_NodeJS_Integration)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_subdirectory(FCT_Node)

add_executable(minitest test.cpp)

target_link_libraries(minitest PUBLIC FCT_Node)

if(DEFINED NODE_LIB_DIR)
    add_custom_command(TARGET minitest POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${NODE_LIB_DIR}/libnode.dll"
            $<TARGET_FILE_DIR:minitest>)
endif()

add_executable(FCT_NodeJS_Integration main.cpp)

target_link_libraries(FCT_NodeJS_Integration PUBLIC FCT_Node)

if(DEFINED NODE_LIB_DIR)
    add_custom_command(TARGET FCT_NodeJS_Integration POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${NODE_LIB_DIR}/libnode.dll"
            $<TARGET_FILE_DIR:FCT_NodeJS_Integration>)
endif()

add_executable(NodejsCallAiDemo CallAiDemo.cpp)

target_link_libraries(NodejsCallAiDemo PUBLIC FCT_Node)

if(DEFINED NODE_LIB_DIR)
    add_custom_command(TARGET NodejsCallAiDemo POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${NODE_LIB_DIR}/libnode.dll"
            $<TARGET_FILE_DIR:NodejsCallAiDemo>)
endif()

add_executable(NetWorkChatDemo NetWorkChatDemo.cpp)

target_link_libraries(NetWorkChatDemo PUBLIC FCT_Node)

if(DEFINED NODE_LIB_DIR)
    add_custom_command(TARGET NetWorkChatDemo POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${NODE_LIB_DIR}/libnode.dll"
            $<TARGET_FILE_DIR:NetWorkChatDemo>)
endif()

add_executable(Server Server.cpp)

target_link_libraries(Server PUBLIC FCT_Node)

if(DEFINED NODE_LIB_DIR)
    add_custom_command(TARGET Server POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            "${NODE_LIB_DIR}/libnode.dll"
            $<TARGET_FILE_DIR:Server>)
endif()