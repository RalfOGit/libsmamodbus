#ifndef __SMAMODBUS_HPP__
#define __SMAMODBUS_HPP__

#include <cstdint>
#include <string>
#include <SmaModbusLowLevel.hpp>
#include <SmaModbusValue.hpp>

namespace libsmamodbus {


    /**
     *  Class implementing access to sma modbus registers.
     *  It provides abstractions for register definitions and methods to read and write registers.
     */
    class SmaModbus : public SmaModbusLowLevel {

    public:

        /**
         *  Enumeration of data access modes used in SMA modbus registers.
         */
        enum class AccessMode : uint8_t {
            RO = 0x01,  //!< Read-only
            WO = 0x02,  //!< Write-only - technically readable, but returns NaN
            RW = 0x03   //!< Read-write
        };

        /**
         *  Enumeration for additional SMA modbus register related information
         */
        enum class Category : uint8_t {
            Normal                 = 0x00,  //<! no further information
            GridGuardCodeProtected = 0x01,  //<! access requires grid guard login
            DeviceControlObject    = 0x02,  //<! device control object for external power control
            CyclicWritingWarning   = 0x04   //<! cyclic writes will destroy the underlying memory cells
        };

        /**
         *  Class encapsulating all relevant information for a given SMA modbus registers.
         */
        class RegisterDefinition {
        public:
            uint16_t addr;              //!< Modbus address
            uint16_t size;              //!< Number of 16-bit words
            DataType type;              //!< SMA data type (S32, U32, ...)
            DataFormat format;          //!< SMA data format (FIX0, FIX1, ...)
            AccessMode mode;            //!< SMA access mode (RO, WO, RW)
            Category category;          //!< SMA register category (GridGuardCodeProtected, DeviceControlObject, CyclicWritingWarning)
            std::string identifier;     //!< SMA identifier name
            std::string description;    //!< Description of register

            /** Constructor */
            RegisterDefinition(uint16_t address, uint16_t numwords, const DataType& dtype,
                const DataFormat& fmt, const AccessMode& access, const Category& cat,
                const std::string& id, const std::string& descr = std::string()) :
                addr(address), size(numwords), type(dtype), format(fmt), mode(access), category(cat), identifier(id), description(descr) {}
        };

        /** Constructor; set member variables. */
        SmaModbus(const std::string& peer, const uint16_t port = 502) : SmaModbusLowLevel(peer, port) {}

        /** Destructor; close the tcp connection. */
        ~SmaModbus(void) {}

        /**
         *  Read SMA modbus register.
         *  @param reg the SMA modbus register definition
         *  @return a value object holding the value itself and associated metadata
         */
        SmaModbusValue readRegister(const RegisterDefinition& reg);

        /**
         *  Write SMA modbus register.
         *  @param reg the SMA modbus register definition
         *  @param value the value object holding the value itself and associated metadata
         *  @return true if successful, false otherwise
         */
        bool writeRegister(const RegisterDefinition& reg, const SmaModbusValue& value);
        bool writeRegister(const RegisterDefinition& reg, double value) {
            return writeRegister(reg, SmaModbusValue(value, reg.type, reg.format));
        }

        // Register definitions
        static RegisterDefinition Register30233(void) { static auto reg = RegisterDefinition(30233, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, Category::Normal, "Inverter.WMax", "Nominal active power limit"); return reg; }
        static RegisterDefinition Register40149(void) { static auto reg = RegisterDefinition(40149, 2, DataType::S32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "Inverter.WModCfg.WCtlComCfg.WSpt", "Active power setpoint"); return reg; }
        static RegisterDefinition Register40151(void) { static auto reg = RegisterDefinition(40151, 2, DataType::ENUM, DataFormat::RAW, AccessMode::WO, Category::DeviceControlObject, "Inverter.WModCfg.WCtlComCfg.WCtlComAct", "Eff./reac. power control via communication"); return reg; }
        static RegisterDefinition Register40236(void) { static auto reg = RegisterDefinition(40236, 2, DataType::ENUM, DataFormat::RAW, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.OpMod", "BMS operating mode"); return reg; }
        static RegisterDefinition Register40793(void) { static auto reg = RegisterDefinition(40793, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatChaMinW", "Min. battery charge capac."); return reg; }
        static RegisterDefinition Register40795(void) { static auto reg = RegisterDefinition(40795, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatChaMaxW", "Max. battery charge capac."); return reg; }
        static RegisterDefinition Register40797(void) { static auto reg = RegisterDefinition(40797, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatDschMinW", "Min. battery discharge capac."); return reg; }
        static RegisterDefinition Register40799(void) { static auto reg = RegisterDefinition(40799, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatDschMaxW", "Max. battery discharge capac."); return reg; }
        static RegisterDefinition Register40801(void) { static auto reg = RegisterDefinition(40801, 2, DataType::S32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.GridWSpt", "Mains exch. capac. target setpoint"); return reg; }
        static RegisterDefinition Register44039(void) { static auto reg = RegisterDefinition(44039, 2, DataType::S32, DataFormat::FIX2, AccessMode::WO, Category::DeviceControlObject, "Inverter.WModCfg.WCtlComCfg.WSptMaxNom", "Maximum active power setpoint"); return reg; }
        static RegisterDefinition Register44041(void) { static auto reg = RegisterDefinition(44041, 2, DataType::S32, DataFormat::FIX2, AccessMode::WO, Category::DeviceControlObject, "Inverter.WModCfg.WCtlComCfg.WSptMinNom", "Minimum active power setpoint"); return reg; }
    };

}   // namespace libsmamodbus

#endif
