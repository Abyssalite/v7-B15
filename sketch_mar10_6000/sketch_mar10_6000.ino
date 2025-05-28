const byte recePin[] = {4,5,6};
const byte sendPin[] = {7,8,9};
const byte readPin = 10;
const byte writePin = 11;

void setup() {
  // put your setup code here, to run once:
  pinMode(readPin, INPUT);
  pinMode(writePin, OUTPUT);
  
  for(int i = 0; i < 3; i++) {
    pinMode(sendPin[i], OUTPUT);
  }  
  for(int i = 0; i < 3; i++) {
    pinMode(recePin[i], INPUT);
  }  
  Serial.begin(9600);
}

void receiveBin(){
  byte n = 0;
  byte bin = 0b00000000;
  bool lState = 0;
  bool cState = 0;

  while(n < 7) {
      while(digitalRead(readPin)) {          
          lState = 1;
          delay(7); 
          // delay(15);
          bin |= (digitalRead(recePin[0]) << n + 0);
          bin |= (digitalRead(recePin[1]) << n + 1);
          bin |= (digitalRead(recePin[2]) << n + 2);
          //delay(15);
      }  
      cState = digitalRead(readPin);

      if (lState && !cState ){
          lState = 0;
          n += 3; 
      }       
  }
      Serial.print((char)bin);
}


void sendBin(byte bin) {
  for (int i = 0; i < 3; i++) {    
    byte part = (bin >> i*3) & 0b00000111;  
    digitalWrite(sendPin[0], (part >> 0) & 1);
    digitalWrite(sendPin[1], (part >> 1) & 1);
    digitalWrite(sendPin[2], (part >> 2) & 1);
    
    digitalWrite(writePin, HIGH);
    //delay(15); 
    delay(50);
    digitalWrite(writePin, LOW);
    //delay(15); 
    delay(50);
  }
}

void loop() {
  // put your main code here, to run repeatedly:
  byte data = 0;
  if(digitalRead(readPin)) {
      receiveBin(); 
    }
    
  if (Serial.available() > 0) {
    data = Serial.read();
    delay(1);
    sendBin(data);
  }
}
