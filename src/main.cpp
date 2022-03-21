/*
  Titre      : Clavier matriciel
  Auteur     : Yvan Tankeu
  Date       : 19/03/2022
  Description: test du Clavier matriciel avec une led
  Version    : 0.0.1
*/

// Libraries
#include <Arduino.h>
#include <Keypad.h> // librairie du clavier à télecharger
#include "WIFIConnector_MKR1000.h"
#include "MQTTConnector.h"

// Constants
#define ROWS 4
#define COLS 4

/*=============Déclaration des broches============*/

// déclaration des leds
const uint8_t LED_ROUGE = 3;
const uint8_t LED_BLEU = 4;
const uint8_t LED_JAUNE = 5;
const uint8_t LED_VERT = 2;

// Broches des colones
const uint8_t col_3 = 6;
const uint8_t col_2 = 7;
const uint8_t col_1 = 8;
const uint8_t col_0 = 9;

// Broches des lignes
const uint8_t ligne_3 = 10;
const uint8_t ligne_2 = 11;
const uint8_t ligne_1 = 12;
const uint8_t ligne_0 = 13;

// Nombre de fois qu'une led selectionné doit blinker
const int NBR_BLINK_CHOIX_LED = 1;

// Nombre de fois qu'une led comfirmer doit blinker
const int NBR_BLINK_COMFIRM_LED = 2;

const unsigned long DELAI_MS = 300;

// Touches du clavier
const char keys[COLS][ROWS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

char key; // touche du calvier

bool choixLED = false;
bool choixIntensiteLED = false;

double valNumerique = 0.0;
double intensite = 0.0;

int val = 0;

// prendra la led selectionée
uint8_t ledComfirmer = 0;

byte ligneBroches[4] = {ligne_0, ligne_1, ligne_2, ligne_3};
byte colonneBroches[4] = {col_0, col_1, col_2, col_3};

// Tableau broches PWM
uint8_t TabLED[4] = {LED_ROUGE, LED_BLEU, LED_JAUNE, LED_VERT};

Keypad clavier = Keypad(makeKeymap(keys), ligneBroches, colonneBroches, ROWS, COLS);

/**
 * @brief Faire blinker une led
 *
 * @param led led à allumer
 * @param nombreBlink le nombre de fois que la led doit blinker
 */
void blink(uint8_t led, int nombreBlink)
{
  for (int i = 0; i < nombreBlink; i++)
  {
    analogWrite(led, 255);
    delay(DELAI_MS);
    analogWrite(led, LOW);
    delay(DELAI_MS);
  }
}

// Eteindre les 04 LED
void offLed()
{
  for (int i = 0; i < 4; i++)
  {
    analogWrite(TabLED[i], LOW);
  }
}
void setup()
{
  // Init Serial USB
  Serial.begin(9600);
  wifiConnect(); //Branchement au réseau WIFI
  MQTTConnect(); //Branchement au broker MQTT
  Serial.println(F("Initialize System"));

  // Broches en mode OUTPUT
  for (int i = 0; i < 4; i++)
  {
    pinMode(TabLED[i], OUTPUT);
  }

  // Eteindre les 04 LED
  offLed();
}

void loop()
{
  key = clavier.getKey();

  switch (key)
  {
  // si on presse sur A,B,C ou D on desisgne la led branché sur le PWM
  case 'A':
    // Enregistre la led appuyé
    ledComfirmer = LED_ROUGE;
    Serial.println(key);
    blink(LED_ROUGE, NBR_BLINK_CHOIX_LED);
    choixLED = true;
    offLed();
    break;
  case 'B':
    ledComfirmer = LED_BLEU;
    Serial.println(key);
    blink(LED_BLEU, NBR_BLINK_CHOIX_LED);
    choixLED = true;
    offLed();
    break;
  case 'C':
    ledComfirmer = LED_JAUNE;
    Serial.println(key);
    blink(LED_JAUNE, NBR_BLINK_CHOIX_LED);
    choixLED = true;
    offLed();
    break;
  case 'D':
    ledComfirmer = LED_VERT;
    Serial.println(key);
    blink(LED_VERT, NBR_BLINK_CHOIX_LED);
    choixLED = true;
    offLed();
    break;
  // Confirmer le choix de la LED
  case '*':
    if (choixLED == true)
    {
      Serial.println(key);
      blink(ledComfirmer, NBR_BLINK_COMFIRM_LED);
      choixLED = false;
    }
    break;
  // Choisir la frequence de clignotement de la led
  case '0':
  case '1':
  case '2':
  case '3':
  case '4':
  case '5':
  case '6':
  case '7':
  case '8':
  case '9':
    //  Verification si le choix de la led a été fait avant de passer le flambeau au choix de l'intensité de la LED
    Serial.println(key);
    valNumerique = double(key) - 48;
    Serial.println(valNumerique);
    // blink(1);
    choixIntensiteLED = true;
    break;

  // Confirmation du choix de l'intensité de la led
  case '#':
    if (choixIntensiteLED == true)
    {
      Serial.println(key);
      // blink(4);
      choixLED = false;
    }
    // Utilisation de la fonction map et la resolution de la pwm qui est de 8 bits
    val = map(((valNumerique * 255) / 9), 0, 9, 1, 255);
    analogWrite(ledComfirmer, val);
    // Serial.println((valNumerique *255) / 9);
     // envoie du statut
  appendPayload("Led", ledComfirmer);
  appendPayload("Intensite", val);
  sendPayload();
   delay(1000);
    break;
  }
}
