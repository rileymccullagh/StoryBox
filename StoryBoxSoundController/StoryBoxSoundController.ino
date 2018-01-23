#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>
#include <EasyTransfer.h>

// teensy audio shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

// global settings
#define MAX_STORIES      20

struct STORY {
  char title[20];
  char author[20];
  char reader[20];
  char filename[13];
};

// EasyTransfer data structures
struct RECIEVE {
  byte action;
};

// GLOBALS
AudioPlaySdWav           playSdWav1;
AudioOutputI2S           i2s1;
AudioConnection          patchCord1(playSdWav1, 0, i2s1, 0);
AudioConnection          patchCord2(playSdWav1, 1, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;

EasyTransfer             ET;

RECIEVE command;

STORY stories[MAX_STORIES];


void setup() {
  // begin debug serial
  Serial.begin(9600);

  // set up SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);

  // check if SD works
  if (!(SD.begin(SDCARD_CS_PIN))) {
    error("No SD Card!");
  }

  // open the metadata on the file
  File file = SD.open("metadata.txt");

  //read each line
  if (file) {
    int index = 0;
    char line[100];
    while(file.available() && index < 20){
      readLine(&file, line);
      if(line[0] != '#'){
        parseLine(line, &stories[index]);
        index++;
      }
      memset(line,'\0',sizeof(line));
    }
    
    // close the file:
    file.close();
  } else {
    while (1) {
      error("No metadata file!");
    }
  }

  for(int i = 0; i<20; i++) {
    Serial.print(i);
    Serial.print(": ");
    Serial.print(stories[i].title);
  }



  // start audio
  AudioMemory(8);
  sgtl5000_1.enable();
  // this volume seems to not distort
  sgtl5000_1.volume(0.6);
  
  // start easytransfer connection with UIController
  Serial1.begin(9600);
  ET.begin(details(command), &Serial1);

  // play boot sound to confirm this has all worked
  playSdWav1.play("boot.wav");
}

void loop() {
}

void readLine(File* file, char* line) {
  bool EOL = false;
  char buff;
  int index = 0;
  while(!EOL && file->available() && index < 100){
    buff = file->read();
    if(buff != '\n') {
      line[index] = buff;
    } else {
      EOL = true;
    }
    index++;
  }
}

void parseLine(char *line, STORY *story){
  Serial.println("parsing line");
  //char elements[4][20];
  //int elementsIndex = 0;
  int buffIndex = 0;
  char buff[20] = "";
  char elements[4][20];
  int elementsIndex = 0;
  
  for(int lineIndex = 0; lineIndex<100; lineIndex++) {
    if(line[lineIndex] == ','){
      Serial.println("Element:");
      Serial.println(buff);
      strncpy(elements[elementsIndex], buff, 20);
      memset(buff,'\0',sizeof(buff));
      buffIndex = 0;
      elementsIndex++;
    } else {
      if(buffIndex < 20){
        buff[buffIndex] = line[lineIndex];
        buffIndex++;
      }
    }
  }
  strncpy(story->title, elements[0], 20);
  strncpy(story->author, elements[1], 20);
  strncpy(story->reader, elements[2], 20);
  strncpy(story->filename, elements[3], 13);
}

void error(String error) {
  Serial.println(error);
  while(1){}
}
