////////////////////////////////////////////////initialisation constante/////////////////////////////////////////////
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ClickEncoder.h>
#include <TimerOne.h>
#include <Wire.h>
#include <SPI.h>
//---------------------------------MCP41100 AKA R2-----------------------------------------------------//

const byte csPin           = 9;      // MCP42100 chip select pin
const int  maxPositions    = 256;     // wiper can move from 0 to 255 = 256 positions
const long rAB             = 50000;   // 50k pot resistance between terminals A and B, 
                                      // mais pour ajuster au multimètre, je mets 92500
const byte rWiper          = 125;     // 125 ohms pot wiper resistance
const byte pot0            = 0x11;    // pot0 addr // B 0001 0001
const byte pot0Shutdown    = 0x21;    // pot0 shutdown // B 0010 0001



//---------------------------------FLEX-----------------------------------------------------//
const int flexPin = A1;      // Pin connected to voltage divider output

// Change these constants according to your project's design
const float VCC = 5;      // voltage at Ardunio 5V line
const float R_DIV = 33000.0;  // resistor used to create a voltage divider
const float flatResistance = 30000.0; // resistance when flat
const float bendResistance = 100000.0;  // resistance at 90 deg



//---------------------------------OLED-----------------------------------------------------//

#define nombreDePixelsEnLargeur 128         // Taille de l'écran OLED, en pixel, au niveau de sa largeur
#define nombreDePixelsEnHauteur 64          // Taille de l'écran OLED, en pixel, au niveau de sa hauteur
#define brocheResetOLED         -1          // Reset de l'OLED partagé avec l'Arduino (d'où la valeur à -1, et non un numéro de pin)
#define adresseI2CecranOLED     0x3C        // Adresse de "mon" écran OLED sur le bus i2c (généralement égal à 0x3C ou 0x3D)

Adafruit_SSD1306 ecranOLED(nombreDePixelsEnLargeur, nombreDePixelsEnHauteur, &Wire, brocheResetOLED);

//---------------------------------Encodeur-----------------------------------------------------//

#define encoder0PinA  2  //CLK Output A Do not use other pin for clock as we are using interrupt
#define encoder0PinB  4  //DT Output B
#define Switch 5 // Switch connection if available
volatile int encoder0Pos = 0;

int menuitem = 1;
int page = 1;
int lastMenuItem = 1;

String menuItem1 = "Config_R2";
String menuItem2 = "Flex_vs_Capteur";

String Config_R2[5] = { "MINIMUM", "12.5K", "25K","37.5K","50k" };
int selectedResistance = 0;

//---------------------------------bluetooth-----------------------------------------------------//

const int speed_serial = 9600;      // communication Bluetooth, il envoie simplement la donnée, qui est une valeur analogique codée sur 10 bit (0 1024)
// A1/4 que je ramène sur 0 255 qui peut se coder en 8 bit




////////////////////////////////////////////////MCP41100 AKA R2/////////////////////////////////////////////

void setPotWiper(int addr, int pos) {
  pos = constrain(pos, 0, 255);            // limit wiper setting to range of 0 to 255
  digitalWrite(csPin, LOW);                // select chip
  SPI.transfer(addr);                      // configure target pot with wiper position
  SPI.transfer(pos);
  digitalWrite(csPin, HIGH);               // de-select chip

  // print pot resistance between wiper and B terminal
  long resistanceWB = ((rAB * pos) / maxPositions ) + rWiper;
}

////////////////////////////////////////////////SETUP/////////////////////////////////////////////

void setup() {
  // put your setup code here, to run once:
  ////////////////////////////////////////////////MCP41100 AKA R2/////////////////////////////////////////////
  digitalWrite(csPin, HIGH);        // chip select default to de-selected
  pinMode(csPin, OUTPUT);           // configure chip select as output
  SPI.begin();


  ////////////////////////////////////////////////Flex/////////////////////////////////////////////
  pinMode(flexPin, INPUT);



  ////////////////////////////////////////////////Encodeur/////////////////////////////////////////////
  pinMode(encoder0PinA, INPUT); 
  digitalWrite(encoder0PinA, HIGH);       // turn on pullup resistor

  pinMode(encoder0PinB, INPUT); 
  digitalWrite(encoder0PinB, HIGH);       // turn on pullup resistor

  attachInterrupt(0, doEncoder, RISING); // encoder pin on interrupt 0 - pin2


  ////////////////////////////////////////////////OLED/////////////////////////////////////////////
  if(!ecranOLED.begin(SSD1306_SWITCHCAPVCC, adresseI2CecranOLED))
    while(1);    // Arrêt du programme (boucle infinie) si échec d'initialisation

  // ***********************************************************************
  // Affichage de 8 lignes à couleurs alternées, dans chaque écran successif
  // ***********************************************************************
  for(byte tailleDeCaractere=1; tailleDeCaractere <=5; tailleDeCaractere++) {
    ecranOLED.clearDisplay();      // Effaçage de l'intégralité du buffer
    ecranOLED.setTextSize(tailleDeCaractere);   // Taille des caractères (1:1, puis 2:1, puis 3:1, puis 4:1,  puis 5:1)
    ecranOLED.setCursor(0, 0);      // Déplacement du curseur en position (0,0), c'est à dire dans l'angle supérieur gauche
    
    ecranOLED.setTextColor(SSD1306_WHITE, SSD1306_BLACK); // Couleur du texte, et couleur du fond               
    ecranOLED.print("bibibi"); // à changer en fonction de ce qu'on veut écrire
    ecranOLED.println("blabla");     // à changer en fonction de ce qu'on veut écrire                     
      
    if (tailleDeCaractere == 5 )
      tailleDeCaractere=1;
    ecranOLED.display();      // Transfert le buffer à l'écran
    delay(2000);
  }


  ////////////////////////////////////////////////bluetooth/////////////////////////////////////////////

  // initialisation des pins 
  pinMode(6,INPUT); //broche 6 en tant que rx (recieve pin), connectée à tx du HC-05
  pinMode(7,OUTPUT); //broche 7 en tant que tx (transmission pin), connectée à rx du HC-05

  
  
  Serial.begin(speed_serial); // initialisation de la connexion série (avec le module bluetooth)
  setupBlueToothConnection(); // démarrage liason série bluetooth cf fonction en bas

}

////////////////////////////////////////////////LOOP/////////////////////////////////////////////

void loop() {
  // put your main code here, to run repeatedly:

  ////////////////////////////////////////////////Capteur/////////////////////////////////////////////
  analogRead(A1); // valeur de la résistance du cpateur graphène après amplification 

  ////////////////////////////////////////////////FLEX/////////////////////////////////////////////
  // Read the ADC, and calculate voltage and resistance from it
  int ADCflex = analogRead(flexPin);
  float Vflex = ADCflex * VCC / 1023.0;
  float Rflex = R_DIV * (VCC / Vflex - 1.0);
  //Serial.println("Resistance: " + String(Rflex) + " ohms");

  // Use the calculated resistance to estimate the sensor's bend angle:
  float angle = map(Rflex, flatResistance, bendResistance, 0, 90.0); // permet de faire une régression linéaire entre le nouveau min et le nouveau max qu'on lui a fournit
  //Serial.println("Bend: " + String(angle) + " degrees");
  Serial.println(); //à changer, écrire sur le OLED
  Serial.println(analogRead(flexPin)*5.0/1023.0); //à changer, écrire sur le OLED

  ////////////////////////////////////////////////Encodeur/////////////////////////////////////////////
  drawMenu();

  readRotaryEncoder();

   ClickEncoder::Button b = encoder->getButton();
   if (b != ClickEncoder::Open) {
   switch (b) {
      case ClickEncoder::Clicked:
         middle=true;
        break;
    }
  }    
  
  if (up && page == 1 ) {
     
    up = false;
    lastMenuItem = menuitem;
    menuitem--;
    if (menuitem==0)
    {
      menuitem=1;
    } 
  }else if (up && page == 2 && menuitem==1 ) {
    up = false;
    selectedResistance--;
    if(selectedResistance == -1)
    {
      selectedResistance = 2;
    }
  }
  else if (up && page == 2 && menuitem==2 ) {
    up = false;
  }

  if (down && page == 1) //We have turned the Rotary Encoder Clockwise
  {

    down = false;
    lastMenuItem = menuitem;
    menuitem++;  
  
  }else if (down && page == 2 && menuitem==1) {
    down = false;
    contrast++;
    setContrast();
  }
  else if (down && page == 2 && menuitem==2) {
    down = false;
    volume++;
  }
   else if (down && page == 2 && menuitem==3 ) {
    down = false;
    selectedLanguage++;
    if(selectedLanguage == 3)
    {
      selectedLanguage = 0;
    }
  }
  else if (down && page == 2 && menuitem==4 ) {
    down = false;
    selectedDifficulty++;
    if(selectedDifficulty == 2)
    {
      selectedDifficulty = 0;
    }
  }
  
  if (middle) //Middle Button is Pressed
  {
    middle = false;
   
    if (page == 1 && menuitem==5) // Backlight Control 
    {
      if (backlight) 
      {
        backlight = false;
        menuItem5 = "Light: OFF";
        turnBacklightOff();
        }
      else 
      {
        backlight = true; 
        menuItem5 = "Light: ON";
        turnBacklightOn();
       }
    }

    if(page == 1 && menuitem ==6)// Reset
    {
      resetDefaults();
    }


    else if (page == 1 && menuitem<=4) {
      page=2;
     }
      else if (page == 2) 
     {
      page=1; 
     }
   }
  // set la valeur de R_2 via l'encodeur rotatoir
  switch (abs(encoder0Pos)%5){
    case 0: 
    setPotWiper(pot0, 0);                    // minimum resistance
    case 1: 
    setPotWiper(pot0, 64);                   // 25% resistance
    case 2: 
    setPotWiper(pot0, 128);                  // 50% resistance
    case 3: 
    setPotWiper(pot0, 192);                  // 75% resistance
    case 4: 
    setPotWiper(pot0, 255);                  // 100% resistance
  }
  if (abs(encoder0Pos) <= 20){
    Serial.print ("Me"); //à changer, écrire sur le OLED
    for (int i=0; i<abs(encoder0Pos); i++){
    Serial.print("e"); //à changer, écrire sur le OLED
  }
  Serial.print ("rde              "); //à changer, écrire sur le OLED
  }
  else{
    Serial.print ("ME");
    for (int i=0; i<abs(encoder0Pos); i++){
    Serial.print("E"); 
  }
  Serial.print ("RDE              ");
  }

  Serial.println (encoder0Pos, DEC); //Angle = (360 / Encoder_Resolution) * encoder0Pos



  delay(3000);

}


////////////////////////////////////////////////Encodeur/////////////////////////////////////////////
void doEncoder() {
  if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==HIGH) {
    encoder0Pos++;
  } else if (digitalRead(encoder0PinA)==HIGH && digitalRead(encoder0PinB)==LOW) {
    encoder0Pos--;
  }
  // Serial.println (encoder0Pos, DEC);  //Angle = (360 / Encoder_Resolution) * encoder0Pos
}
