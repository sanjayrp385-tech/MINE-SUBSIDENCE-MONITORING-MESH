#include <Arduino.h>
#include <Wire.h>
#include <painlessMesh.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// CHANGE THESE FOR EACH BOARD
#define NODE_NUMBER 1
#define HAS_SW420 true
#define IS_GATEWAY false

#define MESH_PREFIX "MINE_MESH"
#define MESH_PASSWORD "Mine12345"
#define MESH_PORT 5555

#define SDA_PIN 21
#define SCL_PIN 22
#define MPU_ADDR 0x68
#define SW420_PIN 27
#define OLED_ADDR 0x3C
#define SEND_INTERVAL_MS 2000

Scheduler userScheduler;
painlessMesh mesh;
Adafruit_SSD1306 display(128, 64, &Wire, -1);

bool mpuOK = false;
bool oledOK = false;
float tiltX = 0, tiltY = 0, vibration = 0;
int swState = 0;
unsigned long lastSend = 0;

bool readReg(uint8_t reg, uint8_t *buf, uint8_t len) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  if (Wire.endTransmission(false) != 0) return false;
  if (Wire.requestFrom(MPU_ADDR, len, true) != len) return false;
  for (uint8_t i=0;i<len;i++) buf[i]=Wire.read();
  return true;
}

bool writeReg(uint8_t reg, uint8_t val) {
  Wire.beginTransmission(MPU_ADDR);
  Wire.write(reg);
  Wire.write(val);
  return Wire.endTransmission() == 0;
}

bool initMPU() {
  uint8_t id;
  if (!readReg(0x75, &id, 1)) return false;

  Serial.printf("MPU WHO_AM_I = 0x%02X\n", id);

  // Prototype accepts MPU6050-class 0x68 and MPU6500-class 0x70 modules.
  if (id != 0x68 && id != 0x70) return false;

  if (!writeReg(0x6B, 0x00)) return false;
  writeReg(0x19, 7);       // sample divider
  writeReg(0x1A, 3);       // DLPF
  writeReg(0x1B, 0x08);    // gyro +/-500 dps
  writeReg(0x1C, 0x10);    // accel +/-8g
  delay(100);
  return true;
}

bool readIMU() {
  uint8_t d[14];
  if (!readReg(0x3B, d, 14)) return false;

  int16_t ax = (d[0]<<8)|d[1];
  int16_t ay = (d[2]<<8)|d[3];
  int16_t az = (d[4]<<8)|d[5];

  float x=ax/4096.0f, y=ay/4096.0f, z=az/4096.0f;

  tiltX = atan2(y, sqrt(x*x+z*z))*180.0f/PI;
  tiltY = atan2(-x, sqrt(y*y+z*z))*180.0f/PI;

  float mag=sqrt(x*x+y*y+z*z);
  vibration=fabs(mag-1.0f);
  return true;
}

String packet() {
  return "N="+String(NODE_NUMBER)+
         ",TX="+String(tiltX,2)+
         ",TY="+String(tiltY,2)+
         ",V="+String(vibration,3)+
         ",SW="+String(swState);
}

void receivedCallback(uint32_t from, String &msg) {
  Serial.printf("RX from %u: %s\n", from, msg.c_str());

  if (IS_GATEWAY) {
    Serial.print("DATA|");
    Serial.println(msg);
  }
}

void newConnectionCallback(uint32_t id) {
  Serial.printf("NEW NODE: %u\n", id);
}

void changedConnectionCallback() {
  Serial.println("CONNECTION CHANGED");
}

void showOLED(const char *status) {
  if (!oledOK) return;

  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.printf("MINE N%d", NODE_NUMBER);
  display.setCursor(0,15);
  display.printf("TX:%.1f TY:%.1f", tiltX, tiltY);
  display.setCursor(0,30);
  display.printf("V:%.3f SW:%d", vibration, swState);
  display.setCursor(0,45);
  display.print(status);
  display.display();
}

void setup() {
  Serial.begin(115200);
  delay(1000);

  Wire.begin(SDA_PIN, SCL_PIN);

  if (HAS_SW420) pinMode(SW420_PIN, INPUT);

  oledOK = display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDR);
  mpuOK = initMPU();

  Serial.println("================================");
  Serial.println("MINE SUBSIDENCE SENSOR NODE");
  Serial.printf("NODE %d\n", NODE_NUMBER);
  Serial.printf("MPU: %s\n", mpuOK ? "OK" : "FAILED");
  Serial.printf("GATEWAY: %s\n", IS_GATEWAY ? "YES" : "NO");

  mesh.setDebugMsgTypes(ERROR | STARTUP | CONNECTION);
  mesh.init(MESH_PREFIX, MESH_PASSWORD, &userScheduler, MESH_PORT);
  mesh.onReceive(&receivedCallback);
  mesh.onNewConnection(&newConnectionCallback);
  mesh.onChangedConnections(&changedConnectionCallback);

  Serial.println("MESH READY");
}

void loop() {
  mesh.update();

  if (mpuOK) readIMU();
  swState = HAS_SW420 ? digitalRead(SW420_PIN) : 0;

  if (millis()-lastSend >= SEND_INTERVAL_MS) {
    lastSend=millis();
    String p=packet();

    bool ok=mesh.sendBroadcast(p);

    Serial.print("TX: ");
    Serial.print(p);
    Serial.print(" -> ");
    Serial.println(ok ? "OK" : "FAILED");

    // Gateway also sends its own local measurement to the Pi.
    if (IS_GATEWAY) {
      Serial.print("DATA|");
      Serial.println(p);
    }

    showOLED(ok ? "TX OK" : "TX FAILED");
  }
}
