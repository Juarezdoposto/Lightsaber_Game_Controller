#include "ibus.h"
#include <MPU6050_tockn.h>
#include <Wire.h>


//Inicialize Gyro
MPU6050 mpu6050(Wire);


//vJoySerialFeeder comunication update,dont mess here.
#define UPDATE_INTERVAL 10 



//Each Analog have 2 axis,so its 2 analog inputs
#define ANALOG_INPUTS_COUNT 2 


byte analogPins[] = {A0, A1};

//Number of Digital ports being used on arduino for the physical buttons
#define DIGITAL_INPUTS_COUNT 4 


byte digitalPins[] = { 3, 4, 5, 6};

//Number of buttons that are activated by movement
#define ACTION_INPUTS_COUNT 1 

//State of the movement "button" stored here
int actionbuttonstate[]={LOW};


//Serial Comunication baud rate
#define BAUD_RATE 115200 


//Total of channels for the vJoySerialFeeder to read 
#define NUM_CHANNELS ( (ANALOG_INPUTS_COUNT)+ (DIGITAL_INPUTS_COUNT)+(ACTION_INPUTS_COUNT) )

//E aqui esse valor informa quantos canais vão receber os dados dos botões e analógicos por voce controlados
IBus ibus(NUM_CHANNELS);

//Inicialize angle y reads
float zeroY=0;

//State of the physical buttons
int auxbutton[3]={LOW};

void setup()
{
  
 //Physical buttons inout pullup inicializing
 pinMode(3, INPUT_PULLUP);
 pinMode(4, INPUT_PULLUP);
 pinMode(5, INPUT_PULLUP);  
 pinMode(6, INPUT_PULLUP);

//Serial comunication Starts
  Serial.begin(BAUD_RATE);
  //I2C Accel/Gyro comunication with Arduino Starts
  Wire.begin();
  mpu6050.begin();
 //Autocalibration for the sensor,optional
  mpu6050.calcGyroOffsets(false);
}


void loop()

{
 
//Accel/Gyro Readings

   mpu6050.update();
   float y=mpu6050.getAngleY(); //Movement Axis measured is Y
   y-=zeroY;


//In my measures i see that -65 angles is a good angle for me to swing the saber  forwards,if you struggling with the activation ,check getangle example and put your own angle.

if(y<-65 )
{
  actionbuttonstate[0]=HIGH;
}
else if(y>-65) 
{
  actionbuttonstate[0]=LOW;
}


//Serial Comunication Intensifies...
int i, bm_ch = 0;
  unsigned long time = millis();

   
       ibus.begin();
  
//State of buttons,and analog readings are delivered to the vJoySerialFeeder
      for(i=0; i < ANALOG_INPUTS_COUNT; i++)
    ibus.write(analogRead(analogPins[i]));

     for(i=0; i <ACTION_INPUTS_COUNT; i++)
     ibus.write(actionbuttonstate[i] == HIGH ? 1023 : 0);


    for(i=0; i < DIGITAL_INPUTS_COUNT; i++)
    ibus.write(digitalRead(digitalPins[i]) == HIGH ? 1023 : 0);
 
     ibus.end();

    time = millis() - time; // time elapsed in reading the inputs
    if(time < UPDATE_INTERVAL)
    // sleep till it is time for the next update
    delay(UPDATE_INTERVAL  - time);

}
  
