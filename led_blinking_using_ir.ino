//led_blink_using_ir
const int irpin=10;
const int green_ledpin=11;
const int yellow_ledpin =12;
const int red_ledpin =13;

void setup() {
  // put your setup code here, to run once:
pinMode(irpin, INPUT);
pinMode(green_ledpin, OUTPUT);
pinMode(yellow_ledpin, OUTPUT);
pinMode(red_ledpin, OUTPUT);
}

void loop() {
  // put your main code here, to run repeatedly:
int sensorvalue = digitalRead(irpin);
if(sensorvalue==HIGH)
{
  digitalWrite(green_ledpin, LOW);
  delay(2000);
  digitalWrite(yellow_ledpin, LOW);
  delay(2000);
  digitalWrite(red_ledpin, LOW);
  delay(2000);
}
else {
  digitalWrite(green_ledpin, HIGH);
  
  digitalWrite(yellow_ledpin, HIGH);
  
  digitalWrite(red_ledpin, HIGH);
  
  Serial.print("no detection");
  //delay(1000);

}

}
