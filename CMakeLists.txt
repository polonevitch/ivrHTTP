cmake_minimum_required(VERSION 2.8)
project(range)

option(STUB "MOCKUP" ON)

if(STUB)
  add_definitions(-DMOCKUP)
  message(STATUS "Building -=MOCKUP=-")
endif()

find_package( ZLIB REQUIRED )
if ( ZLIB_FOUND )
    include_directories( ${ZLIB_INCLUDE_DIRS} )
endif( ZLIB_FOUND )

include_directories($ENV{BASE_INCL})
include_directories($ENV{TP_LIBCURL_INC_DIR})

add_library(base STATIC IMPORTED)
set_property(TARGET base PROPERTY IMPORTED_LOCATION $ENV{DEV_LOCAL_LIB}/libbase.a)

add_library(cppbase STATIC IMPORTED)
set_property(TARGET cppbase PROPERTY IMPORTED_LOCATION $ENV{DEV_LOCAL_LIB}/libcppbase.a)

add_library(curl STATIC IMPORTED)
set_property(TARGET curl PROPERTY IMPORTED_LOCATION $ENV{TP_LIBCURL_LIB_DIR}/libcurl.a)

add_library (${PROJECT_NAME} ivrHttp.cpp ivrHttp.h)
target_link_libraries(${PROJECT_NAME} cppbase base curl ${ZLIB_LIBRARIES})
set_target_properties(${PROJECT_NAME} PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")

add_executable (cTest main.c)
target_link_libraries(cTest ${PROJECT_NAME})
set_target_properties(cTest PROPERTIES COMPILE_FLAGS "-m32" LINK_FLAGS "-m32")
