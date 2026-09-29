# Gateway Protocol

The gateway ESP32 participates in the mesh and forwards telemetry to the Pi over USB serial.

Mesh packet:
`N=1,TX=2.40,TY=-0.80,V=0.012,SW=0`

Pi-facing packet:
`DATA|N=1,TX=2.40,TY=-0.80,V=0.012,SW=0`

The Pi ignores other ESP32 debug output.
