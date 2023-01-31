#include "SD.h"
#include "FS.h"
#include <OneWire.h>
#include <DallasTemperature.h>

#include "ph_grav.h"       
#include "orp_grav.h"
#include "do_grav.h"


#define AN1 32
#define AN2 35
#define AN3 34

#define  SD_CS      5
#define  SD_MOSI    23
#define  SD_MISO    19
#define  SD_CLK     18

const int oneWireBus = 22;


#define LOG_RATE                (5 * 60 * 1000)


Gravity_pH pH = Gravity_pH(AN2);
Gravity_ORP ORP = Gravity_ORP(AN3);
Gravity_DO DO = Gravity_DO(AN1);

OneWire oneWire(oneWireBus);
DallasTemperature sensors(&oneWire);


uint8_t user_bytes_received = 0;
const uint8_t bufferlen = 32;
char user_data[bufferlen];

unsigned long LAST_LOG = 0; 


void parse_cmd(char* string) {
  strupr(string);
  String cmd = String(string);
  if(cmd.startsWith("ORP,CAL")){
    int index = cmd.indexOf(',');
    if(index != -1){
      String param = cmd.substring(index+1, cmd.length());
      int index = param.indexOf(',');
      if(index != -1){
        String param2 = param.substring(index+1, param.length());
        if(param2.equals("CLEAR")){
          ORP.cal_clear();
          Serial.println("CALIBRATION CLEARED");
        }
        else{
          int cal_param = param2.toInt();
          ORP.cal(cal_param);
          Serial.println("ORP CALIBRATED");
        }
      }
    }
  }
  else if (strcmp(string, "PH,CAL,7") == 0) {       
    pH.cal_mid();                                
    Serial.println("MID CALIBRATED");
  }
  else if (strcmp(string, "PH,CAL,4") == 0) {            
    pH.cal_low();                                
    Serial.println("LOW CALIBRATED");
  }
  else if (strcmp(string, "PH,CAL,10") == 0) {      
    pH.cal_high();                               
    Serial.println("HIGH CALIBRATED");
  }
  else if (strcmp(string, "PH,CAL,CLEAR") == 0) { 
    pH.cal_clear();                              
    Serial.println("CALIBRATION CLEARED");
  }
  else if(cmd.startsWith("DO,CAL,CLEAR")){
    DO.cal_clear();
    Serial.println("CALIBRATION CLEARED");
  }
  else if(cmd.startsWith("DO,CAL")){
    DO.cal();
    Serial.println("DO CALIBRATED");
  
  }
}

void setup() {
  Serial.begin(115200);
  delay(200);

  pinMode(SD_CS, OUTPUT);
  digitalWrite(SD_CS, HIGH);

  setupSD();

  
  Serial.println(F(" - For pH, use commands \"PH,CAL,7\", \"PH,CAL,4\", and \"PH,CAL,10\" to calibrate the circuit to those respective values"));
  Serial.println(F("   Use command \"PH,CAL,CLEAR\" to clear the calibration"));
  Serial.println();
  Serial.println(F(" - For ORP, use command \"ORP,CAL,xxx\" to calibrate the circuit to the value xxx \n   \"ORP,CAL,CLEAR\" clears the calibration"));
  Serial.println();
  Serial.println(F(" - For DO, use command \"CAL\" to calibrate the circuit to 100% saturation in air\n   \"DO,CAL,CLEAR\" clears the calibration"));
  Serial.println();
  
  if (pH.begin()) {                                     
    Serial.println("Loaded pH EEPROM");
  }
  if(ORP.begin()){
    Serial.println("Loaded ORP EEPROM");
  }
  if(DO.begin()){
    Serial.println("Loaded DO EEPROM");
  }
  Serial.println();
  LAST_LOG = millis() + (LOG_RATE);
}

void loop() {
  if (Serial.available() > 0) {
    user_bytes_received = Serial.readBytesUntil(13, user_data, sizeof(user_data));
  }

  if (user_bytes_received) {
    parse_cmd(user_data);
    user_bytes_received = 0;
    memset(user_data, 0, sizeof(user_data));
  }

  sensors.requestTemperatures(); 
  float temperatureC = sensors.getTempCByIndex(0);

  Serial.print("Temp ºC: ");
  Serial.print(temperatureC);
  Serial.print(", pH: ");
  Serial.print(pH.read_ph());
  Serial.print(",");
  Serial.print(", ORP: ");
  Serial.print((int)ORP.read_orp());
  Serial.print(",");
  Serial.print(", DO: ");
  Serial.print(DO.read_do_percentage());
  Serial.print("---- pH RAW: ");
  Serial.print(analogRead(AN2));
  Serial.print(",");
  Serial.print(", ORP RAW: ");
  Serial.print(analogRead(AN3));
  Serial.print(",");
  Serial.print(", DO RAW: ");
  Serial.println(analogRead(AN1));

  if (LAST_LOG <= millis()){

    logSDCard();

    LAST_LOG = millis() + (LOG_RATE);
  }

  
  delay(1000);
}
