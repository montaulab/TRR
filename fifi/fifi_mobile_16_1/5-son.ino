/*################################################## page_1 ###################################################*/
void p_1(){ // page HTML réglage du son

  String t; // permetra de stocker du texte

  entete("Montaulab TRR 2026<br>" , refresh ); // cré une nouvelle page qui se rafraichi toute les 5s

  titre( "Réglage du son", "h2" , "#33aa55", "#FFFFFF");

  br(); br();

  input("jouer son :<br>", n_son, "n_son"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  input("Volume du son 0 - 30<br>" , v_son, "v_son"); // input(texte, valeur à modifier, nom de la requette)

  br(); br();

  bouton(" jour le son<br>" , "js", "dddddd", "18" );

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
  p += "21 - voiture recule <BR>";
  p += "22 - bip de recul <BR>";
  p += "23 - bip de recul<BR>";
  p += "24 - Olé<BR>";  
  p += "25 - hi ha ha yé yé<BR>";   
  p += "26 - mario<BR>";     
  p += "27 - bruit de van corne<BR>";   
  p += "28 - farfadet-malicieux<BR>"; 
  p += "29 - voiture qui roule<BR>"; 
  p += "30 - démarreur<BR>"; 

  fin_page();  // fin de page HTML

  delay(1);
  page = 1;
  client.println( p );

} // fin page_1()
