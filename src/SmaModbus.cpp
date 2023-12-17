#include <vector>
#include <cmath>
#include <SmaModbus.hpp>
#include <SmaModbusValue.hpp>

using namespace MB;
using namespace MB::TCP;
using namespace MB::utils;
using namespace libsmamodbus;


std::string SmaModbus::toString(const AccessMode& mode) {
    switch (mode) {
    case AccessMode::RO:  return "RO";
    case AccessMode::WO:  return "WO";
    case AccessMode::RW:  return "RW";
    }
    return "INVALID";
}

std::string SmaModbus::toString(const Category& category) {
    switch (category) {
    case Category::Normal:                  return "Normal";
    case Category::GridGuardCodeProtected:  return "GridGuardCodeProtected";
    case Category::DeviceControlObject:     return "DeviceControlObject";
    case Category::CyclicWritingWarning:    return "CyclicWritingWarning";
    }
    return "INVALID";
}

std::string SmaModbus::RegisterDefinition::toString(void) const {
    char buff[512];
    snprintf(buff, sizeof(buff), "%u %-5s %-4s %-2s %-20s", (unsigned)addr, ::toString(type).c_str(), ::toString(format).c_str(), SmaModbus::toString(mode).c_str(), identifier.c_str());
    return std::string(buff);
}


SmaModbusValue SmaModbus::readRegister(const RegisterDefinition& reg) {
    SmaModbusException exception;
    SmaModbusValue value;

    //if (reg.mode == AccessMode::WO) {
    //    exception = SmaModbusException(SmaModbusErrorCode::InvalidAccessMode, 3, MBFunctionCode::ReadAnalogOutputHoldingRegisters);
    //}
    //else {
        switch (reg.type) {
        case DataType::S32:
        case DataType::U32:
        case DataType::S64:
        case DataType::U64:
        case DataType::ENUM: {
            uint64_t int_value = readUint(reg.addr, reg.size * 2u, exception, false, true);
            value = SmaModbusValue(int_value, (exception.hasError() ? DataType::INVALID : reg.type), reg.format);
            break;
        }
        case DataType::STR32:{
            std::string str_value = readString(reg.addr, reg.size * 2u, exception, false, true);
            value = SmaModbusValue(str_value, (exception.hasError() ? DataType::INVALID : reg.type), reg.format);
            break;
        }
        default:
            exception = SmaModbusException(SmaModbusErrorCode::InvalidDataType, 3, MBFunctionCode::ReadAnalogOutputHoldingRegisters);
            break;
        }
    //}

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
        switch (reg.type) {
        case DataType::S32:
        case DataType::U32:
        case DataType::S64:
        case DataType::U64:
        case DataType::ENUM: {
            uint64_t reg_value = value.operator uint64_t();
            if (value.getDataType() != reg.type || value.getDataFormat() != reg.format) {
                reg_value = SmaModbusValue(value.operator double(), reg.type, reg.format); // apply the register type and format to the given value
            }
            result = writeUint(reg.addr, reg.size * 2u, reg_value, exception, false, true);
            break;
        }
        case DataType::STR32:result = writeString(reg.addr, reg.size * 2u, (std::string)value, exception, false, true); break;
        default:  exception = SmaModbusException(SmaModbusErrorCode::InvalidFormatType, 3, MBFunctionCode::WriteMultipleAnalogOutputHoldingRegisters); break;
        }
    }

    if (exception.hasError() || result == false) {
        printf("writeRegister(%lu, %s) => %s\n", reg.addr, ((std::string)value).c_str(), exception.toString().c_str());
        return false;
    }
    return result;
}


std::vector<SmaModbus::SmaModbusDeviceEntry> SmaModbus::getDeviceMap(void) {
    std::vector <SmaModbusDeviceEntry> entries;
    SmaModbusException exception;

    // temporarily change unit id to 1
    SmaModbusUnitID previous_id = getUnitID();
    setUnitID(SmaModbusUnitID::DEVICE_MAP);

    uint16_t addr = 42109;  // modbus register address of first map entry
    while (addr <= 43085) { // modbus register address of the last possible map entry

        // each entry information is stored in 4 consecutive modbus registers:
        // - 2 bytes susy id
        // - 4 bytes serial number
        // - 2 bytes modbus unit id
        uint64_t value = readUint(addr, 8u, exception, false, true); // read from unit id 1

        // the end of the map is reached when all bytes are 0xff, or a read exception occured
        if (value == (uint64_t)-1 || exception.hasError()) {
            break;
        }
        entries.push_back(SmaModbusDeviceEntry((uint16_t)(value >> 48), (uint32_t)(value >> 16), (uint16_t)(value & 0xffff)));
        addr += 4u;
    }
    setUnitID(previous_id);
    return entries;
}


void SmaModbus::printRegister(const RegisterDefinition& reg, const SmaModbusValue& value) const {
    switch (reg.type) {
    case DataType::S32:
    case DataType::U32:
    case DataType::S64:
    case DataType::U64:
    case DataType::ENUM: {
        uint64_t reg_value = value.operator uint64_t();
        if (value.getDataType() != reg.type || value.getDataFormat() != reg.format) {
            reg_value = SmaModbusValue(value.operator double(), reg.type, reg.format); // apply the register type and format to the given value
        }
        printf("%s:  %08llx %llu\n", reg.toString().c_str(), value.operator uint64_t(), value.operator uint64_t());
        break;
    }
    case DataType::STR32:
        printf("%s:  %s\n", reg.toString().c_str(), value.operator std::string().c_str());
        break;
    default: {
        printf("%s:  %08llx %llu %s\n", reg.toString().c_str(), value.operator uint64_t(), value.operator uint64_t(), value.operator std::string().c_str());
        break;
    }
    }
}


uint8_t SmaModbus::setDefaultUnitID(void) {
    auto map = getDeviceMap();
    for (auto& entry : map) {
        // choose the first map entry of the device map, if it is between 1 and 255
        if (entry.unitID > SmaModbusUnitID::BROADCAST && entry.unitID <= SmaModbusUnitID::MAX) {
            setUnitID((SmaModbusUnitID)entry.unitID);
            return getUnitID();
        }
    }
    return SmaModbusUnitID::MAX;
}
