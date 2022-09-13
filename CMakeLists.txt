cmake_minimum_required(VERSION 3.21)
project(JavaProfilerAgent)

set(JPASRC ${PROJECT_SOURCE_DIR}/src/JProfiler)
set(OUTDIR ${PROJECT_BINARY_DIR}/out)

set(SRC_CPP_FILE 
                ${JPASRC}/cpp/entry.cpp 
                ${JPASRC}/cpp/global.h
                ${JPASRC}/cpp/global.cpp·
                ${JPASRC}/cpp/javaAgent.cpp
                ${JPASRC}/cpp/javaAgent.hpp
                ${JPASRC}/cpp/javaProfiler.cpp
                ${JPASRC}/cpp/javaProfiler.hpp
                )
add_library(jpa SHARED ${SRC_CPP_FILE})
set_target_properties(jpa PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${OUTDIR}/lib)
set_property(TARGET jpa PROPERTY POSITION_INDEPENDENT_CODE ON)

find_package(JNI)
if (JNI_FOUND)
    message (STATUS "JNI_INCLUDE_DIRS=${JNI_INCLUDE_DIRS}")
    message (STATUS "JNI_LIBRARIES=${JNI_LIBRARIES}")
    message (STATUS "JAVA_INCLUDE_PATH=${JAVA_INCLUDE_PATH}")
    message (STATUS "JAVA_INCLUDE_PATH2=${JAVA_INCLUDE_PATH2}")
endif()
INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH})
INCLUDE_DIRECTORIES(${JAVA_INCLUDE_PATH2})

find_package(Java REQUIRED)
include(UseJava)

set(CMAKE_JAVA_INCLUDE_PATH ${JAVA_INCLUDE_PATH}/../lib/tools.jar)
set(CMAKE_JAVA_TARGET_OUTPUT_DIR ${OUTDIR})

add_jar(attach-myagent src/JProfiler/AttachAgent.java ENTRY_POINT AttachAgent OUTPUT_DIR ${OUTDIR})

