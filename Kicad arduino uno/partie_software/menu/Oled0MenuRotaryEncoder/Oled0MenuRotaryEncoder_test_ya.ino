#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_GFX.h>
#include <ClickEncoder.h>
#include <TimerOne.h>

int menuitem = 1;
int page = 1;
int lastMenuItem = 1;

String menuItem1 = "Config_R2";
String menuItem2 = "Flex_vs_Capteur";

boolean backlight = true;
int contrast = 60;
int volume = 50;

String Config_R2[5] = { "MINIMUM", "12.5K", "25K", "37.5K", "50k" };
int selectedResistance = 0;

boolean up = false;
boolean down = false;
boolean middle = false;

ClickEncoder *encoder;
int16_t last, value;

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

void setup() {
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
  turnBacklightOn();

  encoder = new ClickEncoder(A1, A0, A2);
  encoder->setAccelerationEnabled(false);

  display.begin();
  display.clearDisplay();
  setContrast();

  Timer1.initialize(1000);
  Timer1.attachInterrupt(timerIsr);

  last = encoder->getValue();
}

void loop() {

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
    lastMenuItem = menuitem;
    menuitem--;
    if (menuitem == 0) {
      menuitem = 1;
    }
  } else if (up && page == 2 && menuitem == 1) {
    up = false;
    selectedResistance--;
    if (selectedResistance == -1) {
      selectedResistance = 2;
    }
  } else if (up && page == 2 && menuitem == 2) {
    up = false;
  }

  //We have turned the Rotary Encoder Clockwise

  if (down && page == 1) {

    down = false;
    lastMenuItem = menuitem;
    menuitem++;

  } else if (down && page == 2 && menuitem == 1) {
    down = false;
    selectedResistance++;
    if (selectedResistance == 5) {
      selectedResistance = 0;
    }
  } else if (down && page == 2 && menuitem == 2) {
    down = false;
  }

  //Middle Button is Pressed

  if (middle) {
    middle = false;

    if (page == 1 && menuitem == 6)  // Reset
    {
      resetDefaults();
    }


    else if (page == 1 && menuitem <= 4) {
      page = 2;
    } else if (page == 2) {
      page = 1;
    }
  }

  void drawMenu() {

    if (page == 1) {
      display.setTextSize(1);
      display.clearDisplay();
      display.setTextColor(WHITE, BLACK);
      display.setCursor(15, 0);
      display.println("    MAIN MENU");
      display.drawFastHLine(0, 10, 128, WHITE);  //BLACK

      if (menuitem == 1 && frame == 1) {
        displayMenuItem(menuItem1, 15, true);
        displayMenuItem(menuItem2, 25, false);
        displayMenuItem(menuItem3, 35, false);
      } else if (menuitem == 2 && frame == 1) {
        displayMenuItem(menuItem1, 15, false);
        displayMenuItem(menuItem2, 25, true);
        displayMenuItem(menuItem3, 35, false);
      } else if (menuitem == 3 && frame == 1) {
        displayMenuItem(menuItem1, 15, false);
        displayMenuItem(menuItem2, 25, false);
        displayMenuItem(menuItem3, 35, true);
      } else if (menuitem == 4 && frame == 2) {
        displayMenuItem(menuItem2, 15, false);
        displayMenuItem(menuItem3, 25, false);
        displayMenuItem(menuItem4, 35, true);
      }

      else if (menuitem == 3 && frame == 2) {
        displayMenuItem(menuItem2, 15, false);
        displayMenuItem(menuItem3, 25, true);
        displayMenuItem(menuItem4, 35, false);
      } else if (menuitem == 2 && frame == 2) {
        displayMenuItem(menuItem2, 15, true);
        displayMenuItem(menuItem3, 25, false);
        displayMenuItem(menuItem4, 35, false);
      }

      else if (menuitem == 5 && frame == 3) {
        displayMenuItem(menuItem3, 15, false);
        displayMenuItem(menuItem4, 25, false);
        displayMenuItem(menuItem5, 35, true);
      }

      else if (menuitem == 6 && frame == 4) {
        displayMenuItem(menuItem4, 15, false);
        displayMenuItem(menuItem5, 25, false);
        displayMenuItem(menuItem6, 35, true);
      }

      else if (menuitem == 5 && frame == 4) {
        displayMenuItem(menuItem4, 15, false);
        displayMenuItem(menuItem5, 25, true);
        displayMenuItem(menuItem6, 35, false);
      } else if (menuitem == 4 && frame == 4) {
        displayMenuItem(menuItem4, 15, true);
        displayMenuItem(menuItem5, 25, false);
        displayMenuItem(menuItem6, 35, false);
      } else if (menuitem == 3 && frame == 3) {
        displayMenuItem(menuItem3, 15, true);
        displayMenuItem(menuItem4, 25, false);
        displayMenuItem(menuItem5, 35, false);
      } else if (menuitem == 2 && frame == 2) {
        displayMenuItem(menuItem2, 15, true);
        displayMenuItem(menuItem3, 25, false);
        displayMenuItem(menuItem4, 35, false);
      } else if (menuitem == 4 && frame == 3) {
        displayMenuItem(menuItem3, 15, false);
        displayMenuItem(menuItem4, 25, true);
        displayMenuItem(menuItem5, 35, false);
      }
      display.display();
    } else if (page == 2 && menuitem == 1) {
      displayIntMenuPage(menuItem1, contrast);
    }

    else if (page == 2 && menuitem == 2) {
      displayIntMenuPage(menuItem2, volume);
    } else if (page == 2 && menuitem == 3) {
      displayStringMenuPage(menuItem3, language[selectedLanguage]);
    } else if (page == 2 && menuitem == 4) {
      displayStringMenuPage(menuItem4, difficulty[selectedDifficulty]);
    } else if (page == 2 && menuitem == 4) {
      displayStringMenuPage(menuItem4, difficulty[selectedDifficulty]);
    }
  }

  void resetDefaults() {
    contrast = 60;
    volume = 50;
    selectedLanguage = 0;
    selectedDifficulty = 0;
    setContrast();
    backlight = true;
    menuItem5 = "Light: ON";
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

  void displayIntMenuPage(String menuItem, int value) {
    display.setTextSize(1);
    display.clearDisplay();
    display.setTextColor(WHITE, BLACK);
    display.setCursor(15, 0);
    display.println(menuItem);
    display.drawFastHLine(0, 10, 127, WHITE);  //BLACK
    display.setCursor(5, 15);
    display.println("Value");
    display.setTextSize(2);
    display.setCursor(5, 25);
    display.println(value);
    display.setTextSize(2);
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
    display.println("Value");
    display.setTextSize(2);
    display.setCursor(5, 25);
    display.println(value);
    display.setTextSize(2);
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
