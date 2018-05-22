#include <EEPROM.h>
#include <MaxMatrix.h>
#include "LedControl.h"
#include <avr/pgmspace.h>
#include "pitches.h"
//#include <MsTimer2.h> //https://kocoafab.cc/tutorial/view/460

#define maxDisplays 1 // Number of MAX7219's in use.
#define ir_a0 A0
#define button 2 // 버튼 핀 지정
unsigned long game_time =  60000;
// http://www.hardcopyworld.com/ngine/aduino/index.php/archives/686 내부 풀업 저항

byte Buf7219[7]; // "width,height,data[5]" single character buffer.
const int data  = 12;     // DIN or MOSI
const int load  = 10;     // CS
const int clock = 11;     // SCK
int speakerPin = 3; // 원래 3
unsigned long delaytime=100;

// Initial value


byte text_stop = 0;
boolean start_flag = false;
int ball_point = 0;
int prev_col = 0;
int val = 0;
int lasttime = 10;
unsigned long times = 0;
unsigned long prevtimes = 0;
unsigned long prevtimes2 = 0;
volatile byte state = LOW;

MaxMatrix m(data, load, clock, maxDisplays);
// Data array is stored in program memory (see memcpy_P for access).
// Parameters are width, height, character data...
// There is a speed improvement for characters with height 8 bits see lib.

LedControl lc=LedControl(12,11,10,1);

PROGMEM const unsigned char CH[] = {
3, 8, B0000000, B0000000, B0000000, B0000000, B0000000, // space
1, 8, B1011111, B0000000, B0000000, B0000000, B0000000, // !
3, 8, B0000011, B0000000, B0000011, B0000000, B0000000, // "
5, 8, B0010100, B0111110, B0010100, B0111110, B0010100, // #
4, 8, B0100100, B1101010, B0101011, B0010010, B0000000, // $
5, 8, B1100011, B0010011, B0001000, B1100100, B1100011, // %
5, 8, B0110110, B1001001, B1010110, B0100000, B1010000, // &
1, 8, B0000011, B0000000, B0000000, B0000000, B0000000, // '
3, 8, B0011100, B0100010, B1000001, B0000000, B0000000, // (
3, 8, B1000001, B0100010, B0011100, B0000000, B0000000, // )
5, 8, B0101000, B0011000, B0001110, B0011000, B0101000, // * 
5, 8, B0001000, B0001000, B0111110, B0001000, B0001000, // +
2, 8, B10110000, B1110000, B0000000, B0000000, B0000000, // ,
4, 8, B0001000, B0001000, B0001000, B0001000, B0000000, // -
2, 8, B1100000, B1100000, B0000000, B0000000, B0000000, // .
4, 8, B1100000, B0011000, B0000110, B0000001, B0000000, // /
4, 8, B0111110, B1000001, B1000001, B0111110, B0000000, // 0
3, 8, B1000010, B1111111, B1000000, B0000000, B0000000, // 1
4, 8, B1100010, B1010001, B1001001, B1000110, B0000000, // 2
4, 8, B0100010, B1000001, B1001001, B0110110, B0000000, // 3
4, 8, B0011000, B0010100, B0010010, B1111111, B0000000, // 4
4, 8, B0100111, B1000101, B1000101, B0111001, B0000000, // 5
4, 8, B0111110, B1001001, B1001001, B0110000, B0000000, // 6
4, 8, B1100001, B0010001, B0001001, B0000111, B0000000, // 7
4, 8, B0110110, B1001001, B1001001, B0110110, B0000000, // 8
4, 8, B0000110, B1001001, B1001001, B0111110, B0000000, // 9
2, 8, B01010000, B0000000, B0000000, B0000000, B0000000, // :
2, 8, B10000000, B01010000, B0000000, B0000000, B0000000, // ;
3, 8, B0010000, B0101000, B1000100, B0000000, B0000000, // <
3, 8, B0010100, B0010100, B0010100, B0000000, B0000000, // =
3, 8, B1000100, B0101000, B0010000, B0000000, B0000000, // >
4, 8, B0000010, B1011001, B0001001, B0000110, B0000000, // ?
5, 8, B0111110, B1001001, B1010101, B1011101, B0001110, // @
4, 8, B1111110, B0010001, B0010001, B1111110, B0000000, // A
4, 8, B1111111, B1001001, B1001001, B0110110, B0000000, // B
4, 8, B0111110, B1000001, B1000001, B0100010, B0000000, // C
4, 8, B1111111, B1000001, B1000001, B0111110, B0000000, // D
4, 8, B1111111, B1001001, B1001001, B1000001, B0000000, // E
4, 8, B1111111, B0001001, B0001001, B0000001, B0000000, // F
4, 8, B0111110, B1000001, B1001001, B1111010, B0000000, // G
4, 8, B1111111, B0001000, B0001000, B1111111, B0000000, // H
3, 8, B1000001, B1111111, B1000001, B0000000, B0000000, // I
4, 8, B0110000, B1000000, B1000001, B0111111, B0000000, // J
4, 8, B1111111, B0001000, B0010100, B1100011, B0000000, // K
4, 8, B1111111, B1000000, B1000000, B1000000, B0000000, // L
5, 8, B1111111, B0000010, B0001100, B0000010, B1111111, // M
5, 8, B1111111, B0000100, B0001000, B0010000, B1111111, // N
4, 8, B0111110, B1000001, B1000001, B0111110, B0000000, // O
4, 8, B1111111, B0001001, B0001001, B0000110, B0000000, // P
4, 8, B0111110, B1000001, B1000001, B10111110, B0000000, // Q
4, 8, B1111111, B0001001, B0001001, B1110110, B0000000, // R
4, 8, B1000110, B1001001, B1001001, B0110010, B0000000, // S
5, 8, B0000001, B0000001, B1111111, B0000001, B0000001, // T
4, 8, B0111111, B1000000, B1000000, B0111111, B0000000, // U
5, 8, B0001111, B0110000, B1000000, B0110000, B0001111, // V
5, 8, B0111111, B1000000, B0111000, B1000000, B0111111, // W
5, 8, B1100011, B0010100, B0001000, B0010100, B1100011, // X
5, 8, B0000111, B0001000, B1110000, B0001000, B0000111, // Y
4, 8, B1100001, B1010001, B1001001, B1000111, B0000000, // Z
2, 8, B1111111, B1000001, B0000000, B0000000, B0000000, // [
4, 8, B0000001, B0000110, B0011000, B1100000, B0000000, // backslash
2, 8, B1000001, B1111111, B0000000, B0000000, B0000000, // ]
3, 8, B0000010, B0000001, B0000010, B0000000, B0000000, // hat
4, 8, B1000000, B1000000, B1000000, B1000000, B0000000, // _
2, 8, B0000001, B0000010, B0000000, B0000000, B0000000, // `
4, 8, B0100000, B1010100, B1010100, B1111000, B0000000, // a
4, 8, B1111111, B1000100, B1000100, B0111000, B0000000, // b
4, 8, B0111000, B1000100, B1000100, B0000000, B0000000, // c // JFM MOD.
4, 8, B0111000, B1000100, B1000100, B1111111, B0000000, // d
4, 8, B0111000, B1010100, B1010100, B0011000, B0000000, // e
3, 8, B0000100, B1111110, B0000101, B0000000, B0000000, // f
4, 8, B10011000, B10100100, B10100100, B01111000, B0000000, // g
4, 8, B1111111, B0000100, B0000100, B1111000, B0000000, // h
3, 8, B1000100, B1111101, B1000000, B0000000, B0000000, // i
4, 8, B1000000, B10000000, B10000100, B1111101, B0000000, // j
4, 8, B1111111, B0010000, B0101000, B1000100, B0000000, // k
3, 8, B1000001, B1111111, B1000000, B0000000, B0000000, // l
5, 8, B1111100, B0000100, B1111100, B0000100, B1111000, // m
4, 8, B1111100, B0000100, B0000100, B1111000, B0000000, // n
4, 8, B0111000, B1000100, B1000100, B0111000, B0000000, // o
4, 8, B11111100, B0100100, B0100100, B0011000, B0000000, // p
4, 8, B0011000, B0100100, B0100100, B11111100, B0000000, // q
4, 8, B1111100, B0001000, B0000100, B0000100, B0000000, // r
4, 8, B1001000, B1010100, B1010100, B0100100, B0000000, // s
3, 8, B0000100, B0111111, B1000100, B0000000, B0000000, // t
4, 8, B0111100, B1000000, B1000000, B1111100, B0000000, // u
5, 8, B0011100, B0100000, B1000000, B0100000, B0011100, // v
5, 8, B0111100, B1000000, B0111100, B1000000, B0111100, // w
5, 8, B1000100, B0101000, B0010000, B0101000, B1000100, // x
4, 8, B10011100, B10100000, B10100000, B1111100, B0000000, // y
3, 8, B1100100, B1010100, B1001100, B0000000, B0000000, // z
3, 8, B0001000, B0110110, B1000001, B0000000, B0000000, // {
1, 8, B1111111, B0000000, B0000000, B0000000, B0000000, // |
3, 8, B1000001, B0110110, B0001000, B0000000, B0000000, // }
4, 8, B0001000, B0000100, B0001000, B0000100, B0000000, // ~
};

// notes in the melody: 7
int goal_melody[] = {
 NOTE_C5, NOTE_D5,NOTE_E5, NOTE_F5, NOTE_G5, NOTE_A5, NOTE_B5
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int goal_noteDurations[] = {
   16,16,32,64,64,16,8
};

// Mario - 80
int mario_melody[] = {
  NOTE_E7, NOTE_E7, 0, NOTE_E7, 
  0, NOTE_C7, NOTE_E7, 0,
  NOTE_G7, 0, 0,  0,
  NOTE_G6, 0, 0, 0, 

  NOTE_C7, 0, 0, NOTE_G6, 
  0, 0, NOTE_E6, 0, 
  0, NOTE_A6, 0, NOTE_B6, 
  0, NOTE_AS6, NOTE_A6, 0, 

  NOTE_G6, NOTE_E7, NOTE_G7, 
  NOTE_A7, 0, NOTE_F7, NOTE_G7, 
  0, NOTE_E7, 0,NOTE_C7, 
  NOTE_D7, NOTE_B6, 0, 0,

  NOTE_C7, 0, 0, NOTE_G6, 
  0, 0, NOTE_E6, 0, 
  0, NOTE_A6, 0, NOTE_B6, 
  0, NOTE_AS6, NOTE_A6, 0, 

  NOTE_G6, NOTE_E7, NOTE_G7, 
  NOTE_A7, 0, NOTE_F7, NOTE_G7, 
  0, NOTE_E7, 0,NOTE_C7, 
  NOTE_D7, NOTE_B6, 0, 0
};
int mario_noteDurations[] = {
  12, 12, 12, 12, 
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12, 

  12, 12, 12, 12,
  12, 12, 12, 12, 
  12, 12, 12, 12, 
  12, 12, 12, 12, 

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12,

  9, 9, 9,
  12, 12, 12, 12,
  12, 12, 12, 12,
  12, 12, 12, 12
};

// ending 8
int ending_melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
int ending_noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

// starting 7
int start_melody[] = {
  NOTE_C4,0, NOTE_C4,0, NOTE_C4,0, NOTE_C6
};
int start_noteDurations[] = {
  4, 4, 4, 4, 4, 4, 2
};

void play_song(int melody[],int noteDurations[],int total_tones) {
  // iterate over the notes of the melody:
  for (int thisNote = 0; thisNote < total_tones; thisNote++) {
    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000 / noteDurations[thisNote];  
    tone(speakerPin, melody[thisNote], noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    //noTone(speakerPin);
  }
  pinMode(speakerPin,INPUT); // 부저 노이즈를 줄이기 위해 사용했는데 좋음
}

void blink() {
  state = !state;
  //Serial.println(state);
  text_stop++;
  //Serial.print("text_stop=");
  //Serial.println(text_stop);  
  if (text_stop == 2) start_flag=true;
  //Serial.print("start_flag=");
  //Serial.println(start_flag);
}

void setup() {
  //pinMode(LED, OUTPUT);
  m.init();
  m.setIntensity(3);  
  pinMode(button, INPUT_PULLUP); //4번핀과 GND 연결로 항상 1 값 출력, 버튼 눌릴 경우 0값 Detect
  
  Serial.begin(9600);
  Serial.println(digitalRead(button));
  //Serial.print("EEPROM Value : ");
  //EEPROM.write(0,0);
  

  attachInterrupt(digitalPinToInterrupt(button), blink, CHANGE);
  //play_song(mario_melody,mario_noteDurations,80);  

}



// Scrolling Text
//char string[] = "The quick brown fox juumped over the lazy DOG.;:@'+=-/&";  
byte buffer[10]; 
char string[] = "Game Over  ";
char msg[] = "AB12\0";

void printCharWithShift(char c, int shift_speed)
{
  if (c < 32) return;
  c -= 32;
  memcpy_P(Buf7219, CH + 7*c, 7);
  m.writeSprite(maxDisplays*8, 0, Buf7219);
  m.setColumn(maxDisplays*8 + Buf7219[0], 0);


  for (int i=0; i<=Buf7219[0]; i++) 
  {
    delay(shift_speed);
    m.shiftLeft(false, false);
    if (text_stop == 2) {
      m.init();
      return;
     }
  }

}

void printStringWithShift(char* s, int shift_speed)
{
  while (*s != 0)
  {
    printCharWithShift(*s, shift_speed);
    s++;
    if (text_stop == 2) {
      m.init();
      return;
     }
  }
}

void printString(char* s) 
{ 
  int col = 0; 
  while (*s != 0) { 
    if (*s < 32) 
    continue; 
    char c = *s - 32; 
    memcpy_P(buffer, CH + 7*c, 7); 
    m.writeSprite(2, 0, buffer); 
    m.setColumn(2 + buffer[0], 0);    
    //m.writeSprite(col+2, 0, buffer); 
    //m.setColumn(col+2 + buffer[0], 0); 
    col += buffer[0] + 1; s++;
    delay(2000);
    m.init();  
   } 
}

void rows() {
  for(int row=0;row<8;row++) {
    delay(delaytime);
    lc.setRow(0,row,B10100000);
    delay(delaytime);
    lc.setRow(0,row,(byte)0);
    for(int i=0;i<row;i++) {
      delay(delaytime);
      lc.setRow(0,row,B10100000);
      delay(delaytime);
      lc.setRow(0,row,(byte)0);
    }
  }
}
void columns() {
  for(int col=0;col<8;col++) {
    delay(delaytime);
    lc.setColumn(0,col,B10100000);
    delay(delaytime);
    lc.setColumn(0,col,(byte)0);
    for(int i=0;i<col;i++) {
      delay(delaytime);
      lc.setColumn(0,col,B10100000);
      delay(delaytime);
      lc.setColumn(0,col,(byte)0);
    }
  }
}

void single(int a) {
  int prev_row = int(a / 8);
  if ( a % 8 == 0) { 
    prev_row--;
    }
  prev_col = (a-1) % 8;
  
  for(int row=prev_row;row<prev_row+1;row++) {
    for(int col=prev_col;col<prev_col+1;col++) {       
      delay(delaytime);
      lc.setLed(0,row,col,true);
      delay(delaytime);
      for(int i=0;i<2;i++) {
        lc.setLed(0,row,col,false);
        delay(delaytime);
        lc.setLed(0,row,col,true);
        delay(delaytime);
      }
    }
  }
  prev_col = a;
}

void single_del() {
  for(int row=0;row<8;row++) {
    for(int col=0;col<8;col++) {
      lc.setLed(0,row,col,false);
    }
  }  
}


void time_dot(int lt) {
  switch(lt) {
    case 10:
      lc.setRow(0,6,B11000000);
      lc.setRow(0,7,B11111111);
      break;
    case 9:
      lc.setRow(0,6,B10000000);
      lc.setRow(0,7,B11111111);
      break;
    case 8:
      lc.setRow(0,6,B00000000);
      lc.setRow(0,7,B11111111);
      break;
    case 7:
      lc.setRow(0,6,B00000000);
      lc.setRow(0,7,B11111110);
      break;   
    case 6:
      lc.setRow(0,6,B00000000);
      lc.setRow(0,7,B11111100);
      break;     
    case 5:
      lc.setRow(0,6,B00000000);
      lc.setRow(0,7,B11111000);
      break;     
    case 4:
      lc.setRow(0,6,B00000000);
      lc.setRow(0,7,B11110000);
      break; 
    case 3:
      lc.setRow(0,6,B00000000);
      lc.setRow(0,7,B11000000);
      break; 
    case 2:
      lc.setRow(0,6,B00000000);
      lc.setRow(0,7,B10000000);
      break; 
    case 1:
      lc.setRow(0,6,B00000000);
      lc.setRow(0,7,B00000000);
      break; 
  }
}

void start() {
  while(!start_flag) {
    m.shiftLeft(false, true);
    char b[11];
    String str;
    str="Ready " + String(EEPROM.read(0))+ "  ";
    str.toCharArray(b,11); 
    printStringWithShift(b,100);     
  }
  play_song(start_melody,start_noteDurations,7);
  start_flag = true;
  text_stop++;
  lc.setRow(0,6,B11111111);
  lc.setRow(0,7,B11111111);
  
}

void loop() {
  while (!start_flag) {
    start();
  }
  times = millis();
  prevtimes = prevtimes2 = times;
  /*
  Serial.print("timmer");
  Serial.println(times);
  if (times != prevtimes) {
    prevtimes = times;
    time_dot(times);    
  }
  */

  while ((times = millis()-prevtimes) < game_time) {
    int r = analogRead(ir_a0);
    times = (millis()-prevtimes)/1000;
    if (times < 50) {
      if (times > 1 & times <= 10) { lc.setRow(0,6,B11111110); }
      else if (times > 10 & times <= 20) { lc.setRow(0,6,B11111100); }
      else if (times > 20 & times <= 30) { lc.setRow(0,6,B11111000); }
      else if (times > 30 & times <= 40) { lc.setRow(0,6,B11110000); }
      else if (times > 40 & times <= 45) { lc.setRow(0,6,B11100000); }
      else if (times > 45 & times <= 50) { lc.setRow(0,6,B11000000); }
    }
    else {
      Serial.print("game_time/1000 - times = ");
      Serial.println(game_time/1000 - times);
      Serial.print("game_time = ");
      Serial.println(game_time);
      time_dot(game_time/1000 - times);
    }
    

    
    if (r < 950) {  }
    else{
      if (times > 50) { 
        game_time = game_time + 4000; 
        if (((game_time/1000) - times) > 10) {
          Serial.print("game_time = ");
          Serial.println(game_time); 
          Serial.print("times = ");
          Serial.println(times);           
          game_time = game_time - (((game_time/1000) - (times+10)) * 1000);         
        }
        Serial.print("calculated game_time = ");
        Serial.println(game_time); 
      }
      delay(100);
      //m.shiftLeft(false, true);
      //printStringWithShift(string,100); 
      ball_point++;
      single(ball_point);
      //Serial.print("Ball Point = ");
      //Serial.println(ball_point); 
      Serial.print("times = ");
      Serial.println((times = millis()-prevtimes)/1000);     
      play_song(goal_melody,goal_noteDurations,7);
      delay(50);
    }
    if(abs(r - val) > 0){
      val = r;
      delay(50);
    } 
  }

    start_flag = false;
    Serial.print("text_stop=");
    Serial.println(text_stop);    
    if (text_stop > 40) { 
      EEPROM.write(0,0); 
      }
    text_stop = 0; 
    
    prevtimes = 0;
    prevtimes2 = 0;
    prev_col = 0;
    val = 0;
    game_time = 60000;
    state = LOW;
    single_del();
    play_song(ending_melody,ending_noteDurations,8);   
    m.init();
    m.shiftLeft(false, true);
    char b[5];
    String str;
    str=String(ball_point)+"  ";
    str.toCharArray(b,5);    
    for(int i=0;i<3;i++) {
      m.shiftLeft(false, true);
      printStringWithShift(b,100);
    }

    if (EEPROM.read(0) < ball_point) {
      EEPROM.write(0,ball_point);
      play_song(mario_melody,mario_noteDurations,80);
    }    
    ball_point = 0;

}



