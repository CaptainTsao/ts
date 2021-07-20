cmake_minimum_required(VERSION 3.15)
project(time_series)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

include_directories(include/)
#include(include/enum_parser.h)
set(SOURCE_FILES
        main.cpp
        test/test.cpp
        src/file.cpp
        src/file_basic.cpp
        src/time_series_reader.cpp)

set(SOURCE_FILES_NO_MAIN
        test/test.cpp
        src/file.cpp
        src/file_basic.cpp
        src/time_series_reader.cpp include/enum_parser.h include/data_type.h include/column.h include/file.h include/ts_file_definition.h include/time_series.h include/parse.h include/time_series_reader.h include/errors.h)


add_executable(data_test ${SOURCE_FILES})
add_library(data ${SOURCE_FILES_NO_MAIN})

TARGET_LINK_LIBRARIES(data_test gtest pthread boost_thread boost_filesystem boost_system)