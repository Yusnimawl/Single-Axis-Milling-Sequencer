#include "definitions.h"

void setup() {InitTimersSafe();
  Serial.begin(115200);
  // put your setup code here, to run once:
pinMode(EStop,INPUT_PULLUP);
pinMode(JogKanan,INPUT_PULLUP);
pinMode(JogKiri,INPUT_PULLUP);
pinMode(automanualpin,INPUT_PULLUP);
pinMode(StartPin,INPUT_PULLUP);
pinMode(modeMesin,INPUT_PULLUP);
pinMode(Limit1,INPUT_PULLUP);
pinMode(Limit2,INPUT_PULLUP);
pinMode(Limit3,INPUT_PULLUP);
pinMode(Limit4,INPUT_PULLUP);
pinMode(timersignal,INPUT_PULLUP);
pinMode(automanualpin,INPUT_PULLUP);
pinMode(RelaySpindleCoolant,OUTPUT);
pinMode(RelayManual,OUTPUT);
pinMode(timerstarterRelay,OUTPUT);
pinMode(DirPin,OUTPUT);
pinMode(StepPin,OUTPUT);

bacamodemesin();

for (int thisReading = 0; thisReading < numReadings; thisReading++) {
    readings[thisReading] = 0;
  }
}

void loop() {

while(digitalRead(EStop)==LOW){sequence=0;   stop_motor(); digitalWrite(timerstarterRelay,LOW);}
  
while(digitalRead(automanualpin)==HIGH){
  Serial.println("auto mode");
  digitalWrite(RelayManual,HIGH);

  automode();
  Serial.println("end seq");
}
while(digitalRead(automanualpin)==LOW){
  digitalWrite(RelayManual,LOW);
  jogmode();
  Serial.println("jog mode");
}

}
