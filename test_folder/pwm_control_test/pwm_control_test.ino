int in2 = 2;
int in1 = 3;
int ena = 22;
float duty_ratio = 0;
float a = 0;

void setup() {
  // put your setup code here, to run once:
  pinMode(in2, OUTPUT);
  pinMode(in1, OUTPUT);
  pinMode(ena, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
  a = 255 * duty_ratio;
  analogWrite(in2, a);
  analogWrite(in1, 0);
  digitalWrite(ena, HIGH);
}
