/* Copyright (C) 2022 Alain Dube
 * All rights reserved.
 *
 * Projet Domotique
 * Ecole du Web
* Cours Systèmes embarqués (c)2022
 *  
    @file     MySerial.h
    @author   Alain Dubé
    @version  1.1 22/08/15 
    @description
      Démonstration comment utiliser le PORT SERIE RS232 UART ESP32

    platform = ESP32
    OS = Arduino
    Langage : C++


    Historique des versions
        Version    Date       Auteur       Description
        1.1        02/08/15  Alain       Première version du logiciel

**/
#ifndef MYTEMP_H
#define MYTEMP_H

#include <Arduino.h>
#include <string>

#include <Adafruit_Sensor.h>
#include <DHT.h>
#include <DHT_U.h>

class MyTemp {
    private:
       DHT *dht;
    public:
        MyTemp(uint8_t dhtPin, uint8_t dhtType);
        ~MyTemp() { };
        float readTemperature();
};
#endif
