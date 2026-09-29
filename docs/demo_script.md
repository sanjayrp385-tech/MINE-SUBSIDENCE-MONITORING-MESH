# Technical Demo Script

Our system uses five distributed ESP32 sensor nodes. Each node performs local sensing using an MPU-compatible inertial measurement unit. Accelerometer data is used to estimate X and Y inclination, while acceleration-magnitude deviation provides a simple motion and vibration indicator.

Node 1 additionally uses an SW-420 vibration sensor as a digital event trigger.

Each node creates a telemetry packet containing its node ID and sensor values and transmits it through an ESP32 Wi-Fi mesh implemented using painlessMesh.

Node 5 acts as the gateway. It receives mesh packets and forwards them through USB serial to a Raspberry Pi 4.

The Raspberry Pi performs local edge processing and hosts the monitoring dashboard. The core monitoring path is internet-independent.

The key concept is spatial monitoring: neighbouring node behaviour can be analysed together rather than treating each measurement as an isolated sensor alarm.

The prototype therefore provides an early-warning indication of abnormal deformation patterns rather than claiming to predict mine collapse.
