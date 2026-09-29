# Mine Subsidence Monitoring Mesh

Proof-of-concept distributed wireless surface sensor network for abnormal ground-deformation monitoring.

## Architecture

ESP32 sensor nodes -> painlessMesh Wi-Fi mesh -> ESP32 gateway -> USB serial -> Raspberry Pi 4 -> Flask dashboard.

The prototype uses five ESP32 nodes, MPU-compatible IMUs, an optional SW-420 vibration sensor, and optional SSD1306 I2C OLEDs.

## Software versions used in the prototype

- ESP32 Arduino core: 3.0.7
- painlessMesh: 1.5.3
- Adafruit GFX
- Adafruit SSD1306
- Python 3
- Flask
- pyserial

## Node packet

`N=1,TX=2.40,TY=-0.80,V=0.012,SW=0`

`N` node ID, `TX/TY` tilt estimates, `V` acceleration-magnitude deviation, `SW` vibration-event state.

Gateway sends the Pi:

`DATA|N=1,TX=2.40,TY=-0.80,V=0.012,SW=0`

## Wiring

IMU: VCC->3.3V, GND->GND, SDA->GPIO21, SCL->GPIO22.

OLED: VCC->3.3V, GND->GND, SDA->GPIO21, SCL->GPIO22.

Node 1 SW-420: VCC->3.3V, GND->GND, DO->GPIO27.

## Node setup

Use the same firmware and change:

`#define NODE_NUMBER 1`

Node 1:

`#define HAS_SW420 true`

Nodes 2-5:

`#define HAS_SW420 false`

Gateway Node 5:

`#define NODE_NUMBER 5`
`#define IS_GATEWAY true`

## Raspberry Pi

Check serial:

`ls /dev/ttyACM*`
`ls /dev/ttyUSB*`

The prototype used `/dev/ttyACM0`.

Install:

`sudo apt install python3-flask python3-serial`

Run:

`cd dashboard`
`python3 app.py`

Find Pi IP:

`hostname -I`

Open:

`http://PI_IP:5000`

## GitHub

`git init`
`git add .`
`git commit -m "Initial mine subsidence mesh prototype"`
`git branch -M main`
`git remote add origin https://github.com/YOUR_USERNAME/mine-subsidence-mesh.git`
`git push -u origin main`

## Important limitation

This is a competition proof-of-concept, not a certified mine-safety system. Thresholds are demonstration values. The system indicates abnormal deformation patterns; it does not claim to predict mine collapse.
