/************************************************* loop *************************************************/
void loop() {


  t0 = millis();

  client = server.available();  // regarde si il y à une connections
  if ( client ) wifi();         // si un nouveau client,

  int distance_droit = lidar_droit.read();
  int distance_haut = lidar_haut.read();

  if ( lidar_haut.ranging_data.peak_signal_count_rate_MCPS>3  )
    distance_centre = lidar_centre.read();
  else 
     distance_centre = 4000;


  if (distance_centre < d_mini_vit ) vitesse = v_mini;
  else if (distance_droit < distance_dir - d_acc or distance_droit > distance_dir + d_acc ) vitesse = v_mini_centre;
  else if ( distance_centre > d_maxi_vit ) vitesse = v_maxi; 
  
 

 // Serial.print(" / vitesse 1 = ");
 // Serial.print(vitesse);

  //if      (vitesse > v_maxi) vitesse = v_maxi;
 // else if (vitesse < v_mini) vitesse = v_mini;

//  Serial.print(" / vitesse 2 = ");
//  Serial.println(vitesse);



 //////////////////////////////// direction  ////////////////////////////////////
  int largeur_dir = map(vitesse, v_mini, v_maxi, largeur_dir_pv, largeur_dir_gv); // en fonction de la vitesse

  if ( distance_centre < 100 and t0>trecul and marche_vit ) { // si trop près d'un mur
    recul = true; // enclanche la marche arrière
    trecul = t0 + t_recul;  // tempo de recul
    myDFPlayer.play(22); // joue le son du bip de recul
    leds.setPixelColor(1,0xffffff); // feux de recul en blanc
    leds.setPixelColor(4,0xffffff);// feux de recul en blanc
    leds.show();// allume les feux de recul
    direction = map(distance_droit, distance_dir+largeur_dir, distance_dir-largeur_dir, d_gauche, d_droite);
    if (angle_recul>0) direction = angle_recul; // change l'angle de direction quand on recul
  }

  if (recul){
    vitesse = v_recul; // recule en pv  
    if(t0>trecul) {
      recul = false;
      leds.setPixelColor(1,0); // eteint les feux de recul 
      leds.setPixelColor(4,0); // eteint les feux de recul     
      leds.show(); // eteint les feux de recul      
      myDFPlayer.play(18); // joue le son de dérappage
      arson = true;
      son_course = 18;
      tson_course = t0 + 2000;
    }
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

  if(mdirection != direction){ // servo direction 
    if (marche_dir){ // autorise la direction
      servo_direction.write(direction);
    } else servo_direction.write(90);
    mdirection = direction;
  }

  if (marche_vit) {
    servo_vitesse.write(vitesse);
  } else servo_vitesse.write(90);  // arret



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
 
  if ( lidar_haut.ranging_data.peak_signal_count_rate_MCPS>3  ){ // thaut filtre le capteur du haut
    if (distance_haut > d_haut_mini and distance_haut < d_haut_maxi) { // si dans la fourchette
      if (t0>thaut and marche_vit){ // si la tempo n'est pas acrive
        thaut = t0 + 2000; // active la tempo
        //myDFPlayer.play(24); // jour la olé
        arson = true;
        son_course = 24;
        tson_course = t0 + 4000;
        nb_tour_fait ++; // incrémente le nombre de tour

      }
    }

  }


/*  
  Serial.print("distance_haut = ");Serial.print(distance_haut);
  //Serial.print(" / distance_droit = ");Serial.print(distance_droit);
  Serial.println();
*/

  if      (digitalRead(pin_433_MD)) { // A
    marche_dir = true;
    mdirection = -1;
    marche_vit = true;
    nb_tour_fait = 0;
    arson = true;
    son_course = 30; // Démarreur
    tson_course = t0 + 1000;
    vitesse = v_mini;
  }
  else if (digitalRead(pin_433_MV)) { // B
    marche_dir = true;
  }
  else if (digitalRead(pin_433_AD)) { // C
    myDFPlayer.play(n_son);
  }
  else if (digitalRead(pin_433_AV) ) { // D
    marche_vit = false;
    marche_dir = false;
    myDFPlayer.stop();
  }
  else if (nb_tour_fait == nb_tour_a_faire){
    myDFPlayer.play(13); // jour la coucaratcha
    delay(500);
    marche_vit = false;
    marche_dir = false;     
    nb_tour_fait ++;
    t1 = t0 + 3000;
  }  
  else if (nb_tour_fait == nb_tour_a_faire + 1 and t0 > t1){
    myDFPlayer.play(28); // musique
 
    nb_tour_fait ++;
  }  

  if (marche_vit){

    if (mson_course != son_course){
      mson_course = son_course;
      myDFPlayer.play(son_course); 
      if(son_course == 30) delay(1000);
    }

    if (t0 > tson_course and arson){
      son_course = 29;
      arson = false;
    }  
  }



}
