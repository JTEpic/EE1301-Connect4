// Include Particle Device OS APIs
#include "Particle.h"
#include "neopixel.h"

int getLowestEmptyRow(int col);
int setBoardFromString(String inputString);
int resetBoard(String inputString);

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

//yLength is row,xLength is column,board[row][column]
const int xLength=7,yLength=6;
int board[yLength][xLength];

//adjust pins for final physical setup
const int pinCol[xLength]={D11,D12,D13,D14,D19,D18,D8};
int pinData[xLength];
String sendCoord="null";

// IMPORTANT: Set pixel COUNT, PIN and TYPE
//int PIXEL_COUNT = INITAL_OFFSET+PIX_PER_HOLE*xLength*yLength+offsetvalues;
int PIXEL_COUNT = 297;
#define PIXEL_PIN SPI1 // Only use SPI or SPI1 on Photon 2 (SPI is MO or S0 pin; SPI1 is D2)
                      // NOTE: On the Photon 2, this must be a compiler constant!
int PIXEL_TYPE = WS2812;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
//Setup some colors, RGB version
int PixelColorRed = strip.Color(    200, 0, 0  );
int PixelColorBlue  = strip.Color(  0,   0, 200);
int PixelColorOff = strip.Color(    0,   0, 0  );
int PIX_PER_HOLE=4;
int INITAL_OFFSET=2;
//since LED strip wraps around,see num of pixels skipped,big offsets align with LED wrap around
int OFFSET[yLength][xLength]={{2,16,2,  15,2,15,2},
                              {2,2 ,2,  1, 1,2,2},
                              {2,1 ,1  ,1, 2,2,2},
                              {1,2 ,2,2,  2,2,2},
                              {2,2 ,2,3,  2,2,2},
                              {0,2 ,3,2,  3,2,0}};
bool updateLED=true;

void setup() {
  //reset array on boot, (0=open, 1=player 1 (cloud), 2=player 2 (photon))
  for(int x=0;x<xLength;x++){
    for(int y=0;y<yLength;y++){
      board[y][x]=0;
    }
  }
  for(int x=0;x<xLength;x++){
    pinMode(pinCol[x], INPUT);
  }
  strip.begin();
  for(int x=0;x<PIXEL_COUNT;x++){
    strip.setPixelColor(x,PixelColorOff);
  }
  strip.show();
  Particle.variable("grabChip", sendCoord);
  Particle.function("setBoard", setBoardFromString);
  Particle.function("resetBoard", resetBoard);
  Serial.begin(9600);
}

int pinPrev=-1;
void loop() {
  //check if sensor is still covered
  if(pinPrev!=-1){
    if(digitalRead(pinCol[pinPrev])==HIGH){
      Serial.println("Unlocked IR: "+(String)pinCol[pinPrev]);
      pinPrev=-1;
    }else{
      Serial.println("Locked IR: "+(String)pinCol[pinPrev]);
    }
  }
  //goes through all sensors to see if one is covered
  if(pinPrev==-1){
    for(int x = 0; x < xLength; x++){
      pinData[x] = digitalRead(pinCol[x]);
      if(pinData[x] != HIGH){
        pinPrev=x;
        int row = getLowestEmptyRow(x);
        if(row != -1){
          board[row][x] = 2;
          updateLED=true;
          String temp = ((String)row+","+(String)x);
          sendCoord = temp;
        }
        break;
      }
    }
  }

  //set light color according to array
  if(updateLED){
    Serial.println();
    for(int y=0;y<yLength;y++){
      for(int x=0;x<xLength;x++){
        Serial.print(board[y][x]);
      }
      Serial.println();
    }
    Serial.println();

    Serial.println("Updating LED");

    //reset board to off
    for(int x=0;x<PIXEL_COUNT;x++){
      strip.setPixelColor(x,PixelColorOff);
    }

    int pix = INITAL_OFFSET;
    // since LED wraps around
    bool upward = false;
    for (int x = 0; x < xLength; x++){
      if(upward){
        // upwards
        for (int y = yLength - 1; y >= 0; y--){
          if (board[y][x] == 1){
            for(int z=pix;z<(pix+PIX_PER_HOLE);z++){
              strip.setPixelColor(z, PixelColorRed);
            }
          }else if (board[y][x] == 2){
            for(int z=pix;z<(pix+PIX_PER_HOLE);z++){
              strip.setPixelColor(z, PixelColorBlue);
            }
          }else{
            for(int z=pix;z<(pix+PIX_PER_HOLE);z++){
              strip.setPixelColor(z, PixelColorOff);
            }
          }
          pix+=PIX_PER_HOLE;
          pix+=OFFSET[y][x];
        }
        upward = false;
      }
      else{
        // downwards
        for (int y = 0; y < yLength; y++)
        {
          if (board[y][x] == 1){
            for(int z=pix;z<(pix+PIX_PER_HOLE);z++){
              strip.setPixelColor(z, PixelColorRed);
            }
          }else if (board[y][x] == 2){
            for(int z=pix;z<(pix+PIX_PER_HOLE);z++){
              strip.setPixelColor(z, PixelColorBlue);
            }
          }else{
            for(int z=pix;z<(pix+PIX_PER_HOLE);z++){
              strip.setPixelColor(z, PixelColorOff);
            }
          }
          pix+=PIX_PER_HOLE;
          pix+=OFFSET[y][x];
        }
        upward = true;
      }
    }
    strip.show();
    updateLED=false;
  }

  // Log.info("Sending Hello World to the cloud");
  // Particle.publish("Hello world");
   delay(50); // milliseconds and blocking
}

//returns lowest empty row
int getLowestEmptyRow(int col) {
  for (int row = yLength-1; row >= 0; row--) {
  //for(let row=0;row<yLength;row++){
      if (board[row][col] == 0){
        String temp = ((String)col+","+(String)row);
        Serial.println(temp);
        return row;
      }
  }
  Serial.println("Row Full at Col: "+(String)col);
  return -1;
}

//gets webpage coordinate string as row,col and sets array accordingly
//error if this cloud function too long?
int setBoardFromString(String inputString){
  Serial.println("Cloud: "+inputString);

  //find location of comma
  int loc=0;
  for(int x=0;x<(int)inputString.length();x++){
    if(inputString.charAt(x)==','){
      loc=x;
      break;
    }
  }
  int row=atoi(inputString.substring(0,loc));
  int col=atoi(inputString.substring(loc+1,inputString.length()));
  board[row][col]=1;
  updateLED=true;
  return 1;
}

//resets board
int resetBoard(String inputString){
  for(int x=0;x<xLength;x++){
    for(int y=0;y<yLength;y++){
      board[y][x]=0;
    }
  }
  sendCoord="null";
  updateLED=true;
  Serial.println("Reset Board");
  return 1;
}