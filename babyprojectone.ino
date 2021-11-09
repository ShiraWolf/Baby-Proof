// All further required documentation is in the instructable:
// https://www.instructables.com/Baby-Proof-IOT/

#include <ESP8266_Lib.h>
#include <BlynkSimpleShieldEsp8266.h>
#include <Adafruit_CircuitPlayground.h>
#include "pitches.h"
#include <math.h>
#include <Wire.h>
#include <SPI.h>
#include "soundsAndmusic.h"

#define EspSerial Serial1
#define ESP8266_BAUD 9600
#define SOUND_THRESHE 85 // the sound trheshold 
ESP8266 wifi(&EspSerial);

char ssid[] = "";                              //enter your WIFI name (SSID) here 
char pass[] = "" ;                             //enter your WIFI password here 
char auth[] = "";                              // BLYNK Auth' code -> change it

uint8_t appON = 0;
uint8_t GameOn = 0;
int latch = 1;

// he sound that calls out  for hlep 

float homeLat = 32.08595;
float homeLon = 34.79325;
float thresholdDistance = 1; // meter
float lightvalue; // delete later value to cheack light sensor 
float TempC; // tempeture value 
float SoundValue; // sound srnsor vlaue 
float soundPressure; // sound pressure value

uint8_t pads[] = {A1, A2, A3, A4, A5};
uint8_t numberOfPads = sizeof(pads)/sizeof(uint8_t);

 // blynk stepup  
BLYNK_WRITE(V0){
  appON = param.asInt(); // assigning value from pin V0 to a variable
}

BLYNK_WRITE(V1){
  GameOn = param.asInt(); // assigning value from pin V0 to a variable
}


BLYNK_WRITE(V5) {
  float distance;
  GpsParam gps(param);
  //Serial.println(gps.getLat(), 7);
  //Serial.println(gps.getLon(), 7);
  distance = distanceCalc(homeLat, homeLon, gps.getLat(), gps.getLon()); 
  //Serial.println(distance,2);
  if(distance < thresholdDistance)
  {
  //  [do something]  
  }  
  else
  {
    CircuitPlayground.setPixelColor(7, 150,150 ,0);
  }   
} 


void setup() {
  // put your setup code here, to run once:
  CircuitPlayground.begin();
  SerialUSB.begin(9600);
  Serial.begin(115200);
  EspSerial.begin(ESP8266_BAUD);
  delay(100);
  Blynk.begin(auth, wifi, ssid, pass);
}


void loop() {
  // put your main code here, to run repeatedly:
  
  
  if(GameOn==1){
  palyBAByEnglishGame();
  delay(100);
  }
  Blynk.run(); // run BLYNK
  
  if (appON == 1)
  {
         setpixels();
         lightSensorvalue();
         temperatureValue();
         CallForHelp();
         soundPressureValue();

    if(soundPressure > SOUND_THRESHE && latch == 1){ //babay is crying
                  //CircuitPlayground.clearPixels();
                   Blynk.notify("crying baby"); // notification to phone
                  playBabyMusic();
                  latch = 0;
    
  }
  if (latch == 0 && SoundValue < SOUND_THRESHE){
    latch = 1;
    }
    delay(500);
}else{
   CircuitPlayground.clearPixels();
  }

  
}


// sets all the pixels to a value
void setpixels(){
 for (int i = 0; i < 9; i++){
        CircuitPlayground.setPixelColor(i, 150,75 ,0);
      }
}



//print and return the light Sensor value
float lightSensorvalue(){
  
  lightvalue = CircuitPlayground.lightSensor();
 Serial.print("Light Sensor:");
 Serial.println(lightvalue);
  return lightvalue;
}

//print and return the temperature Value
float temperatureValue(){
   TempC = CircuitPlayground.temperature();
  Serial.print("TempC: ");
  Serial.println(TempC);
  return TempC;
  }

//print and return the Sound Sencor Value
 float SoundSencorValue(){
    SoundValue = CircuitPlayground.soundSensor();
    Serial.print("SoundValue: ");
  Serial.println(SoundValue);
  return SoundValue;
    }


// print and return the sound Pressure Value
 float soundPressureValue (){
      soundPressure = CircuitPlayground.mic.soundPressureLevel(50);
     Serial.print("soundPressure: ");
     Serial.println(soundPressure);
  return  soundPressure;
      }


// starts sound of voice calling for help 
void CallForHelp()
{
   if(lightvalue < 5 && TempC > 40){
      delay(20);
      CircuitPlayground.speaker.say(spHELP);
      delay(20);
      CircuitPlayground.setBrightness(255);

      delay(20);
      CircuitPlayground.speaker.say(spRETURN);
  }
}


//starts playing baby music  
 void playBabyMusic()
 {
  for (int thisNote = 0; thisNote < 26; thisNote++) {
            int noteDuration = 1000/BirthdayNotes[thisNote];
            // to calculating note duration  (1 second divided by the note type)
            CircuitPlayground.playTone(BirthdayMelody[thisNote],noteDuration);
            // to distinguish the notes, set a minimum time between them.
           int pauseBetweenNotes = noteDuration * 1.2;
           delay(pauseBetweenNotes);
           noTone(8);
  }

 }

// calculates the distance between the phone and the car 
float distanceCalc(float lat1, float long1, float lat2, float long2){
  float distLat, distLong, distance;
  distLat = (lat1 - lat2) * 111194.9;
  distLong = 111194.9 * (long1 - long2) * cos(radians((lat1 + lat2) / 2));
  distance = sqrt(pow(distLat, 2) + pow(distLong, 2));
  return distance; // in meter
}





 /// make english game sound 
void takeAction(uint8_t pad) {
  Serial.print("PAD "); Serial.print(pad); Serial.print(" playing note: ");
  switch (pad) {
    case A1:
      CircuitPlayground.speaker.say(spBLUE);
      break;
    case A2:
      CircuitPlayground.speaker.say(spTHREE);
      break;
    case A3:
      CircuitPlayground.speaker.say(spYELLOW);
      break;
    case A4:
      CircuitPlayground.speaker.say(spGREEN);
      break;
    case A5:
      CircuitPlayground.speaker.say(spTWO);
      break;
    default:
      Serial.println("THIS SHOULD NEVER HAPPEN.");
  }
}


boolean capButton(uint8_t pad) {
  // Check if capacitive touch exceeds threshold.
  if (CircuitPlayground.readCap(pad) > CAP_THRESHOLD) {
    Serial.println(CircuitPlayground.readCap(pad));
    return true;  
  } else {
    return false;
  }
}

void palyBAByEnglishGame() {
  // Loop over every pad.
  for (int i=0; i<numberOfPads; i++) {
    
    // Check if pad is touched.
    if (capButton(pads[i])) {
      
      // Do something.
      takeAction(pads[i]);
    }
  }
}
