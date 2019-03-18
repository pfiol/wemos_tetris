/**
 * NOTE - MAKE SURE RESET METHOD in Arduino IDE IS "NODEMCU"!!!!!!
 * Also, CPU speed can be 160mhz, but I have found on the modules
 * I have that the flash memory must be kept at 40mhz, and also the
 * flash mode must be DIO. The hardware supports the fastest upload
 * speed of 921600bps. I use "Generic ESP8266 module" as the target
 * device. Hopefully this should work for you!
 * 
 * The device can be powered by USB or 2 x AA batteries (last about 10 hours of use). 
 * The slide switch at the top will switch between the power source.
 */

#include "defs.h"
extern "C"
{
  #include "user_interface.h"
}

#include "pitches.h"
#include <EEPROM.h>
#if NEW_NETWORK
#include "WebSocketsClient.h"
#include "ESP8266WiFi.h"

WebSocketClient webSocket(false);

#else
#include <WebSocketsClient.h>
#include <ESP8266WiFi.h>
WebSocketsClient webSocket;
#endif

#include <SPI.h>
#include <TFT_eSPI.h>
#include <Ticker.h>
#include "music.h"
#include "vars.h"
#include "gfx.h"
#include "headers.h"

TFT_eSPI tft = TFT_eSPI();
TFT_eSprite img = TFT_eSprite(&tft);
TFT_eSprite sprite1 = TFT_eSprite(&tft);
TFT_eSprite sprite2 = TFT_eSprite(&tft);
TFT_eSprite spriteTmp = TFT_eSprite(&tft);

#include "keyboard.h"
#include "menus.h"
#include "stars.h"
#if NEW_NETWORK
#include "networknew.h"
#else
#include "network.h"
#endif
#include "funcs.h"
#include "gameFuncs.h"
#include "screens.h"

void setup() {

  setSong(0);
  za = random(256);
  zb = random(256);
  zc = random(256);
  zx = random(256);

#if USE_SERIAL
  Serial.begin(115200);
  Serial.println("\n\nOK");
#else
  #if(DEVICE_VERSION==0 || DEVICE_VERSION==1)
  //pinMode(BUT_RLEFT,INPUT_PULLUP);
  //pinMode(BUT_RRIGHT,INPUT_PULLUP);
  //pinMode(BUT_RLEFT,INPUT);
  //pinMode(BUT_RRIGHT,INPUT);
  #elif(DEVICE_VERSION==2)
  pinMode(BUT_0,INPUT);
  pinMode(BUT_1,INPUT_PULLUP);
  pinMode(BUT_2,INPUT_PULLUP);
  pinMode(OUT_0,INPUT);
  pinMode(OUT_1,INPUT);
  pinMode(CS_SELECT,OUTPUT);
  #elif(DEVICE_VERSION==3)
  pinMode(BUT_READ,INPUT);
  pinMode(S0,OUTPUT);
  pinMode(S1,OUTPUT);
  pinMode(S2,OUTPUT);
  pinMode(CS_SELECT,OUTPUT);
  #endif
#endif

#if(DEVICE_VERSION==1 || DEVICE_VERSION==3)
  setSpiTo(SCREEN);
  delay(25);
  pinMode(TFT_RST_PIN,OUTPUT);
  digitalWrite(TFT_RST_PIN, LOW);
  delay(25);
  digitalWrite(TFT_RST_PIN, HIGH);
  delay(100);
#endif
  
  makeColors();
  tft.init();
  tft.fillScreen(0);

  system_timer_reinit();
  os_timer_setfn(&timer, timerCallback, NULL);
  os_timer_arm(&timer, 10, true); // 50FPS

  //networkStatus=NETWORK_CONNECTING; //NETWORK_OFFLINE;
  networkStatus=NETWORK_OFFLINE;
  loadConfig();
#if USE_SERIAL
  Serial.print("Name = ");
  Serial.println(myName);
  Serial.print("SSID = ");
  Serial.println(wifiName);
  Serial.print("Pass = ");
  Serial.println(wifiPassword);
#endif
  setState(STATE_INTRO);
//  is2Player=true;
//  setState(STATE_PLAYING);
}

void loop() {
  checkWebSocket();
  reading = analogRead(A0);
  
#if(!USE_SERIAL)
  checkButtons();
#endif
  if(state!=STATE_PLAYING){
    updateStars();
  }
  // Check connection status every second
  if(connectCheckTmr>=CONNECT_CHECK_INTERVAL){
    resetConnectCheckTmr();
    checkConnection();
  }
  if(state!=STATE_PLAYING){
    showNetworkStatus();
  }
  switch(state){
    
    case STATE_INTRO:
      showIntro();
      break;
      
    case STATE_TITLE_SCREEN:
      showTitleScreen();
      break;

    case STATE_PLAYING:
      showStatePlaying();
      break;

    case STATE_CHOOSE_OPPONENT:
      chooseOpponent();
      break;

    case STATE_HIGH_SCORES:
      showHighScores();
      break;
 
    case STATE_SETTINGS:
      showSettings();
      break;

    case STATE_HELP:
      showHelp();
      break;

    case STATE_SETUP_NAME:
      showSetupName();
      break;
    
    case STATE_SETUP_WIFI:
      showSetupWiFi();
      break;

    case STATE_SETUP_ABOUT:
      showSetupAbout();
      break;

    case STATE_CHALLENGED:
      showChallenged();
      break;
      
    default:
      break;
  }
  delay(1);
  yield();
}
