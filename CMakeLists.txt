cmake_minimum_required(VERSION 3.10)
project(QStorable VERSION 1.0.0 LANGUAGES CXX)

# Настройки C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Опции для компонентов
option(QSTORABLE_BUILD_TESTS "Build tests" OFF)
option(QSTORABLE_BUILD_EXAMPLES "Build examples" OFF)

# Поиск Qt (Core)
if(NOT Qt6_FOUND)
    find_package(Qt5 5.15 REQUIRED COMPONENTS Core)
    set(QT_VERSION_MAJOR 5)
else()
    set(QT_VERSION_MAJOR 6)
endif()

# Header-only библиотека
add_library(QStorable INTERFACE
        include/QStorable/qstorable.hpp
        tests/binary/test_base_field.cpp)
target_include_directories(QStorable INTERFACE
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
)
target_link_libraries(QStorable INTERFACE Qt${QT_VERSION_MAJOR}::Core)

# Установка
install(DIRECTORY include/ DESTINATION include)
install(TARGETS QStorable EXPORT QStorableTargets)

# Конфигурация для find_package
include(CMakePackageConfigHelpers)
configure_package_config_file(
        ${CMAKE_CURRENT_SOURCE_DIR}/cmake/QStorableConfig.cmake.in
        ${CMAKE_CURRENT_BINARY_DIR}/QStorableConfig.cmake
        INSTALL_DESTINATION lib/cmake/QStorable
)
write_basic_package_version_file(
        QStorableConfigVersion.cmake
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY SameMajorVersion
)
install(EXPORT QStorableTargets FILE QStorableTargets.cmake
        DESTINATION lib/cmake/QStorable)
install(FILES
        ${CMAKE_CURRENT_BINARY_DIR}/QStorableConfig.cmake
        ${CMAKE_CURRENT_BINARY_DIR}/QStorableConfigVersion.cmake
        DESTINATION lib/cmake/QStorable)

if(BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif()

if(QSTORABLE_BUILD_EXAMPLES AND EXISTS ${CMAKE_CURRENT_SOURCE_DIR}/examples/CMakeLists.txt)
    add_subdirectory(examples)
endif()