#include <vector>
#include <cmath>
#include <SmaModbus.hpp>
#include <SmaModbusValue.hpp>

using namespace MB;
using namespace MB::TCP;
using namespace MB::utils;
using namespace libsmamodbus;


SmaModbusValue SmaModbus::readRegister(const RegisterDefinition& reg) {
    SmaModbusException exception;
    SmaModbusValue value;

    if (reg.mode == AccessMode::WO) {
        exception = SmaModbusException(SmaModbusErrorCode::InvalidAccessMode, 3, MBFunctionCode::ReadAnalogOutputHoldingRegisters);
    }
    else {
        switch (reg.type) {
        case DataType::S32:  //{ value = SmaModbusValue(readUint(reg.addr, reg.size * 2u, exception, false, true), reg.type, reg.format); break; }
        case DataType::U32:  //{ value = SmaModbusValue(readUint(reg.addr, reg.size * 2u, exception, false, true), reg.type, reg.format); break; }
        case DataType::S64:  //{ value = SmaModbusValue(readUint(reg.addr, reg.size * 2u, exception, false, true), reg.type, reg.format); break; }
        case DataType::U64:  //{ value = SmaModbusValue(readUint(reg.addr, reg.size * 2u, exception, false, true), reg.type, reg.format); break; }
        case DataType::ENUM: { value = SmaModbusValue(readUint(reg.addr, reg.size * 2u, exception, false, true), reg.type, reg.format); break; }
        case DataType::STR32:{ value = SmaModbusValue(readString(reg.addr, reg.size * 2u, exception, false, true), reg.type, reg.format); break; }
        default: exception = SmaModbusException(SmaModbusErrorCode::InvalidDataType, 3, MBFunctionCode::ReadAnalogOutputHoldingRegisters); break;
        }
    }

    if (exception.hasError()) {
        printf("readRegister(%lu) => %s\n", reg.addr, exception.toString().c_str());
    }
    return value;
}

bool SmaModbus::writeRegister(const RegisterDefinition& reg, const SmaModbusValue& value) {
    SmaModbusException exception;
    bool result = false;

    if (reg.mode == AccessMode::RO) {
        exception = SmaModbusException(SmaModbusErrorCode::InvalidAccessMode, 3, MBFunctionCode::WriteMultipleAnalogOutputHoldingRegisters);
    }
    else {
        // FIXME: NOT YET OK
        uint64_t reg_value = SmaModbusValue((uint64_t)value, reg.type, reg.format);

        switch (reg.type) {
        case DataType::S32:  result = writeUint(reg.addr, reg.size * 2u, (int32_t)reg_value, exception, false, true); break;
        case DataType::U32:  result = writeUint(reg.addr, reg.size * 2u, (uint32_t)reg_value, exception, false, true); break;
        case DataType::S64:  result = writeUint(reg.addr, reg.size * 2u, (int64_t)reg_value, exception, false, true); break;
        case DataType::U64:  result = writeUint(reg.addr, reg.size * 2u, (uint64_t)reg_value, exception, false, true); break;
        case DataType::ENUM: result = writeUint(reg.addr, reg.size * 2u, (uint32_t)value, exception, false, true); break;
        case DataType::STR32:result = writeString(reg.addr, reg.size * 2u, (std::string)value, exception, false, true); break;
        default:  exception = SmaModbusException(SmaModbusErrorCode::InvalidFormatType, 3, MBFunctionCode::WriteMultipleAnalogOutputHoldingRegisters); break;
        }
    }

    printf("writeRegister(%lu, %s) => %s\n", reg.addr, ((std::string)value).c_str(), exception.toString().c_str());
    if (exception.hasError() || result == false) {
        printf("writeRegister(%lu, %s) => %s\n", reg.addr, ((std::string)value).c_str(), exception.toString().c_str());
        return false;
    }
    return result;
}
