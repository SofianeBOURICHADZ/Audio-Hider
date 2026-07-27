cmake_minimum_required(VERSION 3.16)
project(AudioStegoTool LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)

# Configure RPATH for distribution (helps binary find bundled .so libraries)
set(CMAKE_INSTALL_RPATH "$ORIGIN/lib")
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)

find_package(Qt6 REQUIRED COMPONENTS Widgets)
find_package(OpenSSL REQUIRED)

add_executable(AudioStegoTool
    src/main.cpp
    src/MainWindow.cpp
    src/Engine.cpp
)

target_include_directories(AudioStegoTool PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(AudioStegoTool PRIVATE 
    Qt6::Widgets 
    OpenSSL::Crypto
)

# Standard install instructions needed by deployment tools
install(TARGETS AudioStegoTool DESTINATION bin)