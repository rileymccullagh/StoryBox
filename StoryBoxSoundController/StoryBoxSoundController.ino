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

File metadata;

RECIEVE command;


void setup() {
  // begin debug serial
  Serial.begin(9600);

  // set up SD card
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);

  // check if SD works
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      error("No SD Card!");
    }
  }

  // open the metadata on the file
  metadata = SD.open("metadata.txt");
  if (metadata) {
    Serial.println("metadata.txt:");
    
    // read from the file until there's nothing else in it:
    while (metadata.available()) {
      Serial.write(metadata.read());
    }
    // close the file:
    metadata.close();
  } else {
    while (1) {
      error("No metadata!");
    }
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

void error(char error[]) {
  Serial.println(error);
}




