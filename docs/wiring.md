# Wiring

## IMU
VCC -> ESP32 3.3V
GND -> GND
SDA -> GPIO21
SCL -> GPIO22

## OLED
VCC/VDD -> 3.3V
GND -> GND
SDA -> GPIO21
SCL -> GPIO22

## SW-420 on Node 1
VCC -> 3.3V
GND -> GND
DO -> GPIO27

IMU and OLED share the I2C bus.
