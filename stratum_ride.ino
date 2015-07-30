#define FOOTRELEASE_PIN 4
#define THROTTLE_PIN A0
#define REVERSESWTICH_PIN 6
#define BRAKE_PIN 7
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

bool initCheck() {
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
    delay(20);
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
  if(digitalRead(REVERSESWTICH_PIN)){
    delay(20);
    if(digitalRead(REVERSESWTICH_PIN)){
      reverse = true;      
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
  if(throttlepos >= THROTTLE_MAX) current_speed = 255;
  if(throttlepos <= THROTTLE_ZERO + THROTTLE_THRESH) current_speed = 0;
}

void setMotor(){
  setMotor(current_speed);
}

void setMotor(int speed){
  analogWrite(13, current_speed);
}

