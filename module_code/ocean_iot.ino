/*

   ██████╗  ██████╗███████╗ █████╗ ███╗   ██╗    ██╗ ██████╗ ████████╗
  ██╔═══██╗██╔════╝██╔════╝██╔══██╗████╗  ██║    ██║██╔═══██╗╚══██╔══╝
  ██║   ██║██║     █████╗  ███████║██╔██╗ ██║    ██║██║   ██║   ██║   
  ██║   ██║██║     ██╔══╝  ██╔══██║██║╚██╗██║    ██║██║   ██║   ██║   
  ╚██████╔╝╚██████╗███████╗██║  ██║██║ ╚████║    ██║╚██████╔╝   ██║   
   ╚═════╝  ╚═════╝╚══════╝╚═╝  ╚═╝╚═╝  ╚═══╝    ╚═╝ ╚═════╝    ╚═╝  

                        o
                    o      ______/~/~/~/__           /((
                      o  // __            ====__    /_((
                     o  //  @))       ))))      ===/__((
                        ))           )))))))        __((
                        \\     \)     ))))    __===\ _((
                         \\_______________====      \_((
                                                     \((

    Low cost ocean profiling sensor module. 
    
    Data Structure:  millis, ph, pressure, temperature, depth, altitude
    
    Data is saved directly on SD Card.

    Andres Rico - MIT Media Lab - MAS.S62
    
 */


#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <RF24.h>
#include <avr/io.h>
//#include <TinyGPS.h>
#include "MS5837.h"

//Pressure and Depth Sensor
MS5837 sensor;

/*//Accelerometer Variables
int ADXL345 = 0x53; // The ADXL345 sensor I2C address
float X_out, Y_out, Z_out;  // Outputs*/

//Radio Variables
RF24 radio(1, 0); // CE, CSN 
const byte addresses [][6] = {"10911", "10917"}; // MSN, MSG 

//Declare LED Pins
int led1 = 4; //4, 5
int led2 = 5;

/*//GPS Variables
TinyGPS gps;
long latitude, longitude;
bool NewData = false;*/

String file_name;
//int randomPin = 9; //Pin used to create random number for file name. 
File myFile; //Declare file object for using SD library fucntions.


//Light Sensor
int photoPin = 2; //Input pin from phototransistor.
float lightread;

//PH Sensor
int phPin = 3;
float phlevel;

//Radio connection 
bool connected_base = false;

void setup() {
  
  Serial.begin(115200);
  Serial.println("Hello Oceans!");
  
  //Create File name with random pin value. 
  file_name = "fish.txt";

  //Initialize SD Card
  pinMode(13, OUTPUT);
  if (!SD.begin(13)) {
    Serial.println("SD initialization has failed");
    return;
  }  
  Serial.println("SD Initialized!");

  /*//Initializa GPS
  Serial.println("Initializing GPS Communication...");
  Serial.println("GPS Initialized!");*/
  
  //LED pins as outputs.
  pinMode(led1, OUTPUT);
  pinMode(led2, OUTPUT);

  //Initialize Light Sensor
  pinMode(photoPin, INPUT);

  /*//Inititalize Accelerometer
  Wire.begin(); // Initiate the Wire library
  Wire.beginTransmission(ADXL345); // Start communicating with the device 
  Wire.write(0x2D); // Access/ talk to POWER_CTL Register - 0x2D
  Wire.write(8); // (8dec -> 0000 1000 binary) Bit D3 High for measuring enable 
  Wire.endTransmission();
  delay(10);
  Serial.println("Accelerometer initialized!");*/
  Wire.begin(); // Initiate the Wire library
  //Depth and Temperature Sensor
  while (!sensor.init()) {
    Serial.println("Init failed!");
    Serial.println("Are SDA/SCL connected correctly?");
    Serial.println("Blue Robotics Bar30: White=SDA, Green=SCL");
    return;
  }
  sensor.setModel(MS5837::MS5837_30BA);
  sensor.setFluidDensity(1029); // kg/m^3 (997 freshwater, 1029 for seawater)
  Serial.println("Depth and Temperature Sensor Initialized");

  //Initialize Radio Module
  radio.begin();
  radio.openWritingPipe(addresses[1]);
  radio.openReadingPipe(1, addresses[0]);      
  radio.setPALevel(RF24_PA_MIN);
  radio.startListening();
  Serial.println("Connected to NRF24");

  Serial.println("Lets go swimming!");
}

void loop() {
  
  sensor.read(); //Read depth sensor.
  blynk_blue();
  delay(60000); //Checks depth every minute in power saving mode. 
  while (sensor.depth() > .2) { //Check for depth so that it only logs when underwater. 
    getLight();
    getPH();
    getDepthTemp();
    writeSD();
    //blynk_green();
    delay(500);
  }

  /*getPH();
  getLight();
  getDepthTemp();
  writeSD();
  blynk_blue();*/
  
}

void blynk_green() {
  digitalWrite(led1, HIGH);
  delay(50);
  digitalWrite(led1, LOW);
  delay(50);
}

void blynk_blue() {
  digitalWrite(led2, HIGH);
  delay(50);
  digitalWrite(led2, LOW);
  delay(50);
  
}

void getLight() {
  float accumulated = 0;
  for (int i = 0 ; i < 35; i++) {
    accumulated = accumulated + analogRead(photoPin);
  }
  lightread = accumulated / 35; //Oversample light readings. 
  Serial.print(" light= ");
  Serial.print(lightread);
}

void getPH() {
  float accumulated = 0;
  for (int i = 0 ; i < 35; i++) {
    accumulated = accumulated + analogRead(phPin);
  }
  float phvoltage = (accumulated * 3.3) / 1024; //Oversample light readings. 
  phlevel = 7 - (2.5 - (phvoltage / 35)) * -5;
  Serial.print(" ph= ");
  Serial.print(phlevel);
}

void getDepthTemp() {
  
  sensor.read();

  Serial.print(" Pressure= "); 
  Serial.print(sensor.pressure()); //In mbar
  //Serial.print(" mbar");
  
  Serial.print(" Temperature= "); 
  Serial.print(sensor.temperature()); // In Celcius
  //Serial.println(" deg C");
  
  Serial.print(" Depth= "); 
  Serial.print(sensor.depth()); //In meters
  //Serial.println(" m");
  
  Serial.print(" Altitude= "); 
  Serial.println(sensor.altitude()); //In meters

}



/*void getAcceleration() {
  
  Wire.beginTransmission(ADXL345);
  Wire.write(0x32); // Start with register 0x32 (ACCEL_XOUT_H)
  Wire.endTransmission(false);
  Wire.requestFrom(ADXL345, 6, true); // Read 6 registers total, each axis value is stored in 2 registers
  X_out = ( Wire.read()| Wire.read() << 8); // X-axis value
  X_out = X_out/256; //For a range of +-2g, we need to divide the raw values by 256, according to the datasheet
  Y_out = ( Wire.read()| Wire.read() << 8); // Y-axis value
  Y_out = Y_out/256;
  Z_out = ( Wire.read()| Wire.read() << 8); // Z-axis value
  Z_out = Z_out/256;
  Serial.print(" Xa= ");
  Serial.print(X_out);
  Serial.print(" Ya= ");
  Serial.print(Y_out);
  Serial.print(" Za= ");
  Serial.print(Z_out);
  
}*/


/*bool check_GPS() {
  
  while (Serial.available()) {   //GPS Log
    int c = Serial.read();
    if (gps.encode(c)) {
      // process new gps info here
      NewData = true;
      //digitalWrite(mainLED, HIGH);
    }
    if (NewData) {
      NewData = false;
      long lat, lon;
      unsigned long fix_age;
      gps.get_position(&lat, &lon, &fix_age);
      latitude = lat;
      longitude = lon;
      return true;
    } else {
      return false;
    }
  }
  
}*/

void send_rf_cmd(String message) { //Function sends a command via radio. Takes string as argument and converts to char to send. 

  char mess[80] = "";
  strcpy(mess, message.c_str());
  Serial.println(mess);
  radio.stopListening();
  radio.write(&mess, sizeof(mess));
  
}

void writeSD() {
  
  myFile = SD.open(file_name, FILE_WRITE);
  
  if (myFile) {
    
    myFile.print(millis()); myFile.print(" , ");
    //myFile.print(latitude); myFile.print(" , ");
    //myFile.print(longitude); myFile.print(" , ");
    //myFile.print(X_out); myFile.print(" , ");
    //myFile.print(Y_out); myFile.print(" , ");
    //myFile.print(Z_out); myFile.print(" , ");
    myFile.print(phlevel); myFile.print(" , ");
    myFile.print(sensor.temperature()); myFile.print(" , ");
    myFile.print(sensor.pressure()); myFile.print(" , ");
    myFile.print(sensor.depth()); myFile.print(" , ");
    myFile.print(sensor.altitude()); myFile.print(" , ");
    myFile.println(lightread); 
    myFile.close();
    //blynk_green();
    
  }
  
}

void connectedbase() {

  radio.startListening();

  if (radio.available()) {                      //Looking for the data.
      
      char incoming[10] = "";                      //Saving the incoming data
      radio.read(&incoming, sizeof(incoming));    //Reading the data
      //Serial.println(incoming);
      if (incoming == "BC") {
        connected_base = true;
      } else {
        connected_base = false;
      }
  } else {
    connected_base = false;
  }
  
}
