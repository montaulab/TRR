
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
  //nb_puls_par_tour = ee_read(16); // nombre de puls roue pour un tour
  //nb_puls_pour_10m = ee_read(18); // nombre de puls roue pour faire 10m
  n_son = ee_read(20); // numero du son à jouer avec le bouton
  v_son = ee_read(22); // volume du son à jouer avec le bouton
  largeur_dir_pv = ee_read(24); // volume du son à jouer avec le bouton
  d_maxi_vit = ee_read(26); // volume du son à jouer avec le bouton
  largeur_dir_gv = ee_read(28); // volume du son à jouer avec le bouton
  d_haut_mini = ee_read(30); // fourchette mini de détection du portique
  d_haut_maxi = ee_read(32); // fourchette maxi de détection du portique
  t_recul = ee_read(34); // temps de recul en ms
  v_recul = ee_read(36); // vitesse de recul
  angle_recul = ee_read(38); // angle de recul ( si 0 non actif)
  d_acc = ee_read(40); // temps accélération
  v_mini_centre = ee_read(42);

  //nb_puls_a_faire = nb_puls_par_tour * nb_tour_a_faire + nb_puls_pour_10m; // pour une course

  // pour ESP neuve: // pas utilisé
  if (v_mini>255) v_mini=90;
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

  //pinMode(pin_effet_hall, INPUT_PULLUP); // capteur effet Hall de la roue arrière droite

  init_lidar();

  // télécommande
  pinMode(pin_433_MD, INPUT); // A marche direction
  pinMode(pin_433_MV, INPUT); // B marche vitesse
  pinMode(pin_433_AD, INPUT); // C arret direction
  pinMode(pin_433_AV, INPUT); // D arret vitesse

  //attachInterrupt(digitalPinToInterrupt(pin_effet_hall), ISR_tour_de_roue, RISING);

  leds.begin();
  leds.clear();

  myDFPlayer.begin(Serial2);

  myDFPlayer.volume(30);  //Set volume value. From 0 to 30
  myDFPlayer.play(15);
  delay(1000);
  myDFPlayer.volume(v_son);  //Set volume value. From 0 to 30

  leds.setPixelColor(0,led_rouge);
  leds.setPixelColor(5,led_rouge);
  leds.setPixelColor(6,led_blanche); // led avant
  leds.setPixelColor(11,led_blanche);// led avant
  leds.show();
}