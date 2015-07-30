#define FOOTRELEASE_PIN 4
#define THROTTLE_PIN A0
#define REVERSESWTICH_PIN 6
#define BRAKE_PIN 7
#define FW_PIN 9
#define REV_PIN 10
#define BUZZER_PIN 13
#define THROTTLE_ZERO 185
#define THROTTLE_MAX 860
#define THROTTLE_THRESH 10

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int throttlepos = 0;                // the average



boolean footrelease = false;
boolean reverse = false;
boolean brake = true;
boolean armed = false;
int current_speed = 0;

void setup() {
  //init average array for analoge input
  for (int i = 0; i < numReadings; i++)
    readings[i] = 0;  
  Serial.begin(9600);
  //setPwmFrequency(9,8);
  //beep once for 500ms
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, HIGH);
  delay(500);
  digitalWrite(BUZZER_PIN, LOW);
  delay(200);  
  //setup all i/o pins
  pinMode(FOOTRELEASE_PIN, INPUT);
  pinMode(THROTTLE_PIN, INPUT);
  pinMode(REVERSESWTICH_PIN, INPUT);
  pinMode(BRAKE_PIN, OUTPUT);
  digitalWrite(BRAKE_PIN, LOW);
  Serial.println(analogRead(THROTTLE_PIN));
  if(initCheck()) {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(200);
    digitalWrite(BUZZER_PIN, LOW);
    delay(200);
    digitalWrite(BUZZER_PIN, HIGH);
    delay(1200);
    digitalWrite(BUZZER_PIN, LOW);
  } else {
    digitalWrite(BUZZER_PIN, HIGH);
    delay(2000);
    digitalWrite(BUZZER_PIN, LOW);
    delay(1000);
    setup();
  }
  
}

void loop() {
  checkArmState();
  //Serial.println(throttlepos);
  //Serial.println(current_speed);
  //delay(200);
  if(armed){
    updateThrottle();
    updateReverse();
    setMotor();
    if(brake == armed){
      brake = false;
      setBrake();
    }
  } else {
    brake = true;
    setBrake();
  }
}

boolean initCheck() {
  for(int i = 0; i < 5; i++){
    if(!digitalRead(FOOTRELEASE_PIN) && !digitalRead(REVERSESWTICH_PIN) && analogRead(THROTTLE_PIN) < THROTTLE_ZERO + (THROTTLE_THRESH /2)) {
      return true;
    } else {
      delay(300);
    }
  }
  return false;
}

void setBrake(){
  if(brake){
    setMotor(0);
    digitalWrite(BRAKE_PIN, LOW);
  } else {
    digitalWrite(BRAKE_PIN, HIGH);
  }
}

boolean checkArmState(){
  if(digitalRead(FOOTRELEASE_PIN)){
    delay(10);
    if(digitalRead(FOOTRELEASE_PIN) && (analogRead(THROTTLE_PIN) < THROTTLE_ZERO + (THROTTLE_THRESH /2) || armed)){
      armed = true;
      return armed;
    }
  }
  if(armed){
    brake = true;
    setMotor(0);
    for (int i = 0; i < numReadings; i++)
      readings[i] = 0;
    index = 0;
    total = 0;
    current_speed = 0;
    throttlepos = 0;
  }
  armed = false;
  return armed;
}

void updateReverse() {  
  if(digitalRead(REVERSESWTICH_PIN) == HIGH){
    delay(10);
    if(digitalRead(REVERSESWTICH_PIN) == HIGH){
      reverse = true;   
      return;
    }
  }
  reverse = false;
}

void updateThrottle(){
  total= total - readings[index];         
  readings[index] = analogRead(THROTTLE_PIN);
  if((throttlepos - readings[index]) > 5)
    readings[index] = readings[index] / 2;
  total= total + readings[index];       
  index = index + 1;                    
  if (index >= numReadings)              
    index = 0;                           
  throttlepos = total / numReadings;
  current_speed = map(throttlepos, THROTTLE_ZERO + THROTTLE_THRESH, THROTTLE_MAX, 0, 255);
  if(throttlepos >= THROTTLE_MAX && current_speed > 250) current_speed = 255;
  if(throttlepos <= THROTTLE_ZERO + THROTTLE_THRESH) current_speed = 0;
}

void setMotor(){
  setMotor(current_speed);
}

void setMotor(int speed){
  if(reverse){
    analogWrite(REV_PIN, speed);
    analogWrite(FW_PIN, 0);
  } else {
    analogWrite(FW_PIN, speed);
    analogWrite(REV_PIN, 0);
  }
}


void setPwmFrequency(int pin, int divisor) {
  byte mode;
  if(pin == 5 || pin == 6 || pin == 9 || pin == 10) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 64: mode = 0x03; break;
      case 256: mode = 0x04; break;
      case 1024: mode = 0x05; break;
      default: return;
    }
    if(pin == 5 || pin == 6) {
      TCCR0B = TCCR0B & 0b11111000 | mode;
    } else {
      TCCR1B = TCCR1B & 0b11111000 | mode;
    }
  } else if(pin == 3 || pin == 11) {
    switch(divisor) {
      case 1: mode = 0x01; break;
      case 8: mode = 0x02; break;
      case 32: mode = 0x03; break;
      case 64: mode = 0x04; break;
      case 128: mode = 0x05; break;
      case 256: mode = 0x06; break;
      case 1024: mode = 0x7; break;
      default: return;
    }
    TCCR2B = TCCR2B & 0b11111000 | mode;
  }
}

