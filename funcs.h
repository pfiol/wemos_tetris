
#if(DEVICE_VERSION==3)
bool getMuxBut(uint8_t s2,uint8_t s1,uint8_t s0){
  digitalWrite(S0,s0);
  digitalWrite(S1,s1);
  digitalWrite(S2,s2);
  return (analogRead(BUT_READ)>500);
}
#endif

int reading; // Custom Wemos version
int bouncetime = 120; // Custom Wemos version
long moment = 0; // Custom Wemos version

void readButtonPins(){
  #if(DEVICE_VERSION==0 )   // Custom Wemos version
  
  if((millis() - moment) >= bouncetime){
    if(reading > 50){
      Serial.println(reading);
    }
    if(reading >= 490 && reading <= 560){
        butUp=1;
        butDown=0;
        butLeft=0;
        butRight=0;
    }else if(reading >= 700 && reading <= 740){
        butUp=0;
        butDown=0;
        butLeft=1;
        butRight=0;
    }else if(reading >= 975 && reading <= 1015){
        butUp=0;
        butDown=0;
        butLeft=0;
        butRight=1;
    }else if(reading >= 80 && reading <= 120){
        butUp=0;
        butDown=1;
        butLeft=0;
        butRight=0;
    }
    
    butB=digitalRead(BUT_RLEFT);
    butA=digitalRead(BUT_RRIGHT);
    moment = millis();
  }  
  #elif(DEVICE_VERSION==1) // 6 button version
    pinMode(BUT_LEFT,INPUT);
    butUp=!digitalRead(BUT_UP);
    butDown=!digitalRead(BUT_DOWN);
    butLeft=!digitalRead(BUT_LEFT);
    butRight=digitalRead(BUT_RIGHT);
    butB=!digitalRead(BUT_RLEFT);
    butA=!digitalRead(BUT_RRIGHT);

  #elif(DEVICE_VERSION==2)  // 6 button, using GPIO multiplexing
  
    pinMode(OUT_1,INPUT);
    pinMode(OUT_0,OUTPUT);
    
    digitalWrite(OUT_0,LOW);
    butLeft=!digitalRead(BUT_1);
    butDown=!digitalRead(BUT_2);
    digitalWrite(OUT_0,HIGH);
    butUp=(analogRead(BUT_0)>500);
    
    pinMode(OUT_0,INPUT);
    pinMode(OUT_1,OUTPUT);
    digitalWrite(OUT_1,LOW);
    butA=!digitalRead(BUT_1);
    butB=!digitalRead(BUT_2);
    digitalWrite(OUT_1,HIGH);
    butRight=(analogRead(BUT_0)>500);

  #elif(DEVICE_VERSION==3)  // 8 button using mux
    butUp=    getMuxBut(0,0,0);
    butDown=  getMuxBut(0,0,1);
    butLeft=  getMuxBut(0,1,0);
    butRight= getMuxBut(0,1,1);
    butA=     getMuxBut(1,0,0);
    butB=     getMuxBut(1,0,1);
    butX=     getMuxBut(1,1,0);
    butY=     getMuxBut(1,1,1);
  #endif
}
void checkButtons(){
  readButtonPins();
  /*
  if(butUp||butDown||butLeft||butRight||butA||butB){
    delay(2);
    readButtonPins();
  }*/
  if(waitUp){
    if(!butUp&&!butDown&&!butLeft&&!butRight&&!butA&&!butB){
      waitUp=false;
    }else{
      butUp=butDown=butLeft=butRight=butA=butB=false;
    }
  }
}

uint16_t RGBTo16(uint16_t r,uint16_t g,uint16_t b){
  uint16_t c=(b|(g<<5)|(r<<11));
  uint16_t col=(c>>8)|(c<<8);
  return col;
}

void drawGfx(const uint8_t *g, uint16_t w, uint16_t h, uint16_t col){
  uint16_t n,buffPos=0;
  uint16_t sIX=0;
  uint16_t totalBits=(w*h);
  uint8_t b=0;
  while(sIX<totalBits){
    n=(sIX/8);
    b=(sIX%8);
    if((g[n]&(1<<b))>0)
      img._img[buffPos]=col;
    ++buffPos;
    ++sIX;
  }
}

void drawTxt(char *str, uint16_t w, uint16_t offX, uint16_t offY, uint8_t colorIX, uint8_t vScale){
    
  int16_t cDefIX=0, buffPos;
  int16_t n,i,ix=0,strPos=0,mY=0,mX=offX,charIX,col=0;
  uint8_t c;

  offY=offY*w;
  
  while(str[strPos]!=0 && (mX<w)){
    cDefIX=( (uint16_t)(str[strPos++]-32) * 16 );
    ix=(mX+(mY*w));
    for(n=0;n<16;n++){
      buffPos=ix+(n*w*vScale)+offY;
      c=charDef[cDefIX++];

      if(c!=0){
        col=charRowCol16[(colorIX<<4)+((n+scrollColOff)%16)];
        for(i=0;i<8;i++){
          if(mX>0 && mX<w){
            if((c&(1<<i))>0){
              img._img[buffPos]=col;
              if(vScale==2)
                img._img[buffPos+w]=col;
            }
          }
          ++buffPos;
          ++mX;
        }
        mX-=8;
      }
    }
    mX+=charWidth;
  }
}

void drawTxt(char *str, uint16_t w, uint16_t offX, uint8_t colorIX){
  drawTxt(str, w, offX, 0, colorIX,1);
}

void showButtons(){  
  tft.setTextSize(4);
  tft.drawChar(butLeft  ?'Y':'N',20,100);
  tft.drawChar(butUp    ?'Y':'N',60,60);
  tft.drawChar(butDown  ?'Y':'N',60,140);
  tft.drawChar(butRight ?'Y':'N',100,100);
  tft.drawChar(butA     ?'Y':'N',140,120);
  tft.drawChar(butB     ?'Y':'N',180,80);
}

void makeColors(){
    uint16_t charRowCol[16][3]={
    // {R G B},
    {4,8,4},
    {31,0,0},
    {0,63,0},
    {0,0,31},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {0,0,0},
    {4,0,0},
    {8,0,0},
    {12,0,0},
    {16,0,0},
    {20,0,0},
    {24,0,0},
    {28,0,0},
    {31,0,0},
  };
  for(uint8_t n=0;n<16;n++){
    Serial.print("0x");
    Serial.print(RGBTo16(charRowCol[n][0],charRowCol[n][1],charRowCol[n][2]),HEX);
    Serial.print(",");
  }
}

void setState(uint16_t s){
  switch(s){
    case STATE_INTRO:
      scrollX=200;
      starMinX=100;
      starMaxX=140;
      starMinY=140;
      starMaxY=180;
      starMinKeepOutX=100;
      starMaxKeepOutX=140;
      starMinKeepOutY=140;
      starMaxKeepOutY=179;
      tft.fillScreen(TFT_BLACK);
      break;
    case STATE_TITLE_SCREEN:
      listScrollPos=0;
      listMaxScrollPos=0;
      starMinKeepOutX=120;
      starMinKeepOutY=160;
      starMaxKeepOutX=120;
      starMaxKeepOutY=160;

      tft.fillRect(120-40, 160-60, 40, 80, 0);
      tft.fillRect(120, 140, 40, 80, 0);
      tft.fillRect(45,290,150,16,0);
      break;
    case STATE_HIGH_SCORES:
      zoomWindow(41,53,199,298);
      break;
    case STATE_SETTINGS:
      listScrollPos=0;
      listMaxScrollPos=0;
      tft.fillScreen(TFT_BLACK);
      drawTitleBox(90);
    
      break;
    case STATE_HELP:
      tft.fillScreen(TFT_BLACK);
      drawTitleBox(50);
    
      break;
    case STATE_SETUP_NAME:
    
      break;
    case STATE_SETUP_WIFI:
      tft.fillScreen(TFT_BLACK);
      drawTitleBox(90);
    
      break;
    case STATE_SETUP_ABOUT:
      tft.fillScreen(TFT_BLACK);
      drawTitleBox(90);
    
      break;
    case STATE_CHALLENGED:
      tft.fillScreen(TFT_BLACK);
      drawTitleBox(90);
    
      break;

    case STATE_CHOOSE_OPPONENT:
      tft.fillScreen(TFT_BLACK);
      drawTitleBox(90);
    
      break;

  }
  IX=0;
  IY=0;
  IZ=0;
  HL=0;
  state=s;
}

void openWindow(uint16_t x,uint16_t y,uint16_t x2,uint16_t y2,uint16_t speed,uint8_t r,uint8_t g,uint8_t b){
  starMinXTarget=x;
  starMaxXTarget=x2;
  starMinYTarget=y;
  starMaxYTarget=y2;
  winXSpeed=speed;
  winYSpeed=speed;
  borderCol=tft.color565(r,g,b);
}

bool openingWindow(){
  
  bool ret=false;
  
  if(starMinX<starMinXTarget){
    ret=true;
    if(starMinXTarget-starMinX<winXSpeed)
      starMinX=starMinXTarget;
    else
      starMinX+=winXSpeed;
  }
  
  if(starMinX>starMinXTarget){
    ret=true;
    if(starMinX-starMinXTarget<winXSpeed)
      starMinX=starMinXTarget;
    else
      starMinX-=winXSpeed;
  }

  if(starMinY<starMinYTarget){
    ret=true;
    if(starMinYTarget-starMinY<winYSpeed)
      starMinY=starMinYTarget;
    else
      starMinY+=winYSpeed;
  }
  
  if(starMinY>starMinYTarget){
    ret=true;
    if(starMinY-starMinYTarget<winYSpeed)
      starMinY=starMinYTarget;
    else
      starMinY-=winYSpeed;
  }

  if(starMaxX<starMaxXTarget){
    ret=true;
    if(starMaxXTarget-starMaxX<winXSpeed)
      starMaxX=starMaxXTarget;
    else
      starMaxX+=winXSpeed;
  }
  
  if(starMaxX>starMaxXTarget){
    ret=true;
    if(starMaxX-starMaxXTarget<winXSpeed)
      starMaxX=starMaxXTarget;
    else
      starMaxX-=winXSpeed;
  }

  if(starMaxY<starMaxYTarget){
    ret=true;
    if(starMaxYTarget-starMaxY<winYSpeed)
      starMaxY=starMaxYTarget;
    else
      starMaxY+=winYSpeed;
  }
  
  if(starMaxY>starMaxYTarget){
    ret=true;
    if(starMaxY-starMaxYTarget<winYSpeed)
      starMaxY=starMaxYTarget;
    else
      starMaxY-=winYSpeed;
  }

  if(!ret){
    tft.drawRect(starMinX-1,starMinY-1,(starMaxX-starMinX)+2,(starMaxY-starMinY)+2,0);
    tft.drawRect(starMinX-2,starMinY-2,(starMaxX-starMinX)+4,(starMaxY-starMinY)+4,0);
  }

  for(uint8_t n=0;n<5;n++){
    tft.drawRect(starMinX+n,starMinY+n,(starMaxX-starMinX)-(n*2),(starMaxY-starMinY)-(n*2),(n>1?0:borderCol));
  }
  return ret;
}

bool drawHiScores(){
  // IY, IZ, HL
  img.setColorDepth(16);
  img.createSprite(150,14);
  img.fillSprite(0);
  drawTxt(hiScoreTable[IZ].sig,150,IY-(10*8),(IZ<3?WHITE_TO_BLUE:GREEN_TO_BLUE));
  tmpStr[5]=0;
  uint16_t s=hiScoreTable[IZ].score;
  for(uint8_t n=0;n<5;n++){
    tmpStr[4-n]='0'+(s%10);
    s/=10;
  }
  
  drawTxt(tmpStr,150,150+30-IY,(IZ<3?WHITE_TO_BLUE:GREEN_TO_BLUE));
  img.pushSprite(45, (IZ*20)+75+(24-(IY/4)));
  img.deleteSprite();
  IY+=3;
  if(IY>=96){
    IY=0;
    if(++IZ>=10){
      return false;
    }
  }
  return true;
}

// This is used for direct writes to image buffer
uint16_t getCol(uint8_t r,uint8_t g,uint8_t b){
  uint16_t c=(r<<11)|((g&63)<<5)|(b&31);
  return ((c>>8)|(c<<8));
}

bool hideTitle(){
  uint16_t c=getCol(31,IY,0);
  uint16_t topY=48+37-IY, botY=48+37+IY;

  img.setColorDepth(16);

  img.createSprite(240,6);
  img.fillSprite(0);
  img.fillRect(0,2,239,2,0xFFFF);
  img.pushSprite(0,topY-10);
  img.pushSprite(0,botY+2);
  img.deleteSprite();
  starMinKeepOutY=topY-10;
  starMaxKeepOutY=botY+6;
  starMinKeepOutX=0;
  starMaxKeepOutX=239;

}

bool revealTitle(){

  uint16_t c=getCol(31,IY,0);
  uint16_t topY=48+37-IY, botY=48+37+IY;

  img.setColorDepth(16);

  img.createSprite(240,6);
  img.fillSprite(0);
  img.fillRect(0,2,239,2,0xFFFF);
  img.pushSprite(0,topY-10);
  img.pushSprite(0,botY+2);
  img.deleteSprite();
    
  if(IY<38){
    img.createSprite(240,1);
    img.fillSprite(0);
    drawGfx(titleGfx+( (37-IY) * (240/8) ), 240, 1, c);
    img.pushSprite(0,topY);
    if(IY<36){
      c=getCol(0,37-IY,IY);
      img.fillSprite(0);
      drawGfx(titleGfx+( (37+IY) * (240/8) ), 240, 1, c);
      img.pushSprite(0,botY);
    }
    img.deleteSprite();
  }
  if(++IY==41){
    return false;
  }
  starMinKeepOutY=topY-10;
  starMaxKeepOutY=botY+6;
  starMinKeepOutX=0;
  starMaxKeepOutX=239;
  return true;
}

void copySprite(uint16_t *src, uint16_t *dest, int16_t words){
  while(words-->-1){
    *dest++=*src++;
  }
}


void drawTitle(char *s,uint16_t y,int16_t xOff){
  uint16_t l=0;
  while(s[l]!=0)
    ++l;
  l*=10;
  img.setColorDepth(16);
  img.createSprite(130,28);
  img.fillSprite(0);
  drawTxt(s,130,(xOff==-1?(130-l)/2:xOff),0,RED_TO_YELLOW,2);
  img.pushSprite(56, y);
  img.deleteSprite();
}

void drawTitle(char *s,uint16_t y){
  drawTitle(s,y,-1);
}

void drawTitleBox(uint16_t y){
  IX=1;
  starMinX=100;
  starMaxX=140;
  starMinY=y+4;
  starMaxY=y+6;
  openWindow(41,y-7,199,y+32,3,255,255,255);
}

void zoomWindow(uint16_t x1,uint16_t y1,uint16_t x2,uint16_t y2){
  starMinX=118;
  starMaxX=122;
  starMinY=156;
  starMaxY=164;
  openWindow(x1,y1,x2,y2,3,255,255,255); // 53,34,187,298,5,28,24,28
}



// Tiles use 16 bit color, they afre already on GGGBBBBBRRRRRGGG format ready to blit into buffer for speed
void drawTile(uint8_t x,uint8_t y){
  
}

// Called 50 times per second
void timerCallback(void *pArg) {
  
  if(tmrReset){
    tmrReset=false;
    tmrCnt=0;
  }else{
    ++tmrCnt;
  }

  if(keyTmrReset){
    keyTmrReset=false;
    keyTmrCnt=0;
  }else{
    ++keyTmrCnt;
  }

  if(connectCheckTmrReset){
    connectCheckTmrReset=false;
    connectCheckTmr=0;
  }else{
    ++connectCheckTmr;
  }

  if(++pingTmr==PING_INTERVAL){
    pingTmr=0;
    pingRequest=true;
  }
  
  if(soundType!=SOUND_NONE && (effectIX>-1)){
    if(effectTmr==0){
      effectTmr=song[effectIX].times[effectPos]*4;
      effectFreq=song[effectIX].notes[effectPos++];
      if(effectPos>=song[effectIX].length){
        effectIX=-1;
        if((soundType==SOUND_ALL) && (songIX[0]>-1)){
          if(songFreq[0]==0){
            pinMode(BUZZER,INPUT);
            digitalWrite(BUZZER,0);
          }else{
            pinMode(BUZZER,OUTPUT);
            analogWrite(BUZZER,127);
            analogWriteFreq(songFreq[0]);
          }
        }else{
          pinMode(BUZZER,INPUT);
          digitalWrite(BUZZER,0);
        }
      }else{
        if(effectFreq==0){
          pinMode(BUZZER,INPUT);
          digitalWrite(BUZZER,0);
        }else{
          pinMode(BUZZER,OUTPUT);
          analogWrite(BUZZER,127);
          analogWriteFreq(effectFreq);
        }
      }
    }
    --effectTmr;
  }
  
  if((soundType==SOUND_ALL) && (songIX[0]>-1)){
    for(uint8_t n=0;n<2;n++){
      if(songTmr[n]==0){
        songTmr[n]=song[songIX[n]].times[songPos[n]]*4;
        songFreq[n]=song[songIX[n]].notes[songPos[n]++];
        if(songPos[n]>=song[songIX[n]].length){
          songPos[n]=0;
        }
        if(n==1 || (effectIX==-1)){
          if(songFreq[n]==0){
            pinMode((n==0?BUZZER:BUZZER2),INPUT);
            digitalWrite((n==0?BUZZER:BUZZER2),0);
          }else{
            pinMode((n==0?BUZZER:BUZZER2),OUTPUT);
            analogWrite((n==0?BUZZER:BUZZER2),(n==0?127:64));
            analogWriteFreq(songFreq[n]);
          }
        }
      }
      --songTmr[n];
    }
  }    
}

void resetTmr(){
  tmrReset=true;
  tmrCnt=0;
}

void resetKeyTmr(){
  keyTmrReset=true;
  keyTmrCnt=0;
}

void resetConnectCheckTmr(){
  connectCheckTmrReset=true;
  connectCheckTmr=0;
}

void exitSettings(){
  img.deleteSprite();
  sprite1.deleteSprite();
  sprite2.deleteSprite();
  tft.fillRect(40,80,160,200,0);
  setState(STATE_TITLE_SCREEN);
}

void writeConfig(char *s,uint16_t ix,uint16_t len){
  uint16_t n=0;
  char c=1;
  while(c!=0 && n<len){
    c=s[n++];
    EEPROM.write(ix++, c);
  }
}

void readConfig(char *s,uint16_t ix,uint16_t len){
  uint16_t n=0;
  char c=1;
  while(c!=0 && n<len){
    c=EEPROM.read(ix++);
    s[n++]=c;
  }
}

void loadConfig(){
  EEPROM.begin(300);
  uint8_t b;
  if(EEPROM.read(0)!=APP_SIG0 || EEPROM.read(1)!=APP_SIG1){
    EEPROM.write(0, APP_SIG0);
    EEPROM.write(1, APP_SIG1);
    EEPROM.commit();

    saveConfig();
  }
  readConfig(myName,MY_NAME_IX,MY_NAME_LEN);
  readConfig(wifiName,WIFI_SSID_IX,WIFI_SSID_LEN);
  readConfig(wifiPassword,WIFI_PASS_IX,WIFI_PASS_LEN);
  soundType=EEPROM.read(SOUND_TYPE_IX);
  if(soundType>2)
    soundType=2;
  uint16_t n,i;
  for(n=0;n<10;n++){
    readConfig(hiScoreTable[n].sig,180+(n*10),6);
    hiScoreTable[n].score=0;
    for(i=0;i<4;i++){
      hiScoreTable[n].score|=(EEPROM.read(180+(n*10)+i+6)<<(i*8));
    }
  }
}

void saveConfig(){
  writeConfig(myName,MY_NAME_IX,MY_NAME_LEN);
  writeConfig(wifiName,WIFI_SSID_IX,WIFI_SSID_LEN);
  writeConfig(wifiPassword,WIFI_PASS_IX,WIFI_PASS_LEN);
  EEPROM.write(SOUND_TYPE_IX,soundType);
  uint16_t n,i;
  for(n=0;n<10;n++){
    writeConfig(hiScoreTable[n].sig,180+(n*10),6);
    for(i=0;i<4;i++){
      EEPROM.write(180+(n*10)+i+6,(hiScoreTable[n].score>>(i*8))&0xFF );
    }
  }
  EEPROM.commit();
}

// Draw a 16x16 sprite at x,y with colour col
void drawSprite(const uint8_t * b, int16_t x, int16_t y,uint16_t col){
  spriteTmp.setColorDepth(16);
  spriteTmp.createSprite(16,16);
  spriteTmp.fillSprite(0);

  uint16_t n,i,buffPos=0;
  uint8_t c;
  for(n=0;n<32;n++){
    c=b[n];
    for(i=0;i<8;i++){
      if((c&(1<<i))>0){
        spriteTmp._img[buffPos]=col;
      }
      ++buffPos;
    }
  }
        
  spriteTmp.pushSprite(x, y);
  spriteTmp.deleteSprite();
}

void showNetworkStatus(){
  uint16_t c=0;
  if(networkStatus!=lastNetworkStatus){
    lastNetworkStatus=networkStatus;
    wifiLogoYPos=0;
    wifiLogoTmr=WIFI_LOGO_DISPLAY_TIME;
    wifiLogoTmr2=0;
  }
  if(wifiLogoYPos>-16){
    switch(networkStatus){
      case NETWORK_OFF:
        c=COLOR_GREY;
        break;
      case NETWORK_OFFLINE:
        c=COLOR_RED;
        break;
      case NETWORK_CONNECTING:
        c=COLOR_BLUE;
        break;
      case NETWORK_CONNECTED:
        c=COLOR_GREEN;
        break;
    }

    if((networkStatus==NETWORK_CONNECTED)||(networkStatus==NETWORK_OFF)){
      if(wifiLogoTmr>0){
        --wifiLogoTmr;
      }else{
        if(++wifiLogoTmr2==3){
          wifiLogoTmr2=0;
          --wifiLogoYPos;
        }
      }
    }
        
    drawSprite(wifiLogo,0,wifiLogoYPos,c);
  }
}

void scrollUpText(char *s, uint8_t strLen,uint8_t strNum, uint8_t breakAfterLine){
    if(IY>150)
      IY-=2;
    uint8_t w=(strLen-1)*10;
    img.createSprite(w,IY>150?16:14);
    for(uint8_t n=0;n<strNum;n++){
      img.fillSprite(0);
      drawTxt( (char *)(s+(strLen*n)) ,w,1,0,GREEN_TO_BLUE,1);
      img.pushSprite((240-w)/2,IY+(n*20)+(n>breakAfterLine?8:0));
    }
    img.deleteSprite();
    if(butUp||butDown||butLeft||butRight||butA||butB){
      playEffect(1);
      tft.fillScreen(0);
      setState(STATE_TITLE_SCREEN);
    }

}

void scrollUpText(char *s, uint8_t strLen,uint8_t strNum){
  scrollUpText(s,strLen,strNum,99);
}

void setSpiTo(uint8_t spiDevice){
#if(DEVICE_VERSION==2 || DEVICE_VERSION==3)
  digitalWrite(CS_SELECT,spiDevice);
#endif
}
