void setup() {
  // put your setup code here, to run once:
        pinMode(4, OUTPUT);
        pinMode(5, INPUT);
        pinMode(6, INPUT);
        pinMode(7, INPUT);
        pinMode(8, INPUT);
        Serial.begin(9600);
}

void loop() {
  // put your main code here, to run repeatedly:
  digitalWrite(4, HIGH);
  delay(1000);
  digitalWrite(4, LOW);
  delay(1000);
  Serial.print(digitalRead(5));
  Serial.print(digitalRead(6));
  Serial.print(digitalRead(7));
  Serial.print(digitalRead(8));
  Serial.println();



}
