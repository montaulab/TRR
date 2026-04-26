
#include <Adafruit_NeoPixel.h> // https://github.com/adafruit/Adafruit_NeoPixel/tree/master
#include <WiFi.h>       // pour la gestion du wifi
#include <EEPROM.h>     // EEprom pour la sauvegarde des datas
#include <Wire.h>
#include <VL53L1X.h>    // https://github.com/pololu/vl53l1x-arduino
#include <ESP32Servo.h> // https://madhephaestus.github.io/ESP32Servo/annotated.html
#include "DFRobotDFPlayerMini.h"

DFRobotDFPlayerMini myDFPlayer;

#define pin_effet_hall 13 // capteur effet hall sur roue arière droite
#define pin_leds 14  // GPIO14 led WS2812B
#define pin_servo_vitesse 15 // ECS (variateur) du moteur
#define pin_servo_direction 2 // servomoteur de direction
#define pin_433_MD 32 // Marche Direction télécommande 433mhz
#define pin_433_AV 33 // Arret Vitesse télécommande 433mhz
#define pin_433_MV 34 // Marche Vitesse télécommande 433mhz
#define pin_433_AD 35 // Arret Direction télécommande 433mhz

#define nb_leds 12   // nombre de leds, 6 leds avant, 6 leds arrière

Adafruit_NeoPixel leds(nb_leds, pin_leds, NEO_GRB + NEO_KHZ800);

//void IRAM_ATTR ISR_tour_de_roue(); // fonction IRS en ram au lieu flash

// les servomoteurs
Servo servo_direction;
Servo servo_vitesse;

VL53L1X lidar_centre;
VL53L1X lidar_droit;
VL53L1X lidar_haut;

int minUs = 1000;
int maxUs = 2000;

// SSID & Password
const char* ssid = "le_Wi_Fifi";  // Enter your SSID here
const char* password = "123456789";  //Enter your Password here

// IP Address details
IPAddress local_ip(192, 168, 1, 1);
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);

WiFiServer server(80);  // objet pour le serveur web
WiFiClient client;      // prépare la récetion du client

String p; // contient la page web à afficher

int v_mini,v_maxi;  // vitesse ini et maxi de la voiture
int d_centre;       // centre du servo de direction par défaut 90
int d_droite;       // maxi doite du servo de direction par défaut 180
int d_gauche;       // maxi gauche du servo de direction par défaut 0
int distance_dir = 160;
int largeur_dir_pv = 200;
int largeur_dir_gv = 600;
int d_mini_vit = 1000;
int d_maxi_vit = 2000;
int v_recul; //vitesse de recul 80
int angle_recul;
int d_acc;
int v_mini_centre;
int distance_centre;
int d_haut_mini, d_haut_maxi; // fourchette de détection du portique
unsigned long thaut; // tempo filtrage haut

int nb_tour_a_faire = 5;    // nombre de tour à faire
int nb_tour_fait = 0;  // // nombre de tour fait
//volatile unsigned long nb_puls = 0; // volatile pour l'intéruption
//unsigned int nb_puls_par_tour = 0; // nombre de puls roue pour un tour
//unsigned int nb_puls_pour_10m = 0; // nombre de puls roue pour faire 10m
//unsigned int nb_puls_a_faire = nb_puls_par_tour * nb_tour_a_faire + nb_puls_pour_10m;

int refresh = 0;

int direction = 90; // angle de direction au centre
int vitesse = 90;   // 90 vitesse à 0,
int mdirection;

int coef_vitesse = 20; // divise la distance du lidar centrale pour donner une vitesse
int coef_direction = 5; // divise la diférence entre les deux lidars D/G pour donner la direction

int lidar; // différence entre lidar droit et gauche en cm

bool marche_dir, marche_vit; // mémoire marche arret du servo moteur et vitesse

unsigned long t0,t1; // tempo pour les leds
unsigned long tacc; // tempo accélération
unsigned long tson_course; // tempo pour le son de roulage
int  son_course,mson_course; // son à jouer pendant la course 
bool arson; // anti rebond

unsigned long trecul; // tempo recul
int t_recul; // temps de recul
byte led_ar;
byte mled_ar; // 0 = éteint, 1= rouge, 2 = blanc, 3 = orange clignotant
bool recul = false;

uint32_t led_rouge = 0xFF0000;
uint32_t led_orange = 0x801000;
uint32_t led_blanche = 0x555522;

int n_son = 1; // numéro de son à jouer
int v_son = 30; // volume du son

bool led_cligno = false; // bistable pour faire clignoter les leds

int page = 0; //numéro de page à afficher


// intéruption pour le nombre de tour de roue (ISR)
//void IRAM_ATTR ISR_tour_de_roue() {
//  nb_puls ++;
//}


/*
// audio
1  - double klaxon
2  - simple klaxon clio
3  - simple klaxon
4  -
5  - simple klaxon
6  - plusieurs fois klaxon
7  - double klaxon
8  - moteur qui démarre + roulage
9  -
10 - moteur qui démarre + roulage bruyant
11 - moteur qui démarre + arret
12 - moteur qui démarre + depart calme
13 - la coukaracha
14 - la corne
15 - alarme on off
16 - f40
17 - f1 qui roule
18 - dérapage
19 - alarle qui sonne
20 - vieux klaxon
21 - voiture qui recule
22 - bip recul
23 - bip recul
*/