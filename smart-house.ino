#include <Adafruit_Sensor.h>
#include <Wire.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>
#include <ArduinoJson.h>
#include <DHTesp.h>


const char* ssid = "xxxx";
const char* password = "yyyy";
const char* server="http://api.beebotte.com/v1/data/write/Application/";
// Créez une instance de la classe WiFiClient et la classe HTTPClient
WiFiClient client;
HTTPClient http;

DHTesp dht;

#define S_PIN 1 // Broche du switcher S
#define RED_PIN 2   // Broche pour la LED rouge
#define GREEN_PIN 3  // Broche pour la LED verte
#define BLUE_PIN 4   // Broche pour la LED bleue
#define RELAY_PIN 15
#define PIR_PIN 0    // Broche numérique connectée au capteur de mouvement PIR
#define LEDPIN 16     // Broche numérique connectée à la LED
#define BUZZER_PIN 12
#define LED1_PIN 13     // Broche numérique connectée à LED1
#define GAS_PIN 10   
#define PHOTO_PIN A0  // Broche analogique connectée au capteur de lumière (photoresistor)

void postdata(float Ressource1,float Ressource2,int Ressource3,int Ressource4,int Ressource5){

String payload;
String payload1;
String payload2;
String payload3;
String payload4;
String payload5;

http.addHeader("Content-Type","application/json");
http.addHeader("X-Auth-Token","token_Xaq8DQDSNnQDmM5N");

StaticJsonDocument<1024> doc1;
  doc1["resource"]="Temperature";
  doc1["data"]=Ressource1;
serializeJson(doc1,payload1);
Serial.println(payload1);


StaticJsonDocument<1024> doc2;
  doc2["resource"]="Humidity";
  doc2["data"]=Ressource2;
serializeJson(doc2,payload2);
Serial.println(payload2);


StaticJsonDocument<1024> doc3;
  doc3["resource"]="light";
  doc3["data"]=Ressource3;
serializeJson(doc3,payload3);
Serial.println(payload3);

StaticJsonDocument<1024> doc4;
  doc4["resource"]="motion";
  doc4["data"]=Ressource4;
serializeJson(doc4,payload4);
Serial.println(payload4);


StaticJsonDocument<1024> doc5;
  doc5["resource"]="gas";
  doc5["data"]=Ressource5;
serializeJson(doc5,payload5);
Serial.println(payload5);


StaticJsonDocument<1024> doc;
  doc["records"][0]=payload1;
  doc["records"][1]=payload2;
  doc["records"][2]=payload3;
  doc["records"][3]=payload4;
  doc["records"][4]=payload5;
serializeJson(doc,payload);
Serial.println(payload);

int httpResponseCode = http.POST(payload);
  if (httpResponseCode > 0) {
    Serial.print("HTTP ");
    Serial.println(payload);
}
}
void setup() {
  // Initialisation de la connexion série
  Serial.begin(115200);
  dht.setup(5 , DHTesp::DHT11);

  // Initialisation des broches
  pinMode(PIR_PIN, INPUT);
  pinMode(LEDPIN, OUTPUT);
  pinMode(LED1_PIN, OUTPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  pinMode(GAS_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);
  pinMode(RED_PIN, OUTPUT);
  pinMode(GREEN_PIN, OUTPUT);
  pinMode(BLUE_PIN, OUTPUT);
  pinMode(S_PIN, INPUT);


  // Connexion WiFi
WiFi.begin(ssid, password);
Serial.println("Connecting to WiFi");
while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  http.begin(client,server);
}

void loop() {
  // Lecture des valeurs des capteurs
  int motionValue = digitalRead(PIR_PIN);
  int gasValue = digitalRead(GAS_PIN);
  int lightLevel = analogRead(PHOTO_PIN);
  int s = digitalRead(S_PIN);
  float temperature = dht.getTemperature();
  float humidity = dht.getHumidity();

  // Affichage des valeurs sur la console série
  Serial.print("Mouvement : ");
  Serial.println(motionValue);
  Serial.print("Gaz : ");
  Serial.print(gasValue);
  Serial.println(" B");
  Serial.print("Niveau de lumière : ");
  Serial.println(lightLevel);

  // Clignotement de la LED en rouge
  digitalWrite(RED_PIN, HIGH);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, LOW);
  delay(1000);
  digitalWrite(RED_PIN, LOW);
  delay(1000);

  // Clignotement de la LED en vert
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, HIGH);
  digitalWrite(BLUE_PIN, LOW);
  delay(1000);
  digitalWrite(GREEN_PIN, LOW);
  delay(1000);

  // Clignotement de la LED en bleu
  digitalWrite(RED_PIN, LOW);
  digitalWrite(GREEN_PIN, LOW);
  digitalWrite(BLUE_PIN, HIGH);
  delay(1000);
  digitalWrite(BLUE_PIN, LOW);
  delay(1000);

  // Activation du relais si le niveau de lumière est inférieur à 500
  digitalWrite(RELAY_PIN, lightLevel < 500 ? HIGH : LOW);

  // Activation du buzzer si la concentration de gaz dépasse 500
  digitalWrite(BUZZER_PIN, gasValue == HIGH ? HIGH : LOW);

  // Activation du buzzer en cas de détection de mouvement
  if (motionValue == HIGH) {
    for (int i = 0; i < 3; i++) {
      // Activation du buzzer pendant deux secondes
      digitalWrite(BUZZER_PIN, HIGH);
      delay(2000);
      digitalWrite(BUZZER_PIN, LOW);
      // Attente de deux secondes avant le prochain bip
      delay(2000);
    }
  }

  // Activation de la LED en fonction de la détection de mouvement
  if (lightLevel < 500){
    digitalWrite(LEDPIN, motionValue == HIGH ? HIGH : LOW);
  }
  
  // Vérification de l'interrupteur S
  if (s == HIGH) {
    if (motionValue == HIGH) {
      for (int i = 0; i < 150; i++) {
        // Activation du buzzer pendant deux secondes
        digitalWrite(BUZZER_PIN, HIGH);
        delay(2000);
        digitalWrite(BUZZER_PIN, LOW);
        // Attente de 500 ms avant le prochain bip
        delay(500);
      }
    }
  }
  postdata(temperature,humidity,lightLevel,motionValue,gasValue);

  // Attente de 1 seconde
  delay(1000);
}