bool revealKeyboard(){
  if(keyboardY>160){
    uint16_t s=(keyboardY-160)/4;
    if(s>10)
      s=10;
    else if((s==0) && (keyboardY-160>0))
      s=1;
    keyboardY-=s;
    starMinKeepOutY=keyboardY-1;
    showKeyboard(0,keyboardY,keyboardCase);
    // zoom open the text input window
    if(keyboardY==160){
      starMinX=60;
      starMaxX=180;
      starMinY=142;
      starMaxY=143;
      openWindow(0,129,240,155,1,255,255,255); // 53,34,187,298,5,28,24,28
    }
    return true;
  }else if(openingWindow()){
    starMinKeepOutY=starMinY;
    tft.fillRect(0,156,240,4,0);
    return true;
  }
  return false;
}

bool hideKeyboard(){
  if(keyboardY<320){
    uint16_t s=(320-keyboardY)/4;
    if(s>10)
      s=10;
    else if((s==0) && (320-keyboardY>0))
      s=1;
    keyboardY+=s;
    starMinKeepOutY=keyboardY;
    showKeyboard(0,keyboardY,keyboardCase);
    tft.fillRect(0,keyboardY-s,240,s,0);
    return true;
  }
  return false;
  
}

void showKeyboard(uint16_t startX,uint16_t startY,uint8_t charCase){
  uint16_t x=startX+10, y=0,n,ix=0,ix2=0;
  char c=1;
  charWidth=16;
  img.createSprite(220,30);
  keyboardActiveChar=0;
  for(n=0;n<10;n++){
    tft.drawRect(startX+n,startY+n,240-(n*2),160-(n*2),(n==0?0xffff:0x18C2));
  }
  while(c!=0){
    c=keyboardMap[charCase][ix];
    if(c=='\n'){
      tmpStr[ix2]=0;
      img.fillSprite(0x18C2);
      for(n=0;n<ix2;n++){
        if(y==cursorY && n==cursorX){
          keyboardActiveChar=tmpStr[n];
          img.fillRect((n*16)-2,0,16,20,0xffff);
        }else{
          img.fillRect(n*16,0,12,20,0x4208);
        }
      }
      
      if(cursorX==12){
        keyboardActiveChar=cursorY+1;
      }
      
      if(y!=1){
        img.fillRect(12*16,0,30,20,
          ( 
            (y==0 && keyboardActiveChar==1) || 
            (y==3 && keyboardActiveChar==4) || 
            (y==2 && 
              (keyboardActiveChar==2 || keyboardActiveChar==3)
            ) ? 0xFFFF:0x4208
          )
        );
      }else{
        img.fillRect(12*16,0,30,30,(keyboardActiveChar==2 || keyboardActiveChar==3 ?0xFFFF:0x4208));
      }

      charWidth=8;
      if(y==0){
        drawTxt(leftArrow,220,(12*16)+6,2,BLACK_TO_BLACK,1);
      }else if(y==2){
        drawTxt(return1,220,(12*16)+6,2,BLACK_TO_BLACK,1);
      }else if(y==3){
        charWidth=10;
        drawTxt(Aa,220,(12*16)+5,2,BLACK_TO_BLACK,1);
      }
      charWidth=16;

      drawTxt(tmpStr,220,2,2,BLACK_TO_BLACK,1);
      img.pushSprite(x, (y*30)+startY+10);

      if(y==2){
         tft.fillRect(9+(12*16)+18,startY+50,4,25,0x0);
      }
      ix2=0;
      ++y;
    }else if(c>31){
      tmpStr[ix2++]=c;
    }
    ++ix;
  }

  img.deleteSprite();

  if(cursorY==4){
    keyboardActiveChar=' ';
    tft.fillRect(startX+10,startY+10+(4*30),220,20,0xffff);
  }else{
    tft.fillRect(startX+10,startY+10+(4*30),220,20,0x4208);
  }
  
  charWidth=10;
}

void drawInputTxt(){
  img.setColorDepth(16);
  img.createSprite(220,16);
  img.fillSprite(0);
  if(cursorOn){
    inputStr[inputStrLen]=100+32;
    inputStr[inputStrLen+1]=0;
  }
  int16_t xPos=(220-(inputStrLen*10))/2;
  if(xPos<10){
    xPos=(210-(inputStrLen*10));
  }
  drawTxt(inputStr,220,xPos,0,PURPLE_TO_WHITE,1);
  inputStr[inputStrLen]=0;
  img.pushSprite(10, 133);
  img.deleteSprite();
}

uint16_t doKeyboardInput(char *titleStr, uint16_t maxChars, char *outputStr){
  bool ret=KEYBOARD_ACTIVE;
  switch(keyboardState){
    case KS_IDLE:
      starMinKeepOutX=0;
      starMaxKeepOutX=240;
      starMinKeepOutY=310;
      starMaxKeepOutY=320;
      keyboardState=KS_OPENING;
      keyboardMaxInputChars=maxChars;
      cursorX=0;
      cursorY=0;
      for(uint8_t n=0;n<keyboardMaxInputChars+1;n++){
        inputStr[n]=outputStr[n];
      }
      break;
    case KS_OPENING:
      if(!revealKeyboard()){
        inputStrLen=0;
        while(inputStr[inputStrLen]!=0)
          ++inputStrLen;
        titleStrLen=0;
        while(titleStr[titleStrLen]!=0)
          ++titleStrLen;
        drawInputTxt();
        keyboardState=KS_EDITING;
        cursorOn=true;
        resetTmr();
        titleStrWidth=110;
      }
      break;
    case KS_EDITING:

      img.setColorDepth(16);
      img.createSprite(220,32);
      img.fillSprite(0);
      charWidth=titleStrWidth;
      drawTxt(titleStr,220,(220-(titleStrLen*titleStrWidth))/2,0,RED_TO_YELLOW,2);
      if(titleStrWidth>10)
        titleStrWidth-=2;
      charWidth=10;
      img.pushSprite(10, 50);
      img.deleteSprite();

      if(butUp){
        if(cursorY>0)
          --cursorY;
        else
          cursorY=4; 
        if(cursorY==3 && cursorX==11)
          --cursorY;
        if(cursorY==1 && cursorX==12)
          --cursorY;
      }else if(butDown){
        if(cursorY<4)
          ++cursorY;
        else
          cursorY=0;
        if(cursorY==3 && cursorX==11)
          ++cursorY;
        if(cursorY==2 && cursorX==12)
          ++cursorY;
      }
      
      if(butLeft){
        if(cursorX>0)
          --cursorX;
        else
          cursorX=12;
        if(cursorY==3 && cursorX==11)
          --cursorX;
      }else if(butRight){
        if(cursorX<12)
          ++cursorX;
        else
          cursorX=0;
        if(cursorY==3 && cursorX==11)
          ++cursorX;
      }

      if(butA){
        if(cursorX==12){
          switch(cursorY){
            case 0: // Delete
              if(inputStrLen>0){
                --inputStrLen;
                inputStr[inputStrLen]=0;
              }
              break;
            case 1: // Return
            case 2:
              tft.fillRect(10,50,220,32,0);
              tft.fillRect(0,129,240,27,0);
              for(uint8_t n=0;n<keyboardMaxInputChars+1;n++){
                outputStr[n]=inputStr[n];
              }
              saveConfig();
              keyboardExitType=KEYBOARD_SAVED;
              keyboardState=KS_CLOSING;
              break;
            case 3: // Case
              keyboardCase=(keyboardCase==0?1:0);
              break;
          }
        }else{
          if(inputStrLen<keyboardMaxInputChars){
            inputStr[inputStrLen++]=((cursorY==4)?' ':keyboardMap[keyboardCase][(cursorY*13)+cursorX]);
          }
        }
      }else if(butB){
        tft.fillRect(10,50,220,32,0);
        tft.fillRect(0,129,240,27,0);
        keyboardExitType=KEYBOARD_CLOSED;
        keyboardState=KS_CLOSING;
      }
      
      if(butUp||butDown||butLeft||butRight||butA||butB){
        showKeyboard(0,keyboardY,keyboardCase);
        waitUp=true;
      }

      if(keyboardState!=KS_CLOSING){
        drawInputTxt();
        if(tmrCnt>=CURSOR_BLINK_SPEED){
          resetTmr();
          cursorOn=!cursorOn;
        }
      }
      break;
    case KS_CLOSING:
      if(!hideKeyboard()){
        keyboardState=KS_IDLE;
        ret=keyboardExitType;
      }
      break;
  }

  return ret;
}
