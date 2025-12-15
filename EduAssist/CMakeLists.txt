cmake_minimum_required(VERSION 3.16)

project(EduAssist VERSION 0.1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(Qt6 REQUIRED COMPONENTS Core Quick Sql)


qt_standard_project_setup(REQUIRES 6.8)

qt_add_executable(appEduAssist
    main.cpp
    databasemanager.h
    databasemanager.cpp
)

qt_add_qml_module(appEduAssist
    URI EduAssist
    VERSION 1.0
    QML_FILES
    Main.qml
)


# Qt for iOS sets MACOSX_BUNDLE_GUI_IDENTIFIER automatically since Qt 6.1.
# If you are developing for iOS or macOS you should consider setting an
# explicit, fixed bundle identifier manually though.
set_target_properties(appEduAssist PROPERTIES
#    MACOSX_BUNDLE_GUI_IDENTIFIER com.example.appEduAssist
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

target_link_libraries(appEduAssist PRIVATE
    Qt6::Core
    Qt6::Quick
    Qt6::Sql
)

include(GNUInstallDirs)
install(TARGETS appEduAssist
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
