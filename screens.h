void showIntro(){
  tft.fillRect(120-40, 160-60, 40, 80, 0xFFFF);
  tft.fillRect(120, 140, 40, 80, 0xFFFF);
  if(--scrollX<-160)
    scrollX=150;
  img.setColorDepth(16);
  img.createSprite(150,16);
  img.fillSprite(0);
  drawTxt("PRESS ANY BUTTON",150,scrollX,2);
  img.pushSprite(45, 290);
  img.deleteSprite();
  
  if(butUp||butDown||butLeft||butRight||butA||butB){
    playEffect(1);
    setState(STATE_TITLE_SCREEN);
  }
}

void showTitleScreen(){
  if(IX==0){
    if(!revealTitle())
      IX=1;
  }else if(IX==1){  // Picking option
    IX=2;
    IY=0;
    listScrollPos=0;
    cursorY=0;
    waitUp=false;
  }else if(IX==2){
    handleMenu(mainMenuStr,170,5);
  }else if(IX==9){  // ignore B button in main menu
    IX=2;
  }else if(IX==10){
    playEffect(1);
    IY=41;
    IX=11;
    tft.fillRect(50,170,140,110,0);
  }else if(IX==11){
    if(IY>0){
      --IY;
      hideTitle();
    }else{
      tft.fillRect(0,48+37-10,240,20,0);
      IX=12;
    }
  }else if(IX==12){
    switch(cursorY){
      case 0:
        // 1 player game
        is2Player=false;
        setState(STATE_PLAYING);
        break;
      case 1:
        // 2 player game
        setState(STATE_CHOOSE_OPPONENT);
        break;
      case 2:
        setState(STATE_HIGH_SCORES);
        break;
      case 3:
        setState(STATE_SETTINGS);
        break;
      case 4:
        setState(STATE_HELP);
        break;
    }
  }
}

void showSettings(){
  if(IX==0){
    bool isOpening=openingWindow();
    starMinKeepOutX=starMinX;
    starMinKeepOutY=starMinY;
    starMaxKeepOutX=starMaxX;
    starMaxKeepOutY=starMaxY;
    if(!isOpening){
      drawTitle("SETTINGS",90);
      IX=1;
    }
  }else if(IX==1){
    IX=2;
    IY=0;
    cursorY=0;
    waitUp=false;
    listScrollPos=0;
  //  listMaxScrollPos=0;
  }else if(IX==2){
    handleMenu(soundMenuStr[soundType],150,4);
  }else if(IX==9){  // button B
    playEffect(1);
    exitSettings();
  }else if(IX==10){ // button A
    playEffect(1);
    if(cursorY==2){ // Sound off/effects only/effects and music
      if(++soundType>SOUND_ALL){
        soundType=SOUND_NONE;
      }
      if(soundType!=SOUND_ALL){
        pinMode(BUZZER,INPUT);
        digitalWrite(BUZZER,0);
        #if(DEVICE_VERSION==2 || DEVICE_VERSION==3)
        pinMode(BUZZER2,INPUT);
        digitalWrite(BUZZER2,0);
        #endif
      }
      saveConfig();
      IX=2;
    }else{
      tft.fillRect(0,0,240,320,0);
      waitUp=true;
      switch(cursorY){
        case 0:
          setState(STATE_SETUP_NAME);
          break;
        case 1:
          setState(STATE_SETUP_WIFI);
          break;
        case 3:
          setState(STATE_SETUP_ABOUT);
          break;
      }
    }
  }
}

void showHighScores(){
  if(IX==0){
    bool isOpening=openingWindow();
    starMinKeepOutX=starMinX;
    starMinKeepOutY=starMinY-44;
    starMaxKeepOutX=starMaxX;
    starMaxKeepOutY=starMaxY;
    if(!isOpening){
      drawTitleBox(18);
    }else{
      delay(2);
    }
  }else if(IX==1){
    bool isOpening=openingWindow();
    if(!isOpening){
      drawTitle("HIGH SCORES",18);
      IX=2;
    }else{
      delay(2);
    }
  }else if(IX==2){  // draw high scores
    if(!drawHiScores()){
      IX=3;
    }
    if(!waitUp && (butUp||butDown||butLeft||butRight||butA||butB)){
      playEffect(1);
      tft.fillRect(40,11,160,300,0);
      setState(STATE_TITLE_SCREEN);
    }
    delay(10);
  }else if(IX==3){  // show static high scores
    if(!waitUp && (butUp||butDown||butLeft||butRight||butA||butB)){
      playEffect(1);
      tft.fillRect(40,11,160,300,0);
      setState(STATE_TITLE_SCREEN);
    }
    delay(15);
  }

}

void showSetupName(){
  if(doKeyboardInput(enterNameStr,5,myName)!=KEYBOARD_ACTIVE){
    playEffect(1);
    saveConfig();
    if(webSocketActive){
      sendMyNameToServer();
    }
    setState(STATE_TITLE_SCREEN);
  }
}

void showSetupWiFi(){
  if(IX==0){
    bool isOpening=openingWindow();
    starMinKeepOutX=starMinX;
    starMinKeepOutY=starMinY;
    starMaxKeepOutX=starMaxX;
    starMaxKeepOutY=starMaxY;
    if(!isOpening){
      drawTitle("WiFi NETWORK",90);
      getWiFiAPs();
      IX=1;
      IY=0;
      scrollX=150;
    }
  }else if(IX==1){
    totalAPs=WiFi.scanComplete();
    if(totalAPs>-1){
      Serial.print("APS = ");
      Serial.println(totalAPs);

      IX=2;
//      listMaxScrollPos=totalAPs-4;
//      if(listMaxScrollPos<0)
//        listMaxScrollPos=0;
    }
    scrollX-=2;
    if(scrollX<-110)
      scrollX=150;
    img.createSprite(150,16);
    img.fillSprite(0);
    drawTxt("SCANNING...",150,scrollX,2);
    img.pushSprite(45, 160);
    img.deleteSprite();
  }else if(IX==2){
    tft.fillRect(45,160,150,16,0);
    listScrollPos=0;
    cursorY=0;
    waitUp=false;
    IX=3;
  }else if(IX==3){
    createWiFiList();
    handleMenu(tmpStr,150,4);
  }else if(IX==9){  // B pressed
    tft.fillRect(0,0,240,320,0);
    setState(STATE_TITLE_SCREEN);
  }else if(IX==10){ // A pressed
    WiFi.SSID(cursorY+listScrollPos).toCharArray(wifiName,250);
    Serial.print("Chose: ");
    Serial.println(wifiName);
    saveConfig();
    tft.fillRect(0,0,240,320,0);
    IX=11;
  }else if(IX==11){
    if(doKeyboardInput(enterWiFiPasswordStr,60,wifiPassword)!=KEYBOARD_ACTIVE){
      playEffect(1);
      networkStatus=NETWORK_OFFLINE;
      if(keyboardExitType==KEYBOARD_CLOSED){
        setState(STATE_TITLE_SCREEN);
      }else{
        tft.fillScreen(TFT_BLACK);
        drawTitleBox(90);
        IX=12;
      }
    }
  }else if(IX==12){
    bool isOpening=openingWindow();
    starMinKeepOutX=starMinX;
    starMinKeepOutY=starMinY;
    starMaxKeepOutX=starMaxX;
    starMaxKeepOutY=starMaxY;
    if(!isOpening){
      drawTitle("WiFi NETWORK",90);
      IX=13;
      IY=0;
      scrollX=150;
      waitUp=true;
    }
  }else if(IX==13){
    scrollX-=2;
    if(scrollX<-110)
      scrollX=150;
    img.createSprite(150,16);
    img.fillSprite(0);
    drawTxt("CONNECTING...",150,scrollX,2);
    img.pushSprite(45, 160);
    img.deleteSprite();
    if(networkStatus==NETWORK_CONNECTED){
      IX=14;
    }else if(networkStatus==NETWORK_OFF){
      IX=15;
    }
    if(!waitUp && (butUp||butDown||butLeft||butRight||butA||butB)){
      playEffect(1);
      setState(STATE_TITLE_SCREEN);
    }
  }else if(IX==14){
    scrollX-=2;
    if(scrollX<-100)
      scrollX=150;
    img.createSprite(150,16);
    img.fillSprite(0);
    drawTxt("CONNECTED!",150,scrollX,1);
    img.pushSprite(45, 160);
    img.deleteSprite();
    if(!waitUp && (butUp||butDown||butLeft||butRight||butA||butB)){
      tft.fillRect(0,0,240,320,0);
      playEffect(1);
      setState(STATE_TITLE_SCREEN);
    }
  }else if(IX==15){
    scrollX-=2;
    if(scrollX<-150)
      scrollX=150;
    img.createSprite(150,16);
    img.fillSprite(0);
    drawTxt("CONNECT FAILED!",150,scrollX,4);
    img.pushSprite(45, 160);
    img.deleteSprite();
    if(!waitUp && (butUp||butDown||butLeft||butRight||butA||butB)){
      playEffect(1);
      tft.fillRect(0,0,240,320,0);
      setState(STATE_TITLE_SCREEN);
    }
  }

}

void showSetupAbout(){
  if(IX==0){
    playEffect(8);
    bool isOpening=openingWindow();
    starMinKeepOutX=starMinX;
    starMinKeepOutY=starMinY;
    starMaxKeepOutX=starMaxX;
    starMaxKeepOutY=starMaxY;
    if(!isOpening){
      drawTitle("ABOUT",90);
      IX=1;
      IY=320;
    }
  }else if(IX==1){
    if(IY>150)
      --IY;
    img.createSprite(150,16);
    for(uint8_t n=0;n<6;n++){
      img.fillSprite(0);
      drawTxt(aboutStr[n],150,0,0,GREEN_TO_BLUE,1);
      img.pushSprite(45,IY+(n*20)+(n>3?8:0));
      tft.fillRect(45,IY+(n*20)+(n>3?8:0)+16,150,1,0);
    }
    img.deleteSprite();
    if(butUp||butDown||butLeft||butRight||butA||butB){
      playEffect(1);
      tft.fillRect(0,0,240,320,0);
      setState(STATE_TITLE_SCREEN);
    }
  }
}

void showChallenged(){
  if(IX==0){
    bool isOpening=openingWindow();
    starMinKeepOutX=starMinX;
    starMinKeepOutY=starMinY;
    starMaxKeepOutX=starMaxX;
    starMaxKeepOutY=starMaxY;
    if(!isOpening){
      drawTitle("CHALLENGED!!",90);
      IX=1;
      IY=320;
      waitUp=true;
    }
  }else if(IX==1){
    if(IY>150)
      --IY;
    if((keyTmrCnt%64)==0)
      ++IZ;
    img.createSprite(130,16);
    for(uint8_t n=0;n<5;n++){
      img.fillSprite(0);
      if(n==2){
        if((keyTmrCnt%64)>32){
          drawTxt(opponentName,130,40,0,PURPLE_TO_WHITE,1);
        }
      }else{
        drawTxt(challengedStr[n<2?n:n-1],130,0,0,GREEN_TO_BLUE,1);
      }
      img.pushSprite(50,IY+(n*20)+(n>2?16:(n>1?8:0)));
    }
    img.deleteSprite();
    if(IZ>20 || butB || butRight){
      playEffect(1);
      tft.fillRect(50,150,130,120,0);
      IX=2;
      IY=5;
      waitUp=true;
      networkReject();
    }else if(butA || butLeft){
      playEffect(1);
      tft.fillRect(50,150,130,120,0);
      IX=3;
      IY=5;
    }
  }else if(IX==2){  // Wimp out
    --IY;
    if(IY>-125){
      drawTitle("CHALLENGED!! WIMPED OUT!!",90,IY);
    }
    img.createSprite(120,16);
    for(uint8_t n=0;n<3;n++){
      img.fillSprite(0);
      drawTxt(challengedWimpStr[n],120,0,0,GREEN_TO_BLUE,1);
      img.pushSprite(60,(320+(IY>-151?IY:-150))+(n*20));
    }
    img.deleteSprite();
    if(IY<-600 || (butUp||butDown||butLeft||butRight||butA||butB)){
      playEffect(1);
      tft.fillScreen(0);
      setState(STATE_TITLE_SCREEN);
    }
  }else if(IX==3){
    --IY;
    if(IY>-125){
      drawTitle("CHALLENGED!!   ACCEPTED! ",90,IY);
    }
    img.createSprite(100,16);
    img.fillSprite(0);
    drawTxt(challengedHeroStr,100,0,0,GREEN_TO_BLUE,1);
    img.pushSprite(70,(320+(IY>-151?IY:-150)));
    img.deleteSprite();
    if(IY==-150)
      networkAccept();
    if(IY<-600){
      IX=4;
      IY=0;
      resetKeyTmr();
    }
  }else if(IX==4){
    img.createSprite(100,16);
    img.fillSprite(0);
    if((keyTmrCnt%64)<32)
      drawTxt(timedOutStr,100,0,0,RED_TO_BLUE,1);
    img.pushSprite(70,170);
    img.deleteSprite();
    if(++IY>1000){
      tft.fillScreen(0);
      setState(STATE_TITLE_SCREEN);
    }
  }
}

void showHelp(){
  if(IX==0){
    bool isOpening=openingWindow();
    starMinKeepOutX=starMinX;
    starMinKeepOutY=starMinY;
    starMaxKeepOutX=starMaxX;
    starMaxKeepOutY=starMaxY;
    if(!isOpening){
      drawTitle("HELP",50);
      IX=1;
      IY=0;
      waitUp=true;
    }
  }else if(IX==1){
    img.createSprite(200,16);
    for(uint8_t n=0;n<8;n++){
      img.fillSprite(0);
      drawTxt(helpStr[IY+n],200,0,0,GREEN_TO_BLUE,1);
      img.pushSprite(20,110+(n*21));
    }
    img.deleteSprite();
    if(butDown && IY<(49-8)){
      ++IY;
      waitUp=true;
    }else if(butUp && IY>0){
      --IY;
      waitUp=true;
    }else if(butA || butB){
      tft.fillScreen(0);
      setState(STATE_TITLE_SCREEN);
    }
  }
}
