#include <Stepper.h>
#include <PWM.h>
const int numReadings = 20;
long minrapid = 100;
long maxrapid = 150000;
long minfeed = 100;
long maxfeed = 150000;
long minjog = 100;
long maxjog = 150000;

long readings[numReadings];      // the readings from the analog input
long readIndex = 0;              // the index of the current reading
long total = 0;                  // the running total
long average = 0;

int mode;
//input
int StartPin = 5; 
int EStop = 53;
int Limit1 = 50;
int Limit2 = 48;
int Limit3 = 44;
int Limit4 = 25;
int JogKanan = 26;
int JogKiri = 27;

int automanualpin = 28;
int PotRapid = A0;
int PotFeeding = A1;
int PotJog = A2;
int modeMesin = 29;
int timersignal = 46;
//output
int RelaySpindleCoolant = 9;
int RelayManual = 7;
int StepPin = 11;
int DirPin = 2;
int timerstarterRelay = 3;
long avgrapid;
long avgfeed;
long avgjog;

long averager(long input){
    total = total - readings[readIndex];
    readings[readIndex] = input;
    total = total + readings[readIndex];
    readIndex = readIndex + 1;
  if (readIndex >= numReadings) {
    // ...wrap around to the beginning:
    readIndex = 0;
  }
    average = total / numReadings;
   return average;
}
void feed(int a){
  if(a==1){
    digitalWrite(RelaySpindleCoolant,HIGH);
  }
  else if(a==0){
    digitalWrite(RelaySpindleCoolant,LOW);
  }  
}
void move_motor_feed(int dir){
    digitalWrite(DirPin,dir);
    avgfeed = averager(analogRead(PotFeeding));
    long freq = map(avgfeed, 0, 1023, minfeed, maxfeed);
    SetPinFrequencySafe(StepPin, freq);
    pwmWrite(StepPin, 128);
}

void move_motor_rapid(int dir){
    digitalWrite(DirPin,dir);  
    avgrapid = averager(analogRead(PotRapid));
    long freq = map(avgrapid, 0, 1023, minrapid, maxrapid);
    SetPinFrequencySafe(StepPin, freq);
    pwmWrite(StepPin, 128);
}

void stop_motor(){
    //SetPinFrequencySafe(StepPin, 0);
    //pwmWriteHR(StepPin, 65536);
    digitalWrite(StepPin,LOW);
}

long jog(int dir){
    digitalWrite(DirPin,dir);
//    avgjog = averager(analogRead(PotJog));
//    long freq = map(avgjog, 0, 1023, minjog, maxjog);
    avgrapid = averager(analogRead(PotRapid));
    long freq = map(avgrapid, 0, 1023, minrapid, maxrapid);
    SetPinFrequencySafe(StepPin, freq);
    pwmWrite(StepPin, 128);
    Serial.print(dir);
    
    Serial.print(", ");
    Serial.print(freq);
    Serial.print(", ");
    Serial.println(avgrapid);
}
void jogmode(){
  while(digitalRead(JogKanan)==LOW&&digitalRead(EStop)==HIGH){
  jog(0);
}
 while(digitalRead(JogKiri)==LOW&&digitalRead(EStop)==HIGH){
  jog(1);
}
if((digitalRead(JogKiri)==HIGH)||(digitalRead(JogKanan)==HIGH)){
  stop_motor();
}
if((digitalRead(JogKiri)==LOW)&&(digitalRead(JogKanan)==LOW)){
  stop_motor();
}
}
int sequence=0;
void estophandler(){
    if(digitalRead(EStop)==LOW){sequence=0;    digitalWrite(timerstarterRelay,LOW);
}
}
void bacamodemesin(){
  if(digitalRead(modeMesin)==LOW){mode= 0;}
  else if(digitalRead(modeMesin)==HIGH){mode= 1;}
}
void automode(){
  

  estophandler();


  
  if(sequence==0){//start
        Serial.println(sequence);
    stop_motor();
    feed(0);
    digitalWrite(timerstarterRelay,LOW);
    
    while(digitalRead(automanualpin)==LOW){break;}

  if(digitalRead(StartPin)==LOW&&digitalRead(EStop)==HIGH){sequence=1;} 
  }
  
  while(sequence==1){//start

    feed(1);
    estophandler();
    move_motor_rapid(mode);
    Serial.print("sequence = "); 
    Serial.print(sequence);   
    Serial.print("  mode = ");
    Serial.println(mode);
    if(digitalRead(Limit2)==LOW){sequence=2;}     
  }
  
  while(sequence==2){//feeding
    
    estophandler();
    move_motor_feed(mode);
    Serial.print("sequence = "); 
    Serial.print(sequence);   
    Serial.print("  mode = ");
    Serial.println(mode);
    if(digitalRead(Limit4)==LOW){sequence=3;}     
  }

  while(sequence==3){//stop tunggu
    //feed(0);

    estophandler();
    stop_motor();
    digitalWrite(timerstarterRelay,HIGH);
    Serial.print("sequence = "); 
    Serial.print(sequence);   
    Serial.print("  mode = ");
    Serial.println(mode);
    if(digitalRead(timersignal)==LOW){sequence=4;}     
  }

  while(sequence==4){//balik slow
    //digitalWrite(timerstarterRelay,LOW);
    estophandler();
    move_motor_feed(!mode);
    Serial.print("sequence = ");
    Serial.print(sequence);
    Serial.print("  mode = ");
    Serial.println(mode);
    if(digitalRead(Limit3)==LOW){sequence=5;}     
  }  

    while(sequence==5){//balik cepet
    //digitalWrite(timerstarterRelay,LOW);
    estophandler();
    move_motor_rapid(!mode);
    Serial.print("sequence = ");
    Serial.print(sequence);
    Serial.print("  mode = ");
    Serial.println(mode);
    if(digitalRead(Limit1)==LOW){sequence=0;}     
  }
    
}


//       (rapid)                 (feed)                 (stop wait)                    

//limit3_______________limit1_____________________limit2
