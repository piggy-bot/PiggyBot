#include <Servo.h>
#include <ESP8266WiFiMulti.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClient.h>

//-------SETTINGS---------
#define COIN_AMOUNT 6     //number of coins
#define IRSENSOR A0
#define NET_INDECATOR 12   //Wemos D6
#define SERVO_PIN 2       //Wemos D4
#define GREEN_INDECATOR 16   //Wemos D0
#define RED_INDECATOR 14   //Wemos D5
#define HTTP_RESPONSE_OK 200
#define SYSTEM_TIME_OUT 30000
#define NOT_CONNECTED_TIMEOUT 120000
#define CHECK_IS_OPEN 10000

float coinValue[COIN_AMOUNT] = {0.10, 0.50, 1.0, 2.0, 5.0, 10.0};  
int coinMaxSignal[COIN_AMOUNT] = {600, 1024, 269, 370, 1023, 960};   //Max signal for each coin
int coinMinSignal[COIN_AMOUNT] = {371, 1024, 100, 270, 970, 700};   //min signal for each coin
int coinQuantity[COIN_AMOUNT];  //Number of coins for each coin type
int emptySignal = 0;               //Save empty signal value.  
int sensSignal = 0;
int maxSensSignal = 0;
bool coinInside = false;
bool isBankEmpty = true;
bool gameStart = false;
float goal = -1;
float totalSum = 0;             //Save total sum of money in our pig
bool isOpen = false;

unsigned long systemTimer;
unsigned long noConnectionTimer; 
bool connection = true;
unsigned long checkOpenTimer;

Servo myservo;
int servoPosition = 0;

ESP8266WiFiMulti WiFiMulti;

//-------SETTINGS---------

int tcrt;
int maxValue;

void setup() {
  // put your setup code here, to run once:
  checkOpenTimer=noConnectionTimer = systemTimer = millis();  //initial start time
  ServoClose();
  Serial.begin(9600);
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(IRSENSOR,INPUT);
  pinMode(NET_INDECATOR,OUTPUT);
  pinMode(GREEN_INDECATOR,OUTPUT);
  pinMode(RED_INDECATOR,OUTPUT);
  digitalWrite(GREEN_INDECATOR,HIGH);
  digitalWrite(GREEN_INDECATOR,HIGH);
  digitalWrite(RED_INDECATOR,HIGH);
  WiFiMulti.addAP("TechPublic", "");
  WiFiMulti.addAP("LIAV-PC 3303", "piggybot");
  ConnectToInternet();
  gameStart = InitGoalAndSum();
}

void loop() {
  // put your main code here, to run repeatedly:

  ShowBank();
  Serial.println("Sum: " + String(totalSum));
  Serial.println("Goal: " + String(goal));
  emptySignal = analogRead(IRSENSOR);
  Serial.println("Empty Signal: " + String(emptySignal));
  int coinSignal = ListenToCoin();
  int coinIndex = RecognizeCoin(coinSignal);
  if (coinIndex != -1 ) {
    float coinType = coinValue[coinIndex];
    Serial.println("Coin: " + String(coinType));
    bool sendResult = SendCoinToAzure(coinType);
    if (!sendResult) {
      coinQuantity[coinIndex]++;                                          //Save coin in case of bad NET issue
      isBankEmpty = false;
    } else {
      if (!gameStart){
        gameStart = InitGoalAndSum();
      }
      else
        totalSum+=coinType; 
    }
  } else {
    Serial.println("Bad Coin !!!");
  }
}

int ListenToCoin() {
  maxSensSignal = emptySignal;
  while(1){
    yield();
    digitalWrite(GREEN_INDECATOR,HIGH);
    digitalWrite(RED_INDECATOR,LOW);
    if (WiFi.status() != WL_CONNECTED) {
      digitalWrite(NET_INDECATOR,LOW);
    }
    if (!coinInside && !connection && millis() - noConnectionTimer >= NOT_CONNECTED_TIMEOUT){
      ConnectToInternet(); 
    }
    sensSignal = analogRead(IRSENSOR);
    if (sensSignal > maxSensSignal) 
      maxSensSignal = sensSignal;
    if (sensSignal - emptySignal > 50) 
      coinInside = true;
    if (coinInside && (abs(sensSignal - emptySignal) <= 2))
    {
      coinInside = false;
      systemTimer = millis();                                     //User is using system. Reset timer.
      break;
    }
    if (!coinInside && connection) {
      if (!isBankEmpty && millis() - systemTimer > SYSTEM_TIME_OUT) {        //User doesn't use the system more than 30 sec. We can go and empty the bank.
         Serial.println("GO TO BANK");
         ClearBank();
         systemTimer = millis();
      }
      if (gameStart && totalSum>=goal && millis() - checkOpenTimer >= CHECK_IS_OPEN) {  
         Serial.println("TRY TO OPEN PIG");
         isOpen = OpenPig();
         checkOpenTimer = millis();
      }
    }
  }
  Serial.println("MaxSensValue: " + String(maxSensSignal));
  return maxSensSignal;
}

int RecognizeCoin(int coinSignal){
 // Serial.println(coinSignal);
  for (byte i=0; i< COIN_AMOUNT; i++){
    if (coinSignal >= coinMinSignal[i] && coinSignal <= coinMaxSignal[i] ){
      return i;
    }     
  }
  return -1;
}

String SendHttpRequest(String URL){
  digitalWrite(GREEN_INDECATOR,LOW);
  digitalWrite(RED_INDECATOR,HIGH);
  for (int i=0; i<2; i++) {
    HTTPClient http;
    http.begin(URL);
  
    int httpCode = http.GET();
   
    if (httpCode > 0) {
      if (httpCode == HTTP_RESPONSE_OK) {
        String payload = http.getString();
        Serial.println(payload);
        http.end();
//        digitalWrite(GREEN_INDECATOR,HIGH);
//        digitalWrite(RED_INDECATOR,LOW);
        return payload;
      }
      else {
        String badHttpCode = "bad http code: " + String(httpCode);
        Serial.println(badHttpCode);
      }
    } 
    else {
      Serial.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
      if (connection || millis() - noConnectionTimer >= NOT_CONNECTED_TIMEOUT){
        ConnectToInternet(); 
      }
    }
    http.end();
  }
//  digitalWrite(GREEN_INDECATOR,HIGH);
//  digitalWrite(RED_INDECATOR,LOW);
  return "HTTP_NOT_SEND";
}

bool SendCoinToAzure(float coinType){
  String URL = "http://piggylistener.azurewebsites.net/api/Deposit?code=FxybtubQhduj/mxdIuCbTSvvkupJzkPP0uam8sKSq9IVQhbbzmpCxQ==&name=PiggyTeam&&pigName=Piggy1&money="+String(coinType);
  //String URL = "http://piggylistener.azurewebsites.net/api/ListenToPig?code=L5K5e/VALOilWmk0sxdbfsLMZ3tw2Ny7eugrZPQi57C8u5ys2KgPHg==&name=Michael&coin="+String(coinType);
  String response = SendHttpRequest(URL);
  if (response != "HTTP_NOT_SEND")
    return true;
  return false;
}

void ConnectToInternet(){
  unsigned long connectionStartTime = millis();
  Serial.print("Try to connect");
  digitalWrite(GREEN_INDECATOR,LOW);
  digitalWrite(RED_INDECATOR,HIGH);
  while ((WiFiMulti.run() != WL_CONNECTED) && millis() - connectionStartTime < 30000) { 
    yield();
    delay(200);
    digitalWrite(NET_INDECATOR,HIGH);
    delay(300);
    Serial.print(".");
    digitalWrite(NET_INDECATOR,LOW);
  }
  if (millis() - connectionStartTime >= 30000){
    Serial.println("No Connection :(");
    connection = false;
    noConnectionTimer = millis();
//    digitalWrite(GREEN_INDECATOR,HIGH);
//    digitalWrite(RED_INDECATOR,LOW);
    return;
  }
  Serial.println("...Connected :)");
  //Serial.println(WiFi.localIP()); 
  connection = true;
  digitalWrite(NET_INDECATOR,HIGH);
//  digitalWrite(GREEN_INDECATOR,HIGH);
//  digitalWrite(RED_INDECATOR,LOW);
}

void ShowBank(){
  String bank = "[" + String(coinValue[0]) + "=" + coinQuantity[0] + "] " +
                "[" + String(coinValue[1]) + "=" + coinQuantity[1] + "] " +
                "[" + String(coinValue[2]) + "=" + coinQuantity[2] + "] " +
                "[" + String(coinValue[3]) + "=" + coinQuantity[3] + "] " +
                "[" + String(coinValue[4]) + "=" + coinQuantity[4] + "] " +
                "[" + String(coinValue[5]) + "=" + coinQuantity[5] + "]";
   Serial.println(bank);
}

void ClearBank(){
  digitalWrite(GREEN_INDECATOR,LOW);
  digitalWrite(RED_INDECATOR,HIGH);
  if (!gameStart){
    gameStart = InitGoalAndSum();
  }
  for (byte i=0; i< COIN_AMOUNT; i++){
    int sendAttempt = 0;
    while (coinQuantity[i] > 0){
      digitalWrite(NET_INDECATOR,HIGH);
      bool sendResult = SendCoinToAzure(coinValue[i]);
      digitalWrite(NET_INDECATOR,LOW);
      delay(100);
      if (sendResult) {
        coinQuantity[i]--;
        totalSum+=coinValue[i];
      } else {
        sendAttempt++;
      }
      if (sendAttempt == 3) {
        Serial.println("Can't send coin 3 times, stop clear bank");
//        digitalWrite(GREEN_INDECATOR,HIGH);
//        digitalWrite(RED_INDECATOR,LOW);
        return;
      }
    }     
  }
//  digitalWrite(GREEN_INDECATOR,HIGH);
//  digitalWrite(RED_INDECATOR,LOW);
  digitalWrite(NET_INDECATOR,HIGH);
  isBankEmpty = true;
}

bool InitGoalAndSum(){
  goal = GetGoal();
  totalSum = GetCurrentSum();
  
  if (totalSum > 0 && goal >= 0){
    return true;
  }
  return false;   
}

float GetGoal(){
  String URL = "http://piggylistener.azurewebsites.net/api/getGoal?code=JaeJjoWnQHo3NDxXaYk/JRBCnYWpaTmxjNBn5r0Wsk/griAzthN/0Q==&pigName=Piggy1";
  String response = SendHttpRequest(URL);
  if (response != "HTTP_NOT_SEND")
    return response.toFloat();
  return -1;
}

float GetCurrentSum(){
  String URL = "http://piggylistener.azurewebsites.net/api/GetMoney?code=zOb1CdXaa6i1qhO2agOPVrxnkaMg0nQODlVPN1KyQ0FXPvpHNbH1Xw==&pigName=Piggy1";
  String response = SendHttpRequest(URL);
  if (!response || response == "" || response == "HTTP_NOT_SEND" || response == "No results were found. SOME ANOTHER MSG")
    return 0;
  return response.toFloat();
}

bool OpenPig(){
  String URL = "http://piggylistener.azurewebsites.net/api/TryOpen?code=Vv1MYNdWxjO9XX4FtZdPLdmuhvSv7aZVZzxHIJRocnc8Pbr9tVaMWQ==&pigName=Piggy1";
  String response = SendHttpRequest(URL);
  if (response == "False" || response == "HTTP_NOT_SEND" || response == "No results were found. SOME ANOTHER MSG")
    return false;

  Serial.print("OPEEEEEEN :)");
  digitalWrite(GREEN_INDECATOR,LOW);
  digitalWrite(RED_INDECATOR,HIGH);

  ServoOpen();
  delay(60000);
  ServoClose();
  gameStart = false;
  totalSum = 0;

//  digitalWrite(GREEN_INDECATOR,HIGH);
//  digitalWrite(RED_INDECATOR,LOW);
  return true;
}

void ServoOpen(){
  myservo.attach(SERVO_PIN); 
  for (servoPosition = 0; servoPosition < 80; servoPosition += 5) // goes from 0 degrees to 180 degrees
  { // in steps of 1 degree
    myservo.write(servoPosition); // tell servo to go to position in variable 'pos'
    delay(15); // waits 15ms for the servo to reach the position
  }
  int j=0;
  while (j<5) {
    servoPosition -= 50;
    myservo.write(servoPosition); // tell servo to go to position in variable 'pos'
    delay(200); // waits 15ms for the servo to reach the position
    servoPosition += 50;
    myservo.write(servoPosition); // tell servo to go to position in variable 'pos'
    delay(200); // waits 15ms for the servo to reach the position
    j++;
  }
  myservo.detach();
}

void ServoClose(){
  myservo.attach(SERVO_PIN);
  for (servoPosition = 80; servoPosition >= 1; servoPosition -= 1) // goes from 180 degrees to 0 degrees
  {
    myservo.write(servoPosition); // tell servo to go to position in variable 'pos'
    delay(15); // waits 15ms for the servo to reach the position
  }
  myservo.detach(); 
}
