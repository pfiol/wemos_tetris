bool  butUp=false,
      butDown=false,
      butLeft=false,
      butRight=false,
      butA=false,
      butB=false,
      butX=false, // only active on 8 button variant
      butY=false; // only active on 8 button variant

uint8_t sx[NSTARS] = {};
uint8_t sy[NSTARS] = {};
uint8_t sz[NSTARS] = {};

uint8_t za, zb, zc, zx;
os_timer_t timer;

char webStr[TMP_STR_LEN]="";
char payload[TMP_STR_LEN]="";
uint8_t challengedOpponentIX=0;
char tmpStr[TMP_STR_LEN]="";
char inputStr[64]="";

char rightArrow[]={94+32,95+32,0};
char leftArrow[]={97+32,96+32,0};
char return1[]={97+32,98+32,0};
char Aa[]="Aa";

bool waitUp=false;
uint8_t soundType=SOUND_ALL;
uint16_t totalMenuItems=0;
uint16_t charWidth=10;

char myName[6]="Pedro";
char wifiName[65]="TP-Link_3700";
char wifiPassword[65]="18964196";
int16_t totalAPs=0;
int16_t listWidth=0,listScrollPos=0,listMaxScrollPos=0;
int16_t timeoutCnt=0;

// 10x20 grid on screen, with 2 rows above screen to allow shapes to spawn
uint8_t grid[GRID_HEIGHT][GRID_WIDTH];
uint8_t oldGrid[GRID_HEIGHT][GRID_WIDTH];

char      opponentID[32]="";  // PRIVATE ID!
char      opponentName[16]="XXXXX";
boolean   gotList=false;
char      opponentNameList[MAX_OPPONENTS][6];
char      opponentIDList[MAX_OPPONENTS][10];  // PUBLIC IDs!
uint8_t   totalOpponents=0;
uint8_t   challengeResponseType=RESPONSE_NONE;

uint8_t   rowsToAdd=0;
bool      is2Player=false;
bool      weWin=false;
bool      showingGhostPiece=true;
uint8_t   currentPiece=0;
uint8_t   nextPiece=0;
int8_t    currentRotation=0;
uint32_t  score=0;
uint32_t  enemyScore=0;
uint8_t   level=0;
char      scoreTxt[]="00000";
uint16_t  gameSpeed=0; // delay between automated drop movements - higher = slower
uint8_t   gameState=GAMESTATE_DROPPING;
uint8_t   repeatSpeed=0;
uint8_t   completedLines=0;
int16_t   bouncy[21];
int16_t   bouncyDir[21];

uint16_t  starMinX=100,starMaxX=140,starMinY=140,starMaxY=179;
uint16_t  starMinKeepOutX=100,starMaxKeepOutX=140,starMinKeepOutY=140,starMaxKeepOutY=179;
uint16_t  starMinXTarget=100,starMaxXTarget=140,starMinYTarget=140,starMaxYTarget=179;
uint16_t  winXSpeed=0,winYSpeed=0,borderCol=0;

uint8_t state=STATE_INTRO;
int16_t IX=0,IY=0,IZ=0,HL=0;
int16_t scrollX=200,scrollColOff=0;
int8_t  cursorY=0,cursorX=0;
int8_t  cursorXOff=0;
bool    cursorOn=false;

uint16_t  keyboardY=320;
uint8_t   keyboardCase=0;
uint8_t   keyboardState=KS_IDLE;
uint8_t   keyboardMaxInputChars=0;
char      keyboardActiveChar=0;
uint16_t  keyboardExitType=0;
bool      keyRepeating=false;
bool      keyPressed=false;
uint16_t  inputStrLen=0;
uint16_t  titleStrLen=0;
uint16_t  titleStrWidth=10;

volatile uint16_t tmrCnt=0;
volatile bool     tmrReset=false;
volatile uint16_t keyTmrCnt=0;
volatile bool     keyTmrReset=false;
volatile bool     connectCheckTmrReset=true;
volatile uint16_t connectCheckTmr=0;
volatile uint16_t  pingTmr=0;
volatile  bool    pingRequest=false;

// input keyboard \1 = DEL, \2 + \3 = ENTER (double height) \4 = CAPS \5 = £
const char keyboardMap[2][56]={
  "1234567890-=\n"   "qwertyuiop[]\n" "asdfghjkl;'#\n" "\\zxcvbnm,./\n",
  "!\"#$%^&*()_+\n"  "QWERTYUIOP{}\n" "ASDFGHJKL:@~\n" "|ZXCVBNM<>?\n"
};

// Definition of the 6 game tetrii (+ 1 blank) in each of their 4 angles of rotation. 
// Each tetrii comprises 4 blocks, each having x,y coordinates.
const int8_t pieceDef[TOTAL_SHAPES][TOTAL_ROTATIONS][TOTAL_COORDINATES][2]={
  { 

    // Black
    // <blank>
    {{0,0},{0,0},{0,0},{0,0}},
    {{0,0},{0,0},{0,0},{0,0}},
    {{0,0},{0,0},{0,0},{0,0}},
    {{0,0},{0,0},{0,0},{0,0}},
  },{
    // Cyan
    // [][][][]
    {{-2,0},{-1,0},{0,0},{1,0}},
    {{0,-1},{0,0},{0,1},{0,2}},
    {{-1,0},{0,0},{1,0},{2,0}},
    {{0,-2},{0,-1},{0,0},{0,1}},
    
  },{
    // Blue
    // []
    // [][][]
    {{-1,-1},{-1,0},{0,0},{1,0}},
    {{1,-1},{0,-1},{0,0},{0,1}},
    {{-1,0},{0,0},{1,0},{1,1}},
    {{-1,1},{0,1,},{0,0},{0,-1}},
  },{
    // Orange
    //     []
    // [][][]
    {{-1,0},{0,0},{1,0},{1,-1}},
    {{0,-1},{0,0},{0,1},{1,1}},
    {{-1,1},{-1,0},{0,0},{1,0}},
    {{-1,-1},{0,-1},{0,0},{0,1}},
  },{
    // Yellow
    // [][]
    // [][]
    {{0,-1},{1,-1},{0,0},{1,0}},
    {{0,-1},{1,-1},{0,0},{1,0}},
    {{0,-1},{1,-1},{0,0},{1,0}},
    {{0,-1},{1,-1},{0,0},{1,0}},
  },{
    // Green
    //   [][]
    // [][]
    {{-1,0},{0,0},{0,-1},{1,-1}},
    {{0,-1},{0,0},{1,0},{1,1}},
    {{0,0},{1,0},{0,1},{-1,1}},
    {{-1,-1},{-1,0},{0,0},{0,1}},
  },{
    // Magenta
    //   []
    // [][][]
    {{-1,0},{0,0},{1,0},{0,-1}},
    {{0,-1},{0,0},{0,1},{1,0}},
    {{-1,0},{0,0},{1,0},{0,1}},
    {{-1,0},{0,0},{0,-1},{0,1}},
  },{
    // Red
    // [][]
    //   [][]
    {{-1,-1},{0,-1},{0,0},{1,0}},
    {{0,0},{0,1},{1,0},{1,-1}},
    {{-1,0},{0,0},{0,1},{1,1}},
    {{-1,0},{-1,1},{0,0},{0,-1}},
  }
};

char enterNameStr[]="Enter Your Name!!";
char enterWiFiPasswordStr[]="Enter WiFi Pass!!";
const char mainMenuStr[]="1 PLAYER\n2 PLAYERS\nHIGH SCORES\nSETTINGS\nHELP";
const char soundMenuStr[][38]={
  "MY NAME!\nWIFI SETUP\nSOUND [OFF]\nABOUT",
  "MY NAME!\nWIFI SETUP\nSOUND [FX ]\nABOUT",
  "MY NAME!\nWIFI SETUP\nSOUND [ALL]\nABOUT",
};

const uint16_t charRowCol16[]={
  0xF8,0x80E8,0xD9,0x80C9,0xBA,0x80AA,0x9B,0x809B,0x9C,0x809C,0x9D,0x809D,0x9E,0x809E,0x9F,0x809F,                  // red to yellow
  0xE107,0x6307,0x507,0x8706,0x906,0x8B05,0xD05,0x8F04,0x9104,0x9304,0x9504,0x9804,0x9904,0x9B04,0x9D04,0x9F04,     // green to blue
  0x1FF8,0x9FF8,0x1FF9,0x9FF9,0x1FFA,0x9FFA,0x1FFB,0x9FFB,0x1FFC,0x9FFC,0x1FFD,0x9FFD,0x1FFE,0x9FFE,0x1FFF,0x9FFF,  // purple to white
  0xBFFF,0xBFDE,0xBFBD,0xBF9C,0xBF7B,0xBF5A,0xBF39,0xBF18,0x1F00,0x1F00,0x1F00,0x1F00,0x1F00,0x1F00,0x1F00,0x1F00,  // white to blue
  0x1F8,0x3E8,0x5D8,0x7C8,0x9B8,0xBA8,0xD98,0xF88,0x1178,0x1368,0x1558,0x1748,0x1938,0x1B28,0x1D18,0x1F08,          // red to blue
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x20,0x40,0x60,0x80,0xA0,0xC0,0xE0,0xF8,                                          // black to red
  0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0,                                                  // black to black
};

struct hiScore {
  uint32_t  score;
  char      sig[6];
};

struct hiScore hiScoreTable[10]={
  {1000,"SCOTT"},
  {900, "GLYN"},
  {800, "DAN"},
  {700, "MATT"},
  {600, "AAAAA"},
  {500, "BBBBB"},
  {400, "CCCCC"},
  {300, "DDDDD"},
  {200, "EEEEE"},
  {100, "FFFFF"},
};

const int8_t rssiThreshold[]={-83,-79,-68,-62,-50};

char aboutStr[6][16]={
  "Hi! This is a",
  "little ESP8266",
  "based games",
  "console!",
  "Use Arduino IDE",
  "to write games!"
};

char challengedStr[4][14]={
  "YOU HAVE BEEN",
  "CHALLENGED BY",
  "A TO ACCEPT",
  "B TO WIMP OUT",
};

char challengedWimpStr[3][13]={
  "WE ARE VERY",
  "DISAPPOINTED",
  "  WITH YOU",
};

char noNetworkStr[6][14]={
  "YOU ARE NOT",
  "CONNECTED TO",
  "THE SERVER.",
  "PLEASE WAIT",
  "OR CHECK WIFI",
  "SETTINGS!",  
};

char listTimedOutStr[4][17]={
  "SORRY, COULD NOT",
  "GET THE LIST OF",
  "ENEMIES! PLEASE",
  "TRY AGAIN...",
};

char responseTimedOutStr[4][17]={
  "SORRY, THE ENEMY",
  "DID NOT RESPOND",
  "IN TIME! PLEASE",
  "TRY AGAIN...",
};

char opponentRejectedStr[4][18]={
  "THE OPPONENT HAS",
  "CRAVENLY REJECTED",
  "YOUR CHALLENGE!",
  "PITIFUL!"
};

char opponentBusyStr[4][16]={
  "THE OPPONENT IS",
  "CURRENTLY BUSY",
  "WITH ANOTHER",
  "CHALLENGE!"
};

char challengedHeroStr[11]="GET READY!";
char timedOutStr[11]      ="TIMED OUT!";

char fakeAPList[10][24]={
  "Pretty Fly for a WiFi",
  "My Loverly WiFi!",
  "Some WiFi Network",
  "Virgin Media 1",
  "Some BT Network",
  "Costa Coffee!!",
  "Yet another AP",
  "Scott's Mobile Phone",
  "My Router",
  "The last Access Point"
};

char helpStr[49][21]={
  "Use Settings to",
  "set up WiFi and",
  "also set your name",
  "and also configure",
  "sound.",
  "",
  "The icon in the top",
  "left will show green",
  "when connected. It",
  "will disappear after",
  "a few seconds. The",
  "icon is not shown",
  "while in game.",
  "",
  "You can challenge",
  "players anywhere",
  "in the world if",
  "they are online!",
  "",
  "The high score list",
  "is stored on this",
  "device, so it can",
  "be viewed while",
  "offline. It shows",
  "scores from both",
  "1 and 2 player",
  "games.",
  "",
  "You can earn much",
  "higher scores in 2",
  "player mode, as it",
  "is more challenging.",
  "",
  "Pro-tip: you can use",
  "the UP button to",
  "perform a hard-drop",
  "which will help in",
  "2 player games!",
  "", 
  "This device was",
  "designed, built and",
  "programmed by Solder",
  "",
  "For support, speak",
  "to Scott or Glyn!",
  "",
  "Have fun!",
  "",
  "(A or B to exit)"
};

uint8_t networkStatus=NETWORK_OFF;
uint8_t lastNetworkStatus=NETWORK_UNDEFINED;
bool    webSocketActive=false;
int16_t wifiLogoYPos=-16;
int16_t wifiLogoTmr=0;
int16_t wifiLogoTmr2=0;
