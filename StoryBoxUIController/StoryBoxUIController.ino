#include <EasyTransfer.h>
#include <DebouncedSwitch.h>
#include <LiquidCrystal.h>
#include <Encoder.h>


// DATA TYPES
enum mode
{ 
  idle, play, pause
};

struct STORY
{
  char title[20];
  char author[20];
  char reader[20];
};

// EasyTransfer data structures
struct SEND {
  char action;
};

// PIN DEFINITIONS
const byte ENC1 = 2;     // encoder 1
const byte ENC2 = 3;     // encoder 2
const byte PLAYLED = 4;  // green light
const byte STOPLED = 13; // red light
const byte PLAYBTN = 5;  // green button
const byte STOPBTN = 6;  // red button
const byte RS = 7;       // LCD RS
const byte EN = 8;       // LCD EN
const byte DB4 = 9;      // LCD data
const byte DB5 = 10;     // LCD data
const byte DB6 = 11;     // LCD data
const byte DB7 = 12;     // LCD data

// HARDWARE LIBRARIES
DebouncedSwitch playButton(PLAYBTN);           // Debouncer Library
DebouncedSwitch stopButton(STOPBTN);           // Debouncer Library

LiquidCrystal lcd(RS, EN, DB4, DB5, DB6, DB7); // LCD

Encoder knob(3,2);                             // Rotary encoder

EasyTransfer ET;                               // transfer between UIController and SoundController

// GLOBALS
mode currentMode = idle; // global machine mode

// currently selected story
STORY story;

// command to send to soundcontroller
SEND command;

// play button pause mode flashing
unsigned long previousFlashPollTime = 0;
const long flashInterval = 500;

// encoder position
long encoderPos = -999;

// current story
int currentStoryIndex = 0;






void setup()
{
  // set up LCD
  lcd.begin(20, 4);
  lcd.clear();
  
  // enable LEDs for front buttons
  pinMode(PLAYLED, OUTPUT); // initially low (OFF)
  pinMode(STOPLED, OUTPUT); // initially low (OFF)

  // start machine in idle mode
  stopButtonPressed();

  // start easytransfer connection with SoundController
  Serial.begin(9600);
  ET.begin(details(command), &Serial);
}

void loop()
{  
  playButton.update(); // call these every loop to update switch state
  stopButton.update(); 
  
  if (playButton.isChanged() && playButton.isDown()) { // debounced switch changed state Up or Down
    playPauseButtonPressed();
  }
  if (stopButton.isChanged() && stopButton.isDown()) {
    stopButtonPressed();
  }
  
  // flash the pause LED
  if(currentMode == pause)
  {
    unsigned long flashPollTime = millis();
    if (flashPollTime - previousFlashPollTime >= flashInterval)
    {
      // save the last time LED flashed
      previousFlashPollTime = flashPollTime;

      digitalWrite(PLAYLED, !digitalRead(PLAYLED));
    }
  }

  // poll encoder position
  long change = encoderPos - knob.read();
  
  if (change < -1) {
    sendCommand('f');
  }
  if(change > 1) {
    sendCommand('r');
  }
}

void playPauseButtonPressed()
{
  sendCommand('p');
  switch(currentMode)
  {
    case play:
      currentMode = pause;
      break;
    case pause:
    case idle:
      currentMode = play;
      digitalWrite(STOPLED, LOW);
      digitalWrite(PLAYLED, HIGH);
      break;
    default:
      return;
  }
}

void stopButtonPressed()
{
  sendCommand('s');
  digitalWrite(STOPLED, HIGH);
  digitalWrite(PLAYLED, LOW);
  currentMode = idle;
}

void sendCommand(char action) {
  
}

void displayStory()
{
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print(stories[currentStoryIndex].title);
  lcd.setCursor(0,1);
  lcd.print(stories[currentStoryIndex].author);
  lcd.setCursor(0,2);
  lcd.print("read by");
  lcd.setCursor(0,3);
  lcd.print(stories[currentStoryIndex].reader);
}

