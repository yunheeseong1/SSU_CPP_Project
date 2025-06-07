cmake_minimum_required(VERSION 3.16)

project(cpp_project VERSION 0.1 LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Widgets)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Widgets)

set(PROJECT_SOURCES
      main.cpp
      calendarwidget.cpp
      calendarwidget.h
      calendarwidget.ui
      employee.cpp
      employee.h
)

if(${QT_VERSION_MAJOR} GREATER_EQUAL 6)
    qt_add_executable(cpp_project
        MANUAL_FINALIZATION
        ${PROJECT_SOURCES}
        mainwindow.cpp
        mainwindow.h

        calendarwidget.ui
        calendarwidget.cpp
        calendarwidget.h
        mainwindow.ui
        employee.cpp
        employee.h
        datamanager.h
        datamanager.cpp
        addemployeedialog.h addemployeedialog.cpp
        addemployeedialog.ui
        employeepanelwidget.h employeepanelwidget.cpp
        employeepanelwidget.ui
        worklog.cpp
        worklog.h
        inputworkhoursdialog.h inputworkhoursdialog.cpp
        inputworkhoursdialog.ui
        infodisplaywidget.h
        infodisplaywidget.cpp


    )

else()
    if(ANDROID)
        add_library(cpp_project SHARED
            ${PROJECT_SOURCES}
        )

    else()
        add_executable(cpp_project
            ${PROJECT_SOURCES}
        )
    endif()
endif()

target_link_libraries(cpp_project PRIVATE Qt${QT_VERSION_MAJOR}::Widgets)


if(${QT_VERSION} VERSION_LESS 6.1.0)
  set(BUNDLE_ID_OPTION MACOSX_BUNDLE_GUI_IDENTIFIER com.example.cpp_project)
endif()
set_target_properties(cpp_project PROPERTIES
    ${BUNDLE_ID_OPTION}
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION_MAJOR}.${PROJECT_VERSION_MINOR}
    MACOSX_BUNDLE TRUE
    WIN32_EXECUTABLE TRUE
)

include(GNUInstallDirs)
install(TARGETS cpp_project
    BUNDLE DESTINATION .
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)

if(QT_VERSION_MAJOR EQUAL 6)
    qt_finalize_executable(cpp_project)
endif()
