/*
//line_following_robot
step 1: define all the pins before void setup
step 2: write all the pins mode input/output in void setup
step 3: write the operation in void loop 

*/
//IR sensors pins
int leftsensor =12;
int rightsensor =13;

//l298n motor driver pins
int ena =11;
int enb =10;
int in1 =9;
int in2=8;
int in3=7;
int in4=6;

void setup() {
  // put your setup code here, to run once:
pinMode(leftsensor, INPUT);
pinMode(rightsensor, INPUT);
pinMode(ena, OUTPUT);
pinMode(enb, OUTPUT);
pinMode(in1, OUTPUT);
pinMode(in2, OUTPUT);
pinMode(in3, OUTPUT);
pinMode(in4, OUTPUT);

//set motor speed(0-255)
analogWrite(ena, 150);
analogWrite(enb, 150);

}

void loop() {
  // put your main code here, to run repeatedly:
int leftstate = digitalRead(leftsensor);
int rightstate = digitalRead(rightsensor);

if(leftstate ==LOW && rightstate ==LOW)
{
  moveForward();
}

else if(leftstate ==LOW && rightstate ==HIGH)
{
  rightturn();
}

else if(leftstate ==HIGH && rightstate ==LOW)
{
  leftturn();
}

else
{
  stopmotors();
}

}


void moveForward()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void rightturn()
{
  digitalWrite(in1, HIGH);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, HIGH);
}

void leftturn()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, HIGH);
  digitalWrite(in3, HIGH);
  digitalWrite(in4, LOW);
}

void stopmotors()
{
  digitalWrite(in1, LOW);
  digitalWrite(in2, LOW);
  digitalWrite(in3, LOW);
  digitalWrite(in4, LOW);
}
