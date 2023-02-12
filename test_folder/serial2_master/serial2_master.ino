char value;

void setup() {
  Serial.begin(38400);
  Serial2.begin(38400);

}

void loop() {
  if(Serial.available() > 0)
  {
    value = Serial.read();
    Serial.print("value : ");
    Serial.println(value);
    Serial2.print(value);
    //Serial2.write(value);
  }
}
