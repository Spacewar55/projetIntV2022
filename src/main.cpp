/* Copyright (C) 2022 Alain Dube
 * All rights reserved.
 *
 * Projet Stone de base
 * Ecole du Web
 * Cours Systèmes embarqués (c)2022
 *
    @file     main.cpp
    @author   Alain Dubé
    @version  1.1 22/08/15
    @description
      Démonstration comment utiliser le PORT SERIE pour accèder aux fonctionnalités
      de l'écran STONE en utilisant la classe MyStone et MySerial

    platform = ESP32
    OS : Arduino
    Langage : C++

    Historique des versions
        Version    Date       Auteur          Description
        1.1        22/08/15   Alain           Première version du logiciel
        1.2        11/05/22   Alex De Souza   Ajout de la classe MyTemp

    Fonctionnalités implantées
        Lecture des evénements envoyés par l'écran
        Envoyer une commande à l'écran
          Optenir la version du Firmware de l'écran
          Obtenir la température grace au DHT
 * */

#include <Arduino.h>

#define RXD2 18//16
#define TXD2 19//17
#define BAUD_RATE 115200

#include <iostream>

#include "MyButton.h"
MyButton *myButtonT4 = new MyButton();
MyButton *myButtonT5 = new MyButton();

#include "MyStone.h"
MyStone *myStone;

#define DHTPIN 4
#define DHTTYPE DHT22
#include "MyTemp.h"
MyTemp myTemp(DHTPIN, DHTTYPE);

int compteur = 0;
int compteurMax = 20;
int tempsMinBois = 22.5;
int tempsMaxBois = 27.5;
bool buttonStoneDemarrageFour;

std::string intToHexa(int value){
  char buffer[10];
  sprintf(buffer , "0x%4X", value);
  return (buffer);
};

std::string charToString(char str, std::string formatStr){
  char buffer[10];
  sprintf(buffer , formatStr.c_str(), str);
  return (buffer);
};

std::string intToString(int value, std::string formatStr){
  char buffer[10];
  sprintf(buffer , formatStr.c_str(), value);
  return (buffer);
};

//Thread qui permet de LOOPER et lire le contenu du port serie
//avec l'aide du la fonction getValidsDatasIfExists
void readStoneData() {
  datasRead rd = myStone->getValidsDatasIfExists();
  //std::cout << "GData : " << intToHexa(abs(rd.id)) << " " << rd.command << " " << rd.name << " " << rd.type << "\n";
  switch(rd.id){

      case 0x0002: { //Version
          std::cout << "GData : " << intToHexa(abs(rd.id)) << " " 
                                  << rd.command << " " 
                                  << rd.name << " " 
                                  << rd.type << "\n";
          std::string stoneVersion = rd.name;
          std::cout << "Version : " <<  stoneVersion.c_str() << "\n";

          //std::this_thread::sleep_for(std::chrono::milliseconds(10));

          break;
          }

      case 0x1001: { //Bouton
          std::cout << "GData : " << intToHexa(abs(rd.id)) << " " 
                                  << rd.command << " " 
                                  << rd.name << " " 
                                  << rd.type << "\n";
          std::string stoneBoutton = rd.name;
          std::cout << "Bouton : " <<  stoneBoutton.c_str() << "\n";
          if( (strcmp(stoneBoutton.c_str(),"button1")==0) && (rd.type==2)){
            buttonStoneDemarrageFour = true;
          }

          break;
          }
      }

  if(rd.id<0) std::cout << "Data received ( id: : " << intToHexa(abs(rd.id)) << "  Command: " << rd.command << " Type: " << rd.type<< ")\n";
}

void setup() {
  
  Serial.begin(9600);

  Serial.println("Stone serial Txd is on pin: "+String(TXD2));
  Serial.println("Stone serial Rxd is on pin: "+String(RXD2));

  myStone = new MyStone(115200, SERIAL_8N1, RXD2, TXD2);

  myButtonT4->init(T4);
  myButtonT4->autoSensibilisation(); //Trouve la sensibilité automatiquement

  myButtonT5->init(T5);
  myButtonT5->autoSensibilisation(); //Trouve la sensibilité automatiquement

  cout << std::string("Début de l'exemple Stone de base pour le ESP32")  << "\n";
}

void loop() {
  delay(2000);

  float t = myTemp.readTemperature();

  readStoneData();

  if (isnan(t)){
    Serial.println(F("Failed to read from DHT sensor!"));
    return;
  }

  char strTemperature[64];
  sprintf(strTemperature, "%g Celcius", t);
  myStone->setLabel("lbltempfour", strTemperature);
  
  if(buttonStoneDemarrageFour == true){

    myStone->setLabel("lblmessagefin", "Température du four non correcte");
    sprintf(strTemperature, "%g Celcius", t);
    myStone->setLabel("lbltempfour", strTemperature);

    if(t >= tempsMinBois && t <tempsMaxBois){
      int i = 0;
      myStone->setLabel("lblmessagefin", "Chauffage du bois en cours");
      myStone->setLabel("lbltempfour", strTemperature);
      for(i=compteur; i<compteurMax; i++)
      {
        compteur++;
        char strCompteur[64];
        sprintf(strCompteur, "%d s", compteur);
        myStone->setLabel("lbltemps", strCompteur);
        Serial.println(compteur);
        Serial.print(F("Temperature: "));
        Serial.print(t);
        Serial.print(F("°C"));
        Serial.println();
        delay(1000);
        if(compteur == compteurMax)
        {
          buttonStoneDemarrageFour = false;
          myStone->setLabel("lblmessagefin", "Cuisson du bois terminée");
          myStone->setLabel("lbltempfour", strTemperature);
        }  
      }
    }
  }
  char strCompteur[64];
  sprintf(strCompteur, "%d s", compteur);
  compteur = 0;
  myStone->setLabel("lbltemps", strCompteur);

  Serial.print(F("Temperature: "));
  Serial.print(t);
  Serial.print(F("°C"));
  Serial.println();
}