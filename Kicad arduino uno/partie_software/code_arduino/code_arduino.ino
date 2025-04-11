////////////////////////////////////////////////initialisation constante/////////////////////////////////////////////

//---------------------------------MCP41050 AKA R2-----------------------------------------------------//
#include <SPI.h>
const byte csPin = 9;            // MCP41050 chip select pin MCP41XXX --> XXX kohms donc ici 050kohms
const int maxPositions = 256;    // wiper can move from 0 to 255 = 256 positions
const long rAB = 50000;          // 50k pot resistance between terminals A and B,
                                 // mais pour ajuster au multimètre, je mets 92500
const byte rWiper = 125;         // 125 ohms pot wiper resistance
const byte pot0 = 0x11;          // pot0 addr // B 0001 0001
const byte pot0Shutdown = 0x21;  // pot0 shutdown // B 0010 0001



//---------------------------------FLEX-----------------------------------------------------//
const int flexPin = A1;  // Pin connected to voltage divider output

// Change these constants according to your project's design
const float VCC = 5;                    // voltage at Ardunio 5V line
const float R_DIV = 33000.0;            // resistor used to create a voltage divider
const float flatResistance = 30000.0;   // resistance when flat
const float bendResistance = 100000.0;  // resistance at 90 deg



//---------------------------------OLED et encoder-----------------------------------------------------//

#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

int menuitem = 1;
int frame = 1;
int page = 1;
int lastMenuItem = 1;
// int delais = 1;

String menuItem1 = "Config_R2";
String menuItem2 = "Flex_vs_Capteur";
String menuItem3 = "Reset";

boolean backlight = true;
int contrast = 60;
int volume = 50;

String Config_R2[5] = { "125", "12.5K", "25K", "37.5K", "50k" };
int Config_R2_int[5] = { 125, 12500, 25000, 37500, 50000 };
int Config_R2_8int[5] = { 0, 64, 128, 192, 255 };
float MEASURED_R[2] = { 1, 2 };  //Rcpateur en 1 et Rflex en 2
int selectedResistance = 0;
int selectedMeasure = 0;
float Vcapteur = 0;
int envoie  = 0;

boolean up = false;
boolean down = false;
boolean middle = false;

ClickEncoder *encoder;
int16_t last, value;

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

//---------------------------------bluetooth-----------------------------------------------------//
#include <SoftwareSerial.h>             // Bibliothèque pour gérer la communication avec l'appareil bluetooth
#define rxPin 6                         //Broche 11 en tant que RX, � raccorder sur TX du HC-05
#define txPin 7                         //Broche 10 en tant que RX, � raccorder sur TX du HC-05
#define baudrate 9600                   // baudRate à 9600, identique à celui du moniteur série pour synchroniser la communication
SoftwareSerial mySerial(rxPin, txPin);  // Définition du software serial
int ready = 0;

////////////////////////////////////////////////MCP41050 AKA R2/////////////////////////////////////////////

void setPotWiper(int addr, int pos) {
  pos = constrain(pos, 0, 255);  // limit wiper setting to range of 0 to 255
  digitalWrite(csPin, LOW);      // select chip
  SPI.transfer(addr);            // configure target pot with wiper position
  SPI.transfer(pos);
  digitalWrite(csPin, HIGH);  // de-select chip

  // print pot resistance between wiper and B terminal
  long resistanceWB = ((rAB * pos) / maxPositions) + rWiper;
  //Serial.print("Wiper position: ");
  //Serial.print(pos);
  //Serial.print(" Resistance wiper to B terminal: ");
  //Serial.print(resistanceWB);
  //Serial.println(" ohms");
}

////////////////////////////////////////////////SETUP/////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  ////////////////////////////////////////////////MCP41050 AKA R2/////////////////////////////////////////////
  digitalWrite(csPin, HIGH);  // chip select default to de-selected
  pinMode(csPin, OUTPUT);     // configure chip select as output
  SPI.begin();


  ////////////////////////////////////////////////Flex/////////////////////////////////////////////
  pinMode(flexPin, INPUT);



  ////////////////////////////////////////////////Encodeur et OLED/////////////////////////////////////////////
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  turnBacklightOn();

  encoder = new ClickEncoder(4, 2, 5);
  encoder->setAccelerationEnabled(false);

  display.begin();
  display.clearDisplay();
  setContrast();

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  last = encoder->getValue();


  ////////////////////////////////////////////////bluetooth/////////////////////////////////////////////

  // initialisation des pins
  pinMode(rxPin, INPUT);     //broche 6 en tant que rx (recieve pin), connectée à tx du HC-05
  pinMode(txPin, OUTPUT);    //broche 7 en tant que tx (transmission pin), connectée à rx du HC-05
  mySerial.begin(baudrate);  // On définit le baudrate de la communication avec l'appareil bluetooth à 9600


  Serial.begin(baudrate);  // initialisation de la connexion série (avec le module bluetooth)
}

////////////////////////////////////////////////LOOP/////////////////////////////////////////////

void loop() {
  // put your main code here, to run repeatedly:

  //////////////////////////////////////////////MCP41050 AKA R2/////////////////////////////////////////////
  setPotWiper(pot0, Config_R2_8int[selectedResistance]);  // selection valeur résistance SPI

  ////////////////////////////////////////////////Capteur+bluetooth avec le serial/////////////////////////////////////////////
  Vcapteur = analogRead(A0);
  //float Rcapteur = (1 + ( Config_R2_int[selectedResistance] / 100000 ) ) * 10000 * ( 1024 / Vcapteur ) - 11000;
  MEASURED_R[1] = (1 + (100000 / Config_R2_int[selectedResistance])) * 10000 * (1024 / Vcapteur) - 10000;
  //mySerial.write(Vcapteur/4); // On envoie à l'appareil bluetooth, qui communiquera avec l'appli, la valeur à la sortie de A0
  //delais = delais + 1;
  envoie = Vcapteur/4;
  Serial.println(envoie);
  Serial.println(Vcapteur);

  ////////////////////////////////////////////////FLEX/////////////////////////////////////////////
  // Read the ADC, and calculate voltage and resistance from it
  int ADCflex = analogRead(flexPin);
  float Vflex = ADCflex * VCC / 1023.0;
  //float Rflex = R_DIV * (VCC / Vflex - 1.0);
  MEASURED_R[2] = R_DIV * (Vflex / (VCC - Vflex));
  //Serial.println("Resistance: " + String(Rflex) + " ohms");

  // Use the calculated resistance to estimate the sensor's bend angle:
  //float angle = map(Rflex, flatResistance, bendResistance, 0, 90.0);  // permet de faire une régression linéaire entre le nouveau min et le nouveau max qu'on lui a fournit

  ////////////////////////////////////////////////Encodeur/////////////////////////////////////////////
  drawMenu();

  readRotaryEncoder();

  ClickEncoder::Button b = encoder->getButton();
  if (b != ClickEncoder::Open) {
    switch (b) {
      case ClickEncoder::Clicked:
        middle = true;
        break;
    }
  }

  if (up && page == 1) {

    up = false;
    if (menuitem == 2 && frame == 2) {
      frame--;
    }
    if (menuitem == 3 && frame == 3) {
      frame--;
    }
    lastMenuItem = menuitem;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 1;
    }
  } else if (up && page == 2 && menuitem == 1) {
    up = false;
    selectedResistance--;
    if (selectedResistance == -1) {
      selectedResistance = 4;
    }
  } else if (up && page == 2 && menuitem == 2) {
    up = false;
    selectedMeasure--;
    if (selectedMeasure == -1) {
      selectedMeasure = 1;
    }
  }

  //We have turned the Rotary Encoder Clockwise
  if (down && page == 1) {
    down = false;
    if (menuitem == 3 && lastMenuItem == 2) {
      frame++;
    }
    lastMenuItem = menuitem;
    menuitem++;
    if (menuitem == 3) {
      menuitem--;
    }

  } else if (down && page == 2 && menuitem == 1) {
    down = false;
    selectedResistance++;
    if (selectedResistance == 5) {
      selectedResistance = 0;
    }
  } else if (down && page == 2 && menuitem == 2) {
    down = false;
    selectedMeasure++;
    if (selectedMeasure == 2) {
      selectedMeasure = 0;
    }
  }

  //Middle Button is Pressed
  if (middle) {
    middle = false;

    if (page == 1 && menuitem == 3)  // Reset
    {
      resetDefaults();
    }

    else if (page == 1 && menuitem <= 2) {
      page = 2;
    } else if (page == 2) {
      page = 1;
    }
  }
}

////////////////////////////////////////////////Bluetooth/////////////////////////////////////////////
void setupBlueToothConnection()  // fonction de configuration du module bluetooth
{

  //Serial.begin(speed_serial); //vitesse de bluetooth
  //initialisation de la connexion série (avec le module bluetooth)

  //Serial.print("\r\n+STBD=115200\r\n"); // fixe la vitesse du bluetooth
  //Serial.print("\r\n+STBD=9600\r\n"); // fixe la vitesse du bluetooth
  //Serial.print("\r\n+STWMOD=0\r\n"); //bluetooth en mode esclave
  //Serial.print("\r\n+STNA=Arduino"); //nom de l'appareil
  //Serial.print("\r\n+STPIN=0000\r\n");//code pin "0000"
  //Serial.print("\r\n+STOAUT=1\r\n"); // Permit Paired device to connect me
  //Serial.print("\r\n+STAUTO=0\r\n"); // Auto-connection should be forbidden here
  //delay(2000); // This delay is required.
  //Serial.print("\r\n+INQ=1\r\n"); //make the slave bluetooth inquirable
  //delay(2000); // This delay is required.
  //Serial.flush();
}
////////////////////////////////////////////////Encodeur et OLED/////////////////////////////////////////////
void drawMenu() {

  if (page == 1) {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(WHITE, BLACK);
    display.setCursor(15, 0);
    display.println("    MAIN MENU");
    display.drawFastHLine(0, 10, 128, WHITE);  //BLACK

    if (menuitem == 1 && frame == 1) {
      //mySerial.write("Config_R2");
      displayMenuItem(menuItem1, 15, true);
      displayMenuItem(menuItem2, 25, false);
    } else if (menuitem == 2 && frame == 1) {
      //mySerial.write("Measure");
      displayMenuItem(menuItem1, 15, false);
      displayMenuItem(menuItem2, 25, true);
    } else if (menuitem == 3 && frame == 1) {
      displayMenuItem(menuItem1, 15, false);
      displayMenuItem(menuItem2, 25, false);
      displayMenuItem(menuItem3, 35, true);
    }

    else if (menuitem == 3 && frame == 2) {
      displayMenuItem(menuItem2, 15, false);
      displayMenuItem(menuItem3, 25, true);
    } else if (menuitem == 2 && frame == 2) {
      //mySerial.write("Measure");
      displayMenuItem(menuItem2, 15, true);
      displayMenuItem(menuItem3, 25, false);
    } else if (menuitem == 3 && frame == 3) {
      displayMenuItem(menuItem3, 15, true);
    }
    display.display();
  } else if (page == 2 && menuitem == 1) {
    displayStringMenuPage(menuItem1, Config_R2[selectedResistance]);
    mySerial.write(selectedResistance+1);
    Serial.println(selectedResistance+1);
  } else if (page == 2 && menuitem == 2) {
    if (selectedMeasure == 0) {
      displayfloatMenuPage(menuItem2, MEASURED_R[2], selectedMeasure); // Rflex
      //mySerial.write(MEASURED_R[2]);  // Rflex
    }                                                                   
    else {
      displayfloatMenuPage(menuItem2, MEASURED_R[1], selectedMeasure);  // Rcapteur
      mySerial.write(envoie);
    }
  }
}

void resetDefaults() {
  contrast = 60;
  volume = 50;
  selectedResistance = 0;
  setContrast();
  backlight = true;
  turnBacklightOn();
}

void setContrast() {
  //display.setContrast(contrast);
  display.display();
}

void turnBacklightOn() {
  digitalWrite(7, LOW);
}

void turnBacklightOff() {
  digitalWrite(7, HIGH);
}

void timerIsr() {
  encoder->service();
}

void displayfloatMenuPage(String menuItem, float value, int choix) {
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.setCursor(15, 0);
  display.println(menuItem);
  display.drawFastHLine(0, 10, 127, WHITE);  //BLACK
  display.setCursor(5, 15);
  if (choix == 0) {
    display.println("Rflex ohm");
  } else {
    display.println("Rcapteur ohm");
  }
  display.setTextSize(1);
  display.setCursor(5, 25);
  display.println(value);
  display.setTextSize(1);
  display.display();
}

void displayStringMenuPage(String menuItem, String value) {
  display.setTextSize(1);
  display.clearDisplay();
  display.setTextColor(WHITE, BLACK);
  display.setCursor(15, 0);
  display.println(menuItem);
  display.drawFastHLine(0, 10, 128, WHITE);  //BLACK
  display.setCursor(5, 15);
  display.println("Value (ohm)");
  display.setTextSize(1);
  display.setCursor(5, 25);
  display.println(value);
  display.setTextSize(1);
  display.display();
}

void displayMenuItem(String item, int position, boolean selected) {
  if (selected) {
    display.setTextColor(BLACK, WHITE);  //, BLACK
  } else {
    display.setTextColor(WHITE);  //BLACK,
  }
  display.setCursor(0, position);
  display.print(">" + item);
}

void readRotaryEncoder() {
  value += encoder->getValue();

  if (value / 2 > last) {
    last = value / 2;
    down = true;
    delay(150);
  } else if (value / 2 < last) {
    last = value / 2;
    up = true;
    delay(150);
  }
}
