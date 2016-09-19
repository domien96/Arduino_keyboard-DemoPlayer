/**
   Piano-keyboard simulator.
   7 buttons, 1 buzzer.
   ~~~~~~~~~~~~~~~~~~~~
   Author: Domien VS
   Date: 13/09/2016
*/

#include <LiquidCrystal_I2C.h>

/******************************* Variables **********************************/

/* Note frequencies, octave @ middle C/C4 */
// WARNING: FREQUENCIES IN 10Hz for memory efficiency reasons
int C = 2616, D = 2937, E = 3296, F = 3492, G = 3920, LA = 4400, B = 4939;
int Db = 2772, Eb = 3111, Gb = 3700, Ab = 4153, Bb = 4662;
/* IO pins */
int bzr = 9;
int btn = 10;
int keys = A3; // Pin connected to the 7 keyboard keys
// DURATION OF THE NOTES
#define BPM 100     // you can change this value changing all the others
#define H 2*Q //half 2/4
#define Q 60000/BPM //quarter 1/4 
#define Ei Q/2   //eighth 1/8
#define S Q/4 // sixteenth 1/16
#define W 4*Q // whole 4/4

/* Keyboard buttons ranges for 3.3V: 7 ranges to determine which button was pressed */
float threshold = 0.10 / 2; // Sensitivity from where we assume a button is pressed.

// Corresponding octave and note to play per button (index).
int keyNotes[] = { B,LA,G,F,E,D,C},
keyOctaves[] = {4, 4, 4, 4, 4, 4, 4};

// equals the lower limit of the interval where X is the resistance of the button, all resistors come together near a common resistor of 10K Ohms and then hit ground.
float keyIntervals[] = {threshold,  // R=330k
                        0.20, // R=100k
                        1.00, // R=10k
                        2.17, // R=2.2k
                        2.85, // R=1k
                        3.11, // R=220
                        (3.3 + 3.23) / 2 //R=0; avoid errors due to fluctuations (not always exact 3.3V) ; 3.23 is the value of the 220 Ohm resistor.
                       };

/* LCD */
int lcd_w = 20;
LiquidCrystal_I2C lcd(0x27, lcd_w, 4);

/*****************************FROM HERE NO ADJUSTMENTS************************************************/

/* Service functions */

/**
   Synchroon, wacht dus to t noot voleldig is gespeeld.
*/
void playNote(int pin, float noteName, int octaveIndex, int duration)  {
  // octave 4 is middle octave
  tone(pin, (float) noteName * pow(2, octaveIndex - 4)/10, duration);
  delay(duration);
  noTone(pin);
};

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

/* Determine in which of the 7 intervals the value falls into. */
int getInterval(float x) {
  // Binary search 7 intervals
  if (x < keyIntervals[4]) {
    if (x < keyIntervals[2]) {
      if (x < keyIntervals[1])
        return 0;
      else
        return 1;
    }
    else { //2..3
      if (x < keyIntervals[3])
        return 2;
      else
        return 3;
    }
  } else { // 4..7
    if (x < keyIntervals[6]) {
      if (x < keyIntervals[5])
        return 4;
      else
        return 5;
    }
    else { //6..7
      return 6;
    }
  }
};

void clearLcdLine(int line) {
  for(int i=0;i<lcd_w;i++) {
    lcd.setCursor(i,line);
    lcd.print(" ");
  }
}
/* Preset songs */

class Song {
  public:
    int size;
    String name = "LEEG";
    int **notes = {};

    static Song* instance() {
      static Song INSTANCE;
      return &INSTANCE;
    }

    ~Song() {
      delete notes;
    };
  protected:
    Song() {};

    void initNotes(int notes2d[][3], int _size) {
      for (int i = 0; i < _size; i++)
      {
        notes[i] = &notes2d[i][0];
      }
    }

};

class StarWars : public Song {
  public:
    static StarWars* instance() {
      static StarWars INSTANCE;
      return &INSTANCE;
    }

  private:
    int notes2d[66][3]  = {{LA, 3, Q}, {LA, 3, Q}, {LA, 3, Q}, {F, 3, Ei + S}, {C, 4, S}, {LA, 3, Q}, {F, 3, Ei + S}, {C, 4, S}, {LA, 3, H}, {E, 4, Q}, {E, 4, Q}, {E, 4, Q}, {F, 4, Ei + S}, {C, 4, S}, {Ab, 3, Q}, {F, 3, Ei + S}, {C, 4, S}, {LA, 3, H}, {LA, 4, Q}, {LA, 3, Ei + S}, {LA, 3, S}, {LA, 4, Q}, {Ab, 4, Ei + S}, {G, 4, S}, {Gb, 4, S}, {E, 4, S}, {F, 4, Ei}, {Bb, 3, Ei}, {Eb, 4, Q}, {D, 4, Ei + S}, {Db, 4, S}, {C, 4, S}, {B, 3, S}, {C, 4, Ei}, {F, 3, Ei}, {Ab, 3, Q}, {F, 3, Ei + S}, {LA, 3, S}, {C, 4, Q}, {LA, 3, Ei + S}, {C, 4, S}, {E, 4, H}, {LA, 4, Q}, {LA, 3, Ei + S}, {LA, 3, S}, {LA, 4, Q}, {Ab, 4, Ei + S}, {G, 4, S}, {Gb, 4, S}, {E, 4, S}, {F, 4, Ei}, {Bb, 3, Ei}, {Eb, 4, Q}, {D, 4, Ei + S}, {Db, 4, S}, {C, 4, S}, {B, 3, S}, {C, 4, Ei}, {F, 3, Ei}, {Ab, 3, Q}, {F, 3, Ei + S}, {C, 4, S}, {LA, 3, Q}, {F, 3, Ei + S}, {C, 4, S}, {LA, 3, H}};

    StarWars( ) {
      size = 66;
      notes = new int*[size];
      initNotes(notes2d, size);
      name = "Star Wars March";
    }

};

class MarioTheme : public Song {
  public:
    static MarioTheme* instance() {
      static MarioTheme INSTANCE;
      return &INSTANCE;
    }

  private:
    int notes2d[78][3]  = {{ E, 7, S}, { E, 7, S}, {0, 0, S}, { E, 7, S}, {0,  0, S}, { C, 7, S}, { E, 7, S}, {0, 0, S}, {  G, 7, S}, {0, 0, S}, {0, 0, S}, {0,  0, S}, {  G, 6, S}, {0, 0, S}, {0, 0, S}, {0, 0, S}, {  C, 7, S}, {0, 0, S}, {0, 0, S}, { G, 6, S}, { 0, 0, S}, {0, 0, S}, { E, 6, S}, {0, 0, S}, { 0, 0, S}, { LA, 6, S}, {0, 0, S}, { B, 6, S}, { 0, 0, S}, { Bb, 6, S}, { LA, 6, S}, {0, 0, S}, {  G, 6, 1* Ei/2}, { E, 7, 1 * Ei/2}, { G, 7, 1 * Ei/2}, {  LA, 7, S}, {0, 0, S}, { F, 7, S}, { G, 7, S}, {0,  0, S}, { E, 7, S}, {0, 0, S}, { C, 7, S}, {  D, 7, S}, { B, 6, S}, {0, 0, S}, {0, 0, S}, {  C, 7, S}, {0, 0, S}, {0, 0, S}, { G, 6, S}, { 0, 0, S}, { 0, 0, S}, { E, 6, S}, {0, 0, S}, { 0, 0, S}, { LA, 6, S}, { 0, S}, { B, 6, S}, { 0, 0, S}, { Bb, 6, S}, { LA, 6, S}, {0, 0, S}, { G, 6, 1 * Ei/2}, { E, 7, 1 * Ei/2}, { G, 7, 1 * Ei/2}, {  LA, 7, S}, { 0, 0, S}, { F, 7, S}, { G, 7, S}, { 0, 0, S}, { E, 7, S}, {0, 0, S}, { C, 7, S}, {  D, 7, S}, { B, 6, S}, {0, 0, S}, {0, 0, S}
    };

    MarioTheme( ) {
      size = 78;
      notes = new int*[size];
      initNotes(notes2d, size);
      name = "Mario Theme";
    };
};

class Tetris : public Song {
  public:
    static Tetris* instance() {
      static Tetris INSTANCE;
      return &INSTANCE;
    }

  private:
    int notes2d[55][3]  = {{E,4,Q},{B,3,Ei},{C,4,Ei},{D,4,Q},{D,4,Ei},{C,4,Ei},{LA,3,Q},{LA,3,Ei},{C,4,Ei},{E,4,Q},{D,4,Ei},{C,4,Ei},{B,3,Q},{C,4,Ei},{D,4,Q},{E,4,Q},{C,4,Q},{LA,3,Q},{LA,3,Q},{0,0,Ei},{D,4,Q},{F,4,Ei},{LA,4,Q},{G,4,Ei},{F,4,Ei},{E,4,Q},{C,4,Ei},{E,4,Q},{D,4,Ei},{C,4,Ei},{B,3,Q},{C,4,Ei},{D,4,Q},{E,4,Q},{C,4,Q},{LA,3,Q},{LA,3,Q},{0,0,Ei},{E,3,H},{C,3,H},{D,3,H},{B,2,H},{C,3,H},{LA,2,H},{Ab,3,H},{B,2,Ei},{0,0,Ei},{E,3,H},{C,3,H},{D,3,H},{B,2,H},{C,3,Q},{E,3,Q},{LA,3,H},{Ab,4,H}
    };

    Tetris( ) {
      size = 55;
      notes = new int*[size];
      initNotes(notes2d, size);
      name = "Tetris";
    };
};

class SongPlayer {
  public:
    /**
       First element is the song name.
       Rest are tuples of form: Note,Octaveindex,noteLength(in eights,halves, etc)
    */
    static void play(Song* s) {
      int **info = s->notes;
      for (int i = 0; i < s->size; i++) {
        playNote(bzr, info[i][0], info[i][1], info[i][2]);
      }
    };
};

// MAIN AND LOOP

void setup() {
  // put your setup code here, to run once:
  pinMode(bzr, OUTPUT);
  pinMode(btn, INPUT);
  pinMode(keys, INPUT);
  lcd.begin();
}

void loop() {
  // put your main code here, to run repeatedly:
  /*while (digitalRead(btn) != HIGH) {};
    SongPlayer::play(&MarioTheme::instance());*/
  keyboard_modus();
  demo_modus();
}

void keyboard_modus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("** Keyboard modus **");
  while(true) {
    //INPUTS opvragen
    int btn_prev, btn_curr;
    btn_curr = digitalRead(btn);
    if(btn_prev==HIGH && btn_curr==LOW)
      return;
    btn_prev= btn_curr;
    
    float value;
    int x = digitalRead(btn);
    if ((value = mapf(analogRead(keys), 0, 1023, 0, 3.3)) < threshold) {
        continue;
      };
    lcd.setCursor(0, 1);
    lcd.print("Volt: "+String(value));
    lcd.setCursor(0,2);
    int interval = getInterval(value);
    lcd.print("Knop: "+ String(interval+1));
    lcd.setCursor(0,3);
    lcd.print("Frequentie: "+String( ((float)keyNotes[interval])/10 * pow(2, keyOctaves[interval] - 4)));
    if(x==HIGH)
    playNote(bzr,G,3,350);
    else
    playNote(bzr,keyNotes[interval],keyOctaves[interval],350);
    btn_prev= LOW;
  }
}

/**
 * INPUTS:
 * btn-knop (bij loslaten): einde demo_modus
 * btn_knop+keyboard toets : Selecteren corresponderende nummer 
 * keyboard_toets : Spelen van bijhorend nummer
 */
void demo_modus() {
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("**** Demo modus ****");
  while(true) {
    //INPUTS opvragen
    int btn_prev, btn_curr;
    btn_curr = digitalRead(btn);
    if(btn_prev==HIGH && btn_curr==LOW)
      return;
    float value;
    if ((value = mapf(analogRead(keys), 0, 1023, 0, 3.3)) < threshold) {
        btn_prev= btn_curr;
        continue;
      };
    // FROM HERE: a keyboard key is pressed

    //Get the correct demo song
    Song* s = Song::instance();
    int index = constrain(getInterval(value), 0,6);
    switch(index) {
      case 0:
        s= MarioTheme::instance();
        break;
      case 1:
        //s= StarWars::instance(); commented because of low memory capacity.
        break;
      case 2:
        s= Tetris::instance();
        break;
    };
    // Do the correct handling
    if(btn_curr==HIGH) {
      lcd.setCursor(0,1);
      lcd.print("Geselecteerde demo:");
      clearLcdLine(2);
      lcd.setCursor(0,2);
      lcd.print(s->name);
    } else {
      //play
      lcd.setCursor(0,1);
      lcd.print("Nu aan het spelen:");
      lcd.setCursor(0,2);
      lcd.print(s->name);
      SongPlayer::play(s);
      lcd.clear();
      lcd.setCursor(0,0);
      lcd.print("**** Demo modus ****");
    }
    btn_prev = LOW;
    while(digitalRead(btn) == HIGH) {} // wait for user to release;
  }
}


