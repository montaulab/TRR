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