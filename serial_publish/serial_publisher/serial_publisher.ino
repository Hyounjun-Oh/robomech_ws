
int val1 = 1;
float val2 = 2.23456;
int val3 = 3;
int val4 = 1;
float val5 = 2.28956;
int val6 = 3;

void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);

}

void loop() {
  // put your main code here, to run repeatedly:
  Serial.print(val1);
  Serial.print(" ");
  Serial.print(val2,5);
  Serial.print(" ");
  Serial.print(val3);
  Serial.print("/");
  Serial.print(val4);
  Serial.print(" ");
  Serial.print(val5,5);
  Serial.print(" ");
  Serial.println(val6);
}
