#include <SPI.h>
#include <MFRC522.h>
#include <LiquidCrystal.h>
#define BUZZER 7 
#define SS_PIN 10
#define RST_PIN 9
MFRC522 mfrc522(SS_PIN, RST_PIN);   // Create MFRC522 instance.
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#include <Arduino_LSM6DS3.h>
#include <Firebase_Arduino_WiFiNINA.h>

#define FIREBASE_HOST " "
#define FIREBASE_AUTH " "
#define WIFI_SSID "admin"
#define WIFI_PASSWORD "admin@1234"
FirebaseData firebaseData;
String path = "/IMU_LSM6DS3";
String jsonStr;
 
void setup() 
{
  Serial.begin(9600);   // Initiate a serial communication
  SPI.begin();      // Initiate  SPI bus
  mfrc522.PCD_Init();   // Initiate MFRC522
  Serial.println("Approximate your card to the reader...");
  Serial.println();
  // set up the LCD's number of columns and rows:
 lcd.begin(16, 2);
  //lcd.begin();
  //lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0); // column, row
  lcd.print(" Scan Your RFID "); 
  lcd.setCursor(0,1); // column, row
  lcd.print("   Door Locked   ");
  pinMode(BUZZER, OUTPUT);
  noTone(BUZZER);

   Serial.print("Connecting to WiFi…");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }
  Serial.print(" IP: ");
  Serial.println(WiFi.localIP());
  Serial.println();

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);
  Firebase.reconnectWiFi(true);
  Serial.println("Kandarp");

}
void loop() 
{
   lcd.clear();
  lcd.setCursor(0,0); // column, row
  lcd.print(" Scan Your RFID "); 
  lcd.setCursor(0,1); // column, row
  lcd.print("   Door Locked   ");
  // Look for new cards
  if ( ! mfrc522.PICC_IsNewCardPresent()) 
  {
    return;
  }
  // Select one of the cards
  if ( ! mfrc522.PICC_ReadCardSerial()) 
  {
    return;
  }
  //Show UID on serial monitor
  Serial.print("UID tag :");
  String content= "";
  byte letter;
  for (byte i = 0; i < mfrc522.uid.size; i++) 
  {
     Serial.print(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " ");
     Serial.print(mfrc522.uid.uidByte[i], HEX);
     content.concat(String(mfrc522.uid.uidByte[i] < 0x10 ? " 0" : " "));
     content.concat(String(mfrc522.uid.uidByte[i], HEX));
  }
  Serial.println();
  Serial.print("Message : ");
  content.toUpperCase();
  
  
  
  if (content.substring(1) == "D9 B4 E5 B3" ||  content.substring(1) == "63 4E D3 16" ) //change here the UID of the card/cards that you want to give access
  { 
    if(content.substring(1)==  "D9 B4 E5 B3"){
            jsonStr = "{\"RFID sensor\":" + String(0) + "}";

    }

    if(content.substring(1)==  "63 4E D3 16"){
            jsonStr = "{\"RFID sensor\":" + String(1) + "}";

    }

    Serial.println("Access Granted   ");
    Serial.println();
    lcd.setCursor(0,0); 
    lcd.print(" Access Granted ");
    delay(500);
    lcd.setCursor(0,1); // column, row
    lcd.print(" Door Un-Locked ");
    tone(BUZZER, 2000);
    delay(100);
    noTone(BUZZER);
    delay(50);
    tone(BUZZER, 2000);
    delay(100);
    noTone(BUZZER);
      Serial.println(jsonStr);
     if (Firebase.pushJSON(firebaseData, path + "/rfid", jsonStr)) {
      Serial.println(firebaseData.dataPath() + " = " + firebaseData.pushName());
    }
    else {
     
      Serial.println("Error: " + firebaseData.errorReason());
    }
  }
 
 else   {
   
    jsonStr = "{\"RFID sensor\":" + String(3) + "}";

    

    lcd.setCursor(0,0); // column, row
    lcd.print("Invalid RFID Tag");
    lcd.setCursor(0,1);
    lcd.print(" Access Denied ");
    Serial.println(" Access denied");
    tone(BUZZER, 1500);
    delay(500);
    noTone(BUZZER);
    delay(100);
    tone(BUZZER, 1500);
    delay(500);
    noTone(BUZZER);
    delay(100);
    tone(BUZZER, 1500);
    delay(500);
    noTone(BUZZER);
    lcd.setCursor(0,1); // column, row
    lcd.print("   Door Locked   ");

    if (Firebase.pushJSON(firebaseData, path + "/RFID", jsonStr)) {
      Serial.println(firebaseData.dataPath() + " = " + firebaseData.pushName());
    }
    else {
      Serial.println("hello");
      Serial.println("Error: " + firebaseData.errorReason());
    }
  }
  
 
  delay(4000);
} 

  
