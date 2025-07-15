cmake_minimum_required(VERSION 3.10)
project(QStorableBinaryTest LANGUAGES CXX)

find_package(Qt5 REQUIRED COMPONENTS Core Test)

# Включаем AUTOMOC глобально (для Q_GADGET)
set(CMAKE_AUTOMOC ON)

set(TEST_SOURCES
        test_base_field.cpp
        test_custom_field.cpp
)

foreach(test_file ${TEST_SOURCES})
    # Получаем имя теста без расширения
    get_filename_component(test_name ${test_file} NAME_WE)

    # Создаем исполняемый файл
    add_executable(${test_name} ${test_file} ${QSTORABLE_INCLUDE_DIR}/QStorable/qstorable.hpp)

    # Подключаем зависимости
    target_link_libraries(${test_name} PRIVATE
            Qt5::Core
            Qt5::Test
    )

    # Правильное указание include директорий
    target_include_directories(${test_name} PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}
            ${CMAKE_SOURCE_DIR}/include  # Путь к QStorable
            ${CMAKE_SOURCE_DIR}/src     # Дополнительные исходники
    )

    # Добавляем тест в CTest
    if(BUILD_TESTING)
        add_test(NAME ${test_name} COMMAND ${test_name})
    endif()
endforeach()

add_definitions(-DQS_BINARY_SUPPORT)