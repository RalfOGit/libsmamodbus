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
            bool result1 = writeRegister(ExternalPowerControl(), 802);      //  802: active (i.e. self-consumption becomes deactivated)
            bool result2 = writeRegister(ExternalPowerInWatts(), watts);    // negative means charging, positive means discharging
            bool result3 = writeRegister(BMSOperationMode(), 2289);         // 2289: charge
            // FIXME: NEED TO SET OTHER REGISTERS DEPENDING ON SIGN OF WATTS
            bool result4 = writeRegister(BatteryChargeMaxInWatts(), abs(watts)); // unsigned value
            return result1 && result2 && result3 && result4;
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
            SmaModbusValue value = readRegister(InverterNominalPower());
            if (value.isValid()) {
                double nominal_power = double(value);
                bool result = setPowerRangeInPercent(100.0 * minPower / nominal_power, 100.0 * maxPower / nominal_power);
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
