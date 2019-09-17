// Define light
#define NUM_MATRICES 3
#define NUM_LEDS 64 * NUM_MATRICES
#define DATA_PIN 18
#define CLOCK_PIN 5
#define RING_DATA_PIN 15
#define RING_CLOCK_PIN 14
#define RING_NUM_LEDS 40

// Setup light params
CRGB matrix[NUM_LEDS];
int individualMatrixWidth = 8;
int individualMatrixHeight = 8;
const int moduleHeight = 1;
const int moduleWidth = 3;
int matrixMap[moduleHeight][moduleWidth] = {{0,1,2}};
int orientation[moduleHeight * moduleWidth] = {3,3,3};

// Setup ring light params
CRGB ring[RING_NUM_LEDS];

// Setup other params
int bootCycle = individualMatrixWidth * moduleWidth -1;
int bootRingCycle = 0;
int cancelCycle = individualMatrixWidth * moduleWidth -1;

void fadeAll(uint8_t scale = 250) {
  for (int i = 0; i < NUM_LEDS; i++) {
    matrix[i].nscale8(scale);
  }
}

uint16_t XY( uint16_t x, uint16_t y) {
  uint16_t i;
  uint8_t matrixNum = matrixMap[(moduleHeight - 1) - (y / individualMatrixHeight)][x / individualMatrixWidth]; //Calculates which matrix we are on using the matrix map
  uint8_t relativeX; //x position relative to the origin of the current matrix
  uint8_t relativeY; //y position relative to the origin of the current matrix
  switch (orientation[matrixNum])//Transposes our values based on orientation
  {
    case 0:
      relativeX = x % individualMatrixWidth;
      relativeY = y % individualMatrixHeight;
      break;
    case 1:
      relativeX = 7 - (y % individualMatrixWidth);
      relativeY = x % individualMatrixHeight;
      break;
    case 2:
      relativeX = 7 - (x % individualMatrixWidth);
      relativeY = y % individualMatrixHeight;
      break;
    case 3:
      relativeX = y % individualMatrixWidth;
      relativeY = x % individualMatrixHeight;
      break;
  }
  i = (relativeY * individualMatrixWidth) + relativeX; //Calculates where we are on a matrix
  i += matrixNum * individualMatrixWidth * individualMatrixHeight; //Adds on offset based on which matrix we're on in the chain.
  return i;
}



void lightOff() {
  for (int y = 0; y < (individualMatrixHeight *  moduleHeight); y++) {
    for (int x = individualMatrixWidth * moduleWidth -1; x >= 0; x--) {
      matrix[XY(x, y)].setRGB(0,0,0);
    }
  }

  FastLED.show();
}

void bootLight(bool booting = true) {
  if(booting) {
    for (int y = 0; y < (individualMatrixHeight *  moduleHeight); y++) {
      matrix[XY(bootCycle, y)].setRGB(0,0,255);
    }

    // Ring light
    ring[bootRingCycle].setRGB(0,0,255);  

    FastLED.show();
    delay(10);
    fadeAll(120);

    if(bootCycle == 0) {
      bootCycle = individualMatrixWidth * moduleWidth - 1;
    } else {
      bootCycle--;
    }

    if(bootRingCycle == RING_NUM_LEDS - 1) {
      bootRingCycle = 0;
    } else {
      bootRingCycle++;
    }
  } else {
    lightOff();
  }
}

void errorLight() {
  while(true) {
    for (int x = individualMatrixWidth * moduleWidth -1; x >= 0; x--) {
      for (int y = 0; y < (individualMatrixHeight *  moduleHeight); y++) {
        matrix[XY(x, y)].setRGB(255,255,255);
      }

      FastLED.show();
      delay(50);
      fadeAll(120);
    }
  }
}

void updateLight(int r, int g, int b, unsigned int delayTime = 0) {
  for (int y = 0; y < (individualMatrixHeight *  moduleHeight); y++) {
      matrix[XY(cancelCycle, y)].setRGB(r,g,b);
  }
  
  FastLED.show();
  delay(delayTime);
  fadeAll(120);

  if(cancelCycle == 0) {
    cancelCycle = individualMatrixWidth * moduleWidth - 1;
  } else {
    cancelCycle--;
  }
}
