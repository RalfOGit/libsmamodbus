#ifndef __SMAMODBUSAPIVALUE_HPP__
#define __SMAMODBUSAPIVALUE_HPP__

#include <cstdint>
#include <string>
#include <cmath>
#include <SmaModbus.hpp>


namespace libsmamodbus {

    /**
     *  Enumeration of data types used in SMA modbus registers.
     */
    enum class DataType : uint8_t {
        INVALID = 0,
        U32 = 1,
        S32 = 2,
        U64 = 3,
        S64 = 4,
        ENUM = 5,
        STR32 = 6,
    };

    /**
     *  Enumeration of data formats used in SMA modbus registers.
     */
    enum class DataFormat : uint8_t {
        FIX0 = 1,
        FIX1 = 2,
        FIX2 = 3,
        FIX3 = 4,
        FIX4 = 5,
        DURATION = 6,
        DATETIME = 7,
        TEMP = 8,
        RAW = 9,
        UTF8 = 10,
        FIRMWARE = 11
    };

    /**
     *  Class holding an SMA modbus data value together with its data type and data format.
     * 
     */
    class SmaModbusValue {
    protected:
        uint64_t u64;
        std::string str;
        DataType type;
        DataFormat format;

    public:
        //  Definitions for SMA NaN values
        inline static const uint32_t U32_NaN = 0xffffffff;
        inline static const int32_t  S32_NaN = 0x80000000;
        inline static const uint64_t U64_NaN = 0xffffffffffffffff;
        inline static const int64_t  S64_NaN = 0x8000000000000000;
        inline static const uint32_t Enum_NaN = 0x00fffffd;
        inline static const double   Double_NaN = nan("1");

        static bool isNaN(double value) { return isnan(value); }

        SmaModbusValue(void) : u64(0), type(DataType::INVALID), format(DataFormat::RAW) {}
        SmaModbusValue(uint32_t value, DataType typ = DataType::U32, DataFormat fmt = DataFormat::RAW) : SmaModbusValue((uint64_t)value, typ, fmt) {}
        SmaModbusValue(int32_t  value, DataType typ = DataType::S32, DataFormat fmt = DataFormat::RAW) : SmaModbusValue((uint64_t)(uint32_t)value, typ, fmt) {}
        SmaModbusValue(int64_t  value, DataType typ = DataType::S64, DataFormat fmt = DataFormat::RAW) : SmaModbusValue((uint64_t)value, typ, fmt) {}
        SmaModbusValue(uint64_t value, DataType typ = DataType::U64, DataFormat fmt = DataFormat::RAW) : u64(value), type(typ), format(fmt) {
            uint64_t fix_multiplier = 1;
            switch (format) {
            case DataFormat::FIX1: fix_multiplier = 10; break;
            case DataFormat::FIX2: fix_multiplier = 100; break;
            case DataFormat::FIX3: fix_multiplier = 1000; break;
            case DataFormat::FIX4: fix_multiplier = 10000; break;
            }
            switch (type) {
            case DataType::U32:  if (u64 != U32_NaN) u64 = (uint64_t)((uint32_t)u64 * (uint32_t)fix_multiplier); break;
            case DataType::S32:  if (u64 != S32_NaN) u64 = (uint64_t)(uint32_t)((int32_t)u64 * (int32_t)fix_multiplier); break;
            case DataType::U64:  if (u64 != U64_NaN) u64 = u64 * fix_multiplier; break;
            case DataType::S64:  if (u64 != S64_NaN) u64 = (uint64_t)((int64_t)u64 * (int64_t)fix_multiplier); break;
            }
        }
        SmaModbusValue(const std::string& value, DataType typ = DataType::STR32, DataFormat fmt = DataFormat::RAW) : u64(0), str(value), type(typ), format(fmt) {}

        SmaModbusValue(double value, DataType typ = DataType::U32, DataFormat fmt = DataFormat::RAW) {
            type = typ;
            format = fmt;
            if (!isNaN(value)) {
                switch (format) {
                case DataFormat::FIX1: value *= 10.0; break;
                case DataFormat::FIX2: value *= 100.0; break;
                case DataFormat::FIX3: value *= 1000.0; break;
                case DataFormat::FIX4: value *= 10000.0; break;
                }
            }
            switch (type) {
            case DataType::U32:  u64 = (uint64_t)(value >= 0 && !isNaN(value) ? (uint64_t)(uint32_t)(value + 0.5) : U32_NaN); break;
            case DataType::S32:  u64 = (uint64_t)(isNaN(value) ? S32_NaN : (uint64_t)(uint32_t)(int32_t)(value >= 0 ? value + 0.5 : value - 0.5)); break;
            case DataType::U64:  u64 = (uint64_t)(value >= 0 && !isNaN(value) ? (uint64_t)(value + 0.5) : U64_NaN); break;
            case DataType::S64:  u64 = (uint64_t)(isNaN(value) ? S64_NaN : (uint64_t)(int64_t)(value >= 0 ? value + 0.5 : value - 0.5)); break;
            case DataType::ENUM: u64 = (uint64_t)(value >= 0 && !isNaN(value) ? (uint64_t)(uint32_t)(value + 0.5) : Enum_NaN); break;
            }
        }

        DataType getDataType(void) const { return type; }
        DataFormat getDataFormat(void) const { return format; }

        operator uint32_t(void) const { return (uint32_t)u64; }
        operator int32_t(void) const { return (int32_t)u64; }
        operator uint64_t(void) const { return (uint64_t)u64; }
        operator int64_t(void) const { return (int64_t)u64; }
        operator std::string(void) const {
            std::string result;
            if (type == DataType::STR32) {
                result = str;
            }
            else {
                double dvalue = this->operator double();
                if (isNaN(dvalue)) {
                    result = "NaN";
                }
                else {
                    char buffer[65];
                    switch (format) {
                    case DataFormat::FIX0: snprintf(buffer, sizeof(buffer), "%.0lf", dvalue); break;
                    case DataFormat::FIX1: snprintf(buffer, sizeof(buffer), "%.1lf", dvalue); break;
                    case DataFormat::FIX2: snprintf(buffer, sizeof(buffer), "%.2lf", dvalue); break;
                    case DataFormat::FIX3: snprintf(buffer, sizeof(buffer), "%.3lf", dvalue); break;
                    case DataFormat::FIX4: snprintf(buffer, sizeof(buffer), "%.4lf", dvalue); break;
                    default:               snprintf(buffer, sizeof(buffer), "%lf", dvalue);
                    }
                    result = std::string(buffer);
                }
            }
            return result;
        }
        operator double(void) const {
            double result = nan("2");
            switch (type) {
            case DataType::U32:  result = (u64 == U32_NaN ? Double_NaN : (double)u64); break;
            case DataType::S32:  result = (u64 == S32_NaN ? Double_NaN : (double)(int64_t)u64); break;
            case DataType::U64:  result = (u64 == S32_NaN ? Double_NaN : (double)u64); break;
            case DataType::S64:  result = (u64 == S32_NaN ? Double_NaN : (double)(int64_t)u64); break;
            case DataType::ENUM: result = (u64 == Enum_NaN ? Double_NaN : (double)u64); break;
            }

            if (!isNaN(result)) {
                switch (format) {
                case DataFormat::FIX1:  result /= 10.0; break;
                case DataFormat::FIX2:  result /= 100.0; break;
                case DataFormat::FIX3:  result /= 1000.0; break;
                case DataFormat::FIX4:  result /= 10000.0; break;
                }
            }
            return result;
        }

        bool isValid(void) const {
            bool result = false;
            if (type != DataType::INVALID) {
                switch (type) {
                case DataType::U32:  result = (u64 != U32_NaN); break;
                case DataType::S32:  result = (u64 != S32_NaN); break;
                case DataType::U64:  result = (u64 != S32_NaN); break;
                case DataType::S64:  result = (u64 != S32_NaN); break;
                case DataType::ENUM: result = (u64 != Enum_NaN); break;
                }
            }
            return result;
        }
    };

}   // namespace libsmamodbus

#endif
