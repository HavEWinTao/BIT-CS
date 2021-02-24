cmake_minimum_required(VERSION 3.5)

project(Mail_server LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 COMPONENTS Widgets Core Gui Network Sql REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Core Gui Network Sql REQUIRED)

file(GLOB_RECURSE SOURCES "*.cpp")
file(GLOB_RECURSE HEADERS "*.h")
file(GLOB_RECURSE FORMS "*.ui")
file(GLOB_RECURSE RESOURCES "*.qrc")

add_executable(${PROJECT_NAME} ${SOURCES} ${HEADERS} ${FORMS} ${RESOURCES})

target_link_libraries(Mail_server PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)
target_link_libraries(Mail_server PRIVATE Qt${QT_VERSION_MAJOR}::Core)
target_link_libraries(Mail_server PRIVATE Qt${QT_VERSION_MAJOR}::Gui)
target_link_libraries(Mail_server PRIVATE Qt${QT_VERSION_MAJOR}::Network)
target_link_libraries(Mail_server PRIVATE Qt${QT_VERSION_MAJOR}::Sql)
