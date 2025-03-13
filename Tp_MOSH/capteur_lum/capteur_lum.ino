#define PIN_LED 3

void setup() {
  pinMode (PIN_LED, OUTPUT);
  Serial.dein(9600);
  delay(100);
}

void loop() {
  conat uint16_t SEUIL_NUIT = 400;
  conat uint16_t SEUIL_JOUR = 800;
  conat uint32_t DELAI_JOUR = 5000;

  conat uint32_t DELAI_NUIT = 1000;
  static uint32_t heureJour = 0;
  static uint32_t heureNuit = 0;
  uint16_t luminosite = analogRead(A0);

  enum tEtats {JOUR, DEBUT_NUIT, NUIT, DEBUT_JOUR};
  static tEtats etat = JOUR;


  switch (etat){
    case JOUR:
    if (luminosite<= SEUIL_NUIT){
      etat = DEBUT_NUIT;
      heureNuit=millis();
    }
    break;
    case DEBUT_NUIT:
    if (millis()-heureNuit >= DELAI_NUIT){
      etat = NUIT;
      digitalWrite(PIN_LED, HIGH);
    }
    if (luminosite > SEUIL_NUIT){
      etat = JOUR;
    }
    break;
    case NUIT:
    if (luminosite >= SEUIL_JOUR){
      etat = DEBUT_JOUR;
    }
    break;
    case DEBUT_JOUR:
     
  }
}