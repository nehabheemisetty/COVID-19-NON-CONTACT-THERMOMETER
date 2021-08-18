#include<Wire.h>
#include<Adafruit_MLX90614.h> 
#include<Adafruit_GFX.h>
#include<Adafruit_SSD1306.h> 
Adafruit_MLX90614 mlx=Adafruit_MLX90614();
Adafruit_SSD1306 display(-1);
//Adafruit_SSD1306 display(128,64, &Wire , -1);
float roomTemp; //ambient temperature
float objectTemp,stemp; //object temperature
int readcount=0;
float threshold=0;
//----------- Ultrasonic Sensor preparation
#define echoPin 11 //Echo Pin
#define trigPin 12 //Trigger Pin
int maximumRange=25; //Maximum range needed
int minimumRange=15; //Minimum range needed
long duration,distance; //Duration used to calculate distance
int dtime;
void setup()
{
pinMode(trigPin, OUTPUT);
pinMode(echoPin, INPUT);
Serial.begin(9600);
//setting up the OLED display
display.begin(SSD1306_SWITCHCAPVCC,0x3C);
delay(1000);
display.clearDisplay();
display.setTextColor(WHITE);
}
void loop()
{
digitalWrite(trigPin, LOW);
delayMicroseconds(2);
digitalWrite(trigPin, HIGH);
delayMicroseconds(10);
digitalWrite(trigPin, LOW);
duration=pulseIn(echoPin, HIGH);
//calculate the distance (in cm) based on the speed of sound.
distance=duration*0.034/2;
//reading object and ambient temperature
objectTemp=threshold + mlx.readObjectTempC();
roomTemp=mlx.readAmbientTempC();
//print to serial port
Serial.println("Object:" + String(objectTemp) + ",Ambient:" + String(roomTemp));
Serial.println(distance);
//display on OLED
display.clearDisplay();
display.setTextSize(1);
display.setCursor(0,25);
display.print("Dis:" + String(distance) + "cm");
display.setCursor(65,25);
display.print("Room:" + String(roomTemp).substring(0,4) +"C");
display.display()
display.setTextSize(2);
display.setCursor(0,0);
if(distance > maximumRange)
{
   display.print("GET CLOSER");
}
if(distance < minimumRange) {
   display.print("TOO CLOSE!");
}
if((distance>=minimumRange) && (distance<=maximumRange))
{
  if(readcount == 5){  //after reading 5 consecutive time
     disptemp();
  } else {
   display.print("HOLD ON"); // when in range,ask user to hold position
   stemp=stemp+objectTemp;
   readcount++;
   dtime=200;  //until approx. 5 x 200 ms = 1 sec
  }
} else { //if user is out of range,reset calculation
  dtime=100;
  readcount=0;
  stemp=0;
}
display.display();
delay(dtime);
Serial.println("count :"+String(readcount));
}
void disptemp()
{
  objectTemp=stemp/5; //get the average of the temp
  display.setTextSize(1);
  display.print("YOUR TEMPERATURE:");
  display.setTextSize(2);
  display.setCursor(60,5);
  display.print(String(objectTemp).substring(0,4)+"C");
  display.display();
  readcount=0;
  stemp=0;
  if(objectTemp>=37.5)
  {
    play_alert();
  }
  else
  {
    play_ok;
  }
  dtime=5000; //wait for 5 seconds
}
 void play_ok()
 {  //play three sequential notes when object temperature is below 37.5C
  tone(3,600,1000);
  delay(200);
  tone(3,750,500);
  delay(1000);
  tone(3,1000,500);
  delay(200);
  noTone(3);  
  }
  void play_alert()
  { //beep 3x when object temperature is >=37.5C
  tone(3,2000,1000);
  delay(1000);
  tone(3,3000,1000);
  delay(1000);
  tone(3,4000,1000);
  delay(1000);
  noTone(3);
  }
