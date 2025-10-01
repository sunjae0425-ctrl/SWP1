// Arduino pin assignment
#define PIN_LED  9
#define PIN_TRIG 12   // sonar sensor TRIGGER
#define PIN_ECHO 13   // sonar sensor ECHO

// configurable parameters
#define SND_VEL 346.0     // sound velocity at 24 celsius degree (unit: m/sec)
#define INTERVAL 25       // sampling interval (unit: msec)
#define PULSE_DURATION 10 // ultra-sound Pulse Duration (unit: usec)
#define _DIST_MIN 100.0   // minimum distance to be measured (unit: mm)
#define _DIST_MAX 300.0   // maximum distance to be measured (unit: mm)

#define TIMEOUT ((INTERVAL / 2) * 1000.0) // maximum echo waiting time (unit: usec)
#define SCALE (0.001 * 0.5 * SND_VEL)     // coefficent to convert duration to distance

unsigned long last_sampling_time;   // unit: msec

void setup() {
  pinMode(PIN_LED, OUTPUT);
  pinMode(PIN_TRIG, OUTPUT);
  pinMode(PIN_ECHO, INPUT);
  Serial.begin(57600);
  last_sampling_time = millis();
}

void loop() {
  unsigned long time = millis();
  if (time < last_sampling_time + INTERVAL) return;
  last_sampling_time += INTERVAL;

  // trigger sonar pulse
  digitalWrite(PIN_TRIG, HIGH);
  delayMicroseconds(PULSE_DURATION);
  digitalWrite(PIN_TRIG, LOW);

  // measure echo duration
  unsigned long duration = pulseIn(PIN_ECHO, HIGH, TIMEOUT);
  float distance = duration * SCALE;  // distance in mm

  // 거리 값 출력 (Serial Plotter용)
  Serial.println(distance);

  // 거리 기반 LED 밝기 제어 (삼각형 패턴)
  int brightness;
  if (distance <= _DIST_MIN || distance >= _DIST_MAX) {
    brightness = 255;  // 꺼짐
  } else if (distance == 200) {
    brightness = 0;    // 최대 밝기
  } else if (distance < 200) {
    brightness = map(distance, 100, 200, 255, 0); // 100→255(꺼짐), 200→0(최대밝기)
  } else {
    brightness = map(distance, 200, 300, 0, 255); // 200→0, 300→255
  }

  analogWrite(PIN_LED, brightness);
}
