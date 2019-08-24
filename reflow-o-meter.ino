#include <Wire.h>
#include <FastLED.h>
#include "SparkFun_VL53L1X.h"
#include "light_control.h"

// Setup detectors
SFEVL53L1X distanceSensor;
#define NUMBER_OF_SENSORS 3
#define CALIBRATION_SAMPLE 500
//Store distance readings to get rolling average
#define HISTORY_SIZE 8
int history[NUMBER_OF_SENSORS][HISTORY_SIZE];
byte historySpot[NUMBER_OF_SENSORS];
bool board = true;
long calibration[NUMBER_OF_SENSORS];
int clearReading = 3;
#define DIFF_FACTOR 20

#define INTERRUPT_PIN 21
volatile bool cancelled;

void IRAM_ATTR cancelLight() {
  cancelled = true;
}

void setup(void){
  Wire.begin();

  Serial.begin(9600);

  // Setup light
  LEDS.addLeds<APA102, DATA_PIN, CLOCK_PIN, BGR, DATA_RATE_MHZ(1)>(matrix, NUM_LEDS);
  LEDS.setBrightness(10);
  lightOff();

  // Boot
  Serial.println("Welcome to the Reflow-o-Meter");
  Serial.println("Booting...");

  bool errors = false;

  bootLight();
 
  // Setup detectors
  Serial.println("\tSetting up detectors...");
  
  for (byte x = 0 ; x < NUMBER_OF_SENSORS ; x++) {
    enableMuxPort(x); //Tell mux to connect to port X

    if (distanceSensor.init() == false) {
      if(distanceSensor.getSensorID() != 0) {
        Serial.print("\tDetector ");
        Serial.print(x + 1);
        Serial.println(" online");

        distanceSensor.setDistanceModeShort();

        // Calibrate
        Serial.println("\tCalibrating Detector");

        calibration[x] = 0;
        int calibrationHistory[CALIBRATION_SAMPLE];
        
        for(int i = 0; i < CALIBRATION_SAMPLE; i++) {
          bootLight();
          distanceSensor.startRanging(); //Write configuration block of 135 bytes to setup a measurement
          calibrationHistory[i] = distanceSensor.getDistance(); //Get the result of the measurement from the sensor
          distanceSensor.stopRanging();
        }

        // Calculate calibration
        for (int i = 0 ; i < CALIBRATION_SAMPLE; i++) {
          calibration[x] += calibrationHistory[i];
        }

        calibration[x] /= CALIBRATION_SAMPLE;

        Serial.print("\tCalibration ");
        Serial.print(calibration[x]);
        Serial.println("mm");

        // Set up history
        for (int i = 0 ; i < HISTORY_SIZE ; i++) {
          history[x][i] = calibration[x];
        }
        
        Serial.println("\tFinished Calibrating Detector");
      } else {
        Serial.print("\tDetector ");
        Serial.print(x + 1);
        Serial.println(" offline");
        errors = true;
      }
    }

    disableMuxPort(x);
  }
  
  Serial.println("\tSetting up detectors complete.");

  bootLight(false);

  // Setup interrupt
  pinMode(INTERRUPT_PIN, INPUT_PULLUP);
  attachInterrupt(INTERRUPT_PIN, cancelLight, FALLING);
  
  // Finish booting
  if(errors) {
    Serial.println("Some errors occur while booting");
    errorLight();   
  } else {
    Serial.println("Ready");
    LEDS.setBrightness(50);
  }
}

void loop(void) {
  for (byte x = 0 ; x < NUMBER_OF_SENSORS ; x++) {
    enableMuxPort(x);

    distanceSensor.startRanging();
    int distance = distanceSensor.getDistance(); 
    distanceSensor.stopRanging();
    
    Serial.print("Sensor: ");
    Serial.print(x);
    Serial.print("\tDistance(mm): ");
    Serial.print(distance);
    //Serial.println();

    if(clearReading > 2) {
      history[x][historySpot[x]] = distance;
    } else {
      clearReading++;
    }
    if (++historySpot[x] == HISTORY_SIZE) historySpot[x] = 0;

    long avgDistance = 0;
    for (int i = 0 ; i < HISTORY_SIZE ; i++)
      avgDistance += history[x][i];

    avgDistance /= HISTORY_SIZE;
    Serial.print("\tavgDistance: ");
    Serial.print(avgDistance);

    if(avgDistance > (calibration[x] - DIFF_FACTOR) && board) {
      board = false;
      Serial.print("\tBoard Not Detected");
      //Serial.println();
    } else if (avgDistance <= (calibration[x] - DIFF_FACTOR) && !board) {
      board = true;
      Serial.print("\tBoard Detected");
      //Serial.println();
   }

Serial.println();
    disableMuxPort(x); //Tell mux to disconnect from this port

    if(board) {
      break;
    } 
  }

  if(board) {
    unsigned long startTime, startTimeDiff;

    startTime = millis();
    cancelled = false;
    
    while(!cancelled) {
      startTimeDiff = millis() - startTime;
    
      if(startTimeDiff < 15000) {
        updateLight(0,255,0,20);
      } else if(startTimeDiff < 25000) {
        updateLight(255,215,0,20);
      } else {
        updateLight(255,0,0,20);
      }
    }
    // Turn light off
    lightOff();

    // Reset history
    clearReading = 0;
    for (byte x = 0 ; x < NUMBER_OF_SENSORS ; x++) {
      for (int i = 0 ; i < HISTORY_SIZE ; i++) {
        history[x][i] = calibration[x];
      }
    }
    
    // Wait
    delay(10000);
  } else {
    delay(1);
  }
}
