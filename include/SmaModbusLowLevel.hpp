#ifndef __SMAMODBUSLOWLEVEL_HPP__
#define __SMAMODBUSLOWLEVEL_HPP__

#include <cstdint>
#include <vector>
#include <string>
#include <MB/TCP/connection.hpp>


namespace libsmamodbus {

    /**
     *  All possible modbus error codes
     *  @note Contains standard modbus error codes, non-standard custom error codes from libmodbus and further custom error codes defined below.
     */
    enum SmaModbusErrorCode : uint8_t {
        NoError = 0,
        // MBErrorCodes go here
        InvalidDataType = 0x40,
        InvalidFormatType = 0x41,
        InvalidAccessMode = 0x42,
        InvalidNumberOfRegisters = 0x43,
        UnsupportedOperation = 0x44
    };


    /**
     *  Class extending the ModbusException class; just to simplify use of SmaModbusErrorCodes
     */
    class SmaModbusException : public MB::ModbusException {
    public:
        SmaModbusException(SmaModbusErrorCode errorCode, uint8_t slaveId = 0xFF, MB::utils::MBFunctionCode functionCode = MB::utils::Undefined) noexcept :
            MB::ModbusException((MB::utils::MBErrorCode)errorCode, slaveId, functionCode) {}

        SmaModbusException(const MB::ModbusException& mb) noexcept :
            SmaModbusException((SmaModbusErrorCode)mb.getErrorCode(), mb.slaveID(), mb.functionCode()) {}

        SmaModbusException(void) noexcept :
            SmaModbusException(SmaModbusErrorCode::NoError) {}

        SmaModbusErrorCode getErrorCode(void) const { return (SmaModbusErrorCode)MB::ModbusException::getErrorCode(); }

        bool hasError(void) const {
            return (getErrorCode() != SmaModbusErrorCode::NoError);
        }
    };


    /**
     *  Class encapsulating the modbus API from libmodbus.
     */
    class SmaModbusLowLevel {

    private:
        MB::TCP::Connection modbus;
        std::string peer_ip;
        uint16_t    peer_port;

        bool ensureConnection(void);

    public:
        // constructor / destructor
        SmaModbusLowLevel(const std::string& peer, const uint16_t port = 502) : peer_ip(peer), peer_port(port), modbus(-1) {}
        ~SmaModbusLowLevel(void) {}

        // low-level read operationwrappers around the libmodbus API
        uint64_t readUint(uint16_t addr, size_t nbytes  = 4, SmaModbusException& exception = SmaModbusException(), bool allow_exception = false, bool print_exception = true);
        std::string readString(uint16_t addr, size_t nbytes = 16, SmaModbusException& exception = SmaModbusException(), bool allow_exception = false, bool print_exception = true);
        std::vector<uint16_t> readWords(uint16_t addr, size_t num_words, SmaModbusException& exception = SmaModbusException(), bool allow_exception = false, bool print_exception = true);

        // low-level write operationwrappers around the libmodbus API
        bool writeUint(uint16_t addr, size_t nbytes, uint64_t value, SmaModbusException& exception = SmaModbusException(), bool allow_exception = false, bool print_exception = true);
        bool writeString(uint16_t addr, size_t nbytes, const std::string& value, SmaModbusException& exception = SmaModbusException(), bool allow_exception = false, bool print_exception = true);
        bool writeWords(uint16_t addr, const std::vector<uint16_t>& value, SmaModbusException& exception, bool allow_exception, bool print_exception);
    };

}   // namespace libsmamodbus

#endif
