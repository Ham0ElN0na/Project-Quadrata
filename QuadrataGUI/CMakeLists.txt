cmake_minimum_required(VERSION 3.16)
project(QuadrataGUI VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

# Find Qt6 first, fall back to Qt5
find_package(Qt6 COMPONENTS Core Gui Widgets QUIET)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 COMPONENTS Core Gui Widgets REQUIRED)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

set(SOURCES
    main.cpp
    MainWindow.cpp
    SplashPage.cpp
    LogoWidget.cpp
    LoginPage.cpp
    AdminDashboard.cpp
    CustomerDashboard.cpp
)

set(HEADERS
    MainWindow.h
    SplashPage.h
    LogoWidget.h
    LoginPage.h
    AdminDashboard.h
    CustomerDashboard.h
)

add_executable(QuadrataGUI WIN32 ${SOURCES} ${HEADERS})

if(QT_VERSION_MAJOR EQUAL 6)
    target_link_libraries(QuadrataGUI PRIVATE Qt6::Core Qt6::Gui Qt6::Widgets)
else()
    target_link_libraries(QuadrataGUI PRIVATE Qt5::Core Qt5::Gui Qt5::Widgets)
endif()

# Copy JSON data files next to the executable for development
set(DATA_SOURCE_DIR "${CMAKE_SOURCE_DIR}/../Project Quadrata")
if(EXISTS "${DATA_SOURCE_DIR}/Inventory.json")
    configure_file("${DATA_SOURCE_DIR}/Inventory.json" "${CMAKE_BINARY_DIR}/Inventory.json" COPYONLY)
    configure_file("${DATA_SOURCE_DIR}/Users.json"     "${CMAKE_BINARY_DIR}/Users.json"     COPYONLY)
    configure_file("${DATA_SOURCE_DIR}/session.json"   "${CMAKE_BINARY_DIR}/session.json"   COPYONLY)
endif()
