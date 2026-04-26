
/************************************************* wifi *************************************************/
void wifi(){

    refresh = 1;
    String request = client.readStringUntil('\r'); // récupère la requette
    client.flush(); // efface le tampon pour les prochaines requettes

    //Serial.print("debut - request = ");
    //Serial.println(request);

    if (request.indexOf("?d_mini_vit") != -1 ) { // distance passage en pv
      d_mini_vit = val(request);
      ee_write(d_mini_vit , 12); // enregistre v_maxi dans la EEprom à l'adresse 12
      p_2(); // affiche la page web 2
    }
    else if (request.indexOf("?d_maxi_vit") != -1 ) { // distance passage gv
      d_maxi_vit = val(request);
      ee_write(d_maxi_vit , 26); // enregistre v_maxi dans la EEprom à l'adresse 26
      p_2(); // affiche la page web 2
    }
    else if (request.indexOf("?distance_dir") != -1 ) { // distance du bord droit
      distance_dir = val(request);
      ee_write(distance_dir , 14); // enregistre v_mini dans la EEprom à l'adresse 14
      p_2(); // affiche la page web 2
    }
    else if (request.indexOf("?ldpv") != -1 ) { // fourchette de direction en pv
      largeur_dir_pv = val(request);
      ee_write(largeur_dir_pv , 24); // enregistre v_mini dans la EEprom à l'adresse 24
      p_2(); // affiche la page web 2
    }
    else if (request.indexOf("?ldgv") != -1 ) { // largeur de direction en gv
      largeur_dir_gv = val(request);
      ee_write(largeur_dir_gv , 28); // enregistre v_mini dans la EEprom à l'adresse 28
      p_2(); // affiche la page web 2
    }
    else if (request.indexOf("?v_mini") != -1 ) { // vitesse mini
      v_mini = val(request);
      ee_write(v_mini , 0); // enregistre v_mini dans la EEprom à l'adresse 0
      p_2(); // affiche la page web 2
    }
    else if (request.indexOf("?v_maxi") != -1 ) { // vitesse maxi
      v_maxi = val(request);
      ee_write(v_maxi , 2); // enregistre v_maxi dans la EEprom à l'adresse 2
      p_2(); // affiche la page web 2
    }
    else if (request.indexOf("?d_centre") != -1 ) { // servo direction au centre 90
      d_centre = val(request);
      ee_write(d_centre , 4); // enregistre d_centre dans la EEprom à l'adresse 4
      p_2(); // affiche la page web 2
    }
    else if (request.indexOf("?d_droite") != -1 ) { // servo direction a droite 180
      d_droite = val(request);
      ee_write(d_droite , 6); // enregistre d_droite dans la EEprom à l'adresse 6
      p_2(); // affiche la page web 2
    }
    else if (request.indexOf("?d_gauche") != -1 ) { // servo direction a gauche 0 
      d_gauche = val(request);
      ee_write(d_gauche , 8); // enregistre d_gauche dans la EEprom à l'adresse 8
      p_2(); // affiche la page web 2
    }
    else if (request.indexOf("?ntour") != -1 ) { // si la requette nb_tour_a_faire
      nb_tour_a_faire = val(request);
      //nb_puls_a_faire = nb_puls_par_tour * nb_tour_a_faire + nb_puls_pour_10m;
      ee_write(nb_tour_a_faire , 10); // enregistre nb_tour_fait dans la EEprom à l'adresse 10
      p_3(); // affiche la page web 3
    }
    else if (request.indexOf("?nb_tour_fait") != -1 ) { // si la requette est nombre de tour de circuit fait
      nb_tour_fait = val(request);
      p_3(); // affiche la page web 3
    }
    /*
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
    */
    else if (request.indexOf("?n_son") != -1 ) { // numéro de son à jouer
      n_son = val(request);
      ee_write(n_son , 20); //
      p_1(); // affiche la page web 1
    }
    else if (request.indexOf("?v_son") != -1 ) { // volume du son
      v_son = val(request);
      ee_write(v_son , 22); // enregistre EEprom à l'adresse 22
      myDFPlayer.volume(v_son);
      p_1(); // affiche la page web 1
    }
    else if (request.indexOf("/js") != -1 ) { // joue un son
      myDFPlayer.play(n_son);
      p_1(); // affiche la page web 0
    }    
    else if (request.indexOf("/p0") != -1 ) { // affiche la page acceuil
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/p1") != -1 ) { // affiche la page son
      p_1(); // affiche la page web 0
    }
     else if (request.indexOf("/p2") != -1 ) { // affiche reglage voiture
      p_2(); // affiche la page réglage
    }
     else if (request.indexOf("/p3") != -1 ) { // affiche réglage piste
      p_3(); // affiche la page réglage
    }
    else if (request.indexOf("/_mdv") != -1 ) { // bouton vert marche voiture
      marche_dir = true;
      marche_vit = true;
      nb_tour_fait = 0;
      arson = true;
      son_course = 30; // Démarreur
      tson_course = t0 + 1000;
      vitesse = v_mini;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/mv") != -1 ) { // bouton marche vitesse
      marche_vit = true;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/md") != -1 ) { // bouton marche direction
      marche_dir = true;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/ad") != -1 ) { // bouton stop direction
      marche_dir = false;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/av") != -1 ) { // bouton stop vitesse
      marche_vit = false;
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("/_adv") != -1 ) { // bouton stop
      marche_dir = false;
      marche_vit = false;
      myDFPlayer.stop();
      p_0(); // affiche la page web 0
    }
    else if (request.indexOf("?dhmini") != -1 ) { // fourchette mini portique
      d_haut_mini = val(request);
      ee_write(d_haut_mini , 30); //
      p_3(); // affiche la page web 3
    }   
    else if (request.indexOf("?dhmaxi") != -1 ) { // fourchette maxi portique
      d_haut_maxi = val(request);
      ee_write(d_haut_maxi , 32); //
      p_3(); // affiche la page web 3
    } 
    else if (request.indexOf("?t_recul") != -1 ) { //  temps de recul
      t_recul = val(request);
      ee_write(t_recul , 34); //
      p_2(); // affiche la page web 3
    } 
    else if (request.indexOf("?v_recul") != -1 ) { //  temps de recul
      v_recul = val(request);
      ee_write(v_recul , 36); //
      p_2(); // affiche la page web 3
    } 
    else if (request.indexOf("?angle_recul") != -1 ) { //  temps de recul
      angle_recul = val(request);
      ee_write(angle_recul , 38); //
      p_2(); // affiche la page web 3
    }    
    else if (request.indexOf("?d_acc") != -1 ) { //  temps de recul
      d_acc = val(request);
      ee_write(d_acc , 40); //
      p_2(); // affiche la page web 3
    }  
    else if (request.indexOf("?v_m_centre") != -1 ) { //  temps de recul
      v_mini_centre = val(request);
      ee_write(v_mini_centre , 42); //
      p_2(); // affiche la page web 3
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