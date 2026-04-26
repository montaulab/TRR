
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