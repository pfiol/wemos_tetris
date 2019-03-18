void getWiFiAPs(){
  networkDisconnect();
  delay(200);
  WiFi.scanNetworks(true,true);
}

void showWiFiAPs(){
  for (int i = 0; i < totalAPs; ++i) {
    Serial.print(i + 1);
    Serial.print(": ");
    Serial.print(WiFi.SSID(i));
    Serial.print(" (");
    int16_t rssi=WiFi.RSSI(i);
    Serial.print(rssi,DEC);
    Serial.print(")");
    Serial.println((WiFi.encryptionType(i) == ENC_TYPE_NONE) ? " " : "*");
  }
}
void createOpponentList(){
  uint16_t n,i,ix=0;
  memset(tmpStr,0,TMP_STR_LEN);
  for(n=0;n<totalOpponents;n++){
    i=0;
    while( (opponentNameList[n][i]!=0) && (i<5) && (ix<TMP_STR_LEN-1) )
      tmpStr[ix++]=opponentNameList[n][i++];
    tmpStr[ix++]='\n';

  }
  tmpStr[ix-1]=0;
}

void createWiFiList(){
  uint16_t n,i,ix=0;
  memset(tmpStr,0,TMP_STR_LEN);
  char s[250]="";
  // For FAKE list
  //totalAPs=10;
  listMaxScrollPos=totalAPs-4;
  for(n=0;(n<totalAPs) && (n<30);n++){
    //char *s=fakeAPList[n];
    WiFi.SSID(n).toCharArray(s,250);
    i=0;
    while( (s[i]!=0) && (i<15) && (ix<TMP_STR_LEN-1) )
      tmpStr[ix++]=s[i++];
    tmpStr[ix++]='\n';
  }
  tmpStr[ix-1]=0;
}

void networkDisconnect(){
  networkStatus=NETWORK_OFF;
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
}

void checkConnection(){
  if(networkStatus==NETWORK_OFF)
    return;
  if(networkStatus==NETWORK_CONNECTED){
    uint16_t s=WiFi.status();
    if((s==WL_CONNECTION_LOST || s==WL_DISCONNECTED)){
      Serial.println("Connection lost");
      networkStatus=NETWORK_OFFLINE;
    }
    return;
  }
  if(networkStatus==NETWORK_CONNECTING){
    uint16_t s=WiFi.status();
    if((s==WL_CONNECTED)){
      networkStatus=NETWORK_CONNECTED;
      Serial.print("Connected.\nSignal strength: ");
      Serial.println(WiFi.RSSI());
    }else if(s==WL_CONNECT_FAILED){
      Serial.println("Connect failed");
      networkStatus=NETWORK_OFF;
    }
    return;
  }
  if(wifiPassword[0]==0 && wifiName[0]==0){
    networkStatus=NETWORK_OFF;
    return;
  }
  Serial.print("Connecting to ");
  Serial.print(wifiName);
  Serial.print(" using password ");
  Serial.println(wifiPassword);
  WiFi.begin(wifiName, wifiPassword);
  networkStatus=NETWORK_CONNECTING;
}

void socketSend(char *s){
  while(webSocket.sending()){ // purge buffer
  }
  webSocket.send(s);
}

void checkWebSocket(){
  if(!webSocketActive && networkStatus==NETWORK_CONNECTED){
    webSocket.connect(GAME_SERVER_URL, "/", GAME_SERVER_PORT);
    webSocketActive=true;
  }else if(webSocketActive && networkStatus!=NETWORK_CONNECTED){
    webSocketActive=false;
  }

  if(webSocketActive && !webSocket.isConnected()){
    Serial.printf("[WSc] Disconnected!\n");
    webSocketActive=false;
  }else if(!webSocketActive && webSocket.isConnected()){
    webSocketActive=true;
    Serial.printf("[WSc] Connected to url: %s\n", payload);
    sendMyNameToServer();
    sendPrivateIDToServer();
  }

  if(webSocketActive){
    webSocket.sending();
    if(pingRequest){
      socketSend("PING|");
      pingRequest=false;
    }
  }

}

void sendMyNameToServer(){
  if(!webSocketActive)
    return;
  sprintf(webStr,"SET_NAME|%s",myName);
  socketSend(webStr);
}

void sendPrivateIDToServer(){
  if(!webSocketActive)
    return;
  // the chipID is the private ID...
  uint64_t chipid=ESP.getChipId();
  sprintf(webStr,"SET_PRIVATE_ID|%d:%d:%d:%d:%d:%d",chipid%0xFF,(chipid>>8)%0xFF,(chipid>>16)%0xFF,(chipid>>24)%0xFF,(chipid>>32)%0xFF,(chipid>>40)%0xFF);
  socketSend(webStr);
}

void networkAccept(){
  if(!webSocketActive)
    return;
  sprintf(webStr,"ACCEPT|%s",opponentID);
  socketSend(webStr);
}

void networkReject(){
  if(!webSocketActive)
    return;
  sprintf(webStr,"REJECT|%s",opponentID);
  socketSend(webStr);
}

void networkSendClearedRows(uint8_t n){
  if(!webSocketActive)
    return;
  sprintf(webStr,"ADD_ROW|%s|%d",opponentID,n);
  socketSend(webStr);
}


void networkSendYouWin(){
  if(!webSocketActive)
    return;
  sprintf(webStr,"YOU_WIN|%s",opponentID);
  socketSend(webStr);
}

void networkSendStart(){
  if(!webSocketActive)
    return;
  sprintf(webStr,"START|%s",opponentID);
  socketSend(webStr);
}

void networkGetList(){
  gotList=false;
  if(!webSocketActive)
    return;
  socketSend("LIST|");
}

void networkSendChallenge(uint8_t){
  challengeResponseType=RESPONSE_NONE;
  if(!webSocketActive)
    return;
  sprintf(webStr,"CHALLENGE|%s|",opponentIDList[challengedOpponentIX]);
  socketSend(webStr);
}

uint8_t getCmdData(char *p,char *d,uint8_t maxLen){
  d[0]=0;
  if(p[0]!='|')
    return 0;
  uint8_t ix=0;
  ++p;
  while(p[ix]!=0 && p[ix]!='"' && p[ix]!='|' && ix<maxLen){
    d[ix]=p[ix++];
  }
  d[ix]=0;
  return ix+1;
}


void handlePayload(char *payload){
  Serial.printf("Websocket received: %s\n", payload);
  char *p=payload+22;
  switch(p[0]){
    case 'A':
    case 'R':
    case 'I':
      if(p[0]=='A')
        challengeResponseType=RESPONSE_ACCEPT;
      else if(p[0]=='R')
        challengeResponseType=RESPONSE_REJECT;
      else
        challengeResponseType=RESPONSE_BUSY;
      ++p;
      p+=getCmdData(p,opponentID,30);
      break;
    case 'L': // List of opponents - data is | delimited, with name|id|name|id list
      {
        ++p;
/*
        char fakeList[]="|AAAAA|00001|BBBBB|00002|CCCCC|00003|DDDDD|00004|EEEEE|00005|FFFFF|00006|GGGGG|00007|HHHHH|00008";
        p=fakeList;
*/            
        uint8_t len=1;
        totalOpponents=0;
        while(len>0 && totalOpponents<MAX_OPPONENTS){
          len=getCmdData(p,opponentNameList[totalOpponents],6);
          if(len==0){
            for(uint8_t n=0;n<5;n++)
              opponentNameList[totalOpponents][n]='?';
            opponentNameList[totalOpponents][5]=0;
          }
          p+=len;
          len=getCmdData(p,opponentIDList[totalOpponents],9);
          p+=len;
          if(len>0){
            ++totalOpponents;
          }
        }
        gotList=true;
      }
      break;
      
    case 'C': // Challenge
      ++p;
      p+=getCmdData(p,opponentID,30);
      getCmdData(p,opponentName,11);
      // auto-send rejection as this player is busy playing
      if(state>STATE_PICK_PLAYER){
        sprintf(webStr,"INGAME|%s",opponentID);
        socketSend(webStr);
      }else{
        setState(STATE_CHALLENGED);
      }
      break;
      
    case 'S': // Start game!
      is2Player=true;
      setState(STATE_PLAYING);
      break;
    
    case '+': // Add row(s)
      if(state==STATE_PLAYING){
        ++p;
        p+=getCmdData(p,opponentID,30);
        ++p;
        rowsToAdd=(uint8_t)p[0]-'0';
        if(rowsToAdd>4)
          rowsToAdd=4;
        enemyScore+=(rowsToAdd*rowsToAdd*5*2);
      }
      break;

    case 'W': // We win!
      weWin=true;
      break;
      
    default:
      break;
  }
}
