//#include <AltSoftSerial.h>
#include <FastLED.h>  //http://fastled.io/docs/3.1/class_c_pixel_view.html
//https://github.com/FastLED/FastLED/wiki/Controlling-leds
#pragma GCC optimize ("-O3") //fixing a bug in fastled


#define LED_PIN     19 //led control pin
#define NUM_LEDS    16   
#define ledPin LED_BUILTIN // 7

//led array
CRGB leds[NUM_LEDS];

//sensors to led mapping
const int serial2Sensor[16] = {2,3,4,5,6,7,8,9,10,11,12,14,15,16,17,18}; //sensors pins
const int sensor2Led[16] = {7,6,0,10,14,13,12,11,9,15,8,5,2,3,1,4}; //led position by sensor pin order

//led program
void (*ledProg)();
bool newProg; // true when new prog is selected

int state = 99; // for debug
String BTRead = ""; //input from BT

void setup() {
  //randomSeed(analogRead(5)); //read from led pin as analog to reset random sequence
// Led pins init
  pinMode(ledPin, OUTPUT);
  for (int i=2; i<=12; i++) 
    pinMode(i, INPUT);
  for (int i=14; i<=18; i++) 
    pinMode(i, INPUT);
    
  FastLED.addLeds<WS2812, LED_PIN, GRB>(leds, NUM_LEDS);

  //BT Start
  Serial.begin(9600);

  fill_solid(leds,16,CRGB::Blue);
  FastLED.delay(30);
  delay(2000);
  FastLED.clear();
  FastLED.delay(30);
  
  
  //altSerial.begin(9600); // Default communication rate of the Bluetooth module
// setLedOn();

}

void loop() {

  newProg = false;
  if(Serial.available() > 0){ // Checks whether data is comming from the serial port BT
    BTRead = Serial.readString(); // Reads the data from BT
    state = BTRead.substring(0,2).toInt();  //read the program
    //assign program ledProg contains the function
    newProg = true;
    if (state == 45) ledProg = clearPrg;
    else if (state == 41) ledProg = ravePrg;
    else if (state == 42) ledProg = partyPrg;
    else if (state == 44) ledProg = NULL;
    else if (state == 43) ledProg = snakePrg;
    else if (state == 46) ledProg = solidPrg;
    else if (state == 47) ledProg = wavePrg;
    else if (state == 48) ledProg = rainPrg;
    else if (state == 49) ledProg = checkPrg;
    state = 50;
  }
  if (state == 50) {
      Serial.println(BTRead);
      state = 99;
  }

//run led prog and refresh leds
  if (ledProg) ledProg();
  FastLED.delay(30);
  
}

void partyPrg()
{

  static uint8_t hue = 10;
  fill_rainbow(leds,16,hue++,50);
}


void rainPrg()
{
  
  static uint8_t hue = 0;
  fill_rainbow(leds,16,hue++,10);
}

void clearPrg()
{

   //fill_solid(leds,16,CRGB::Black);
  FastLED.clear();
}


void solidPrg()
{

  static CRGB backCol,spCol;
  char * pEnd;

  if (newProg) {  
 //   int fstComma = BTRead.indexOf(",");
//   int sndComma = BTRead.indexOf(",",fstComma + 1);
//   backCol = CRGB(BTRead.substring(3,fstComma).toInt(),BTRead.substring(fstComma + 1,sndComma).toInt(),BTRead.substring(sndComma + 1).toInt());
    backCol = strtol(BTRead.substring(3).c_str(), &pEnd, 10); //convert the color to long
    spCol = strtol(pEnd+1, NULL, 10); //convert the color to long

    //fill_solid(leds,16,backCol);
  }
  
  for (int i=0; i<16; i++)  {
    if (!digitalRead(serial2Sensor[i])) {
      if (leds[sensor2Led[i]] == backCol) leds[sensor2Led[i]]=(spCol) ? spCol : CRGB(random(255),random(255),random(255)); 
    }
    else leds[sensor2Led[i]]= backCol;
  }
  
} 

void checkPrg()
{

  static CRGB backCol,spCol;
  char * pEnd;
  
  if (newProg) {  
    backCol = strtol(BTRead.substring(3).c_str(), &pEnd, 10); //convert the color to long
    spCol = strtol(pEnd+1, NULL, 10); //convert the color to long
  }

  if (!spCol) spCol = CRGB(random(255),random(255),random(255));
  
  for (int i=0; i<8; i++)  {
      leds[2*i]= backCol; 
      leds[2*i+1]= spCol; 
  }
  
} 

void wavePrg()
{
  //leds around each led , 99 - no led
  static const int nextLed[16][4] = { 7,99,99,1, //up,down,left,right
                                      6,99,0,2,
                                      5,99,1,3,
                                      4,99,2,99,
                                      11,3,5,99,
                                      10,2,6,4,
                                      9,1,7,5,
                                      8,0,99,6,
                                      15,7,99,9,
                                      14,6,8,10,
                                      13,5,9,11,
                                      12,4,10,99,
                                      99,11,13,99,
                                      99,10,14,12,
                                      99,9,15,13,
                                      99,8,99,14};


    int waveLed[16][4];  
  
    FastLED.clear();
    for (int i=0; i<16; i++)  {
      if (!digitalRead(serial2Sensor[i])) {
        leds[sensor2Led[i]]=CRGB::Red;
        for (int k=0; k<4; k++) waveLed[sensor2Led[i]][k] = sensor2Led[i];
       }
      else {
        leds[sensor2Led[i]]=CRGB::Black;
        for (int k=0; k<4; k++) waveLed[sensor2Led[i]][k] = 99;
      }
     }
    FastLED.delay(20);
    delay(200);       
  
    for (int j=0; j<3; j++) {
      for (int i=0; i<16; i++)  { 
        for (int k=0; k<4; k++) {
          if (waveLed[i][k]<16) waveLed[i][k] = nextLed[waveLed[i][k]][k];
          if (waveLed[i][k]<16) {
            if (!leds[waveLed[i][k]]) leds[waveLed[i][k]] = CRGB::Red ;
            else leds[waveLed[i][k]] = CRGB::Orange;
          }
        }
      }
      FastLED.delay(20);
      delay(200); 
    }
   
  /*
    FastLED.clear();
    for (int i=0; i<16; i++)  {
      if (!digitalRead(serial2Sensor[i])) {
        leds[sensor2Led[i]]=CRGB::Red;
        FastLED.delay(20);
        delay(200);       
        int waveLed[4] = {sensor2Led[i],sensor2Led[i],sensor2Led[i],sensor2Led[i]};
        for (int j=0; j<3; j++) {
          for (int k=0; k<4; k++) {
            if (waveLed[k]<16) waveLed[k] = nextLed[waveLed[k]][k];
            if (waveLed[k]<16) leds[waveLed[k]]=CRGB::Red;
          }
          FastLED.delay(20);
          delay(200);       
        }
      }
      
    }*/
}




void snakePrg()
{
  

  static CRGB backCol,snekCol,tempCol;
  static int objPos[NUM_LEDS];
  char * pEnd;

    if(newProg){
        for (int i = 0 ; i<NUM_LEDS; i++) objPos[i]=0;
        backCol = strtol(BTRead.substring(3).c_str(), &pEnd, 10); //convert the color to long
        snekCol = strtol(pEnd+1, NULL, 10); //convert the color to long
        tempCol = snekCol;
        if (!snekCol) snekCol = CRGB(random(255),random(255),random(255)); 
        fill_solid(leds,NUM_LEDS,backCol);
        FastLED.delay(20);
    }

    for (int i=0; i<NUM_LEDS; i++)  {
       if (!digitalRead(serial2Sensor[i])) {
            if (!objPos[i]){
              objPos[i] = 1;
              //if (leds[sensor2Led[i]] == backCol) leds[sensor2Led[i]]=snekCol ; 
              //FastLED.delay(20);
              //delay(200);
              leds[sensor2Led[i]]=snekCol; 
              for(int j=1; j<NUM_LEDS; j++){
                if (sensor2Led[i]+j<NUM_LEDS) leds[sensor2Led[i]+j]=snekCol;
                if (sensor2Led[i]-j>-1) leds[sensor2Led[i]-j]=snekCol;
                FastLED.delay(20);
                delay(200); 
              }
              
              if (!tempCol) snekCol = CRGB(random(255),random(255),random(255));
              else {
                tempCol = backCol;
                backCol = snekCol;
                snekCol = tempCol;
              }
            }
       }
       else objPos[i] = 0;
       
    }
} 
