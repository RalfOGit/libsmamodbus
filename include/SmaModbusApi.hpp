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
            bool result1 = writeRegister(Register40151(), 803);     // set external power control to inactive (803)
            bool result2 = writeRegister(Register40236(), 1438);    // set bms operation mode to automatic (1438)
            return result1 && result2;
        }

        /**
         * Start external power control mode, where charging / discharging is controlled by the given power value.
         * the mode switch can be verified by event messages in the UI
         * @param power power value in watts - negative means charging, positive means discharging
         */
        bool setExternalPowerControlMode(double watts) {
            bool result = true;
            result &= writeRegister(Register40151(), 802);          // set external power control to active (802), i.e. self-consumption becomes deactivated
            result &= writeRegister(Register40149(), watts);        // set external power in watts, negative means charging, positive means discharging
            if (watts < 0) {    // force charge
                result &= writeRegister(Register40236(), 2289);     // set bms operation mode to charge (2289)
                result &= writeRegister(Register40793(), -watts);   // set minimum battery charging power in watts
                result &= writeRegister(Register40795(), -watts);   // set maximum battery charging power in watts
                result &= writeRegister(Register40797(), 0.0);      // set minimum battery discharging power in watts
                result &= writeRegister(Register40799(), 0.0);      // set maximum battery discharging power in watts
            }
            else {              // force discharge
                result &= writeRegister(Register40236(), 2290);     // set bms operation mode to discharge (2290)
                result &= writeRegister(Register40793(), 0.0);      // set minimum battery charging power in watts
                result &= writeRegister(Register40795(), 0.0);      // set maximum battery charging power in watts
                result &= writeRegister(Register40797(), watts);    // set minimum battery discharging power in watts
                result &= writeRegister(Register40799(), watts);    // set maximum battery discharging power in watts
            }
            return result;
        }

        /**
         * Set the power range for charge/discharge; this setting is applied to both modes (self-consumption or externally controlled)
         * - a positive power value means discharge
         * - a negative power value means charge
         * - if both registers are written with the same value, this sets an exact power value
         */
        bool setPowerRangeInPercent(double minPercent, double maxPercent) {
            bool result1 = writeRegister(Register44039(), maxPercent);  // set maximum power range in percent
            bool result2 = writeRegister(Register44041(), minPercent);  // set minimum power range in percent
            return result1 && result2;
        }

        bool setPowerRangeInWatts(double minPower, double maxPower) {
            double nominal_power = getNominalPower();
            if (SmaModbusValue::isNaN(nominal_power) == false) {
                bool result = setPowerRangeInPercent(100.0 * minPower / nominal_power, 100.0 * maxPower / nominal_power);
                return result;
            }
            return false;
        }

        double getNominalPower(void) {
            SmaModbusValue value = readRegister(Register30233());   // get inverter nominal power in watts
            if (value.isValid()) {
                return double(value);
            }
            return SmaModbusValue::Double_NaN;
        }

    };

}   // namespace libsmamodbus

#endif
