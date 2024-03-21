/*
  Author: Sakib Ahmed @ahmadsum1 
  Date: 11.03.2023

  Taken skeleton code from:
      Rui Santos
      Complete project details at https://RandomNerdTutorials.com/telegram-control-esp32-esp8266-nodemcu-outputs/
      
      Project created using Brian Lough's Universal Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot
      Example based on the Universal Arduino Telegram Bot Library: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot/blob/master/examples/ESP8266/FlashLED/FlashLED.ino
*/


#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>   // Universal Telegram Bot Library written by Brian Lough: https://github.com/witnessmenow/Universal-Arduino-Telegram-Bot

#include <ArduinoJson.h>

#include "arduino_secrets.h" 
///////please enter your sensitive data in the Secret tab/arduino_secrets.h
char ssid[] = SECRET_SSID;        // your network SSID (name)
char password[] = SECRET_PASS;    // your network password (use for WPA, or use as key for WEP)

// // Initialize Telegram BOT    MOVED TO "arduino_secrets.h"
// #define BOTtoken 
// #define CHAT_ID 

WiFiClientSecure client;
UniversalTelegramBot bot(BOTtoken, client);

// Checks for new messages every 1 second.
int botRequestDelay = 1000;
unsigned long lastTimeBotRan;

const int ledPin = LED_BUILTIN;
bool ledState = LOW;
#ifdef cheapYellowLCD
#define CYD_LED_BLUE 17
#define CYD_LED_RED 4
#define CYD_LED_GREEN 16
#endif

String dht_output = "";

// Handle what happens when you receive new messages
void handleNewMessages(int numNewMessages) {
  Serial.println("handleNewMessages");
  Serial.println(String(numNewMessages));

  for (int i=0; i<numNewMessages; i++) {
    // Chat id of the requester
    String chat_id = String(bot.messages[i].chat_id);
    if (chat_id != CHAT_ID){
      bot.sendMessage(chat_id, "Unauthorized user", "");
      continue;
    }
    
    // Print the received message
    String text = bot.messages[i].text;
    Serial.println(text);

    String from_name = bot.messages[i].from_name;

    if (text == "/start") {
      String welcome = "Welcome, " + from_name + ".\n";
      welcome += "Use the following commands to control your outputs.\n\n";
      welcome += "/ledon to turn GPIO ON \n";
      welcome += "/ledoff to turn GPIO OFF \n";
      welcome += "/dht to get temperature&humidity \n";
      welcome += "/status to request current GPIO state \n";
      welcome += "/WoL to Wake PC \n";
      bot.sendMessage(chat_id, welcome, "");
    }

    if (text == "/ledon") {
      bot.sendMessage(chat_id, "LED state set to ON", "");
      ledState = HIGH;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/ledoff") {
      bot.sendMessage(chat_id, "LED state set to OFF", "");
      ledState = LOW;
      digitalWrite(ledPin, ledState);
    }
    
    if (text == "/dht") {
      bot.sendMessage(chat_id, dht_output, "");
    }
    
    if (text == "/status") {
      if (digitalRead(ledPin)){
        bot.sendMessage(chat_id, "LED is ON", "");
      }
      else{
        bot.sendMessage(chat_id, "LED is OFF", "");
      }
      bot.sendMessage(chat_id, "\n"+dht_output, "");
    }
    if (text == "/WoL") {
      bot.sendMessage(chat_id, "Sending magic packet to wake PC", "");
      wakeMyPC();
    }
  }
}


void setup() {
  Serial.begin(115200);

  #ifdef ESP8266
    configTime(0, 0, "pool.ntp.org");      // get UTC time via NTP
    client.setTrustAnchors(&cert); // Add root certificate for api.telegram.org
  #endif

  pinMode(ledPin, OUTPUT);
  digitalWrite(ledPin, ledState);
  
  // Connect to Wi-Fi
  // setup_wifi_reconnect();
  // wm_setup();
  RollingClock_setup(); // handles display and wifi manager

  #ifdef ESP32
    client.setCACert(TELEGRAM_CERTIFICATE_ROOT); // Add root certificate for api.telegram.org
  #endif
 
  // setup WoL
  setup_WoL();

  // setup DHT
  setup_dht();

  //async udp
  setup_asudp();

}

void loop() {

  // loop_wifi_reconnect(); // handle wifi disconnected situation
  // wm_loop();
  RollingClock_loop(); // handles display and wifi manager loop

  if (millis() > lastTimeBotRan + botRequestDelay)  {
    int numNewMessages = bot.getUpdates(bot.last_message_received + 1);

    while(numNewMessages) {
      Serial.println("got response");
      handleNewMessages(numNewMessages);
      numNewMessages = bot.getUpdates(bot.last_message_received + 1);
    }
    lastTimeBotRan = millis();
  }
  // read temp humidty
  loop_dht();

}