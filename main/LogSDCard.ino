// Initialize SD card

void setupSD(){

  SD.begin(SD_CS);  
  if(!SD.begin(SD_CS)) {
    Serial.println("Card Mount Failed");
    return;
  }
  uint8_t cardType = SD.cardType();
  if(cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  Serial.println("Initializing SD card...");
  if (!SD.begin(SD_CS)) {
    Serial.println("ERROR - SD card initialization failed!");
    return;    // init failed
  }

  // If the data.txt file doesn't exist
  // Create a file on the SD card and write the data labels
  File file = SD.open("/data.txt");
  if(!file) {
    Serial.println("File doens't exist");
    Serial.println("Creating file...");
    String dataMessage;
    dataMessage = "Reading ID,";
  
    dataMessage += " Temperatura,"; 
    
    
    dataMessage += " pH,"; 
    dataMessage += " pH RAW,"; 
    
    dataMessage += " ORP,"; 
    dataMessage += " ORP RAW,"; 
    
    dataMessage += " DO,"; 
    dataMessage += " DO RAW,"; 

    dataMessage += "\r\n";
    Serial.println(dataMessage.c_str());  
    writeFile(SD, "/data.txt", dataMessage.c_str());
  }
  else {
    Serial.println("File already exists");  
  }
  file.close();
}



// Write the sensor readings on the SD card
void logSDCard(){
  String dataMessage;
 // dataMessage = String(readingID) + "," + String(dayStamp) + "," + String(timeStamp) + "," + 
  //              String(temperature) + "\r\n";

sensors.requestTemperatures(); 

  dataMessage = String(millis()) + ",";
  dataMessage += String(sensors.getTempCByIndex(0)) + ","; 
  
  dataMessage += String(pH.read_ph()) + ","; 
  dataMessage += String(analogRead(AN2)) + ","; 

  
  dataMessage += String((int)ORP.read_orp()) + ","; 
  dataMessage += String(analogRead(AN3)) + ","; 

  
  dataMessage += String(DO.read_do_percentage()) + ","; 
  dataMessage += String(analogRead(AN1)) + ","; 

  dataMessage += "\r\n";
           
  Serial.print("Save data: ");
  Serial.println(dataMessage);
  appendFile(SD, "/data.txt", dataMessage.c_str());
}

// Write to the SD card (DON'T MODIFY THIS FUNCTION)
void writeFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if(!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if(file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}

// Append data to the SD card (DON'T MODIFY THIS FUNCTION)
void appendFile(fs::FS &fs, const char * path, const char * message) {
  Serial.printf("Appending to file: %s\n", path);

  File file = fs.open(path, FILE_APPEND);
  if(!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if(file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
