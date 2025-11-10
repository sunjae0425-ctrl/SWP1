#include <Servo.h>

// Arduino pin assignment
#define PIN_IR    0         // IR sensor at Pin A0
#define PIN_LED   9
#define PIN_SERVO 10

#define _DUTY_MIN 1000      // 0 degree
#define _DUTY_NEU 1500      // 90 degree
#define _DUTY_MAX 2000      // 180 degree

#define _DIST_MIN 100.0     // 10cm
#define _DIST_MAX 250.0     // 25cm

#define EMA_ALPHA 0.4       // EMA Filter alpha (조금 더 부드럽게)
#define LOOP_INTERVAL 20    // Loop Interval (unit: msec)

Servo myservo;
unsigned long last_loop_time;

float dist_prev = _DIST_MIN;
float dist_ema = _DIST_MIN;

void setup()
{
  pinMode(PIN_LED, OUTPUT);
  myservo.attach(PIN_SERVO);
  myservo.writeMicroseconds(_DUTY_NEU);
  Serial.begin(1000000); // 1,000,000 bps
}

void loop()
{
  unsigned long time_curr = millis();
  int duty;
  float a_value, dist_raw;

  if (time_curr < (last_loop_time + LOOP_INTERVAL))
    return;
  last_loop_time += LOOP_INTERVAL;

  a_value = analogRead(PIN_IR);

  dist_raw = ((6762.0 / (a_value - 9.0)) - 4.0) * 10.0 - 60.0;

  // Range filter
  if(dist_raw < _DIST_MIN) dist_raw = _DIST_MIN;
  if(dist_raw > _DIST_MAX) dist_raw = _DIST_MAX;

  // EMA filter
  dist_ema = EMA_ALPHA * dist_raw + (1.0 - EMA_ALPHA) * dist_prev;
  dist_prev = dist_ema;

  // LED control (EMA값 기준, margin 2cm)
  if(dist_ema >= (_DIST_MIN + 2) && dist_ema <= (_DIST_MAX - 2))
    digitalWrite(PIN_LED, HIGH);
  else
    digitalWrite(PIN_LED, LOW);

  // Servo map (direct 계산)
  duty = _DUTY_MIN + (dist_ema - _DIST_MIN) * (_DUTY_MAX - _DUTY_MIN) / (_DIST_MAX - _DIST_MIN);
  myservo.writeMicroseconds(duty);

  // Serial output
  Serial.print("IR:"); Serial.print(a_value);
  Serial.print(", dist_raw:"); Serial.print(dist_raw);
  Serial.print(", ema:"); Serial.print(dist_ema);
  Serial.print(", servo:"); Serial.println(duty);
}