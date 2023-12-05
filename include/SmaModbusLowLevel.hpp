#ifndef __SMAMODBUSLOWLEVEL_HPP__
#define __SMAMODBUSLOWLEVEL_HPP__

#include <cstdint>
#include <string>
#include <vector>
#include <MB/TCP/connection.hpp>


namespace libsmamodbus {

    /**
     *  All possible modbus error codes.
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
     *  Class extending the ModbusException class; just to simplify use of SmaModbusErrorCodes.
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
     *  It provides low-level read and write operations for the most basic data types defined for sma modbus registers:
     *  - S32, U32, S64, U64, ENUM are all mapped to uint64_t with leading zeroes
     *  - STR32 is mapped to std::string, potentially including '\0' characters
     */
    class SmaModbusLowLevel {

    private:
        MB::TCP::Connection modbus;
        std::string peer_ip;
        uint16_t    peer_port;

        //!< ensure that the tcp connection is established, called by read and write methods for lazy initialization
        bool ensureConnection(void);

    public:
        /**
         *  Constructor; set member variables.
         */
        SmaModbusLowLevel(const std::string& peer, const uint16_t port = 502) : peer_ip(peer), peer_port(port), modbus(-1) {}

        /**
         *  Destructor; close the tcp connection.
         */
        ~SmaModbusLowLevel(void) {}

        /**
         *  Read an integral value of nbytes from the given modbus address.
         *  @param modbus address
         *  @nbytes number of bytes to be read from the modbus address; must be an even number
         *  @exception output parameter to receive any modbus exception information
         *  @allow_exception if true, the method will throw an SmaModbusException in case of an error
         *  @print_exception if true, the method will print exception information to stdout
         *  @return an uint64 value holding the bit pattern read from the modbus address
         */
        uint64_t readUint(uint16_t addr, size_t nbytes  = 4, SmaModbusException& exception = SmaModbusException(), bool allow_exception = false, bool print_exception = true);

        /**
         *  Read a string value of nbytes from the given modbus address.
         *  @param modbus address
         *  @nbytes number of bytes to be read from the modbus address; must be an even number
         *  @exception output parameter to receive any modbus exception information
         *  @allow_exception if true, the method will throw an SmaModbusException in case of an error
         *  @print_exception if true, the method will print exception information to stdout
         *  @return a string value holding characters read from the modbus address; this may include '\0' characters
         */
        std::string readString(uint16_t addr, size_t nbytes = 16, SmaModbusException& exception = SmaModbusException(), bool allow_exception = false, bool print_exception = true);

        /**
         *  Read a vector of uint16 values from the given modbus address. This is the most low-level read method.
         *  @param modbus address
         *  @num_words number of uint16 words to be read from the modbus address
         *  @exception output parameter to receive any modbus exception information
         *  @allow_exception if true, the method will throw an SmaModbusException in case of an error
         *  @print_exception if true, the method will print exception information to stdout
         *  @return a vector of uint16 values as read from the modbus address
         */
        std::vector<uint16_t> readWords(uint16_t addr, size_t num_words, SmaModbusException& exception = SmaModbusException(), bool allow_exception = false, bool print_exception = true);

        /**
         *  Write an integral value of nbytes to the given modbus address.
         *  @param modbus address
         *  @nbytes number of bytes to be written to the modbus address; must be an even number
         *  @value an uint64 value holding the bit pattern to be written to the modbus address
         *  @exception output parameter to receive any modbus exception information
         *  @allow_exception if true, the method will throw an SmaModbusException in case of an error
         *  @print_exception if true, the method will print exception information to stdout
         *  @return true if successful
         */
        bool writeUint(uint16_t addr, size_t nbytes, uint64_t value, SmaModbusException& exception = SmaModbusException(), bool allow_exception = false, bool print_exception = true);

        /**
         *  Write a string value of nbytes to the given modbus address.
         *  @param modbus address
         *  @nbytes number of bytes to be written to the modbus address; must be an even number
         *  @value a string value to be written to the modbus address. The written string is extended to nbytes if necessary
         *  @exception output parameter to receive any modbus exception information
         *  @allow_exception if true, the method will throw an SmaModbusException in case of an error
         *  @print_exception if true, the method will print exception information to stdout
         *  @return true if successful
         */
        bool writeString(uint16_t addr, size_t nbytes, const std::string& value, SmaModbusException& exception = SmaModbusException(), bool allow_exception = false, bool print_exception = true);

        /**
         *  Write a vector of uint16 values to the given modbus address. This is the most low-level write method.
         *  @param modbus address
         *  @value a vector of uint16 values to be written to the modbus address
         *  @exception output parameter to receive any modbus exception information
         *  @allow_exception if true, the method will throw an SmaModbusException in case of an error
         *  @print_exception if true, the method will print exception information to stdout
         *  @return true if successful
         */
        bool writeWords(uint16_t addr, const std::vector<uint16_t>& value, SmaModbusException& exception, bool allow_exception, bool print_exception);
    };

}   // namespace libsmamodbus

#endif
