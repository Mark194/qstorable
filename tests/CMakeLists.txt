# Включение тестового фреймворка
enable_testing()

# Поиск Qt Test
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Test)

# Добавление тестового исполняемого файла
add_executable(qstorable_tests
        test_serializer.cpp
        test_adapters.cpp
        main.cpp  # Содержит QTEST_MAIN
)
target_link_libraries(qstorable_tests
        PRIVATE
        QStorable
        Qt${QT_VERSION_MAJOR}::Test
)

# Регистрация в CTest
add_test(NAME qstorable_unit_tests COMMAND qstorable_tests)