project("smamodbus")

cmake_minimum_required(VERSION 3.10)

set(CMAKE_CXX_STANDARD 17)

set(COMMON_SOURCES
    src/SmaModbus.cpp
    src/SmaModbusApi.cpp
    src/SmaModbusLowLevel.cpp
    src/SmaModbusValue.cpp
)

add_library(${PROJECT_NAME} STATIC
    ${COMMON_SOURCES}
)

# add dependencies (adapt to your needs)
add_subdirectory(libmodbus)

add_dependencies(${PROJECT_NAME} Modbus_Core Modbus_TCP)
target_include_directories(${PROJECT_NAME} PUBLIC include Modbus_Core Modbus_TCP)

if (MSVC)
target_link_libraries(${PROJECT_NAME} Modbus_Core Modbus_TCP ws2_32.lib)
else()
target_link_libraries(${PROJECT_NAME} Modbus_Core Modbus_TCP)
endif()
