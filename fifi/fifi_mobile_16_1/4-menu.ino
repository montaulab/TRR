/*################################################## page_0 ###################################################*/
void p_0(){ // menu général

  String t; // permetra de stocker du texte

  entete("Montaulab TRR 2026<br>" , refresh ); // cré une nouvelle page qui se rafraichi toute les 5s

  titre( "Données de la meilleure voiture", "h2" , "#33aa55", "#FFFFFF");

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

  br(); 
  p +="<hr>";
  br();

  bouton(" Réglage du son " , "p1","dddddd","18" );

  br(); br();

  bouton(" Réglage voiture " , "p2","dddddd","18" );

  br(); br();

  bouton(" Réglage piste " , "p3","dddddd","18" );

  br(); br();


  fin_page();  // fin de page HTML

  delay(1);
  page = 0;

  client.println( p );

} // fin page_0()
