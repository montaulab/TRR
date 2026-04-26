/*################################################## page_3 ###################################################*/
void p_3(){ // page HTML réglage piste

  String t; // permetra de stocker du texte

  entete("Montaulab TRR 2026<br>" , refresh ); // cré une nouvelle page qui se rafraichi toute les 5s

  titre( "Réglage de la piste", "h3" , "#33aa55", "#FFFFFF");

  br(); br();

  input("Nb tour de circuit à faire<br>" , nb_tour_a_faire, "ntour"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Nb tour de circuit fait<br>" , nb_tour_fait, "nb_tour_fait"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();
/*
  input("Nb puls roue<br>" , nb_puls, "nb_puls"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Nb puls roue pour faire un tour de circuit<br>" , nb_puls_par_tour, "nb_tr"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Nb puls roue pour faire 10m<br>" , nb_puls_pour_10m , "nb_r10m"); // input(texte, valeur à modifier, nom de la requette)
*/
  br(); line();

  input("distance mini portique<br>" , d_haut_mini, "dhmini"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Distance maxi portique<br>" , d_haut_maxi , "dhmaxi"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();
  
  bouton(" refresh " , "","dddddd","18" );

  br(); br();

  bouton(" Accueil " , "p0", "dddddd", "18" );

  fin_page();  // fin de page HTML

  delay(1);
  page = 3;
  client.println( p );

} // fin page_3()