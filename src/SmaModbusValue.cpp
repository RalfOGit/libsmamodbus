#include <SmaModbus.hpp>
#include <SmaModbusValue.hpp>

using namespace libsmamodbus;

const double SmaModbusValue::Double_NaN = nan("1");         //!< NaN value for double data types


namespace libsmamodbus {

    std::string toString(const DataType& type) {
        switch (type) {
        case DataType::INVALID: return "INVALID";
        case DataType::U32:     return "U32";
        case DataType::S32:     return "S32";
        case DataType::U64:     return "U64";
        case DataType::S64:     return "S64";
        case DataType::ENUM:    return "ENUM";
        case DataType::STR32:   return "STR32";
        }
        return "INVALID";
    }

    std::string toString(const DataFormat& format) {
        switch (format) {
        case DataFormat::FIX0:      return "FIX0";
        case DataFormat::FIX1:      return "FIX1";
        case DataFormat::FIX2:      return "FIX2";
        case DataFormat::FIX3:      return "FIX3";
        case DataFormat::FIX4:      return "FIX4";
        case DataFormat::DURATION:  return "DUR";
        case DataFormat::DATETIME:  return "DT";
        case DataFormat::TEMP:      return "TEMP";
        case DataFormat::RAW:       return "RAW";
        case DataFormat::UTF8:      return "UTF8";
        case DataFormat::FIRMWARE:  return "FW";
        }
        return "INVALID";
    }
};
