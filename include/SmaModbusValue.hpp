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
    std::string toString(const DataType& type);

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
    std::string toString(const DataFormat& format);

    /**
     *  Class holding an SMA modbus data value together with its data type and data format.
     * 
     */
    class SmaModbusValue {
    public:
        //  Definitions for SMA NaN values
        static const uint32_t U32_NaN = 0xffffffff;          //!< NaN value for SMA data type U32
        static const int32_t  S32_NaN = 0x80000000;          //!< NaN value for SMA data type S32
        static const uint64_t U64_NaN = 0xffffffffffffffff;  //!< NaN value for SMA data type U64
        static const int64_t  S64_NaN = 0x8000000000000000;  //!< NaN value for SMA data type S64
        static const uint32_t Enum_NaN = 0x00fffffd;         //!< NaN value for SMA data type ENUM
        static const double   Double_NaN;                    //!< NaN value for double data types

        uint64_t    u64;    //!< value for numeric types incl. enum/tags
        std::string str;    //!< value for string type
        DataType    type;   //!< data type
        DataFormat  format; //!< data format

        /** Check if the given double value is a nan value */
        static bool isNaN(double value) { return isnan(value); }

        /** Default constructor. */
        SmaModbusValue(void) : u64(0), type(DataType::INVALID), format(DataFormat::RAW) {}

        /** Construct from numeric integer value. */
        SmaModbusValue(uint64_t value, const DataType typ = DataType::U64, const DataFormat fmt = DataFormat::RAW) : u64(value), type(typ), format(fmt) {
            switch (type) {       // ensure 32-bit significant values to avoid failures on NaN checks
            case DataType::U32:
            case DataType::S32:
            case DataType::ENUM:  u64 &= (uint32_t)-1; break;
            }
        }

        /** Construct from string value. */
        SmaModbusValue(const std::string& value, const DataType typ = DataType::STR32, const DataFormat fmt = DataFormat::RAW) : u64(0), str(value), type(typ), format(fmt) {}

        /** Construct from numeric floating point value. */
        SmaModbusValue(double value, const DataType typ = DataType::U32, const DataFormat fmt = DataFormat::RAW) : u64(0), type(typ), format(fmt) {
            const bool isValid = !isNaN(value);
            if (isValid) {
                switch (format) {
                case DataFormat::FIX1: value *= 10.0; break;
                case DataFormat::FIX2: value *= 100.0; break;
                case DataFormat::FIX3: value *= 1000.0; break;
                case DataFormat::FIX4: value *= 10000.0; break;
                }
            }
            switch (type) {
            case DataType::U32:  u64 = (uint64_t)(value >= 0 && isValid ? (uint64_t)(uint32_t)(value + 0.5) : U32_NaN); break;
            case DataType::S32:  u64 = (uint64_t)(isValid ? (uint64_t)(uint32_t)(int32_t)(value >= 0 ? value + 0.5 : value - 0.5) : S32_NaN); break;
            case DataType::U64:  u64 = (uint64_t)(value >= 0 && isValid ? (uint64_t)(value + 0.5) : U64_NaN); break;
            case DataType::S64:  u64 = (uint64_t)(isValid ? (uint64_t)(int64_t)(value >= 0 ? value + 0.5 : value - 0.5) : S64_NaN); break;
            case DataType::ENUM: u64 = (uint64_t)(value >= 0 && isValid ? (uint64_t)(uint32_t)(value + 0.5) : Enum_NaN); break;
            }
        }

        /** Convert numeric value to floating point */
        double toDouble(void) const {
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

        /** Convert value to a string representation. */
        std::string toString(void) const {
            std::string result;
            if (type == DataType::STR32) {
                result = str;
            }
            else {
                double dvalue = toDouble();
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

        /** Check if the value is valid. Invalid data types and NaN values are considered as invalid. */
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
