/*
  Reading distance from the laser based VL53L1X
  By: Nathan Seidle
  SparkFun Electronics
  Date: April 4th, 2018
  License: This code is public domain but you buy me a beer if you use this and we meet someday (Beerware license).

  SparkFun labored with love to create this code. Feel like supporting open source hardware? 
  Buy a board from SparkFun! https://www.sparkfun.com/products/14667

  This example demonstrates how to read and average distance, the measurement status, and the signal rate.

  Are you getting weird readings? Be sure the vacuum tape has been removed from the sensor.
*/

#include <Wire.h>
#include "SparkFun_VL53L1X.h"

//Optional interrupt and shutdown pins.
#define SHUTDOWN_PIN 2
#define INTERRUPT_PIN 3

SFEVL53L1X distanceSensor;
//Uncomment the following line to use the optional shutdown and interrupt pins.
//SFEVL53L1X distanceSensor(Wire, SHUTDOWN_PIN, INTERRUPT_PIN);

//Store distance readings to get rolling average
#define HISTORY_SIZE 5
int history[HISTORY_SIZE];
byte historySpot;
bool board = true;
long cal = 0;
void setup(void)

#define NUMBER_OF_SENSORS 3
{
  Wire.begin();

  Serial.begin(9600);
  Serial.println("VL53L1X Qwiic Test");

  for (byte x = 0 ; x < NUMBER_OF_SENSORS ; x++) {
    enableMuxPort(x); //Tell mux to connect to port X

    if (distanceSensor.begin() == true)
      Serial.println("Sensor online!");

    distanceSensor.setDistanceModeShort();
    
    disableMuxPort(x);
  }

  

//  for (int x = 0 ; x < HISTORY_SIZE ; x++)
//    history[x] = 150;
//
//  int calHistory[500];
//  
//  for(int i = 0; i < 500; i++) {
//    distanceSensor.startRanging(); //Write configuration block of 135 bytes to setup a measurement
//    calHistory[i] = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
//    distanceSensor.stopRanging();
//  }
//
//    
//
//  for (int i = 0 ; i < 500 ; i++)
//    cal += calHistory[i];
//
//  cal /= 500;
//
//  Serial.println(cal);
}

void loop(void)
{

  for (byte x = 0 ; x < NUMBER_OF_SENSORS ; x++) {
    enableMuxPort(x); //Tell mux to connect to this port, and this port only

    //long startTime = millis();
    distanceSensor.startRanging(); //Write configuration block of 135 bytes to setup a measurement
    int distance = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
    distanceSensor.stopRanging();
    //long endTime = millis();
    Serial.print("Sensor: ");
    Serial.print(x);
    Serial.print("\tDistance(mm): ");
    Serial.print(distance);
  Serial.println();
//    history[historySpot] = distance;
//    if (++historySpot == HISTORY_SIZE) historySpot = 0;
//  
//    long avgDistance = 0;
//    for (int x = 0 ; x < HISTORY_SIZE ; x++)
//      avgDistance += history[x];
//  
//    avgDistance /= HISTORY_SIZE;
//    Serial.print("\tavgDistance: ");
//    Serial.print(avgDistance);
//  
//    Serial.print("\tHz: ");
//    Serial.print(1000.0 / (float)(endTime - startTime), 2);
//  
//    if(avgDistance > (cal -20) && board) {
//      board = false;
//      Serial.print("\tBoard Not Detected");
//      //Serial.println();
//    } else if (avgDistance <= (cal -20) && !board) {
//      board = true;
//      Serial.print("\tBoard Detected");
//      //Serial.println();
//    }

    disableMuxPort(x); //Tell mux to disconnect from this port
  }

  delay(1); //Wait for next reading
  
  

//  float distanceInches = avgDistance * 0.0393701;
//  float distanceFeet = distanceInches / 12;
//
//  Serial.print("\tavgDistance(ft): ");
//  Serial.print(distanceFeet, 2);

//  int signalRate = distanceSensor.getSignalRate();
//  Serial.print("\tSignal rate: ");
//  Serial.print(signalRate);
//
//  byte rangeStatus = distanceSensor.getRangeStatus();
//  Serial.print("\tRange Status: ");

  //Make it human readable
//  switch (rangeStatus)
//  {
//    case 0:
//      Serial.print("Good");
//      break;
//    case 1:
//      Serial.print("Signal fail");
//      break;
//    case 2:
//      Serial.print("Sigma fail");
//      break;
//    case 7:
//      Serial.print("Wrapped target fail");
//      break;
//    default:
//      Serial.print("Unknown: ");
//      Serial.print(rangeStatus);
//      break;
//  }

  

  
}
