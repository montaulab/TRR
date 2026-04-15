
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

void IRAM_ATTR ISR_tour_de_roue(); // fonction IRS en ram au lieu flash

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
int d_haut_mini, d_haut_maxi; // fourchette de détection du portique

int nb_tour_a_faire = 5;    // nombre de tour à faire
int nb_tour_fait = 0;  // // nombre de tour fait
volatile unsigned long nb_puls = 0; // volatile pour l'intéruption
unsigned int nb_puls_par_tour = 0; // nombre de puls roue pour un tour
unsigned int nb_puls_pour_10m = 0; // nombre de puls roue pour faire 10m
unsigned int nb_puls_a_faire = nb_puls_par_tour * nb_tour_a_faire + nb_puls_pour_10m;

int refresh = 0;

int direction = 90; // angle de direction au centre
int vitesse = 90;   // 90 vitesse à 0,

int coef_vitesse = 20; // divise la distance du lidar centrale pour donner une vitesse
int coef_direction = 5; // divise la diférence entre les deux lidars D/G pour donner la direction

int lidar; // différence entre lidar droit et gauche en cm

bool marche_dir, marche_vit; // mémoire marche arret du servo moteur et vitesse

unsigned long t0,t1; // tempo pour les leds
unsigned long tacc;
uint32_t led_rouge = 0xFF0000;
uint32_t led_orange = 0x801000;
uint32_t led_blanche = 0x555522;

int n_son = 1; // numéro de son à jouer
int v_son = 30; // volume du son

bool led_cligno = false; // bistable pour faire clignoter les leds

int page = 0; //numéro de page à afficher

const int n_distance = 5;  // nonbre de valeur à mémorisé
int f_distance[n_distance]; // stock des valeurs
int v_distance = 0; // total divisé par n_distance
int t_distance = 0; // total distance droit
int c_distance = 0; // curseur

/************************************************* setup *************************************************/
void setup() {

  Serial.begin(115200); // port série pour le débug
  Serial2.begin(9600, SERIAL_8N1, 16, 17);
  Wire.begin();
  Wire.setClock(400000); // use 400 kHz I2C

  EEPROM.begin(80); // réserve 10 octets dans la EEprom

  v_mini = ee_read(0);    // vitesse mini lecture eeprom adresse 0
  v_maxi = ee_read(2);    // vitesse maxi lecture eeprom adresse 2
  d_centre = ee_read(4);  // centre direction (90) eeprom adresse 4
  d_droite = ee_read(6);  // droite direction (180)eeprom adresse 6
  d_gauche = ee_read(8);  // gauche direction (0) eeprom adresse 8
  nb_tour_a_faire = ee_read(10); // nombre de tour à faire eeprom adresse 10
  d_mini_vit = ee_read(12); // coéf vitesse (20) eeprom adresse 10
  distance_dir = ee_read(14); // coef direction (5) eeprom adresse 10
  nb_puls_par_tour = ee_read(16); // nombre de puls roue pour un tour
  nb_puls_pour_10m = ee_read(18); // nombre de puls roue pour faire 10m
  n_son = ee_read(20); // numero du son à jouer avec le bouton
  v_son = ee_read(22); // volume du son à jouer avec le bouton
  largeur_dir_pv = ee_read(24); // volume du son à jouer avec le bouton
  d_maxi_vit = ee_read(26); // volume du son à jouer avec le bouton
  largeur_dir_gv = ee_read(28); // volume du son à jouer avec le bouton
  d_haut_mini = ee_read(30); // fourchette mini de détection du portique
  d_haut_maxi = ee_read(32); // fourchette maxi de détection du portique

  nb_puls_a_faire = nb_puls_par_tour * nb_tour_a_faire + nb_puls_pour_10m; // pour une course

  // pour ESP neuve: // pas utilisé
  if (v_mini>255) v_mini=0;
  if (v_maxi>255) v_maxi=150;
  if (d_centre>255) d_centre=90;
  if (d_droite>255) d_droite=180;
  if (d_gauche>255) d_gauche=0;

  WiFi.softAP(ssid, password);     // cré un point acces avec les codes défini avant
  WiFi.softAPConfig(local_ip, gateway, subnet); // configure les adresse ip du serveur

  IPAddress IP = WiFi.softAPIP(); // récupère l'adresse ip du serveur
  Serial.print("AP adresse IP : ");
  Serial.println(IP);

  server.begin();
  Serial.println("demarrage du serveur");
  delay(100);

  ESP32PWM::allocateTimer(0);
	ESP32PWM::allocateTimer(1);
	ESP32PWM::allocateTimer(2);
  ESP32PWM::allocateTimer(3);

  servo_direction.attach(pin_servo_direction, minUs, maxUs);
	servo_vitesse.attach(pin_servo_vitesse, minUs, maxUs);

  servo_vitesse.write(vitesse);
  servo_direction.write(direction);

  pinMode(pin_effet_hall, INPUT_PULLUP); // capteur effet Hall de la roue arrière droite

  init_lidar();

  // télécommande
  pinMode(pin_433_MD, INPUT); // A marche direction
  pinMode(pin_433_MV, INPUT); // B marche vitesse
  pinMode(pin_433_AD, INPUT); // C arret direction
  pinMode(pin_433_AV, INPUT); // D arret vitesse

  attachInterrupt(digitalPinToInterrupt(pin_effet_hall), ISR_tour_de_roue, RISING);

  leds.begin();
  leds.clear();

  myDFPlayer.begin(Serial2);

  myDFPlayer.volume(v_son);  //Set volume value. From 0 to 30
  myDFPlayer.play(15);

  leds.setPixelColor(0,led_rouge);
  leds.setPixelColor(5,led_rouge);
  leds.setPixelColor(6,led_blanche);
  leds.setPixelColor(11,led_blanche);
  leds.show();
}

// intéruption pour le nombre de tour de roue (ISR)
void IRAM_ATTR ISR_tour_de_roue() {
  nb_puls ++;

}


/************************************************* loop *************************************************/
void loop() {


  t0 = millis();

  /*
  Serial.print(t0);
  Serial.println();
  t0 = millis();
  */

  client = server.available();  // regarde si il y à une connections
  if ( client ) wifi();         // si un nouveau client,

  /*
  t_distance -= f_distance[n_distance];
  f_distance[n_distance] = lidar_droit.read();
  t_distance += f_distance[n_distance];
  int distance_droit = t_distance/n_distance;
  c_distance ++;
  if (c_distance == n_distance ) c_distance = n_distance;
*/


  int distance_droit = lidar_droit.read();
  int distance_haut = lidar_haut.read();
  int distance_centre = lidar_centre.read();

  ////////////////////////////  vitesse  ////////////////////////////////////////
  //vitesse = v_mini + distance_centre / coef_vitesse;
  // vitesse = map(distance_centre, d_mini_vit, d_maxi_vit, v_mini, v_maxi); // vitesse en fonction du lidar centre
  if (t0>tacc){
    tacc = t0 + 1000;
    if ( distance_centre > d_maxi_vit ) {
      if (vitesse < v_maxi) vitesse ++;
    } 
  }
if (distance_centre<d_mini_vit) vitesse = v_mini;

 // Serial.print(" / vitesse 1 = ");
 // Serial.print(vitesse);

  if      (vitesse > v_maxi) vitesse = v_maxi;
  else if (vitesse < v_mini) vitesse = v_mini;

//  Serial.print(" / vitesse 2 = ");
//  Serial.println(vitesse);

  if (marche_vit) {
    servo_vitesse.write(vitesse);
  } else servo_vitesse.write(90);  // arret


 //////////////////////////////// direction  ////////////////////////////////////
  int largeur_dir = map(vitesse, v_mini, v_maxi, largeur_dir_pv, largeur_dir_gv); // en fonction de la vitesse

  if ( distance_centre < 100 ) {// recul
    servo_vitesse.write(80);

    direction = map(distance_droit, distance_dir+largeur_dir, distance_dir-largeur_dir, d_gauche, d_droite);
  } else direction = map(distance_droit, distance_dir-largeur_dir, distance_dir+largeur_dir, d_gauche, d_droite);

  if (direction >d_droite) direction = d_droite;
  else if(direction < d_gauche) direction = d_gauche;


/*
  Serial.print(" / direction 2 = "); Serial.print(direction);
  Serial.print(" / largeur_dir = "); Serial.print(largeur_dir);
  Serial.print(" / vitesse = ");     Serial.print(vitesse);
  Serial.print(" / distance_centre = ");
  Serial.print(distance_centre);
  Serial.println();
*/

  if (marche_dir){
    servo_direction.write(direction);
  } else servo_direction.write(90);

 // Serial.println(direction);
 // Serial.print("range: ");
 // Serial.print(lidar_haut.ranging_data.range_mm);
//  Serial.print("\tstatus: ");
 // Serial.print(VL53L1X::rangeStatusToString(lidar_haut.ranging_data.range_status));
 // Serial.print("\tpeak signal: ");
  //Serial.print(lidar_haut.ranging_data.peak_signal_count_rate_MCPS);
 // Serial.print("\tambient: ");
 // Serial.print(lidar_haut.ranging_data.ambient_count_rate_MCPS);

 // Serial.println();

 // détection du portique
 if (lidar_haut.ranging_data.peak_signal_count_rate_MCPS>3){
  if (distance_haut > d_haut_mini and distance_haut < d_haut_maxi) myDFPlayer.play(13);
 }


/*  Serial.print("distance_haut = ");Serial.print(distance_haut);
  Serial.print(" / distance_droit = ");Serial.print(distance_droit);
  Serial.println();
*/

  if      (digitalRead(pin_433_MD)) marche_dir = true; // A
  else if (digitalRead(pin_433_MV)) marche_vit = true; // B
  else if (digitalRead(pin_433_AD)) marche_dir = false;// C
  else if (digitalRead(pin_433_AV)) marche_vit = false;// D
  else if (nb_puls > nb_puls_a_faire) marche_vit = false;






/*
  if (t0>t1){ // les leds
    t1 = t0 + 500;
    if (led_cligno) {
      leds.clear();
      leds.show();
      led_cligno = false;
    } else {
    //leds.setPixelColor(random(nb_leds), random(0xffffff) );
    leds.setPixelColor(0,led_rouge);
    leds.setPixelColor(5,led_rouge);
    leds.setPixelColor(6,led_blanche);
    leds.setPixelColor(11,led_blanche);
    led_cligno = true;
    leds.show();//leds.Color(255, 0, 0)
    }
  }
 */

}




//*************************************** init_lidar ******************************************
void init_lidar(){
    pinMode(18,OUTPUT);
    digitalWrite(18,0);
    pinMode(19,OUTPUT);
    digitalWrite(19,0);
    delay(100);

    Wire.begin(21, 22);
    delay(100);

    lidar_centre.setTimeout(500);
    if (!lidar_centre.init()) {
        //Serial.println("defaut lidar_centre");
        while (1);
    }
    lidar_centre.setAddress(0x2B);
    lidar_centre.startContinuous(50);
    lidar_centre.setDistanceMode(VL53L1X::Long);
    lidar_centre.setMeasurementTimingBudget(50000);
    delay(100);
    digitalWrite(18,1);
    delay(100);

    lidar_droit.setTimeout(500);
    if (!lidar_droit.init()) {
        //Serial.println("Defaut lidar_droit");
        while (1);
    }
    lidar_droit.setAddress(0x2C);
    lidar_droit.startContinuous(50);
    lidar_droit.setDistanceMode(VL53L1X::Long);
    lidar_droit.setMeasurementTimingBudget(50000);

    delay(100);
    digitalWrite(19,1);
    delay(100);

    lidar_haut.setTimeout(1);
    if (!lidar_haut.init()) {
        //Serial.println("Defaut lidar_haut");
        while (1);
    }
    lidar_haut.startContinuous(20);
    lidar_haut.setDistanceMode(VL53L1X::Short);
    lidar_haut.setMeasurementTimingBudget(20000);
}

/************************************************* wifi *************************************************/
void wifi(){

    refresh = 1;
    String request = client.readStringUntil('\r'); // récupère la requette
    client.flush(); // efface le tampon pour les prochaines requettes

    //Serial.print("debut - request = ");
    //Serial.println(request);

    if (request.indexOf("?d_mini_vit") != -1 ) { // requette c_vit coef_direction
      d_mini_vit = val(request);
      ee_write(d_mini_vit , 12); // enregistre v_maxi dans la EEprom à l'adresse 2
      p_2(); // affiche la page web 0
    }
    else if (request.indexOf("?d_maxi_vit") != -1 ) { // requette c_vit coef_direction
      d_maxi_vit = val(request);
      ee_write(d_maxi_vit , 26); // enregistre v_maxi dans la EEprom à l'adresse 2
      p_2(); // affiche la page web 0
    }
    else if (request.indexOf("?distance_dir") != -1 ) { // si la requette est c_dir (coef_direction)
      distance_dir = val(request);
      ee_write(distance_dir , 14); // enregistre v_mini dans la EEprom à l'adresse 0
      p_2(); // affiche la page web 0
    }
    else if (request.indexOf("?ldpv") != -1 ) { // si la requette est c_dir (coef_direction)
      largeur_dir_pv = val(request);
      ee_write(largeur_dir_pv , 24); // enregistre v_mini dans la EEprom à l'adresse 0
      p_2(); // affiche la page web 0
    }
    else if (request.indexOf("?ldgv") != -1 ) { // si la requette est c_dir (coef_direction)
      largeur_dir_gv = val(request);
      ee_write(largeur_dir_gv , 28); // enregistre v_mini dans la EEprom à l'adresse 0
      p_2(); // affiche la page web 0
    }
    else if (request.indexOf("?v_mini") != -1 ) { // si la requette est v_mini
      v_mini = val(request);
      ee_write(v_mini , 0); // enregistre v_mini dans la EEprom à l'adresse 0
      p_2(); // affiche la page web 0
    }
    else if (request.indexOf("?v_maxi") != -1 ) { // si la requette est /led
      v_maxi = val(request);
      ee_write(v_maxi , 2); // enregistre v_maxi dans la EEprom à l'adresse 2
      p_2(); // affiche la page web 0
    }
    else if (request.indexOf("?d_centre") != -1 ) { // si la requette est /led
      d_centre = val(request);
      ee_write(d_centre , 4); // enregistre d_centre dans la EEprom à l'adresse 4
      p_2(); // affiche la page web 0
    }
    else if (request.indexOf("?d_droite") != -1 ) { // si la requette est /led
      d_droite = val(request);
      ee_write(d_droite , 6); // enregistre d_droite dans la EEprom à l'adresse 6
      p_2(); // affiche la page web 0
    }
    else if (request.indexOf("?d_gauche") != -1 ) { // si la requette est /led
      d_gauche = val(request);
      ee_write(d_gauche , 8); // enregistre d_gauche dans la EEprom à l'adresse 8
      p_2(); // affiche la page web 0
    }
    else if (request.indexOf("?ntour") != -1 ) { // si la requette nb_tour_a_faire
      nb_tour_a_faire = val(request);
      nb_puls_a_faire = nb_puls_par_tour * nb_tour_a_faire + nb_puls_pour_10m;
      ee_write(nb_tour_a_faire , 10); // enregistre nb_tour_fait dans la EEprom à l'adresse 10
      p_3(); // affiche la page web 0
    }
    else if (request.indexOf("?nb_tour_fait") != -1 ) { // si la requette est nombre de tour de circuit fait
      nb_tour_fait = val(request);
      p_3(); // affiche la page web 0
    }
    else if (request.indexOf("?nb_puls") != -1 ) { // si la requette est nombre de tour de roue
      nb_puls = val(request);
      p_3(); // affiche la page web 0
    }
    else if (request.indexOf("?nb_tr") != -1 ) { // si la requette est nombre de puls roue pour un tour
      nb_puls_par_tour = val(request);
      nb_puls_a_faire = nb_puls_par_tour * nb_tour_a_faire + nb_puls_pour_10m;
      ee_write(nb_puls_par_tour , 16); // enregistre nombre de tour de roue dans la EEprom à l'adresse 16
      p_3(); // affiche la page web 0
    }
    else if (request.indexOf("?nb_r10m") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      nb_puls_pour_10m = val(request);
      nb_puls_a_faire = nb_puls_par_tour * nb_tour_a_faire + nb_puls_pour_10m;
      ee_write(nb_puls_pour_10m , 18); // enregistre nb_tour_fait dans la EEprom à l'adresse 10
      p_3(); // affiche la page web 0
    }
    else if (request.indexOf("?n_son") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      n_son = val(request);
      ee_write(n_son , 20); //
      p_1(); // affiche la page web 0
    }
    else if (request.indexOf("?v_son") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      v_son = val(request);
      ee_write(v_son , 22); //
      myDFPlayer.volume(v_son);
      p_1(); // affiche la page web 0
    }
    else if (request.indexOf("/p0") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/p1") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      p_1(); // affiche la page web 0
    }
    else if (request.indexOf("/js") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      myDFPlayer.play(n_son);
      p_1(); // affiche la page web 0
    }
     else if (request.indexOf("/p2") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      p_2(); // affiche la page réglage
    }
     else if (request.indexOf("/p3") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      p_3(); // affiche la page réglage
    }
    else if (request.indexOf("/_mdv") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      marche_dir = true;
      marche_vit = true;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/mv") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      marche_vit = true;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/md") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      marche_dir = true;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/ad") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      marche_dir = false;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/av") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      marche_vit = false;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/_adv") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      marche_dir = false;
      marche_vit = false;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("?dhmini") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      d_haut_mini = val(request);
      ee_write(d_haut_mini , 30); //
      p_3(); // affiche la page web 0
    }   
    else if (request.indexOf("?dhmaxi") != -1 ) { // si la requette est nombre de puls roue pour faire 10m
      d_haut_maxi = val(request);
      ee_write(d_haut_maxi , 32); //
      p_3(); // affiche la page web 0
    }      
    else {
      refresh = 0;
      if      (page == 0) p_0(); // acceuil
      else if (page == 1) p_1(); // reglage son
      else if (page == 2) p_2();  // reglage voiture
      else if (page == 3) p_3();  // reglage piste
    }// affiche la page web 0
  //Serial.print("fin - page = ");
  //Serial.println(page);
  client.println();
  client.stop();
}

int val(String requette){
  int a = requette.indexOf("=") ;
  requette.remove(0, a + 1);
  a = requette.indexOf(" ") ;
  requette.remove(a);
  return requette.toInt(); // récupère la valeur numérique
}

/************************************************* ee_write *************************************************/
void ee_write(int data, int adresse){ // écrit la EEprom
  EEPROM.write( adresse , data / 256 );  // écrit la valeur dans la EEprom octet 0
  EEPROM.write( adresse + 1 , data - ( data * 256 ) );  // écrit la valeur dans la EEprom octet 1
  EEPROM.commit();
}

/************************************************* ee_read *************************************************/
int ee_read(int adresse){ // lit la EEprom et retourn le résultat
 return 256 * EEPROM.read( adresse ) + EEPROM.read( adresse + 1 );
}


/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////// gestion des pages html //////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*################################################## page_0 ###################################################*/
void p_0(){ // menu général

  String t; // permetra de stocker du texte

  entete("Montaulab TRR 2026<br>" , refresh ); // cré une nouvelle page qui se rafraichi toute les 5s

  titre( "Données de la meilleure voiture", "h2" , "#33aa55", "#FFFFFF");

  br(); br();

  bouton(" Réglage du son " , "p1","dddddd","18" );

  br(); br();

  bouton(" Réglage voiture " , "p2","dddddd","18" );

  br(); br();

  bouton(" Réglage piste " , "p3","dddddd","18" );

  br(); br();

  p +="<hr>";

  br(); br();

  bouton(" Marche direction + vitesse " , "_mdv","55ff55","18" );

  br(); br();

  bouton(" Marche direction " , "md","dddddd","18" );

  br(); br();

  bouton(" Marche vitesse " , "mv","dddddd","18" );

  br(); br();

  bouton(" Arrêt direction " , "ad","dddddd","18" );

  br(); br();

  bouton(" Arrêt vitesse " , "av","dddddd","18" );

  br(); br();

  bouton(" Arrêt direction + vitesse " , "_adv" , "ff5555","18" );

  fin_page();  // fin de page HTML

  delay(1);
  page = 0;

  client.println( p );

} // fin page_0()


/*################################################## page_1 ###################################################*/
void p_1(){ // page HTML réglage du son

  String t; // permetra de stocker du texte

  entete("Montaulab TRR 2026<br>" , refresh ); // cré une nouvelle page qui se rafraichi toute les 5s

  titre( "Réglage du son", "h2" , "#33aa55", "#FFFFFF");

  br(); br();

  input("jouer son :\n" , n_son, "n_son"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Volume du son 0 - 30 \n" , v_son, "v_son"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  bouton(" jour le son " , "js", "dddddd", "18" );

  br(); br();

  bouton(" refresh " , "","dddddd","18" );

  br(); br();

  bouton(" Accueil " , "p0", "dddddd", "18" );

  br(); br();
  p += "1  - double klaxon <BR>";
  p += "2  - simple klaxon clio <BR>";
  p += "3  - simple klaxon <BR>";
  p += "4  -  <BR>";
  p += "5  - simple klaxon <BR>";
  p += "6  - plusieurs fois klaxon <BR>";
  p += "7  - double klaxon <BR>";
  p += "8  - moteur qui démarre + roulage <BR>";
  p += "9  -  <BR>";
  p += "10 - moteur qui démarre + roulage bruyant <BR>";
  p += "11 - moteur qui démarre + arret <BR>";
  p += "12 - moteur qui démarre + depart calme <BR>";
  p += "13 - la coukaracha <BR>";
  p += "14 - la corne  <BR>";
  p += "15 - alarme on off <BR>";
  p += "16 - f40 <BR>";
  p += "17 - f1 qui roule <BR>";
  p += "18 - dérapage <BR>";
  p += "19 - alarle qui sonne <BR>";
  p += "20 - vieux klaxon<BR>";

  fin_page();  // fin de page HTML

  delay(1);
  page = 1;
  client.println( p );

} // fin page_1()


/*################################################## page_2 ###################################################*/
void p_2(){ // parametre voiture
  String t; // permetra de stocker du texte

  entete("Montaulab TRR 2026<br>" , refresh ); // cré une nouvelle page qui se rafraichi toute les 5s

  titre( "Réglage de la voiture", "h2" , "#33aa55", "#FFFFFF");

  br(); br();

  input("vitesse mini ( de 0 à 180 )\n" , v_mini, "v_mini"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("vitesse maxi ( de 0 à 180 )\n" , v_maxi, "v_maxi"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Distance mini petite vitesse" , d_mini_vit, "d_mini_vit"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Distance maxi grande vitesse" , d_maxi_vit, "d_maxi_vit"); // input(texte, valeur à modifier, nom de la requette)

  br(); line(); br();

  input("Distance du bord droit" ,distance_dir , "distance_dir"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("largeur direction pv" ,largeur_dir_pv , "ldpv"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("largeur direction gv" ,largeur_dir_gv , "ldgv"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("angle de direction au centre (défaut 90)\n" , d_centre, "d_centre"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("angle de direction maxi à droite (défaut 180)\n" , d_droite, "d_droite"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("angle de direction maxi à gauche (défaut 0)\n" ,d_gauche, "d_gauche"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  bouton(" refresh " , "","dddddd","18" );

  br(); br();

  bouton(" accueil " , "p0","dddddd","18" );

  fin_page();  // fin de page HTML

  delay(1);
  page = 2;
  client.println( p );

} // fin page_2()


/*################################################## page_3 ###################################################*/
void p_3(){ // page HTML réglage piste

  String t; // permetra de stocker du texte

  entete("Montaulab TRR 2026<br>" , refresh ); // cré une nouvelle page qui se rafraichi toute les 5s

  titre( "Réglage de la piste", "h3" , "#33aa55", "#FFFFFF");

  br(); br();

  input("Nb tour de circuit à faire\n" , nb_tour_a_faire, "ntour"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Nb tour de circuit fait\n" , nb_tour_fait, "nb_tour_fait"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Nb puls roue\n" , nb_puls, "nb_puls"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Nb puls roue pour faire un tour de circuit\n" , nb_puls_par_tour, "nb_tr"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Nb puls roue pour faire 10m\n" , nb_puls_pour_10m , "nb_r10m"); // input(texte, valeur à modifier, nom de la requette)

  br(); line();

  input("distance mini portique" , d_haut_mini, "dhmini"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Distance maxi portique" , d_haut_maxi , "dhmaxi"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();
  
  bouton(" refresh " , "","dddddd","18" );

  br(); br();

  bouton(" Accueil " , "p0", "dddddd", "18" );

  fin_page();  // fin de page HTML

  delay(1);
  page = 3;
  client.println( p );

} // fin page_3()


// fonction pour gérer le HTML plus facilement
/*##################################################  entête  ###################################################*/
void entete(String texte, int temps){// header texte=titre de la fenetre, r = au temps en seconde avant de rafraichir la fenetre
   // entête des pages html
  p = "<!doctype html>\n";
  p += "<head>\n";
  p += "<meta charset=\"utf-8\">\n";
  p += "<title>TRR - fifi82 2026</title>\n";
  if(temps) p += "<meta http-equiv=\"refresh\" content=\"" + String(temps) + ";/\">\n"; // permet de recharger la page toute les r secondes
  p += "<link rel=\"stylesheet\" href=\"style.css\">\n";
  p += "<script src=\"script.js\"></script>\n";
  p += "<meta name='viewport' content='width=device-width, initial-scale=1.0'>\n";
  p += "</head>\n";
  p += "<body><center>";
  p += "<h1><p style=\"background-color: #000000; color: white;\">" + texte + "<br>";
  p += "</h1>";
}

/*################################################## bouton ###################################################*/
void bouton(String texte, String requette, String couleur, String fonte){// \" affiche un " en HTML

  p +=  "<input type=\"button\" onclick=\"window.location.href = '/" + requette + "';\" value=\"\n  " + texte;
  p +=  "  \n \"style=\"background-color: #" + couleur + "; color: #F000000; font-size:"+fonte+"px\" /><br>";
}

/*################################################## fin page ###################################################*/
void fin_page(){
  p+= "<p style=\"background-color: #ffffff;color: white;\">. . . . . . . . . . . . . . . . . . . . . .</p></h2></center></body> </html>";
}

/*################################################## texte ###################################################*/
void texte(String texte, String h){
  p +="<" + h + ">" + texte + "</" + h + ">" +"<br>";
}

/*################################################## titre ###################################################*/
void titre2(String texte, String cf, String ct){ // h=taille du texte, cf=couleur du fond, ct=couleur du texte
  p += "<p style=\"background-color:"+ cf +"; color: "+ ct +";\">"+texte+"</p>";
}

/*################################################## titre ###################################################*/
void titre(String texte, String h, String cf, String ct){ // h=taille du texte, cf=couleur du fond, ct=couleur du texte
  p += "<" + h + "><p style=\"background-color:"+ cf +"; color: "+ ct +";\">"+texte+"</p></" + h + ">";
}

/*################################################## br ###################################################*/
void br(){ // retour à la ligne
  p += "<br>";
}

/*################################################## input ###################################################*/
void input(String texte, int val, String name){ //
  p += "<form>" + texte +"<input type=\"number\" name=\"" + name + "\" value=\"" + String(val) + "\" /> </form>";
}

/*################################################## line ###################################################*/
void line(){ //
  p += "<hr size=\"10px\" color=\"green\" />";
}

/*################################################## bp_radio ###################################################*/
//void bp_radio(String nom, String val1, String val1){ //
  //p += <input type="radio" id="contactChoice1" name="contact" value="email" />

/*
  <div>
    <input type="radio" id="contactChoice1" name="contact" value="email" />
    <label for="contactChoice1">Adresse électronique</label>

    <input type="radio" id="contactChoice2" name="contact" value="telephone" />
    <label for="contactChoice2">Téléphone</label>

    <input type="radio" id="contactChoice3" name="contact" value="courrier" />
    <label for="contactChoice3">Courrier</label>
  </div>
*/
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
*/
