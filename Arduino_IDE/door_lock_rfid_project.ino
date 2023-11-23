#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>
#include <Arduino_LSM6DS3.h>
#include <Firebase_Arduino_WiFiNINA.h>

#define SS_PIN 10
#define RST_PIN 9
#define FIREBASE_HOST "smartdoorlock-1accb-default-rtdb.firebaseio.com"
#define FIREBASE_AUTH "6kaRJfZlk5TokBkvstkap7ZFsWbI1xv2eh6u7xd2"
#define WIFI_SSID "Bokep"
#define WIFI_PASSWORD "wifiwifi"

MFRC522 mfrc522(SS_PIN, RST_PIN);
Servo myServo;
FirebaseData firebaseData;
String fireStatus = "";


void setup() 
{
  Serial.begin(9600);   
  SPI.begin();     
  mfrc522.PCD_Init();   // Initialize MFRC522 card reader
  myServo.attach(4);    //servomotor is attached to pin 4
  myServo.write(0);     //set servomotor start position to 0
  Serial.print("Connecting to WiFi...");
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    Serial.print(".");
    delay(300);
  }
  Serial.println();
  Serial.println("Connected to IP: ");
  Serial.print(WiFi.localIP());
  Serial.println();
  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH, WIFI_SSID, WIFI_PASSWORD);   //initialize connection to Firebase Real time database
  Firebase.reconnectWiFi(true);
  Serial.println("Put your card to the reader...");
}

void loop() 
{
  if (Firebase.getString(firebaseData, "/unlock"))  //check database if unlocked by smartphone app
  {
    fireStatus = firebaseData.stringData();         //store the unlock value in fireStatus ("True" or "False")
    if (fireStatus == "True")                       //if fireStatus' unlock value if "True", it means smartphone app sends the command to unlock
    {
      Serial.println("Unlocking from Mobile Application");
      myServo.write(90);                            //unlock the door lock by moving the servo motor to 90 degrees position
      delay(10000);                                  //add 10 seconds delay before auto lock initiates
      myServo.write(0);                             //lock the door lock by moving the servo motor to original position
      Firebase.setString(firebaseData, "/unlock", "False");   //send value of "False" to database to indicate that door lock has been reset to lock position
    }
  }
  else
  {
    // Only print the error if there's a failure in getting the string from database
    Serial.println("Error: " + firebaseData.errorReason());
  }

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
  //Print UID of presented RFID card on serial monitor
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
  if (content.substring(1) == "62 CF 76 51")   //UID of registered RFID card
  {
    Serial.println("Authorized access.");
    Serial.println();
    delay(500);
    myServo.write(90);                        //unlock the door lock by moving the servo motor to 90 degrees position
    delay(10000);                             //add 10 seconds delay before auto lock initiates
    myServo.write(0);                         //lock the door lock by moving the servo motor to original position
  }

else   {
    Serial.println(" Access denied");         //if presented card does not match the registered UID, print Access denied.
  }
}