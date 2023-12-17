#ifndef __SMAMODBUSAPI_HPP__
#define __SMAMODBUSAPI_HPP__

#include <cstdint>
#include <string>
#include <SmaModbus.hpp>

namespace libsmamodbus {

    class SmaModbusApi : public SmaModbus {

    public:
        /** constructor */
        SmaModbusApi(const std::string& peer, uint16_t port, const SmaModbusUnitID& unit_id) : SmaModbus(peer, port, unit_id) {}

        /** destructor */
        ~SmaModbusApi(void) {}

        /**
         * Start self-consumption mode (SelfCsmp).
         * i.e. charging/discharging is controlled by the power sign at the grid connection point
         * the mode switch  can be verified by event messages in the UI
         */
        bool setSelfConsumptionMode(void) {
            bool result1 = writeRegister(Register40151(), 803);     // set external power control to inactive (803)
            //bool result2 = writeRegister(Register40236(), 1438);    // set bms operation mode to automatic (1438)
            return result1/* && result2*/;
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
            //if (watts < 0) {    // force charge
            //    result &= writeRegister(Register40236(), 2289);     // set bms operation mode to charge (2289)
            //    result &= setBatteryPowerRange(-watts, -watts, 0.0, 0.0);
            //}
            //else {              // force discharge
            //    result &= writeRegister(Register40236(), 2290);     // set bms operation mode to discharge (2290)
            //    result &= setBatteryPowerRange(0.0, 0.0, watts, watts);
            //}
            return result;
        }

        bool setBatteryPowerRange(double min_charge_watts, double max_charge_watts, double min_discharge_watts, double max_discharge_watts) {
            bool result = true;
            result &= writeRegister(Register40793(), min_charge_watts);     // set minimum battery charging power in watts
            result &= writeRegister(Register40795(), max_charge_watts);     // set maximum battery charging power in watts
            result &= writeRegister(Register40797(), min_discharge_watts);  // set minimum battery discharging power in watts
            result &= writeRegister(Register40799(), max_discharge_watts);  // set maximum battery discharging power in watts
            return result;
        }


        /**
         * Set the power range for charge/discharge in percent of the nominal power.
         * This setting is applied to both modes (self-consumption or externally controlled).
         * - a positive value means discharge
         * - a negative value means charge
         * - if both registers are written with the same value, this sets an exact power value
         * Examples:
         * - minPercent: -10, maxPercent: 15 => power range from 10% charge to 15% discharge
         * - minPercent: -10, maxPercent:  0 => power range from 10% charge to  0% charge; no discharge
         * - minPercent: -10, maxPercent:-10 => exact power setting at 10% charge
         * - minPercent:  15, maxPercent: 15 => exact power setting at 15% discharge
         * @param minPercent minimum power in percent of the nominal power
         * @param maxPercent maximum power in percent of the nominal power
         * @return true if successful, false otherwise
         */
        bool setPowerRangeInPercent(double minPercent, double maxPercent) {
            bool result1 = writeRegister(Register44039(), maxPercent);  // set maximum power range in percent
            bool result2 = writeRegister(Register44041(), minPercent);  // set minimum power range in percent
            return result1 && result2;
        }

        /**
         * Set the power range for charge/discharge in watts.
         * @see setPowerRangeInPercent
         * @param minPower minimum power in watts
         * @param maxPower maximum power in watts
         * @return true if successful, false otherwise
         */
        bool setPowerRangeInWatts(double minPower, double maxPower) {
            double nominal_power = getNominalPower();
            if (SmaModbusValue::isNaN(nominal_power) == false) {
                return setPowerRangeInPercent(100.0 * minPower / nominal_power, 100.0 * maxPower / nominal_power);
            }
            return false;
        }

        /**
         * Get nominal power value of the inverter.
         * @return power in watts
         */
        double getNominalPower(void) {
            SmaModbusValue value = readRegister(Register30233());   // get inverter nominal power in watts
            if (value.isValid()) {
                return double(value);
            }
            return SmaModbusValue::Double_NaN;
        }

        /**
         * Get total power value at grid connection point.
         * @return power in watts; >0 means power import, <0 means power export
         */
        double getGridPowerInWatts(void) {
            int32_t positiveActivePowerTotal = readRegister(SmaModbus::Register30865());
            int32_t negativeActivePowerTotal = readRegister(SmaModbus::Register30867());
            return positiveActivePowerTotal - negativeActivePowerTotal;
        }
    };

}   // namespace libsmamodbus

#endif
