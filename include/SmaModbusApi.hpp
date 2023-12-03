#ifndef __SMAMODBUSAPI_HPP__
#define __SMAMODBUSAPI_HPP__

#include <cstdint>
#include <string>
#include <SmaModbus.hpp>

namespace libsmamodbus {

    class SmaModbusApi : public SmaModbus {

    public:
        /** constructor */
        SmaModbusApi(const std::string& peer, const uint16_t port) : SmaModbus(peer, port) {}

        /** destructor */
        ~SmaModbusApi(void) {}

        /**
         * Start self-consumption mode (SelfCsmp).
         * i.e. charging/discharging is controlled by the power sign at the grid connection point
         * the mode switch  can be verified by event messages in the UI
         */
        bool setSelfConsumptionMode(void) {
            bool result1 = writeRegister(ExternalPowerControl(), 803);   //  803: inactive
            bool result2 = writeRegister(BMSOperationMode(), 1438);      // 1438: automatic
            return result1 && result2;
        }

        /**
         * Start external power control mode, where charging / discharging is controlled by the given power value.
         * the mode switch can be verified by event messages in the UI
         * @param power power value in watts - negative means charging, positive means discharging
         */
        bool setExternalPowerControlMode(double watts) {
            writeRegister(ExternalPowerControl(), 802);      //  802: active (i.e. self-consumption becomes deactivated)
            writeRegister(ExternalPowerInWatts(), watts);    // negative means charging, positive means discharging
            writeRegister(BMSOperationMode(), 2289);         // 2289: charge
            // FIXME: NEED TO SET OTHER REGISTERS DEPENDING ON SIGN OF WATTS
            writeRegister(BatteryChargeMaxInWatts(), watts); // unsigned value
        }

        /**
         * Set the power range for charge/discharge; this setting is applied to both modes (self-consumption or externally controlled)
         * - a positive power value means discharge
         * - a negative power value means charge
         * - if both registers are written with the same value, this sets an exact power value
         */
        bool setPowerRangeInPercent(double minPercent, double maxPercent) {
            bool result1 = writeRegister(PowerRangeMinInPercent(), minPercent);
            bool result2 = writeRegister(PowerRangeMaxInPercent(), maxPercent);
            return result1 && result2;
        }

        bool setPowerRangeInWatts(double minPower, double maxPower) {
            double nominal_power = readRegister(InverterNominalPower());
            if (nominal_power < 1000000) {
                bool result = setPowerRangeInPercent(minPower / nominal_power, maxPower / nominal_power);
                return result;
            }
            return false;
        }

        double getNominalPower(void) {
            return 2500.00;
        }
    };

}   // namespace libsmamodbus

#endif
