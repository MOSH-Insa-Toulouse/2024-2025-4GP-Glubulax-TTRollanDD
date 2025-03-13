/* Melody
* (cleft) 2005 D. Cuartielles for K3
*
* This example uses a piezo speaker to play melodies.  It sends
* a square wave of the appropriate frequency to the piezo, generating
* the corresponding tone.
*
* The calculation of the tones is made following the mathematical
* operation:
*
*       timeHigh = period / 2 = 1 / (2 * toneFrequency)
*
* where the different tones are described as in the table:
*
* note     frequency     period timeHigh
* c         261 Hz         3830 1915
* d         294 Hz         3400 1700
* e         329 Hz         3038 1519
* f         349 Hz         2864 1432
* g         392 Hz         2550 1275
* a         440 Hz         2272 1136
* b         493 Hz         2028 1014
* C         523 Hz         1912 956
*
* http://www.arduino.cc/en/Tutorial/Melody
*/
  
int speakerPin = 9;

int length = 15; // the number of notes
char notes[] = "ccggaagffeeddc "; // a space represents a rest
char dark[] ="E5A5C6B5A5E6D#6B5A5C6B5G#5B4E5E5A5C6B5A5E6C#7B6G#6A#6G#6E6B5D#5C6A5E5E5A5C6B5A5E6C#7B6G#6A#6G#6E6B5D#5C6A5"
int beats[] = { 1, 1, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 4 };
int beats2[] = { 4, 3, 1, 2, 4, 2, 6, 5, 4, 1, 2, 4, 2, 8, 8, 3,1,2,4,2,4,3,3,2,3,1,2,4,2,8,8,4,3,1,2,4,2,6,4,4,1,2,4,2,8,8,3,1,2,4,2,4,3,3,2,3,1,2,4,2,8};
int notes2[] = {659.26,880.0,1046.5,987.77,880.0,1318.52,1174.66,987.77,880.0,1046.5,987.77,830.61,932.33,
						659.26,
						659.26,
						880.0,
						1046.5,
						987.77,
						880.0,
						1318.52,
						1567.98,
						1479.9,
						1396.92,
						1108.74,
						1396.92,
						1318.52,
						1244.5,
						622.25,
						1046.5,
						880.0,
						
						659.26,
						
						
						659.26,
						880.0, 
						1046.5,
						987.77,
						880.0,
						1318.52,
						1174.66,
						987.77,
						880.0,
						1046.5,
						987.77,
						830.61,
						932.33,
						659.26,
						659.26,
						880.0,
						1046.5,
						987.77,
						880.0,
						1318.52,
						1567.98,
						1479.9,
						1396.92,
						1108.74,
						1396.92,
						1318.52,
						1244.5,
						622.25,
						1046.5,
						880.0
						
						
						
						
						//659.26,//mi4
						//1318.52,//mi5
						//880.0,//la4
						//1046.5,//do5
						//987.77,//si4
						//1174.66,//re5
						//830.61,//sol#4
						//932.33,//la#4
						//1567.98,//sol5
						//1479.9,//fa#5
						//1396.92,//fa5
						//1108.74,//do#5
						//1244.5,//re#5
						//622.25,//re#4
						};
int tempo = 300;

void playTone(int tone, int duration) {
  for (long i = 0; i < duration * 1000L; i += tone * 2) {
    digitalWrite(speakerPin, HIGH);
    delayMicroseconds(tone);
    digitalWrite(speakerPin, LOW);
    delayMicroseconds(tone);
  }
}

void playNote(char note, int duration) {
  char names[] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
  char names2[] ={'E5',
'A5',
'C6',
'B5',
'E6',
'D#6',
'C#7',
'B6',
'G#6',
'A#6',
'B5',
'D#5',
'G#5',
'B4'}
  int tones[] = { 1915, 1700, 1519, 1432, 1275, 1136, 1014, 956 };
  int tones2[] = { 659.26,
880.0,
1046.5,
987.77,
1318.52,
1174.66,
1567.98,
1479.9,
1396.92,
1108.74,
1244.5,
622.25,
830.61,
932.33,};
  
  // play the tone corresponding to the note name
  for (int i = 0; i < sizeof(names); i++) {
  //for (int i = 0; i < 8; i++) {
    if (names[i] == note) {
      playTone(tones[i], duration);
    }
  }
}

void setup() {
  pinMode(speakerPin, OUTPUT);
}

void loop() {
  for (int i = 0; i < length; i++) {
    if (notes[i] == ' ') {
      delay(beats[i] * tempo); // rest
    } else {
      playNote(notes[i], beats[i] * tempo);
    }
    
    // pause between notes
    delay(tempo / 2);
  }
}
