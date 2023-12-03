#ifndef __SMAMODBUS_HPP__
#define __SMAMODBUS_HPP__

#include <cstdint>
#include <string>
#include <SmaModbusLowLevel.hpp>
#include <SmaModbusValue.hpp>

namespace libsmamodbus {


    /**
     *  Class implementing access to sma modbus registers.
     */
    class SmaModbus : public SmaModbusLowLevel {

    public:

        /**
         *  Enumeration of data access modes used in SMA modbus registers.
         */
        enum class AccessMode : uint8_t {
            RO = 0x01,
            WO = 0x02,
            RW = 0x03
        };

        /**
         *  Class encapsulating all relevant information for a given SMA modbus registers.
         */
        class RegisterDefinition {
        public:
            uint16_t addr;
            uint16_t size;
            DataType type;
            DataFormat format;
            AccessMode mode;
            std::string identifier;
            std::string description;

            RegisterDefinition(const std::string& id, uint16_t address, uint16_t numwords, const DataType& dtype,
                const DataFormat& fmt, const AccessMode& access, const std::string& descr = std::string()) :
                identifier(id), addr(address), size(numwords), type(dtype),
                format(fmt), mode(access), description(descr) {}
        };

        /** constructor */
        SmaModbus(const std::string& peer, const uint16_t port = 502) : SmaModbusLowLevel(peer, port) {}

        /** destructor */
        ~SmaModbus(void) {}

        // register read methods
        SmaModbusValue readRegister(const RegisterDefinition& reg);

        // register write methods
        bool writeRegister(const RegisterDefinition& reg, const SmaModbusValue& value);

        /**
         * Register 40149
         * - power value taken into account when external power control is activated
         * - a positive value means discharge; a negative value means charge
         */
        static RegisterDefinition ExternalPowerInWatts(void) { static auto reg = RegisterDefinition("Inv.Ext.W", 40149, 2, DataType::S32, DataFormat::FIX0, AccessMode::RW, ""); return reg; }

        /**
         * Register 40151
         * - activate or deactivate external power control
         * - if activated (802), external power values can be applied
         * - if deactivated (803), self-consumption mode is activated; this is the normal operating mode
         */
        static RegisterDefinition ExternalPowerControl(void) { static auto reg = RegisterDefinition("Inv.Ext.Ctrl", 40151, 2, DataType::ENUM, DataFormat::RAW, AccessMode::RW, ""); return reg; }

        /**
         * Register 44039
         * - controls the power range for charge/discharge, independent on the mode (self-consumption or externally controlled)
         * - register 44039 controls the maximum value of the power range; a positive value means discharge; a negative value means charge
         */
        static RegisterDefinition PowerRangeMaxInPercent(void) { static auto reg = RegisterDefinition("Inv.MaxW.Prc", 44039, 2, DataType::S32, DataFormat::FIX2, AccessMode::WO, ""); return reg; }

        /**
         * Register 44041
         * - controls the power range for charge/discharge, independent on the mode (self-consumption or externally controlled)
         * - register 44041 controls the minmum value of the power range; a positive value means discharge; a negative value means charge
         */
        static RegisterDefinition PowerRangeMinInPercent(void) { static auto reg = RegisterDefinition("Inv.MinW.Prc", 44041, 2, DataType::S32, DataFormat::FIX2, AccessMode::WO, ""); return reg; }

        /**
         * Register 40236
         * - set the BMS operation mode (303: Off, 308: On, 1438: Auto, 2289: Charge, 2290: Discharge, 2424: Default)
         */
        static RegisterDefinition BMSOperationMode(void) { static auto reg = RegisterDefinition("CmpBMS.OpMod", 40236, 2, DataType::ENUM, DataFormat::RAW, AccessMode::RW, ""); return reg; }

        /**
         * Register 40795
         * - set the maximum battery charge power in watts
         */
        static RegisterDefinition BatteryChargeMaxInWatts(void) { static auto reg = RegisterDefinition("batChaMaxW", 40795, 2, DataType::U32, DataFormat::FIX0, AccessMode::RW, ""); return reg; }

        /**
         * Register 30233
         * - get the nominal power rating of the inverter in watts
         */
        static RegisterDefinition InverterNominalPower(void) { static auto reg = RegisterDefinition("invWLim", 30233, 2, DataType::U32, DataFormat::FIX0, AccessMode::RO, ""); return reg; }
    };

}   // namespace libsmamodbus

#endif
