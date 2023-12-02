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
set(QUAZIP_PATH      "C:/dev/quazip")
set(ZLIB_PATH        "C:/dev/zlib")

# Build settings
project(${PROJECT_NAME} VERSION ${PROJECT_VERSION} LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

set(CMAKE_PREFIX_PATH ${QT6_PATH};${QUAZIP_PATH})
find_package(Qt6 REQUIRED COMPONENTS Widgets)

set(ZLIB_INCLUDE_DIR ${ZLIB_PATH}/include)
set(ZLIB_LIBRARY ${ZLIB_PATH}/lib/zlib.lib)
find_package(QuaZip-Qt6)

add_executable(${PROJECT_NAME} WIN32
    icons/icons.qrc
    source/odg/OdgControlPoint.h
    source/odg/OdgControlPoint.cpp
    source/odg/OdgCurve.h
    source/odg/OdgCurve.cpp
    source/odg/OdgDrawing.h
    source/odg/OdgDrawing.cpp
    source/odg/OdgFontStyle.h
    source/odg/OdgFontStyle.cpp
    source/odg/OdgGlobal.h
    source/odg/OdgGlobal.cpp
    source/odg/OdgGluePoint.h
    source/odg/OdgGluePoint.cpp
    source/odg/OdgMarker.h
    source/odg/OdgMarker.cpp
    source/odg/OdgItem.h
    source/odg/OdgItem.cpp
    source/odg/OdgMarker.h
    source/odg/OdgMarker.cpp
    source/odg/OdgPage.h
    source/odg/OdgPage.cpp
    source/odg/OdgReader.h
    source/odg/OdgReader.cpp
    source/odg/OdgStyle.h
    source/odg/OdgStyle.cpp
    source/odg-items/OdgCurveItem.h
    source/odg-items/OdgCurveItem.cpp
    source/odg-items/OdgEllipseItem.h
    source/odg-items/OdgEllipseItem.cpp
    source/odg-items/OdgGroupItem.h
    source/odg-items/OdgGroupItem.cpp
    source/odg-items/OdgLineItem.h
    source/odg-items/OdgLineItem.cpp
    source/odg-items/OdgPathItem.h
    source/odg-items/OdgPathItem.cpp
    source/odg-items/OdgPolygonItem.h
    source/odg-items/OdgPolygonItem.cpp
    source/odg-items/OdgPolylineItem.h
    source/odg-items/OdgPolylineItem.cpp
    source/odg-items/OdgRectItem.h
    source/odg-items/OdgRectItem.cpp
    source/odg-items/OdgRoundedRectItem.h
    source/odg-items/OdgRoundedRectItem.cpp
    source/odg-items/OdgTextEllipseItem.h
    source/odg-items/OdgTextEllipseItem.cpp
    source/odg-items/OdgTextItem.h
    source/odg-items/OdgTextItem.cpp
    source/odg-items/OdgTextRoundedRectItem.h
    source/odg-items/OdgTextRoundedRectItem.cpp
    source/widgets/DrawingWidget.h
    source/widgets/DrawingWidget.cpp
    source/widgets/PagesWidget.h
    source/widgets/PagesWidget.cpp
    source/widgets/PropertiesWidget.h
    source/widgets/PropertiesWidget.cpp
    source/JadeWindow.h
    source/JadeWindow.cpp
    source/main.cpp
)

include_directories(source)
include_directories(source/odg)
include_directories(source/odg-items)
include_directories(source/widgets)

configure_file(source/version.h.in version.h)
include_directories(${CMAKE_CURRENT_BINARY_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Widgets)
target_link_libraries(${PROJECT_NAME} PRIVATE QuaZip::QuaZip)

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
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/plugins/styles/qwindowsvistastyled.dll" ${STYLES_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Core5Compatd.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QUAZIP_PATH}/bin/quazip1-qt6d.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ZLIB_PATH}/bin/zlibd.dll" ${BUILD_DIR})
else()
    add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Core.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Gui.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Widgets.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${PLATFORM_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/plugins/platforms/qwindows.dll" ${PLATFORM_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${STYLES_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/plugins/styles/qwindowsvistastyle.dll" ${STYLES_SUBDIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QT6_PATH}/bin/Qt6Core5Compat.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${QUAZIP_PATH}/bin/quazip1-qt6.dll" ${BUILD_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_if_different "${ZLIB_PATH}/bin/zlib.dll" ${BUILD_DIR})
endif()

# Install settings
install(TARGETS ${PROJECT_NAME} DESTINATION ".")
install(FILES "${QT6_PATH}/bin/Qt6Core.dll" DESTINATION ".")
install(FILES "${QT6_PATH}/bin/Qt6Gui.dll" DESTINATION ".")
install(FILES "${QT6_PATH}/bin/Qt6Widgets.dll" DESTINATION ".")
install(FILES "${QT6_PATH}/plugins/platforms/qwindows.dll" DESTINATION "platforms")
install(FILES "${QT6_PATH}/plugins/styles/qwindowsvistastyle.dll" DESTINATION "styles")
install(FILES "${QT6_PATH}/bin/Qt6Core5Compat.dll" DESTINATION ".")
install(FILES "${QUAZIP_PATH}/bin/quazip1-qt6.dll" DESTINATION ".")
install(FILES "${ZLIB_PATH}/bin/zlib.dll" DESTINATION ".")
