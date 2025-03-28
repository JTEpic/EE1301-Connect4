// Include Particle Device OS APIs
#include "Particle.h"

// Let Device OS manage the connection to the Particle Cloud
SYSTEM_MODE(AUTOMATIC);

// Run the application and system concurrently in separate threads
SYSTEM_THREAD(ENABLED);

// Show system, cloud connectivity, and application logs over USB
// View logs with CLI using 'particle serial monitor --follow'
SerialLogHandler logHandler(LOG_LEVEL_INFO);

const int xLength=7,yLength=6;
int board[yLength][xLength];

int pinCol[xLength]={D7,0,0,0,0,0,0};

void setup() {
  //reset array on boot, (0=open, 1=player 1, 2=player 2)
  for(int x=0;x<xLength;x++){
    for(int y=0;y<yLength;y++){
      board[x][y]=0;
    }
  }
  for(int x=0;x<xLength;x++){
    pinMode(pinCol[x], INPUT);
  }
  Serial.begin(9600);
}

void loop() {
  int data0 = analogRead(pinCol[0]);
  // Log.info("Sending Hello World to the cloud");
  // Particle.publish("Hello world");
  // delay( 10 * 1000 ); // milliseconds and blocking
}