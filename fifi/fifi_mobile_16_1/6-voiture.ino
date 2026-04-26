
/*################################################## page_2 ###################################################*/
void p_2(){ // parametre voiture
  String t; // permetra de stocker du texte

  entete("Montaulab TRR 2026<br>" , refresh ); // cré une nouvelle page qui se rafraichi toute les 5s

  titre( "Réglage de la voiture", "h2" , "#33aa55", "#FFFFFF");

  titre( "Vitesse", "h3" , "#3355aa", "#FFFFFF");
  br();

  input("vitesse mini ( de 0 à 180 )<br>" , v_mini, "v_mini"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("vitesse mini centre<br>" , v_mini_centre, "v_m_centre"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("vitesse maxi ( de 0 à 180 )<br>" , v_maxi, "v_maxi"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();
  
  input("distance du bord pv<br>" , d_acc, "d_acc"); // input(texte, valeur à modifier, nom de la requette)

  br();  br();

  input("Distance mini petite vitesse<br>" , d_mini_vit, "d_mini_vit"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Distance maxi grande vitesse<br>" , d_maxi_vit, "d_maxi_vit"); // input(texte, valeur à modifier, nom de la requette)

  br(); titre( "Direction", "h3" , "#3355aa", "#FFFFFF"); br();

  input("Distance du bord droit<br>" ,distance_dir , "distance_dir"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("largeur direction pv<br>" ,largeur_dir_pv , "ldpv"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("largeur direction gv<br>" ,largeur_dir_gv , "ldgv"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("angle de direction au centre (défaut 90)<br>" , d_centre, "d_centre"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("angle de direction maxi à droite (défaut 180)<br>" , d_droite, "d_droite"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("angle de direction maxi à gauche (défaut 0)<br>" ,d_gauche, "d_gauche"); // input(texte, valeur à modifier, nom de la requette)

  br(); titre( "Recul", "h3" , "#3355aa", "#FFFFFF"); br();

  input("temps de recul ms<br>" ,t_recul , "t_recul"); // input(texte, valeur à modifier, nom de la requette)
  
  br(); br();
  
  input("vitesse de recul de recul<br>" ,v_recul , "v_recul"); // input(texte, valeur à modifier, nom de la requette)
  
  br(); br();

  input("angle de recul de recul<br>" ,angle_recul , "angle_recul"); // input(texte, valeur à modifier, nom de la requette)
  
  br(); br(); 

  bouton(" refresh " , "","dddddd","18" );

  br(); br();

  bouton(" accueil " , "p0","dddddd","18" );

  fin_page();  // fin de page HTML

  delay(1);
  page = 2;
  client.println( p );

} // fin page_2()
