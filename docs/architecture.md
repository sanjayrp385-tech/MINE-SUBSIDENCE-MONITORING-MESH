# Architecture

1. Distributed sensing: five ESP32 nodes measure local inclination and motion.
2. Wireless mesh: painlessMesh provides multi-hop communication.
3. Gateway: Node 5 forwards mesh packets to the Raspberry Pi over USB.
4. Edge processing: the Pi parses packets and maintains current node state.
5. Dashboard: Flask provides local monitoring.

Intended advanced analysis:

raw sensor data -> filtering/calibration -> node anomaly -> neighbour comparison -> spatial correlation -> risk-zone indication.

The prototype does not claim to predict mine collapse.
