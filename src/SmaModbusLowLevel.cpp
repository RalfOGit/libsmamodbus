#include <SmaModbusLowLevel.hpp>

using namespace MB;
using namespace MB::TCP;
using namespace MB::utils;
using namespace libsmamodbus;


bool SmaModbusLowLevel::ensureConnection(void) {
    if (modbus.getSockfd() < 0) {
        modbus = MB::TCP::Connection::with(peer_ip, peer_port);
    }
    return true;
}


uint64_t SmaModbusLowLevel::readUint(uint16_t addr, size_t nbytes,  SmaModbusException& exception, bool allow_exception, bool print_exception) {
    std::vector<uint16_t> words = readWords(addr, nbytes / 2u, exception, allow_exception, false);
    if (!exception.hasError() && (nbytes > sizeof(uint64_t) || words.size() * 2u != nbytes)) {
        exception = SmaModbusException(InvalidNumberOfRegisters, 3, MBFunctionCode::ReadAnalogOutputHoldingRegisters);
    }
    if (exception.hasError()) {
        if (print_exception) {
            printf("readUint(%lu) => %s\n", (unsigned long)addr, exception.toString().c_str());
        }
        if (allow_exception) {
            throw exception;
        }
    }
    uint64_t result = 0;
    for (const auto word : words) {
        result = (result << 16) | word;
    }
    return result;
}


std::string SmaModbusLowLevel::readString(uint16_t addr, size_t nbytes, SmaModbusException& exception, bool allow_exception, bool print_exception) {
    std::vector<uint16_t> words = readWords(addr, nbytes / 2u, exception, allow_exception, false);
    if (!exception.hasError() && words.size() * 2u != nbytes) {
        exception = SmaModbusException(InvalidNumberOfRegisters, 3, MBFunctionCode::ReadAnalogOutputHoldingRegisters);
    }
    if (exception.hasError()) {
        if (print_exception) {
            printf("readString(%lu, %lu) => %s\n", (unsigned long)addr, (unsigned long)nbytes, exception.toString().c_str());
        }
        if (allow_exception) {
            throw exception;
        }
    }
    std::string result;
    for (const auto word : words) {
        result.append(1, (unsigned char)(word >> 8));
        result.append(1, (unsigned char)(word));
    }
    return result;
}


std::vector<uint16_t> SmaModbusLowLevel::readWords(uint16_t addr, size_t num_words, SmaModbusException& exception, bool allow_exception, bool print_exception) {
    std::vector<uint16_t> result;
    try {
        ensureConnection();
        ModbusRequest request(3, MBFunctionCode::ReadAnalogOutputHoldingRegisters, addr, (uint16_t)num_words);
        modbus.sendRequest(request);
        ModbusResponse response = modbus.awaitResponse();
        auto values = response.registerValues();
        for (const auto &value : values) {
            result.push_back(value.reg());
        }
        if (result.size() != num_words) {
            throw SmaModbusException(InvalidNumberOfRegisters, 3, MBFunctionCode::ReadAnalogOutputHoldingRegisters);
        }
    }
    catch (ModbusException ex) {
        exception = SmaModbusException(ex);
        if (print_exception) {
            printf("readWords(%lu) => %s\n", (unsigned long)addr, ex.toString().c_str());
        }
        if (allow_exception) {
            throw ex;
        }
    }
    return result;
}


bool SmaModbusLowLevel::writeUint(uint16_t addr, size_t nbytes, uint64_t value, SmaModbusException& exception, bool allow_exception, bool print_exception) {
    std::vector<uint16_t> words;
    for (size_t i = nbytes; i > 0; i -= 2) {
        words.push_back((uint16_t)(value >> ((i - 2) * 8u)));
    }
    if (words.size() * 2u != nbytes) {
        throw SmaModbusException(InvalidNumberOfRegisters, 3, MBFunctionCode::WriteMultipleAnalogOutputHoldingRegisters);
    }
    bool result = writeWords(addr, words, exception, allow_exception, false);
    if (exception.hasError()) {
        if (print_exception) {
            printf("writeUint(%lu, %lu, %lu) => %s\n", (unsigned long)addr, (unsigned long)nbytes, (unsigned long)value, exception.toString().c_str());
        }
        if (allow_exception) {
            throw exception;
        }
    }
    return result;
}


bool SmaModbusLowLevel::writeString(uint16_t addr, size_t nbytes, const std::string& value, SmaModbusException& exception, bool allow_exception, bool print_exception) {
    std::string str(value);
    if (str.size() < nbytes) {
        str.append(nbytes - str.size(), 0);
    }
    std::vector<uint16_t> words;
    for (size_t i = 0; i < str.size(); i += 2) {
        words.push_back((uint16_t)(str[i] << 8) | (uint16_t)str[i + 1]);
    }
    if (words.size() * 2u != nbytes) {
        throw SmaModbusException(InvalidNumberOfRegisters, 3, MBFunctionCode::WriteMultipleAnalogOutputHoldingRegisters);
    }
    bool result = writeWords(addr, words, exception, allow_exception, false);
    if (exception.hasError()) {
        if (print_exception) {
            printf("writeString(%lu, %lu, %s) => %s\n", (unsigned long)addr, (unsigned long)nbytes, value.c_str(), exception.toString().c_str());
        }
        if (allow_exception) {
            throw exception;
        }
    }
    return result;
}


bool SmaModbusLowLevel::writeWords(uint16_t addr, const std::vector<uint16_t> &value, SmaModbusException& exception, bool allow_exception, bool print_exception) {
    try {
        ensureConnection();
        std::vector<ModbusCell> modbus_cells;
        for (const auto& word : value) {
            modbus_cells.push_back(ModbusCell(word));
        }
        ModbusRequest request(3, MBFunctionCode::WriteMultipleAnalogOutputHoldingRegisters, addr, (uint16_t)modbus_cells.size(), modbus_cells);
        modbus.sendRequest(request);
        ModbusResponse response = modbus.awaitResponse();
    }
    catch (ModbusException ex) {
        exception = SmaModbusException(ex);
        if (print_exception) {
            printf("writeWords(%lu, ...) => %s\n", (unsigned long)addr, ex.toString().c_str());
        }
        if (allow_exception) {
            throw ex;
        }
        return false;
    }
    return true;
}
