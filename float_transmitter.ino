//Headers for DS3231 RTC
#include <RTClib.h>
#include <Wire.h>

//Headers for XBee
#include <SoftwareSerial.h>

//Motor and solenoid relay pins
#define PUMP_INPUT_1 A1
#define PUMP_INPUT_2 A0
#define SOLENOID_RELAY_1 A1
#define SOLENOID_RELAY_2 A0

//Into external bladder
#define FORWARD true 
//Into internal bladder
#define BACKWARD false

//Pump time (ms)
#define PUMP_TIME 9500

//Profile delay (ms)
#define PROFILE_DELAY 8000

//Initialize RTC
DS3231 Rtc;
char buf[20];

//Initialize XBee
SoftwareSerial XBee(9, 10);

bool writetime;
bool profile;

String XBeeData;
int a;

void wirelessprint(char* message) {
  XBee.write(message);
  XBee.write('\n');
}

void pumpon(bool direction){
  if(direction){
    //Forward
    digitalWrite(PUMP_INPUT_1, LOW);
    digitalWrite(PUMP_INPUT_2, HIGH);
  }
  else{
    //Backwards
    digitalWrite(PUMP_INPUT_1, HIGH);
    digitalWrite(PUMP_INPUT_2, LOW);
  }
}

void pumpoff(){
  digitalWrite(PUMP_INPUT_1, LOW);
  digitalWrite(PUMP_INPUT_2, LOW);
}

void serialflush(){
  char dummy;
  while(XBee.available() > 0){
    dummy = XBee.read();
  }
}

void pumpdelay(int delaytime, bool direct, bool transmit){
  int b;
  for(int i = 0; i < PUMP_TIME; i++){
    if(transmit){
      if(!(i % 1000)){
        DateTime current = Rtc.now();
        wirelessprint("Company number: R1");
        wirelessprint(current.tostr(buf));
      }
    }
    if(!(i % 200)){
      b++;
    }
    if(!(b % 2)){
      pumpon(direct);
    }
    else{
      pumpoff();
    }
    delay(1);
  }
  pumpoff();
}

void setup() {
  pinMode(PUMP_INPUT_1, OUTPUT);
  pinMode(PUMP_INPUT_2, OUTPUT);
  digitalWrite(SOLENOID_RELAY_1, LOW);
  digitalWrite(SOLENOID_RELAY_2, LOW);
  digitalWrite(PUMP_INPUT_1, LOW);
  digitalWrite(PUMP_INPUT_2, LOW);
  //Initalize FTDI
  Serial.begin(9600);
  XBee.begin(9600);
  //Initialize RTC
  Wire.begin();
  Rtc.begin();
  wirelessprint("Current time: ");
  wirelessprint((Rtc.now()).tostr(buf));
  wirelessprint('\n');
  DateTime compiled = DateTime(__DATE__, __TIME__);
  DateTime newtime = DateTime(compiled.year(), compiled.month(), compiled.day(), compiled.hour() + 4, compiled.minute(), compiled.second() + 5);
  if (!Rtc.isrunning()) {
    Serial.println("Setting RTC to compiled time");
    wirelessprint("Setting RTC to compiled time");
    Rtc.adjust(newtime);
  }
  if(Rtc.now() < newtime){
    Serial.println("Current time less than compiled time");
    Rtc.adjust(newtime);
  }
  wirelessprint("Type anything to start filling");
  while(true){
    if(XBee.available() > 0){
      break;
    }
  }
  serialflush();
  wirelessprint("Beginning filling in 5 seconds");
  for(int i = 5; i >= 0; i--){
    XBee.print("Filling in ");
    XBee.print(i+1);
    XBee.print(" seconds\n");
    delay(1000);
  }
  wirelessprint("Beginning filling");
  pumpon(BACKWARD);
  delay(PUMP_TIME);
  pumpdelay(PROFILE_DELAY, BACKWARD, false);
  wirelessprint("Type anything to exhaust");
  serialflush();
  while(true){
    if(XBee.available() > 0){
      break;
    }
  }
  serialflush();
  wirelessprint("Beginning exhaust in 5 seconds");
  for(int i = 5; i >= 0; i--){
    XBee.print("Exhausting in ");
    XBee.print(i+1);
    XBee.print(" seconds\n");
    delay(1000);
  }
  wirelessprint("Beginning exhaust");
  pumpon(FORWARD);
  delay(PUMP_TIME+4000);
  pumpdelay(PROFILE_DELAY, FORWARD, false);
  serialflush();
  wirelessprint("Type anything to start profiles");
  while(true){
    if(!(a % 2)){
      pumpon(FORWARD);
    }
    else{
      pumpoff();
    }
    if(XBee.available() > 0){
      break;
    }
    DateTime current = Rtc.now();
    wirelessprint("R1");
    wirelessprint(current.tostr(buf));
    delay(1000);
    a++;
  }
  serialflush();
  pumpoff();
  wirelessprint("Beginning profiles in 10 seconds");
  for(int i = 9; i >= 0; i--){
    XBee.print("Diving in ");
    XBee.print(i+1);
    XBee.print(" seconds\n");
    delay(1000);
  }
  wirelessprint("Beginning profile 1");
  //Descend
  //solenoidopen(); 
  pumpon(BACKWARD);
  delay(PUMP_TIME);
  //solenoidclose();
  pumpoff();
  delay(PROFILE_DELAY);
  //pumpdelay(PROFILE_DELAY, BACKWARD, false);
  pumpoff();
  //Ascent
  pumpon(FORWARD);
  //solenoidopen();
  delay(PUMP_TIME+4000);
  //solenoidclose();
  pumpoff();
  //Report time + team number
  pumpdelay(PUMP_TIME+6000, FORWARD, false);
  for(int i = 0; i < 10; i++){
    if(!(i % 2)){
      pumpon(FORWARD);
    }
    else{
      pumpoff();
    }
    DateTime current = Rtc.now();
    wirelessprint("Company number: R1");
    wirelessprint(current.tostr(buf));
    delay(1000);
  }
  serialflush();
  wirelessprint("Type anything to start second profile");
  pumpon(FORWARD);
  while(true){
    if(XBee.available() > 0){
      break;
    }
  }
  serialflush();
  pumpoff();
  wirelessprint("Beginning profile 2");
  //Descend
  pumpon(BACKWARD);
  delay(PUMP_TIME);
  pumpoff();
  //pumpdelay(PROFILE_DELAY, BACKWARD, false);
  //pumpoff();
  delay(PROFILE_DELAY);
  //Ascent
  pumpon(FORWARD+4000);
  delay(PUMP_TIME+4000);
  //
  pumpoff();
  pumpdelay(PUMP_TIME+8000, FORWARD, false);
  pumpoff();
  a=0;
}

void loop() {
  if(!(a % 2)){
      pumpon(FORWARD);
    }
    else{
      pumpoff();
    }
  //Report time + team number
  DateTime current = Rtc.now();
  wirelessprint("Company number: R1");
  wirelessprint(current.tostr(buf));
  delay(1000);
  a++;
}
