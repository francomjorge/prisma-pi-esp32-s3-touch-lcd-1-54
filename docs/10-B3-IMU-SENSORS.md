# B3 — IMU / Sensors

Status: IMPLEMENTED — CI and physical validation pending.

## Scope
- QMI8658 on the existing shared I2C bus.
- WHO_AM_I validation before enabling the driver.
- Accelerometer and gyroscope acquisition.
- Temperature acquisition.
- Normalized engineering units: g, deg/s and deg C.
- B0/B1/B2 regression remains intact.

## Configuration
- I2C address: board BSP `PRISMA_IMU_ADDR` (0x6B).
- Accelerometer: +/-4 g, 125 Hz.
- Gyroscope: +/-512 deg/s, 125 Hz.
- INT pin remains reserved in BSP; polling is used in this build.

## Acceptance
1. ESP-IDF 5.5.1 build passes.
2. QMI8658 WHO_AM_I is 0x05.
3. Stationary board reports acceleration magnitude near 1 g.
4. Rotation produces coherent gyro response on the corresponding axes.
5. Repeated reads do not disturb touch/audio on the shared system.
6. No watchdog, panic or unexplained reset during a 30-minute sensor/UI/audio regression run.

## Deferred
- Interrupt-driven motion events.
- Calibration persistence.
- Gesture classification.
- Sensor fusion/orientation estimation.
These can be added later on top of the B3 raw sensor service without changing the BSP contract.
