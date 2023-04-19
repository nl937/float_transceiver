//Headers for DS3231 RTC
#include <RTClib.h>
#include <Wire.h>

//Headers for XBee
#include <SoftwareSerial.h>

//Motor and solenoid relay pins
#define PUMP_INPUT_1 A3
#define PUMP_INPUT_2 A2
#define SOLENOID_RELAY_1 A1
#define SOLENOID_RELAY_2 A0

//Into external bladder
#define FORWARD true 
//Into internal bladder
#define BACKWARD false

//Pump time (ms)
#define PUMP_TIME 10000

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

void solenoidopen(){
  digitalWrite(SOLENOID_RELAY_2, HIGH);
}

void solenoidclose(){
  digitalWrite(SOLENOID_RELAY_2, LOW);
}

void setup() {
  digitalWrite(SOLENOID_RELAY_1, LOW);
  digitalWrite(SOLENOID_RELAY_2, LOW);
  //Initalize FTDI
  Serial.begin(9600);
  XBee.begin(9600);
  Serial.println("FE-01 initializing");
  Serial.print("\nCompiled at: ");
  Serial.print(__DATE__);
  Serial.print(__TIME__);
  wirelessprint(__DATE__);
  wirelessprint(__TIME__);
  Serial.print("\n");
  Wire.begin();
  Rtc.begin();
  DateTime compiled = DateTime(__DATE__, __TIME__);
  if (!Rtc.isrunning()) {
    Serial.println("Setting RTC to compiled time");
    wirelessprint("Setting RTC to compiled time");
    Rtc.adjust(compiled);
  }
  if(Rtc.now() < compiled){
    Serial.println("Current time less than compiled time");
    Rtc.adjust(compiled);
  }
  wirelessprint("Type anything to start profiles");
  while(true){
    /*while(XBee.available() > 0){
      char data = XBee.read();
      wirelessprint(data);
      XBeeData.concat(data);
      if(data == '\n'){ 
        break;
      }
    }
    if(XBeeData == "BEGIN\n"){
      break;
    }
    else{
      XBeeData = "";
    }*/
    if(XBee.available() > 0){
      break;
    }
  }
  wirelessprint("Beginning profiles in 10 seconds");
  for(int i = 9; i >= 0; i--){
    XBee.print("Diving in ");
    XBee.print(i+1);
    XBee.print(" seconds\n");
    delay(1000);
  }
  wirelessprint("Beginning profile 1");
  //Descend
  solenoidopen();
  pumpon(BACKWARD);
  delay(PUMP_TIME);
  solenoidclose();
  pumpoff();
  delay(PROFILE_DELAY);
  //Ascent
  pumpon(FORWARD);
  solenoidopen();
  delay(PUMP_TIME);
  solenoidclose();
  pumpoff();
  //Report time + team number
  for(int i = 0; i < 30; i++){
    DateTime current = Rtc.now();
    wirelessprint("Deep Sea Tactics! :)");
    wirelessprint(current.tostr(buf));
    delay(1000);
  }
  wirelessprint("Beginning profile 2");
  //Descend
  solenoidopen();
  pumpon(BACKWARD);
  delay(PUMP_TIME);
  solenoidclose();
  pumpoff();
  delay(PROFILE_DELAY);
  //Ascent
  solenoidopen();
  pumpon(FORWARD);
  delay(PUMP_TIME);
  solenoidclose();
  pumpoff();
}

void loop() {
  //Report time + team number
  DateTime current = Rtc.now();
  wirelessprint("Deep Sea Tactics! :)");
  wirelessprint(current.tostr(buf));
  delay(1000);
}
