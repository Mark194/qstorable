cmake_minimum_required(VERSION 3.10)
project(QStorableBinaryTest LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Core Test)

set(CMAKE_AUTOMOC ON)

set(TEST_SOURCES
        test_base_field.cpp
        test_custom_field.cpp
        test_array_field.cpp
        test_custom_array_field.cpp
        test_real_data.cpp
)

foreach(test_file ${TEST_SOURCES})
    get_filename_component(test_name ${test_file} NAME_WE)

    add_executable(${test_name} ${test_file} ${QSTORABLE_INCLUDE_DIR}/QStorable/qstorable.hpp)

    target_link_libraries(${test_name} PRIVATE
            Qt5::Core
            Qt5::Test
    )

    target_include_directories(${test_name} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/include
            ${CMAKE_SOURCE_DIR}/src
    )

    if(BUILD_TESTING)
        add_test(NAME ${test_name} COMMAND ${test_name})
    endif()
endforeach()

add_definitions(-DQS_BINARY_SUPPORT)