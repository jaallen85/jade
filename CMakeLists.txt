# File: CMakeLists.txt
# Copyright (C) 2023  Jason Allen
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <https://www.gnu.org/licenses/>.


cmake_minimum_required(VERSION 3.22)

# Set these project variables correctly to ensure a smooth build
set(PROJECT_NAME     Jade)
set(PROJECT_VERSION  1.5.0)
set(QT6_PATH         "C:/dev/qt/6.5.3/msvc2019_64")

# Build settings
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH ${QT6_PATH})
find_package(Qt6 REQUIRED COMPONENTS Widgets)

add_executable(${PROJECT_NAME} WIN32
    icons/icons.qrc
    source/odg/OdgDrawing.h
    source/odg/OdgDrawing.cpp
    source/widgets/DrawingWidget.h
    source/widgets/DrawingWidget.cpp
    source/widgets/PagesWidget.h
    source/widgets/PagesWidget.cpp
    source/widgets/PropertiesWidget.h
    source/widgets/PropertiesWidget.cpp
    source/widgets/StylesWidget.h
    source/widgets/StylesWidget.cpp
    source/JadeWindow.h
    source/JadeWindow.cpp
    source/main.cpp
)

include_directories(source)
include_directories(source/odg)
include_directories(source/widgets)

configure_file(source/version.h.in version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)

set(BUILD_DIR $<TARGET_FILE_DIR:${PROJECT_NAME}>)
set(PLATFORM_SUBDIR $<TARGET_FILE_DIR:${PROJECT_NAME}>/platforms)
set(STYLES_SUBDIR $<TARGET_FILE_DIR:${PROJECT_NAME}>/styles)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Cored.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Guid.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Widgetsd.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PLATFORM_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/plugins/platforms/qwindowsd.dll" ${PLATFORM_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${STYLES_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/plugins/styles/qwindowsvistastyled.dll" ${STYLES_SUBDIR})
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Core.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Gui.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Widgets.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PLATFORM_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/plugins/platforms/qwindows.dll" ${PLATFORM_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${STYLES_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/plugins/styles/qwindowsvistastyle.dll" ${STYLES_SUBDIR})
endif()

# Install settings
install(TARGETS ${PROJECT_NAME} DESTINATION ".")
install(FILES "${QT6_PATH}/bin/Qt6Core.dll" DESTINATION ".")
install(FILES "${QT6_PATH}/bin/Qt6Gui.dll" DESTINATION ".")
install(FILES "${QT6_PATH}/bin/Qt6Widgets.dll" DESTINATION ".")
install(FILES "${QT6_PATH}/plugins/platforms/qwindows.dll" DESTINATION "platforms")
install(FILES "${QT6_PATH}/plugins/styles/qwindowsvistastyle.dll" DESTINATION "styles")
