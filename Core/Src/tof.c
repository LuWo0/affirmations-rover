#include "VL53L1X_api.h"

void tof_init(void) {
	if (VL53L1X_SensorInit(0x29) != 0) {
		return;
	}
	VL53L1X_SetDistanceMode(0x29, 2);
	VL53L1X_StartRanging(0x29);
}
