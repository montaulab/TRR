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
