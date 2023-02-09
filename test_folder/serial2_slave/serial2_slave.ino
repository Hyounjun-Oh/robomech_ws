char value;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(38400);
  Serial2.begin(38400);
  pinMode(10, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  if(Serial2.available() > 0)
  {
    value = Serial2.read();
    if (value == '1'){
      digitalWrite(10, HIGH);
    }else{
      digitalWrite(10,LOW);
    }
  }
}
