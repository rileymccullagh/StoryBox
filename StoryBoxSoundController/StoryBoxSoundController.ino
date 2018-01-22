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

struct RECIEVE {
  byte action;
};

AudioPlaySdWav           playSdWav1;
AudioOutputI2S           i2s1;
AudioConnection          patchCord1(playSdWav1, 0, i2s1, 0);
AudioConnection          patchCord2(playSdWav1, 1, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;

EasyTransfer             ET; 

RECIEVE command;


void setup() {
  AudioMemory(8);
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.6);
  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    while (1) {
      //cannot access sd card error
    }
  }
  
  Serial1.begin(9600);
  ET.begin(details(command), &Serial1);
}

void loop() {
  if(ET.receiveData()){
    if(command.action == 1){
      playSdWav1.play("boot.wav");
    }
  }
}




