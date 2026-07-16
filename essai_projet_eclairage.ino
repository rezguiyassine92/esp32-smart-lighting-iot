#include "DHT.h"
#include <LiquidCrystal_I2C.h>

// Initialisation des capteurs et afficheur LCD
DHT dht(5, DHT22);
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Définition des broches
const int brocheLDR = 32;
const int brocheLED = 4;
const int brocheMouvement = 23;

// Variables pour la gestion du mouvement
bool maintienActif = false;
unsigned int debutMaintien = 0;
const unsigned long dureeMaintien = 30000; // 30 secondes

void setup() {
pinMode(brocheLED, OUTPUT);
pinMode(brocheMouvement, INPUT);
Serial.begin(9600);
lcd.init();
lcd.backlight();
dht.begin();
}

void loop() {
float temperature = dht.readTemperature();
float humidite = dht.readHumidity();
int valeurLDR = analogRead(brocheLDR);
bool mouvementDetecte = digitalRead(brocheMouvement);

int puissanceLED = 0;
bool alerteActive = false;
String messageAlerte = "";

if (temperature < 0 && humidite > 95) {
puissanceLED = 255;
alerteActive = true;
messageAlerte = "Alerte Brouillard";} 

else if (temperature < 2 && humidite > 90) {
puissanceLED = 204;
alerteActive = true;
messageAlerte = "Alerte Brume";} 
  
else {
// Cas où la luminosité est élevée (Jour) → LED éteinte
  if (valeurLDR >= 800) {
puissanceLED = 0;} // Lumière éteinte

// Cas Aube (500 ≤ LDR < 800)
else if (valeurLDR >= 500 && valeurLDR < 800) {
  if (mouvementDetecte) {
  puissanceLED = 153; // 60% d'intensité
  maintienActif = true;
  debutMaintien = millis();}

else if (maintienActif && (millis() - debutMaintien < dureeMaintien)) {
  puissanceLED = 153; }   // Maintien de l'éclairage à 60%
else {
  puissanceLED = 102; // Éclairage à 40% sans mouvement
  maintienActif = false;
  }  }

  // Cas Crépuscule (200 ≤ LDR < 500)
else if (valeurLDR >= 200 && valeurLDR < 500) {
  if (mouvementDetecte) {
  puissanceLED = 204; // 80% d'intensité
  maintienActif = true;
  debutMaintien = millis();}

else if (maintienActif && (millis() - debutMaintien < dureeMaintien)) {
  puissanceLED = 204;   } // Maintien de l'éclairage à 80%
else {
  puissanceLED = 153; // Éclairage à 60% sans mouvement
  maintienActif = false;
}  }
  
  // Cas Nuit Profonde (LDR < 200)
else {
  if (mouvementDetecte) {
  puissanceLED = 255; // 100% d'intensité
  maintienActif = true;
  debutMaintien = millis(); }
else if (maintienActif && (millis() - debutMaintien < dureeMaintien)) {
  puissanceLED = 255; }   // Maintien de l'éclairage à 100%
else {
  puissanceLED = 204; // Éclairage à 80% sans mouvement
  maintienActif = false;}
  }    }

// Appliquer la puissance LED
analogWrite(brocheLED, puissanceLED);

// Affichage sur LCD 
static int dernierRefresh = 0;
if (millis() - dernierRefresh > 500) {
  dernierRefresh = millis();
  lcd.clear();

if (alerteActive) {
// Mode Alerte
  lcd.setCursor(0,0);
  lcd.print(messageAlerte);
  lcd.setCursor(0,1);
  lcd.print("T:");
  lcd.print(temperature,1);
  lcd.print("C H:");
  lcd.print(humidite,1);
  lcd.print("%");} 
else {
// Mode Normal
  lcd.setCursor(0,0);
  lcd.print("T:");
  lcd.print(temperature,1);
  lcd.print("C H:");
  lcd.print(humidite,1);
  lcd.print("%");
  lcd.setCursor(0,1);
  lcd.print("Mov:");
  lcd.print(mouvementDetecte ? "On" : "Off");
  lcd.print(" LED:");
  lcd.print((puissanceLED * 100) / 255);
  lcd.print("%");}              }

// Affichage des données sur le moniteur //
  Serial.println(" donnees capteurs :");
  Serial.print("temperature: "); Serial.print(temperature,1); Serial.println(" °C");
  Serial.print("humidite:    "); Serial.print(humidite,1); Serial.println(" %");
  Serial.print("luminosite:  "); Serial.print(valeurLDR); Serial.println(" lx");
  Serial.print("mouvement:   "); Serial.println(mouvementDetecte ? "detecte" : "aucun");
  Serial.print("puissance led: "); Serial.print((puissanceLED * 100) / 255); Serial.println(" %");
  delay(500);
}
