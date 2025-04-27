const byte sendPin[] = {7,8,9};
const byte recePin[] = {4,5,6};
const byte WRPin = 11;
const byte syncPin = 10;

void setup() {
  // put your setup code here, to run once:
  for(int i = 0; i < 3; i++) {
    pinMode(sendPin[i], OUTPUT);
  }  
  for(int i = 0; i < 3; i++) {
    pinMode(recePin[i], INPUT);
  }  
  pinMode(WRPin, INPUT);
  pinMode(syncPin, INPUT);

  Serial.begin(9600);
}

void receiveBin(){
  byte n = 0;
  byte bin = 0b00000000;
  bool lState = 0;
  bool cState = 0;

  while(digitalRead(WRPin)) {
      if(n < 7) {
        while(digitalRead(syncPin)) {          
          lState = digitalRead(syncPin);
          delay(1);
          bin |= (digitalRead(recePin[0]) << n + 0);
          bin |= (digitalRead(recePin[1]) << n + 1);
          bin |= (digitalRead(recePin[2]) << n + 2);
        }  
        cState = digitalRead(syncPin);

        if (lState && !cState ){
          lState = 0;
          n += 3;
        } 
      }       
  }
      Serial.print((char)bin);
}


void sendBin(byte bin) {
  pinMode(WRPin, OUTPUT);
  pinMode(syncPin, OUTPUT);
  digitalWrite(WRPin, HIGH);

  for (int i = 0; i < 3; i++) {    
    byte part = (bin >> i*3) & 0b00000111;  
    digitalWrite(sendPin[0], (part >> 0) & 1);
    digitalWrite(sendPin[1], (part >> 1) & 1);
    digitalWrite(sendPin[2], (part >> 2) & 1);
    digitalWrite(syncPin, HIGH);
    
    delay(20);
    digitalWrite(syncPin, LOW);
    delay(40);
  }

  digitalWrite(WRPin, LOW);
  pinMode(syncPin, INPUT);
  pinMode(WRPin, INPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  byte data = 0;
  if(digitalRead(WRPin)) {
      receiveBin(); 
    }
    
  if (Serial.available() > 0) {
    data = Serial.read();
    sendBin(data);
    delay(6);
  }
}
