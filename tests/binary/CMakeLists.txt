cmake_minimum_required(VERSION 3.10)
project(QStorableBinaryTest LANGUAGES CXX)

# Поиск Qt5 компонентов
find_package(Qt5 REQUIRED COMPONENTS Core Test)

set(SOURCES TestQStorable.cpp ${QSTORABLE_INCLUDE_DIR}/QStorable/qstorable.hpp)

# Создаем тестовый исполняемый файл
add_executable(qstorable_binary_test ${SOURCES})

# Подключаем автогенерацию moc
set_target_properties(qstorable_binary_test PROPERTIES
        AUTOMOC ON
)

# Линкуем необходимые Qt5 модули
target_link_libraries(qstorable_binary_test
        PRIVATE
        Qt5::Core
        Qt5::Test
)

target_include_directories(qstorable_binary_test PRIVATE ${QSTORABLE_INCLUDE_DIR})

# Подключаем заголовочные файлы основного проекта
target_include_directories(qstorable_binary_test
        PRIVATE
        ${CMAKE_SOURCE_DIR}/src
)

# Добавляем тест в CTest
if(BUILD_TESTING)
    include(CTest)
    add_test(NAME binary_serialization COMMAND qstorable_binary_test)
endif()

add_definitions(-DQS_BINARY_SUPPORT)