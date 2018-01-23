#include <EasyTransfer.h>
#include <DebouncedSwitch.h>
#include <LiquidCrystal.h>
#include <Encoder.h>


// DATA TYPES
enum mode
{ 
  idle, play, pause
};

struct story
{
  char title[20];
  char author[20];
  char reader[20];
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
 // set up fake stories
story stories[4] = {
  {"The Wind in the Willows", "Kenneth Grahame", "Cousin Riley"},
  {"Winnie the Pooh", "AA Milne", "Cousin Riley"},
  {"The Cat in the Hat", "Dr Seuss", "Cousin Marlowe"},
  {"Hairy MacLary", "Lyndley Dodd", "Auntie Cassie"}
};

// play button pause mode flashing
unsigned long previousFlashPollTime = 0;
const long flashInterval = 500;

// encoder position
long encoderPos = -999;

// current story
int currentStoryIndex = 0;




void setup()
{
  // enable LEDs for front buttons
  pinMode(PLAYLED, OUTPUT); // initially low (OFF)
  pinMode(STOPLED, OUTPUT); // initially low (OFF)

  // set up LCD
  lcd.begin(20, 4);

  // start machine in idle mode
  stopButtonPressed();
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
  long newEncoderPos;
  newEncoderPos = knob.read();
  if (newEncoderPos != encoderPos) {
    if(newEncoderPos > encoderPos)
    {
      currentStoryIndex++;
    } else {
      currentStoryIndex--;
    }
    if(currentStoryIndex < 0)
    {
      currentStoryIndex = 0;
    }
    if(currentStoryIndex > sizeof(stories)-1)
    {
      currentStoryIndex = sizeof(stories)-1;
    }
    encoderPos = newEncoderPos;
    displayStory();
  }
}

void playPauseButtonPressed()
{
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
  digitalWrite(STOPLED, HIGH);
  digitalWrite(PLAYLED, LOW);
  currentMode = idle;
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

