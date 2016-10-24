#include <Wire.h>
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_GFX.h"
#include "Arduino.h"
#include "Keypad.h"
#include <stdlib.h>

/* definitions and variables related to the keypad */
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 6, 7, 8}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {2, 3, 4}; //connect to the column pinouts of the keypad
char todisplay = '-';
Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );


/* definitions and variables related to the screen display */
#define SCREEN_WIDTH 17
#define CHAR_WIDTH 6
Adafruit_8x16matrix matrix = Adafruit_8x16matrix();
int spacing = 0;
char scrolltext[100];

/* definitions and variables related to game semantics */
#define NUM_GUESSES 8
#define NUM_CODE_BITS 8
byte guessCounter = 0;
bool secretCode[NUM_CODE_BITS];
bool guessHistory[NUM_GUESSES][NUM_CODE_BITS];
bool correctCount[NUM_GUESSES][NUM_CODE_BITS];
bool isGameWon = false;
bool isGameLost = false;


void scroll_text(String toscroll) {
  matrix.clear();
  int spacing = toscroll.length()*CHAR_WIDTH;
  matrix.setTextSize(1);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(LED_ON);
  matrix.setRotation(1);
  for (int x=(SCREEN_WIDTH); x>(0-(spacing)); x--) {
    matrix.clear();
    matrix.setCursor(x,0);
    matrix.print(toscroll);
    matrix.writeDisplay();
    delay(80);
    //read_and_store_key();
  }
}

void pick_a_code(){
  for(byte i=0;i<NUM_CODE_BITS;i++){
    secretCode[i] = random(2);
  }
}

void reset_the_board(){
  randomSeed(analogRead(0));
  pick_a_code();
  guessCounter = 0;
  isGameWon = false;
  isGameLost = false;
  for(byte i=0;i<NUM_GUESSES;i++){
    for(byte t=0;t<NUM_CODE_BITS;t++){
      guessHistory[i][t]=0;
      correctCount[i][t]=0;
    }
  }
  matrix.clear();
}

void display_secretCode(){
  Serial.print("------------\nSecret Code:\n");
  for(byte guessBit=0;guessBit<NUM_CODE_BITS;guessBit++){
    Serial.print(secretCode[guessBit]); 
  }
  Serial.print("\n");
  Serial.print("-------------\n");
}

void display_guessHistory(){
  Serial.print("------------\nGuess History:\n");
  for(byte guessNumber=0;guessNumber<NUM_GUESSES;guessNumber++){
    for(byte guessBit=0;guessBit<NUM_CODE_BITS;guessBit++){
      Serial.print(guessHistory[guessNumber][guessBit]); 
    }
    Serial.print("\n");
  }
  Serial.print("-------------\n");
}

void display_correctCount(){
  Serial.print("------------\nCorrect Count:\n");
  for(byte guessNumber=0;guessNumber<NUM_GUESSES;guessNumber++){
    for(byte guessBit=0;guessBit<NUM_CODE_BITS;guessBit++){
      Serial.print(correctCount[guessNumber][guessBit]); 
    }
    Serial.print("\n");
  }
  Serial.print("-------------\n");
}

void render_the_board(){
  matrix.setRotation(1);
  matrix.clear();
  for(byte guessNumber=0;guessNumber<NUM_GUESSES;guessNumber++){
    for(byte guessBit=0;guessBit<NUM_CODE_BITS;guessBit++){
      byte xGuess = guessBit;
      byte xCorrect = guessBit+8;
      byte y = guessNumber;   
      matrix.drawPixel(xGuess,y,guessHistory[guessNumber][guessBit]?LED_ON:LED_OFF);
      matrix.drawPixel(xCorrect,y,correctCount[guessNumber][guessBit]?LED_ON:LED_OFF);
    }
  }
  matrix.writeDisplay();
}


bool score_the_row(byte guessCounter){
  byte numberCorrect = 0;
  for(byte i=0; i<NUM_CODE_BITS; i++){
    if(secretCode[i] == guessHistory[guessCounter][i]){
      //increment the number guesses and turn on the associated correct count led
      correctCount[guessCounter][NUM_CODE_BITS-numberCorrect-1] = true;
      numberCorrect++;
    }
  }
  sprintf(scrolltext, "you got %d correct", numberCorrect);
  Serial.println(scrolltext);
  display_secretCode();
  display_guessHistory();
  display_correctCount();
  return (numberCorrect == NUM_CODE_BITS);
}

void process_winner(){
  isGameWon = true;
  sprintf(scrolltext, "You won in %d guesses", guessCounter);
  scroll_text(scrolltext);  
}

void process_loser(){
  isGameLost = false;
  sprintf(scrolltext, "You Lost.", guessCounter);
  scroll_text(scrolltext);  
}

void read_and_render(){
  char key = keypad.getKey();
  if (key == '1'){
    guessHistory[guessCounter][0] = 1;
  }
  if (key == '2'){
    guessHistory[guessCounter][1] = 1;
  }
  if (key == '3'){
    guessHistory[guessCounter][2] = 1;
  } 
  if (key == '4'){
    guessHistory[guessCounter][3] = 1;
  } 
  if (key == '5'){
    guessHistory[guessCounter][4] = 1;
  } 
  if (key == '6'){
    guessHistory[guessCounter][5] = 1;
  } 
  if (key == '7'){
    guessHistory[guessCounter][6] = 1;
  } 
  if (key == '8'){
    guessHistory[guessCounter][7] = 1;
  }  
  if (key == '9'){
    //reset the row
    for(byte i=0;i<NUM_CODE_BITS;i++){
      guessHistory[guessCounter][i] = 0;  
    }
  } 
  if (key == '#'){
    //score the row
    bool isWinner = score_the_row(guessCounter);
    guessCounter++;
    if(isWinner){
      process_winner();  
    }
    else if(guessCounter >= NUM_GUESSES){
      process_loser();
    }
  }
  if (key == '*'){
    //Start a New Game
    reset_the_board();
  }
  render_the_board();    
}
void setup() {
  // put your setup code here, to run once:
  Serial.begin(9600);
  Serial.println("MasterMind2");
  matrix.begin(0x70);  // pass in the address
  scroll_text(">MasterMind<");
  reset_the_board();
}

void loop() {
  
  read_and_render();
  delay(30);
}
