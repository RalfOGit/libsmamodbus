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
            uint64_t int_value = readUint(unitID, reg.addr, reg.size * 2u, exception, false, true);
            value = SmaModbusValue(int_value, (exception.hasError() ? DataType::INVALID : reg.type), reg.format);
            break;
        }
        case DataType::STR32:{
            std::string str_value = readString(unitID, reg.addr, reg.size * 2u, exception, false, true);
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
            uint64_t reg_value = SmaModbusValue((uint64_t)value, reg.type, reg.format); // apply the register type and format to the given value
            result = writeUint(unitID, reg.addr, reg.size * 2u, reg_value, exception, false, true);
            break;
        }
        case DataType::STR32:result = writeString(unitID, reg.addr, reg.size * 2u, (std::string)value, exception, false, true); break;
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

    uint16_t addr = 42109;  // modbus register address of first map entry
    while (addr <= 43085) { // modbus register address of the last possible map entry

        // each entry information is stored in 4 consecutive modbus registers:
        // - 2 bytes susy id
        // - 4 bytes serial number
        // - 2 bytes modbus unit id
        uint64_t value = readUint(SmaModbusUnitID::DEVICE_MAP, addr, 8u, exception, false, true); // read from unit id 1

        // the end of the map is reached when all bytes are 0xff, or a read exception occured
        if (value == (uint64_t)-1 || exception.hasError()) {
            break;
        }
        entries.push_back(SmaModbusDeviceEntry((uint16_t)(value >> 48), (uint32_t)(value >> 16), (uint16_t)(value & 0xffff)));
        addr += 4u;
    }
    return entries;
}


uint8_t SmaModbus::setDefaultUnitID(void) {
    auto map = getDeviceMap();
    if (map.size() > 0) {
        // choose the first map entry of the device map, if it is between 1 and 255
        uint16_t unitid = map[0].unitID;
        if (unitid > SmaModbusUnitID::BROADCAST && unitid <= SmaModbusUnitID::MAX) {
            setUnitID((SmaModbusUnitID)unitid);
            return getUnitID();
        }
    }
    return SmaModbusUnitID::MAX;
}
