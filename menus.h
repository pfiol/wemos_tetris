void createCursors(){
  charWidth=8;
  sprite1.setColorDepth(16);
  sprite1.createSprite(16,16);
  
  sprite2.setColorDepth(16);
  sprite2.createSprite(16,16);

  img.createSprite(16,16);
  img.fillSprite(0);
  drawTxt(rightArrow,16,0,0,RED_TO_BLUE,1);
  copySprite(img._img,sprite1._img,16*16);
  
  img.fillSprite(0);
  drawTxt(leftArrow,16,0,0,RED_TO_BLUE,1);
  copySprite(img._img,sprite2._img,16*16);
  img.deleteSprite();
  charWidth=10;
}

void clearCursors(){
  sprite1.deleteSprite();
  sprite2.deleteSprite();
}

// Create pick list from \n delimited string with item at cursorY hilighted
// Uses scrollListPos to skip items at start
void makeList(const char * s,uint16_t y,uint16_t col,uint16_t hiCol, uint8_t maxMenuItems){
  uint16_t n=0,ix=0,cnt=0,h,i=0;
  uint16_t w=0;
  
  while(s[n]!=0){
    tmpStr[n]=s[n];
    if(s[n]=='\n'){
      if((n-i)-1>w){
        w=(n-i)-1;
      }
      i=n;
      ++cnt;
    }
    ++n;
  }

  if((n-i)>w)
    w=n-i;

  totalMenuItems=cnt+1;
  listMaxScrollPos=totalMenuItems-maxMenuItems;
  if(listMaxScrollPos<0)
    listMaxScrollPos=0;
  if(totalMenuItems>maxMenuItems)
    totalMenuItems=maxMenuItems;
  if(listScrollPos>listMaxScrollPos)
    listScrollPos=listMaxScrollPos;

  uint16_t slp=listScrollPos;
  //w=(w-1)*10;
  w*=10;
  tmpStr[n]=0;

  cnt=0;
  bool atEnd=false;
  n=0;
  while(slp>0){
    if(tmpStr[n++]=='\n')
      --slp;
  }
  ix=n;
  listWidth=w;
  img.createSprite(w,16);
  while(!atEnd){
    if((tmpStr[n]=='\n') || (tmpStr[n]==0)){
      if(tmpStr[n]==0)
        atEnd=true;
      tmpStr[n]=0;
      img.fillSprite(0);
      drawTxt(tmpStr+ix,w,0,0,(cnt==cursorY?hiCol:col),1);
      img.pushSprite((240-listWidth)/2, y+(cnt*24));
      ++cnt;
      ++n;
      ix=n;
      if(cnt>=totalMenuItems)
        atEnd=true;
    }
    ++n;
  }
  img.deleteSprite();

  cursorXOff=((240-listWidth)/2)-30;

  createCursors();
  sprite1.pushSprite(cursorXOff+(IY/8), y+(cursorY*24));
  sprite2.pushSprite((239-16)-cursorXOff-(IY/8), y+(cursorY*24));
  clearCursors();
  
  if(++IY==64)
    IY=0;

}

void clearCursors(uint16_t y){
  tft.fillRect(cursorXOff, y+(cursorY*24),24,16,0);
  tft.fillRect((239-16)-cursorXOff-8, y+(cursorY*24),24,16,0);
}

void handleMenu(const char *s, uint16_t y,uint8_t maxMenuItems){
  makeList(s,y,RED_TO_YELLOW,PURPLE_TO_WHITE,maxMenuItems);
  bool changed=false;
  if(!waitUp){
    if(butUp){
      waitUp=true;
      if(cursorY>0){
        clearCursors(y);
        --cursorY;
        changed=true;
      }else if(listScrollPos>0){
        --listScrollPos;
        changed=true;
      }
      IY=0;
    }else if(butDown){
      waitUp=true;
      if(cursorY<totalMenuItems-1){
        clearCursors(y);
        ++cursorY;
        changed=true;
      }else if(listScrollPos<listMaxScrollPos){
        ++listScrollPos;
        changed=true;
      }
      if(changed){
        IY=0;
      }
#if USE_SERIAL
    }else if(butRight){
#else
    }else if(butA){
#endif
      clearCursors(y);
      waitUp=true;
      IX=10;
    }else if(butB){
      clearCursors(y);
      waitUp=true;
      IX=9;
    }
  }
}
