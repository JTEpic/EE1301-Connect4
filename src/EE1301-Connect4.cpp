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
const int pinCol[xLength]={D7,0,0,0,0,0,0};
int pinData[xLength];
const int LIGHT_SENSOR_THRESHOLD=2500;
String sendCoord="null";

// IMPORTANT: Set pixel COUNT, PIN and TYPE
int PIXEL_COUNT = xLength*yLength;
#define PIXEL_PIN SPI // Only use SPI or SPI1 on Photon 2 (SPI is MO or S0 pin; SPI1 is D2)
                      // NOTE: On the Photon 2, this must be a compiler constant!
int PIXEL_TYPE = WS2812;

Adafruit_NeoPixel strip = Adafruit_NeoPixel(PIXEL_COUNT, PIXEL_PIN, PIXEL_TYPE);
//Setup some colors, RGB version
/*int PixelColorCyan = strip.Color(   0, 100, 100);
int PixelColorRed  = strip.Color(  80,   0,   0);
int PixelColorGold = strip.Color(  60,  50,   5);*/
  
//Setup some colors, GRB version
int PixelColorRed = strip.Color(    0,  100, 0  );
int PixelColorBlue  = strip.Color(  0,  0,   100);
int PixelColorOff = strip.Color(    0,  0,   0  );

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
  for(int x=0;x<xLength*yLength;x++){
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
  if(pinPrev!=-1&&pinData[pinPrev] < LIGHT_SENSOR_THRESHOLD){
    pinPrev=-1;
  }

  //goes through all sensors to see if one is covered
  if(pinPrev==-1){
    for(int x = 0; x < xLength; x++){
      pinData[x] = analogRead(pinCol[x]);
      if(pinData[x] > LIGHT_SENSOR_THRESHOLD){
        pinPrev=x;
        int row = getLowestEmptyRow(x);
        if(row != -1){
          board[row][x] = 2;
          sendCoord = row+","+x;
        }
        break;
      }
    }
  }

  //set light color according to array,add short delay?
  int pix=0;
  for(int x=0;x<xLength;x++){
    for(int y=0;y<yLength;y++){
      if(board[y][x]==1)
        strip.setPixelColor(x,PixelColorRed);
      else if(board[y][x]==2){
        strip.setPixelColor(x,PixelColorBlue);
      }else{
        strip.setPixelColor(x,PixelColorOff);
      }
      pix++;
    }
  }
  strip.show();

  // Log.info("Sending Hello World to the cloud");
  // Particle.publish("Hello world");
  // delay(1000); // milliseconds and blocking
}

//returns lowest empty row
int getLowestEmptyRow(int col) {
  for (int row = yLength-1; row >= 0; row--) {
  //for(let row=0;row<yLength;row++){
      if (board[row][col] == 0){
        Serial.println(col+","+row);
        return row;
      }
  }
  return -1;
}

//gets webpage coordinate string as row,col and sets array accordingly
//error if this cloud function too long?
int setBoardFromString(String inputString){
  Serial.println(inputString);

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
  return 1;
}

//resets board
int resetBoard(String inputString){
  for(int x=0;x<xLength;x++){
    for(int y=0;y<yLength;y++){
      board[y][x]=0;
    }
  }
  return 1;
}