cmake_minimum_required(VERSION 2.6)
project(datavis)

add_definitions("-Wall -Wextra")

set(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/lib)

include_directories(${PROJECT_SOURCE_DIR})

file(GLOB lib_source "src/*.cpp" "datavis/*.hpp")
list(SORT lib_source)

file(GLOB apps_source "apps/*.cpp")
list(SORT apps_source)

find_package(OpenSceneGraph)
include_directories(${OpenSceneGraph_INCLUDE_DIRS})
add_definitions(${OpenSourceGraph_DEFINITIONS})

add_library(${PROJECT_NAME} SHARED ${lib_source})


message(STATUS "\n -- Applications: ")
add_custom_target(check COMMAND ${CMAKE_CTEST_COMMAND})
foreach(app_src_file ${apps_source})
    get_filename_component(app_base ${app_src_file} NAME_WE)
    message(STATUS "Adding application ${app_base}")
    add_executable(${app_base} ${app_src_file})
    target_link_libraries(${app_base} ${PROJECT_NAME} osg osgViewer osgText osgGA)
    add_dependencies(check ${app_base})
endforeach(app_src_file)
