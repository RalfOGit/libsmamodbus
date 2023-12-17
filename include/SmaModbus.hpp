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
        static std::string toString(const AccessMode& mode);

        /**
         *  Enumeration for additional SMA modbus register related information
         */
        enum class Category : uint8_t {
            Normal                 = 0x00,  //<! no further information
            GridGuardCodeProtected = 0x01,  //<! access requires grid guard login
            DeviceControlObject    = 0x02,  //<! device control object for external power control
            CyclicWritingWarning   = 0x04   //<! cyclic writes will destroy the underlying memory cells
        };
        static std::string toString(const Category& category);

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

            std::string toString(void) const;
        };


        /** Constructor; set member variables. */
        SmaModbus(const std::string& peer, uint16_t port = 502, const SmaModbusUnitID& unit_id  = SmaModbusUnitID::DEVICE_0) : SmaModbusLowLevel(peer, port, unit_id) {}

        /** Destructor; close the tcp connection. */
        ~SmaModbus(void) {}

        /**
         *  Read SMA modbus register.
         *  @param reg the SMA modbus register definition
         *  @return a value object holding the value itself and associated metadata
         */
        SmaModbusValue readRegister(const RegisterDefinition& reg, bool print = false);

        /**
         *  Write SMA modbus register.
         *  @param reg the SMA modbus register definition
         *  @param value the value object holding the value itself and associated metadata
         *  @return true if successful, false otherwise
         */
        bool writeRegister(const RegisterDefinition& reg, const SmaModbusValue& value, bool print = false);
        bool writeRegister(const RegisterDefinition& reg, double value, bool print = false) {
            return writeRegister(reg, SmaModbusValue(value, reg.type, reg.format), print);
        }

        /**
         *  Set the default unit id to be used for readRegister and writeRegister.
         *  The default unit id is choose to be the first map entry of the device map, if it is between 1 and 255
         *  @return the unit id to be used, 0xff in case of a failure to obtain a default unit id
         */
        uint8_t setDefaultUnitID(void);

        /**
         *  Print register value to stdout.
         *  @param reg register definition
         *  @param value register value
         */
        void printRegister(const RegisterDefinition& reg, const SmaModbusValue& value) const;

        // Register definitions as documented in MODBUS-HTML_SBS3.7-6.0-10_GG10-V13
        static RegisterDefinition Register30001(void) { static auto reg = RegisterDefinition(30001, 2, DataType::U32, DataFormat::RAW, AccessMode::RO, Category::Normal, "Modbus.Profile", "Modbus profile"); return reg; }
        static RegisterDefinition Register30003(void) { static auto reg = RegisterDefinition(30003, 2, DataType::U32, DataFormat::RAW, AccessMode::RO, Category::Normal, "Nameplate.SusyId", "Nameplate susy id"); return reg; }
        static RegisterDefinition Register30005(void) { static auto reg = RegisterDefinition(30005, 2, DataType::U32, DataFormat::RAW, AccessMode::RO, Category::Normal, "Nameplate.SerNum", "Nameplate serial number"); return reg; }
        static RegisterDefinition Register30051(void) { static auto reg = RegisterDefinition(30051, 2, DataType::ENUM, DataFormat::RAW, AccessMode::RO, Category::Normal, "Nameplate.MainModel", "Nameplate device class"); return reg; }
        static RegisterDefinition Register30053(void) { static auto reg = RegisterDefinition(30053, 2, DataType::ENUM, DataFormat::RAW, AccessMode::RO, Category::Normal, "Nameplate.Model", "Nameplate model"); return reg; }
        static RegisterDefinition Register30059(void) { static auto reg = RegisterDefinition(30059, 2, DataType::U32, DataFormat::FIRMWARE, AccessMode::RO, Category::Normal, "Nameplate.PkgRev", "Nameplate package revision"); return reg; }
        static RegisterDefinition Register30193(void) { static auto reg = RegisterDefinition(30193, 2, DataType::U32, DataFormat::DATETIME, AccessMode::RO, Category::Normal, "DtTm.Tm", "UTC system time"); return reg; }
        static RegisterDefinition Register30233(void) { static auto reg = RegisterDefinition(30233, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, Category::Normal, "Inverter.WMax", "Nominal active power limit"); return reg; }

        static RegisterDefinition Register30843(void) { static auto reg = RegisterDefinition(30843, 2, DataType::S32, DataFormat::FIX3, AccessMode::RO, Category::Normal, "Bat.Amp", "Battery current"); return reg; }
        static RegisterDefinition Register30845(void) { static auto reg = RegisterDefinition(30845, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, Category::Normal, "Bat.ChaStt", "Current battery state of charge"); return reg; }
        static RegisterDefinition Register30847(void) { static auto reg = RegisterDefinition(30847, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, Category::Normal, "Bat.Diag.ActlCapacNom", "Current battery capacity"); return reg; }
        static RegisterDefinition Register30857(void) { static auto reg = RegisterDefinition(30857, 2, DataType::S32, DataFormat::FIX0, AccessMode::RO, Category::Normal, "Bat.Diag.CapacThrpCnt", "Number of battery charge throughputs"); return reg; }
        static RegisterDefinition Register30955(void) { static auto reg = RegisterDefinition(30955, 2, DataType::ENUM, DataFormat::RAW, AccessMode::RO, Category::Normal, "Bat.OpStt", "Battery oper. status"); return reg; }

        static RegisterDefinition Register30865(void) { static auto reg = RegisterDefinition(30865, 2, DataType::S32, DataFormat::FIX0, AccessMode::RO, Category::Normal, "Metering.GridMs.W.TotIn", "Grid metering total watts import"); return reg; }
        static RegisterDefinition Register30867(void) { static auto reg = RegisterDefinition(30867, 2, DataType::S32, DataFormat::FIX0, AccessMode::RO, Category::Normal, "Metering.GridMs.W.TotOut", "Grid etering total watts export"); return reg; }
        static RegisterDefinition Register31259(void) { static auto reg = RegisterDefinition(31259, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, Category::DeviceControlObject, "Metering.GridMs.W.phsA", "Grid metering watts export phase A"); return reg; }
        static RegisterDefinition Register31261(void) { static auto reg = RegisterDefinition(31261, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, Category::DeviceControlObject, "Metering.GridMs.W.phsB", "Grid metering watts export phase B"); return reg; }
        static RegisterDefinition Register31263(void) { static auto reg = RegisterDefinition(31263, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, Category::DeviceControlObject, "Metering.GridMs.W.phsC", "Grid metering watts export phase C"); return reg; }
        static RegisterDefinition Register31265(void) { static auto reg = RegisterDefinition(31265, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, Category::DeviceControlObject, "Metering.GridMs.WIn.phsA", "Grid metering watts import phase A"); return reg; }
        static RegisterDefinition Register31267(void) { static auto reg = RegisterDefinition(31267, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, Category::DeviceControlObject, "Metering.GridMs.WIn.phsB", "Grid metering watts import phase B"); return reg; }
        static RegisterDefinition Register31269(void) { static auto reg = RegisterDefinition(31269, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, Category::DeviceControlObject, "Metering.GridMs.WIn.phsC", "Grid metering watts import phase C"); return reg; }

        static RegisterDefinition Register40149(void) { static auto reg = RegisterDefinition(40149, 2, DataType::S32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "Inverter.WModCfg.WCtlComCfg.WSpt", "Active power setpoint"); return reg; }
        static RegisterDefinition Register40151(void) { static auto reg = RegisterDefinition(40151, 2, DataType::ENUM, DataFormat::RAW, AccessMode::WO, Category::DeviceControlObject, "Inverter.WModCfg.WCtlComCfg.WCtlComAct", "Eff./reac. power control via communication"); return reg; }
        static RegisterDefinition Register40153(void) { static auto reg = RegisterDefinition(40153, 2, DataType::S32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "Inverter.WModCfg.WCtlComCfg.VarSpt", "Reactive power setpoint"); return reg; }

        static RegisterDefinition Register40236(void) { static auto reg = RegisterDefinition(40236, 2, DataType::ENUM, DataFormat::RAW, AccessMode::RW, Category::DeviceControlObject, "CmpBMS.OpMod", "BMS operating mode"); return reg; }
#if 0
        static RegisterDefinition Register40793(void) { static auto reg = RegisterDefinition(40793, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatChaMinW", "Min. battery charge capac."); return reg; }
        static RegisterDefinition Register40795(void) { static auto reg = RegisterDefinition(40795, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatChaMaxW", "Max. battery charge capac."); return reg; }
        static RegisterDefinition Register40797(void) { static auto reg = RegisterDefinition(40797, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatDschMinW", "Min. battery discharge capac."); return reg; }
        static RegisterDefinition Register40799(void) { static auto reg = RegisterDefinition(40799, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatDschMaxW", "Max. battery discharge capac."); return reg; }
        static RegisterDefinition Register40801(void) { static auto reg = RegisterDefinition(40801, 2, DataType::S32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.GridWSpt", "Mains exch. capac. target setpoint"); return reg; }
#else
        static RegisterDefinition Register40793(void) { static auto reg = RegisterDefinition(44431, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatChaMinW", "Min. battery charge capac."); return reg; }
        static RegisterDefinition Register40795(void) { static auto reg = RegisterDefinition(44433, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatChaMaxW", "Max. battery charge capac."); return reg; }
        static RegisterDefinition Register40797(void) { static auto reg = RegisterDefinition(44435, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatDschMinW", "Min. battery discharge capac."); return reg; }
        static RegisterDefinition Register40799(void) { static auto reg = RegisterDefinition(44437, 2, DataType::U32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.BatDschMaxW", "Max. battery discharge capac."); return reg; }
        static RegisterDefinition Register40801(void) { static auto reg = RegisterDefinition(44439, 2, DataType::S32, DataFormat::FIX0, AccessMode::WO, Category::DeviceControlObject, "CmpBMS.GridWSpt", "Mains exch. capac. target setpoint"); return reg; }
#endif
        static RegisterDefinition Register44039(void) { static auto reg = RegisterDefinition(44039, 2, DataType::S32, DataFormat::FIX2, AccessMode::WO, Category::DeviceControlObject, "Inverter.WModCfg.WCtlComCfg.WSptMaxNom", "Maximum active power setpoint"); return reg; }
        static RegisterDefinition Register44041(void) { static auto reg = RegisterDefinition(44041, 2, DataType::S32, DataFormat::FIX2, AccessMode::WO, Category::DeviceControlObject, "Inverter.WModCfg.WCtlComCfg.WSptMinNom", "Minimum active power setpoint"); return reg; }

        /**
         *  Class encapsulating a device entry available from the unit id device assignment
         */
        class SmaModbusDeviceEntry {
        public:
            uint16_t susyID;
            uint32_t serialNumber;
            uint16_t unitID;
            SmaModbusDeviceEntry(uint16_t susy_id, uint32_t serial, uint16_t unit_id) : susyID(susy_id), serialNumber(serial), unitID(unit_id) {}
        };

        /**
         *  Read map of unit id device assignment.
         *  This likely returns two entries:
         *  - SMA device:     susyID,   serialNumber,   UnitID 3
         *  - Sunspec device: susyID 0, serialNumber 1, UnitID 126
         *  @return a value object holding the value itself and associated metadata
         */
        std::vector<SmaModbusDeviceEntry> getDeviceMap(void);
    };

}   // namespace libsmamodbus

#endif
