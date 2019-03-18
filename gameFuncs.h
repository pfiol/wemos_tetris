// resets grid and draws it to display
void resetGrid(){
  uint8_t x,y;
  for(y=FIRST_VISIBLE_ROW;y<GRID_HEIGHT;y++){
    for(x=0;x<GRID_WIDTH;x++){
      grid[y][x]=0;
      oldGrid[y][x]=1;
    }
  }
  drawGrid();
}

// draws grid, with any changes since last draw
void drawGrid(){
  uint8_t x,y,gridBlock,maskSides;
  for(y=FIRST_VISIBLE_ROW;y<GRID_HEIGHT;y++){
    for(x=0;x<GRID_WIDTH;x++){
      if(grid[y][x]!=oldGrid[y][x]){
        gridBlock=grid[y][x];
        if(gridBlock==100){
          maskSides=0;
          gridBlock=9;
          if(y>FIRST_VISIBLE_ROW && grid[y-1][x]==100)
            maskSides|=1;
          if(y<GRID_HEIGHT-1 && grid[y+1][x]==100)
            maskSides|=2;
          if(x>0 && grid[y][x-1]==100)
            maskSides|=4;
          if(x<GRID_WIDTH-1 && grid[y][x+1]==100)
            maskSides|=8;
          drawGhostColorSprite(block+(gridBlock*(12*12)),(x*13)+GRID_X,((y-FIRST_VISIBLE_ROW)*13)+GRID_Y,maskSides);
        }else{
          drawColorSprite(block+(gridBlock*(12*12)),(x*13)+GRID_X,((y-FIRST_VISIBLE_ROW)*13)+GRID_Y);
        }
        oldGrid[y][x]=grid[y][x];
      }
    }
  }
}

// Opponent clears X rows, so add that number to your stack
void addRows(uint8_t num){
  uint8_t x,y;
  for(y=FIRST_VISIBLE_ROW;y<GRID_HEIGHT-num;y++){
    for(x=0;x<GRID_WIDTH;x++){
      grid[y][x]=grid[y+num][x];
      oldGrid[y][x]=100;
      oldGrid[y+num][x]=100;
    }
  }
  uint8_t hole=0;
  for(y=GRID_HEIGHT-num;y<GRID_HEIGHT;y++){
    hole=rng()%10;
    for(x=0;x<GRID_WIDTH;x++){
      grid[y][x]=(x==hole?0:8);
      oldGrid[y][x]=10;
    }
  }
}

// shunt all blocks down by one
void clearRow(uint8_t row){
  uint8_t x,y;
  for(y=row;y>1;y--){
    for(x=0;x<GRID_WIDTH;x++){
      oldGrid[y][x]=100;
      oldGrid[y-1][x]=100;
      grid[y][x]=grid[y-1][x];
    }
  }
}

bool isRowComplete(uint8_t r){
  bool ret=true;
  for(uint8_t n=0;n<GRID_WIDTH;n++){
    if(grid[r][n]==0){
      ret=false;
      break;
    }
  }
  return ret;
}

bool anyRowComplete(){
  bool ret=false;
  for(uint8_t n=FIRST_VISIBLE_ROW;n<GRID_HEIGHT;n++){
    if(isRowComplete(n)){
      ret=true;
      break;
    }
  }
  return ret;
}

// Animate row clearance
bool doClearRows(){
  uint8_t y,x;
  ++IY;
  for(y=GRID_HEIGHT-1;y>FIRST_VISIBLE_ROW-1;y--){
    if(isRowComplete(y)){
      for(x=0;x<GRID_WIDTH;x++){
       drawColorSprite(block+(grid[y][x]*(12*12)),(x*13)+GRID_X,((y-FIRST_VISIBLE_ROW)*13)+GRID_Y,IY); 
      }
    }
  }
  if(IY==12){
    bool moreToClear=true;
    x=0;
    while(moreToClear){
      moreToClear=false;
      for(y=GRID_HEIGHT-1;y>FIRST_VISIBLE_ROW-1;y--){
        if(isRowComplete(y)){
          clearRow(y);
          ++x;
          moreToClear=true;
          break;
        }
      }
    }
    completedLines+=x;
    if(completedLines>=10){
      completedLines-=10;
      if(gameSpeed>3){
        ++level;
        gameSpeed-=3;
      }else if(gameSpeed>1){
        ++level;
        gameSpeed=1;
      }
      showLevel();
    }
    playEffect(x+2);  // tetris sounds 1,2,3,4
    if(is2Player)
      networkSendClearedRows(x);
    score+=(x*x*5)*(is2Player?2:1); // 5, 20, 45, 80 or 10,40,90,160
    if(score>99999)
      score=99999;
    showScore();
    return false;
  }
  delay(5);
  return true;
}

void drawGameOver(){
  img.setColorDepth(16);
  img.createSprite(90,28);
  img.fillSprite(0);
  char s[]="GAME OVER";
  if(IZ==2 && IY<20){
    s[IY/2]=0;
  }
  drawTxt(s,90,1,0,0,2);
  img.pushSprite(38, 145);
  img.deleteSprite();

}
bool doYouWinAnim(){
  uint8_t yPos,n,i,x;
  for(n=0;n<GRID_HEIGHT;n++){
    for(i=0;i<GRID_WIDTH;i++){
      grid[n][i]=0;
    }
  }
  for(x=0;x<3;x++){
    for(n=0;n<5;n++){
      for(i=0;i<4;i++){
        yPos=(GRID_HEIGHT-1)-(IY-n)+i+(x*8);
        if(yPos>2 && yPos<GRID_HEIGHT){
          grid[yPos][4-n]=3;
          grid[yPos][5+n]=3;
        }
      }
    }
  }
  
  // Overlay text
  for(n=0;n<6*9;n++){
    yPos=((GRID_HEIGHT-1)-IZ)+n;
    if(yPos>2 && yPos<GRID_HEIGHT){
      for(i=0;i<8;i++){
        if((bigChars[n]&(1<<7-i))!=0){
          grid[yPos][i+1]=7;
        }
      }
    }
  }
  
  if(++IY>(GRID_HEIGHT+4)){
    IY=(GRID_HEIGHT+4)-7;
  }
  if(((++HL)%2)==0){
    if(++IZ>(6*9)+(GRID_HEIGHT-2))
      IZ=0;
  }
  delay(20);
  if(HL>((6*9)+(GRID_HEIGHT-2))*5*2)
    return false;
  return true;
}

bool doYouLoseAnim(){
  uint8_t yPos,n,i,x;
  for(n=0;n<GRID_HEIGHT;n++){
    for(i=0;i<GRID_WIDTH;i++){
      grid[n][i]=0;
    }
  }
  for(x=0;x<3;x++){
    for(n=0;n<5;n++){
      for(i=0;i<4;i++){
        yPos=(GRID_HEIGHT-1)-(IY-n)+i+(x*8);
        if(yPos>2 && yPos<GRID_HEIGHT){
          grid[yPos][4-n]=2;
          grid[yPos][5+n]=2;
        }
      }
    }
  }
  
  // Overlay text
  for(n=0;n<5*9;n++){
    yPos=((GRID_HEIGHT-1)-IZ)+n;
    if(yPos>2 && yPos<GRID_HEIGHT){
      for(i=0;i<8;i++){
        if((bigChars[n+(6*9)]&(1<<7-i))!=0){
          grid[yPos][i+1]=7;
        }
      }
    }
  }
  
  if(++IY>(GRID_HEIGHT+4)){
    IY=(GRID_HEIGHT+4)-7;
  }
  if(((++HL)%2)==0){
    if(++IZ>(5*9)+(GRID_HEIGHT-2))
      IZ=0;
  }
  delay(20);
  if(HL>((5*9)+(GRID_HEIGHT-2))*5*2)
    return false;
  return true;  
}

bool doGameOverAnim(){
  int8_t n,i;
  bool ret=true;
  if(IZ==0){
    for(n=0;n<GRID_WIDTH;n++){
      if(grid[(GRID_HEIGHT-1)-IY][n]!=0)
        grid[(GRID_HEIGHT-1)-IY][n]=2;
    }
    if(tmrCnt>=5){
      resetTmr();
      if(++IY==(GRID_HEIGHT-FIRST_VISIBLE_ROW)+1){
        IZ=1;
        IY=0;
      }
    }
  }else if(IZ==1){
    if(tmrCnt>=2){
      resetTmr();
      for(n=GRID_HEIGHT-1;n>FIRST_VISIBLE_ROW-2;n--){
        for(i=0;i<GRID_WIDTH;i++){
          grid[n][i]=grid[n-1][i];
        }
      }
      if(++IY>24){
        for(n=0;n<7;n++){
          for(i=0;i<3;i++){
            HL=n+(i*7);
            bouncy[HL]=((n<3?6-n:n)*10)+250+(i*40);
            bouncyDir[HL]=-2;
          }
        }
        IZ=2;
      }
    }
  }else if(IZ==2){
      if(score>hiScoreTable[9].score){
    img.createSprite(90,14);
    img.fillSprite(0);
    if(tmrCnt%64>32){
      drawTxt("NEW HIGH!",90,1,0,1,1);
    }
    img.pushSprite(38, 266);
    img.deleteSprite();
  }

    for(n=0;n<7;n++){
      for(i=0;i<3;i++){
        HL=n+(i*7);
        if(bouncy[HL]<250){
          drawColorSprite(block+((i+1)*12*12),28+(n*16),bouncy[HL]);
          tft.fillRect(28+(n*16),bouncy[HL]-1,14,2,0);
          tft.fillRect(28+(n*16),bouncy[HL]+14,14,2,0);
        }
        bouncy[HL]+=bouncyDir[HL];
        if(bouncy[HL]==66 && bouncyDir[HL]==-2){
          bouncyDir[HL]=2;
        }else if(bouncy[HL]==248 && bouncyDir[HL]==2){
          bouncyDir[HL]=-2;
        }
      }
      drawGameOver();
    }
  }

  
  return ret;
}

void setupGameScreen(){
  uint16_t n=0;
  tft.fillScreen(0);
  for(n=0;n<32;n++){
    tft.fillRect(0,n*2,240,1,tft.color565(252-(n*8),0,0));
    tft.fillRect(0,319-n*2,240,1,tft.color565(0,0,252-(n*8)));
  }
  drawFramedBox(16,33,131,261);
  drawFramedBox(158,33,69,36);
  drawFramedBox(158,82,69,50);  // -1
  drawFramedBox(158,145,69,36); // -3

  img.setColorDepth(16);
  img.createSprite(60,14);
  
  img.fillSprite(0);
  drawTxt("SCORE",60,6,0);
  img.pushSprite(163, 38);

  img.fillSprite(0);
  drawTxt("NEXT",60,11,0);
  img.pushSprite(163, 87);

  img.fillSprite(0);
  drawTxt("LEVEL",60,6,0);
  img.pushSprite(163,150);

  if(is2Player){
    drawFramedBox(158,195,69,50); // -4
    img.fillSprite(0);
    drawTxt("ENEMY",60,6,0);
    img.pushSprite(163, 200);
    drawFramedBox(158,258,69,36);
    img.fillSprite(0);
    drawTxt("SCORE",60,6,0);
    img.pushSprite(163, 263);
    img.deleteSprite();
    img.createSprite(60,28);
    img.fillSprite(0);
    drawTxt(opponentName,60,6,0,PURPLE_TO_WHITE,2);
    img.pushSprite(163, 216);
  }

  img.deleteSprite();

  showScore();
  if(is2Player)
    showEnemyScore();
  showLevel();
}


void drawFramedBox(uint16_t x,uint16_t y,uint16_t w,uint16_t h){
  tft.fillRect(x,y,w,h,0);
  tft.drawRect(x-1,y-1,w+2,h+2,tft.color565(60,60,60));
  tft.drawRect(x-2,y-2,w+4,h+4,tft.color565(60,60,60));
}

// Draw a 12x12 sprite at x,y
void drawColorSprite(const uint16_t * b, int16_t x, int16_t y){

  spriteTmp.setColorDepth(16);
  spriteTmp.createSprite(12,12);

  for(uint16_t n=0;n<12*12;n++){
    spriteTmp._img[n]=b[n];
  }

  spriteTmp.pushSprite(x, y);
  spriteTmp.deleteSprite();
}

// Draw a 12x12 sprite at x,y, with spacing at top (to fade out cleared rows)
void drawColorSprite(const uint16_t * b, int16_t x, int16_t y, int16_t h){

  spriteTmp.setColorDepth(16);
  spriteTmp.createSprite(12,12);
  spriteTmp.fillSprite(0);
  for(uint16_t n=0;n<12*(12-h);n++){
    spriteTmp._img[n+(h*12)]=b[n];
  }
  spriteTmp.pushSprite(x, y);
  spriteTmp.deleteSprite();
}

// Draw a 12x12 sprite at x,y - with masked sides.
// maskSides is a bitwise uint8_t = bit 0 = top, bit 1= bottom bit 2=left bit 3=right. If bits are set,
// then mask that side of pixels.
void drawGhostColorSprite(const uint16_t * b, int16_t x, int16_t y, uint8_t maskSides){

  spriteTmp.setColorDepth(16);
  spriteTmp.createSprite(12,12);
  uint8_t xPos,yPos,ix=0;
  for(yPos=0;yPos<12;yPos++){
    for(xPos=0;xPos<12;xPos++){
      if(
        (yPos==0 && (maskSides&1)>0 && (xPos>0 && xPos<11))
        ||
        (yPos==11 && (maskSides&2)>0 && (xPos>0 && xPos<11))
        ||
        (xPos==0 && (maskSides&4)>0 && (yPos>0 && yPos<11))
        ||
        (xPos==11 && (maskSides&8)>0 && (yPos>0 && yPos<11))
      ){
        spriteTmp._img[ix]=0;
      }else{
        spriteTmp._img[ix]=b[ix];
      }
      ++ix;
    }
  }

  spriteTmp.pushSprite(x, y);
  spriteTmp.deleteSprite();
}

void drawBigNum(uint16_t ix){
  uint8_t i,n;
  for(n=0;n<11;n++){
    for(i=0;i<8;i++){
      if((bigNum[(ix*11)+n]&(1<<i))>0){
        grid[7+n][8-i]=7;
      }
    }
  }
}

void showScore(){
  img.createSprite(60,14);
  img.fillSprite(0);
  uint16_t s=score;
  for(int8_t n=4;n>-1;n--){
    scoreTxt[n]='0'+(s%10);
    s/=10;
  }
  drawTxt(scoreTxt,60,5,1);
  img.pushSprite(163, 54);
  img.deleteSprite();
}

void showEnemyScore(){
  img.createSprite(60,14);
  img.fillSprite(0);
  uint16_t s=enemyScore;
  for(int8_t n=4;n>-1;n--){
    scoreTxt[n]='0'+(s%10);
    s/=10;
  }
  drawTxt(scoreTxt,60,5,1);
  img.pushSprite(163, 279);
  img.deleteSprite();
}

void showLevel(){
  img.createSprite(60,14);
  img.fillSprite(0);
  scoreTxt[0]='0'+((level/10)%10);
  scoreTxt[1]='0'+(level%10);
  scoreTxt[2]=0;
  drawTxt(scoreTxt,60,25,1);
  img.pushSprite(163, 166);
  img.deleteSprite();
}

void getNextPiece(){
  currentPiece=nextPiece;
  nextPiece=(rng()%7)+1;
  tft.fillRect(163,103,60,25,0);
  const int8_t (*b)[2]=pieceDef[nextPiece][0];
  uint16_t xStart=(nextPiece==1?180:(nextPiece==4?167:173))+13;
  uint16_t yStart=(nextPiece==1?98:104)+12;
  for(uint8_t n=0;n<4;n++){
    drawColorSprite(block+(nextPiece*12*12),xStart+(b[n][0]*13),yStart+(b[n][1]*13));
  }
  cursorX=4;
  cursorY=3;
  currentRotation=0;
  gameState=GAMESTATE_DROPPING;
  resetTmr();
}

void showStatePlaying(){
  if(IX==0){
    weWin=false;
    score=0;
    enemyScore=0;
    level=1;
    completedLines=0;
    gameSpeed=GAME_SPEED_START;
    gameState=GAMESTATE_DROPPING;
    setupGameScreen();
    getNextPiece(); // pre-load the "next" piece, which is the first piece of the game!
    repeatSpeed=REPEAT_SPEED_RESET;
    keyRepeating=false;
    keyPressed=false;
    rowsToAdd=0;
    IX=1;
  }else if(IX==1){
    resetGrid();
    IX=2;
    IZ=1;
    HL=0;
    playEffect(9);
  }else if(IX==2){
    uint8_t yPos;
    for(uint8_t n=0;n<5;n++){
      yPos=13-(IY-n);
      if(yPos>2 && yPos<14){
        grid[yPos][4-n]=4-IZ;
        grid[yPos][5+n]=4-IZ;
      }
      yPos=16-(IY-n);
      if(yPos>2 && yPos<14){
        grid[yPos][4-n]=0;
        grid[yPos][5+n]=0;
      }
      yPos=13+(IY-n);
      if(yPos<23 && yPos>13){
        grid[yPos][4-n]=4-IZ;
        grid[yPos][5+n]=4-IZ;
      }
      yPos=10+(IY-n);
      if(yPos<23 && yPos>13){
        grid[yPos][4-n]=0;
        grid[yPos][5+n]=0;
      }
    }
    if(!(HL==2 && IZ==3))
      drawBigNum(HL);
    drawGrid();
    if(++IY>19){
      if(++IZ==4){
        IZ=1;
        if(++HL==3){
          getNextPiece();
          movePiece(cursorX,cursorY,currentRotation);
          resetGrid();
          IX=3;
        }else{
          playEffect(9);
        }
      }
      IY=0;
    }
    delay(10);
  }else if(IX==3){
    inGame();
  }
}

void clearGhostPiece(){
  uint8_t x,y;
  for(y=0;y<GRID_HEIGHT;y++){
    for(x=0;x<GRID_WIDTH;x++){
      if(grid[y][x]==100)
        grid[y][x]=0;
//        oldGrid[y][x]=100;
    }
  }
}

void clearPiece(int8_t x,int8_t y,uint8_t r){
  const int8_t (*b)[2]=pieceDef[currentPiece][r];
  for(uint8_t n=0;n<4;n++){
    grid[y+b[n][1]][x+b[n][0]]=0;
  }
  clearGhostPiece();
}

bool checkCanMove(int8_t x,int8_t y,uint8_t r){
  bool ret=true;
  const int8_t (*b)[2]=pieceDef[currentPiece][r];
  byte gridBlock;
  for(uint8_t n=0;n<4;n++){
    gridBlock=grid[y+b[n][1]][x+b[n][0]];
    if(
      (gridBlock!=0 && gridBlock<9)
      ||
      (y+b[n][1]>=GRID_HEIGHT)
      ||
      (x+b[n][0]>=GRID_WIDTH)
      ||
      (x+b[n][0]<0)
    ){
      ret=false;
      break;
    }
  }
  return ret;
}

void drawGhostPiece(){
  uint8_t cX=cursorX,cY=cursorY;
  while(checkCanMove(cX,cY+1,currentRotation)){
    ++cY;
  }
  const int8_t (*b)[2]=pieceDef[currentPiece][currentRotation];
  for(uint8_t n=0;n<4;n++){
    grid[cY+b[n][1]][cX+b[n][0]]=100;
    oldGrid[cY+b[n][1]][cX+b[n][0]]=0;
  }

}

void drawPiece(){
  drawGhostPiece();
  const int8_t (*b)[2]=pieceDef[currentPiece][currentRotation];
  for(uint8_t n=0;n<4;n++){
    grid[cursorY+b[n][1]][cursorX+b[n][0]]=currentPiece;
  }
} 

bool movePiece(int8_t x,int8_t y,int8_t r){
  bool ret=false;
  if(r<0)
    r=3;
  else if(r>3)
    r=0;
  clearPiece(cursorX,cursorY,currentRotation);
  if(checkCanMove(x,y,r)){
    cursorX=x;
    cursorY=y;
    currentRotation=r;
    ret=true;
  }
  drawPiece();
  return ret;
}

// Actual game play!
void inGame(){
  switch(gameState){
    case GAMESTATE_DROPPING:
      if(butUp){
        waitUp=true;
      }else if(butDown||butLeft||butRight||butA||butB){
        if(keyPressed){
          keyRepeating=true;
        }else{
          keyPressed=true;
          resetKeyTmr();
        }
      }else{
        repeatSpeed=REPEAT_SPEED_RESET;
        keyPressed=false;
        keyRepeating=false;
      }
      if(keyRepeating){
        if(keyTmrCnt>=repeatSpeed){
          resetKeyTmr();
          repeatSpeed=REPEAT_SPEED_REPEAT;
        }else{
          butUp=butDown=butLeft=butRight=butA=butB=false;
        }
      }
      if(butLeft){
        // Allow sliding piece when it cannot move down any more
        clearPiece(cursorX,cursorY,currentRotation);
        if(!checkCanMove(cursorX,cursorY+1,currentRotation)){
          resetTmr();
        }
        movePiece(cursorX-1,cursorY,currentRotation);
      }
      if(butRight){
        // Allow sliding piece when it cannot move down any more
        clearPiece(cursorX,cursorY,currentRotation);
        if(!checkCanMove(cursorX,cursorY+1,currentRotation)){
          resetTmr();
        }
        movePiece(cursorX+1,cursorY,currentRotation);
      }
      if(butDown){
        movePiece(cursorX,cursorY+1,currentRotation);
      }
      if(butA){
        movePiece(cursorX,cursorY,currentRotation-1);
      }
      if(butB){
        movePiece(cursorX,cursorY,currentRotation+1);
      }
      if(butUp){
        int16_t cursorYStart=cursorY,cursorYEnd=0;
        uint8_t currentPieceStart=currentPiece;
        clearGhostPiece();
        while(movePiece(cursorX,cursorY+1,currentRotation)){
          --cursorY;
          drawPiece();
          ++cursorY;
          drawGrid();
          delay(5);
        }
        cursorYEnd=cursorY;
        cursorY=cursorYStart;
        while(cursorY<cursorYEnd){
          clearPiece(cursorX,cursorY,currentRotation);
          ++cursorY;
          drawGrid();
          delay(5);
        }
        drawPiece();
        tmrCnt=gameSpeed+1;
      }
      
      if(tmrCnt>gameSpeed){
        resetTmr();
        if(!movePiece(cursorX,cursorY+1,currentRotation)){
          if(anyRowComplete()){
            gameState=GAMESTATE_CLEARING;
            IY=0;
            IZ=0;
            HL=0;
          }else{
            playEffect(2);
            addRows(rowsToAdd);
            rowsToAdd=0;
            getNextPiece();
            if(!checkCanMove(cursorX,cursorY,currentRotation)){
              if(is2Player)
                networkSendYouWin();
              playEffect(7);
              IY=0;
              IZ=0;
              HL=0;
              cursorX=0;
              gameState=(is2Player?GAMESTATE_WE_LOSE:GAMESTATE_GAME_OVER);
              if(is2Player){
                enemyScore*=2;
                showEnemyScore();
              }
              resetTmr();
            }else{
              movePiece(cursorX,cursorY,currentRotation);
            }
          }
        }
      }
      if(weWin){
        IY=0;
        IZ=0;
        HL=0;
        waitUp=true;
        score*=2; // Get double points for winning!
        showScore();
        gameState=GAMESTATE_WE_WIN;
      }
      break;
    case GAMESTATE_CLEARING:
    
      if(!doClearRows()){
        getNextPiece();
        movePiece(cursorX,cursorY,currentRotation);
        gameState=GAMESTATE_DROPPING;
        delay(100);
      }
    
      break;
    case GAMESTATE_WE_WIN:
      if( !doYouWinAnim() || (HL>10 && (butUp||butDown||butLeft||butRight||butA||butB) ) ){
        checkIfNewHighScore();
        drawGrid();
        playEffect(1);
        tft.fillScreen(0);
        setState(STATE_TITLE_SCREEN);
      }
      break;
    case GAMESTATE_WE_LOSE:
      if( !doYouLoseAnim() || (HL>10 && (butUp||butDown||butLeft||butRight||butA||butB) ) ){
        checkIfNewHighScore();
        drawGrid();
        playEffect(1);
        tft.fillScreen(0);
        setState(STATE_TITLE_SCREEN);
      }
      break;
    case GAMESTATE_GAME_OVER:
      if( !doGameOverAnim() || (IZ==2 && (butUp||butDown||butLeft||butRight||butA||butB)) ){
        checkIfNewHighScore();
        playEffect(1);
        tft.fillScreen(0);
        setState(STATE_TITLE_SCREEN);
      }
      break;
  }
  drawGrid();
}

void chooseOpponent(){
  if(IX==0){
    bool isOpening=openingWindow();
    starMinKeepOutX=starMinX;
    starMinKeepOutY=starMinY;
    starMaxKeepOutX=starMaxX;
    starMaxKeepOutY=starMaxY;
    if(!isOpening){
      drawTitle("CHOOSE ENEMY",90);
      if(webSocketActive){
        networkGetList();
        IX=1;
        waitUp=true;
      }else{
        IY=320;
        IX=99;
        waitUp=true;
      }
    }
  }else if(IX==1){
    if(gotList){
      listScrollPos=0;
      cursorY=0;
      IZ=0;
      IX=2;
      IY=0;
    }
    if(IY++>600){ // approx 10 seconds
      IY=320;
      IX=98;
    }
    if(butB){
      tft.fillScreen(0);
      setState(STATE_TITLE_SCREEN);
    }
  }else if(IX==2){  // got list!
    createOpponentList();
    uint8_t oNum=6;
    if(oNum>totalOpponents)
      oNum=totalOpponents;
    handleMenu(tmpStr,150+(10*(6-oNum)),oNum);
  }else if(IX==9){  // B pressed
    tft.fillScreen(0);
    setState(STATE_TITLE_SCREEN);
  }else if(IX==10){ // A pressed
    drawTitle("CHALLENGING!",90);
    challengedOpponentIX=cursorY+listScrollPos;
    networkSendChallenge(challengedOpponentIX);
    HL=0;
    while(opponentNameList[challengedOpponentIX][HL]!=0 && (HL<5)){
      opponentName[HL]=opponentNameList[challengedOpponentIX][HL++];
    }
    opponentName[HL]=0;
    HL=0;
    IZ=0;
    timeoutCnt=0;
    tft.fillRect(40,150,160,140,0);
    resetTmr();
    waitUp=true;
    IX=11;
  }else if(IX==11){
    img.createSprite(100,14);
    img.fillSprite(0);
    drawTxt("YOU HAVE",100,11,0,PURPLE_TO_WHITE,1);
    img.pushSprite(70,160);
    img.fillSprite(0);
    drawTxt("CHALLENGED",100,1,0,PURPLE_TO_WHITE,1);
    img.pushSprite(70,180);
    img.deleteSprite();

    HL=0;
    while(opponentName[HL]!=0 && HL<5)
      ++HL;
    img.createSprite(HL*10,28);
    img.fillSprite(0);
    if((tmrCnt%64)>32){
      ++timeoutCnt;
      drawTxt(opponentName,HL*10,1,0,RED_TO_BLUE,2);
    }
    img.pushSprite((240-(HL*10))/2,220);
    img.deleteSprite();
    
    if(challengeResponseType==RESPONSE_ACCEPT){
      networkSendStart();
      IX=94;
      IY=0;
    }else if(challengeResponseType==RESPONSE_REJECT){
      tft.fillRect(50,150,140,150,0);
      IX=96;
      IY=320;
    }else if(challengeResponseType==RESPONSE_BUSY){
      tft.fillRect(50,150,140,150,0);
      IX=97;
      IY=320;
    }
    
    if(butB){
      playEffect(1);
      tft.fillRect(0,0,240,320,0);
      setState(STATE_TITLE_SCREEN);
    }

    else if(timeoutCnt>=8*32*5){ // timeout waiting for response
      tft.fillRect(50,150,140,150,0);
      IX=95;
      IY=320;
    }

  }else if(IX==94){ // wait for server to echo the start message back, so we start at around the same time as the opponent...
    if(++IY>=100){
      tft.fillRect(50,150,140,150,0);
      IX=99;
//      tft.fillScreen(0);
//      is2Player=true;
//      setState(STATE_PLAYING);
    }
  }else if(IX==95){ // opponent response timeout
    scrollUpText((char *)responseTimedOutStr,17,4);
  }else if(IX==96){ // opponent rejected!
    scrollUpText((char *)opponentRejectedStr,18,4);
  }else if(IX==97){ // opponent busy...
    scrollUpText((char *)opponentBusyStr,16,4);
  }else if(IX==98){ // timeout getting list
    scrollUpText((char *)listTimedOutStr,17,4);
  }else if(IX==99){ // no network
    scrollUpText((char *)noNetworkStr,14,6,2);
  }
}

void checkIfNewHighScore(){
  uint8_t n,i,ix;
  for(uint8_t n=0;n<10;n++){
    if(score>hiScoreTable[n].score){
      for(i=9;i>n;i--){
        hiScoreTable[i].score=hiScoreTable[i-1].score;
        for(ix=0;ix<6;ix++){
          hiScoreTable[i].sig[ix]=hiScoreTable[i-1].sig[ix];
        }
      }
      hiScoreTable[n].score=score;
      for(ix=0;ix<6;ix++){
        hiScoreTable[n].sig[ix]=myName[ix];
      }
      saveConfig();
      break;
    }
  }
}
