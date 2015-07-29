#define FOOTRELEASE_PIN 4
#define THROTTLE_PIN 5
#define REVERSESWTICH_PIN 6

#define BUZZER_PIN 8
#define THROTTLE_ZERO 0
#define THROTTLE_MAX 255
#define THROTTLE_THRESH 40

const int numReadings = 10;

int readings[numReadings];      // the readings from the analog input
int index = 0;                  // the index of the current reading
int total = 0;                  // the running total
int throttlepos = 0;                // the average

int BRAKE_PIN = A0;

boolean footrelease = false;
boolean reverse = false;
boolean brake = true;
boolean armed = false;
byte current_speed = 0;

void setup() {
  //init average array for analoge input
  for (int i = 0; i < numReadings; i++)
    readings[i] = 0;  
    
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
  updateThrottle();
  updateReverse();
  if(armed){
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
    if(!digitalRead(FOOTRELEASE_PIN) && !digitalRead(REVERSESWTICH_PIN) && analogRead(THROTTLE_PIN) < THROTTLE_ZERO + THROTTLE_THRESH) {
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
    if(digitalRead(FOOTRELEASE_PIN)){
      armed = true;
      return armed;
    }
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
  total= total + readings[index];       
  index = index + 1;                    
  if (index >= numReadings)              
    index = 0;                           
  throttlepos = total / numReadings;
  current_speed = map(throttlepos, THROTTLE_ZERO + THROTTLE_THRESH, THROTTLE_MAX, 0, 255);        
}

void setMotor(){
  setMotor(current_speed);
}

void setMotor(byte speed){
  
}

