cmake_minimum_required(VERSION 3.12)

# Define Project (Name, Version, Description, Languages)
project(
    lightHTTPServer
    VERSION 1.0.0
    DESCRIPTION
        "A C++ Library to using nothing but sockets, threading and nlohmann::json to serve webpages"
    LANGUAGES CXX
)

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.3/json.tar.xz DOWNLOAD_EXTRACT_TIMESTAMP OLD)
FetchContent_MakeAvailable(json)

find_package(OpenSSL REQUIRED)

# Variable for SRC_DIR to reference later
set(SRC_DIR ${CMAKE_SOURCE_DIR}/src)

# add Library target
add_library(${PROJECT_NAME} SHARED 
    ${SRC_DIR}/lightHTTPServer.cpp
    ${SRC_DIR}/HTTP_Message.cpp
    ${SRC_DIR}/ThreadPool.cpp
)

set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)

target_link_libraries(${PROJECT_NAME} PRIVATE nlohmann_json::nlohmann_json OpenSSL::SSL OpenSSL::Crypto)

# Tell CMAKE where our includes live
target_include_directories(${PROJECT_NAME} PUBLIC
    $<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

# definitions of CMAKE_INSTALL_LIBDIR, CMAKE_INSTALL_INCLUDEDIR and others
include(GNUInstallDirs)

# install the target and create export-set
install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-Targets
    ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR} # lib
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR} # bin
)

# generate and install export file
install(EXPORT ${PROJECT_NAME}-Targets
    FILE ${PROJECT_NAME}-config.cmake
    DESTINATION cmake
)

# Variable for where the public Headers are located (the include folder)
set(PUBLIC_HEADERS ${CMAKE_SOURCE_DIR}/include/pokemon-tcg-sdk.hpp)

# Actually "install" (copy) the public headers into the proper location
install(FILES ${PUBLIC_HEADERS} DESTINATION include)

# Add the test imports
include(CTest)

# Main test for the library (make sure it works)
set(TEST_DIR ${CMAKE_SOURCE_DIR}/tests)
add_executable(mainTest ${TEST_DIR}/test.cpp)
target_link_libraries(mainTest ${PROJECT_NAME} nlohmann_json::nlohmann_json)
add_test(NAME MainTest COMMAND ${CMAKE_CURRENT_BINARY_DIR}/mainTest)
