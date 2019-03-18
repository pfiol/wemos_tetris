void readButtonPins();
void checkButtons();
uint16_t RGBTo16(uint16_t r,uint16_t g,uint16_t b);
void drawTxt(char *str, uint16_t w, uint16_t offX, uint16_t offY, uint8_t colorIX, uint8_t vScale);
void drawTxt(char *str, uint16_t w, uint16_t offX, uint8_t colorIX);
uint8_t __attribute__((always_inline)) rng();
void updateStars();
void showButtons();
void showIntro();
void makeColors();
void setState(uint16_t s);
void openWindow(uint16_t x,uint16_t y,uint16_t x2,uint16_t y2,uint16_t speed,uint8_t r,uint8_t g,uint8_t b);
bool openingWindow();
bool drawHiScores();
uint16_t getCol(uint8_t r,uint8_t g,uint8_t b);
bool revealTitle();
void copySprite(uint16_t *src, uint16_t *dest, int16_t words);
void makeList(char * s,uint16_t col);
void drawTitle(char *s,uint16_t);
void drawTitle(char *s,uint16_t,int16_t);
void drawTitleBox(uint16_t);
void zoomWindow(uint16_t x1,uint16_t x2,uint16_t y1,uint16_t y2);
void clearCursors(uint16_t y);
void handleMenu(const char *s, uint16_t y, uint8_t);

void timerCallback(void *pArg);
void resetTmr();
void resetKeyTmr();
void resetConnectCheckTmr();
void exitSettings();

void writeConfig(char *s,uint16_t ix,uint16_t len);
void readConfig(char *s,uint16_t ix,uint16_t len);
void loadConfig();
void saveConfig();

void drawSprite(const uint8_t * b, int16_t x, int16_t y,uint16_t col);
void showNetworkStatus();
void scrollUpText(char *s, uint8_t strLen,uint8_t strNum,uint8_t breakAfterLine);
void scrollUpText(char *s, uint8_t strLen,uint8_t strNum);

void setSpiTo(uint8_t);

// Keyboard functions
bool revealKeyboard();
bool hideKeyboard();
void showKeyboard(uint16_t startX,uint16_t startY,uint8_t charCase);
void drawInputTxt();
uint16_t doKeyboardInput(uint16_t maxChars);
void clearMenuSprites();

// Network functions
void getWiFiAPs();
void showWiFiAPs();
void createOpponentList();
void createWiFiList();
void networkDisconnect();
void checkConnection();
void socketSend(char *s);
void checkWebSocket();
void sendMyNameToServer();
void sendPrivateIDToServer();
void networkAccept();
void networkReject();
void networkSendClearedRows(uint8_t);
void networkSendYouWin();
void networkGetList();
void networkSendChallenge(uint8_t);


#if NEW_NETWORK
uint8_t getCmdData(char *p,char *d);

#else
uint8_t getCmdData(uint8_t *p,char *d);
void webSocketEvent(WStype_t type, uint8_t * payload, size_t length);
#endif

// Game functions
void resetGrid();
void drawGrid();
void clearRows(uint8_t);
void clearRow(uint8_t row);
bool isRowComplete(uint8_t r);
bool anyRowComplete();
bool doClearRows();
void drawTile(uint8_t x,uint8_t y);
bool doYouWinAnim();
bool doYouLoseAnim();
bool doGameOverAnim();
void setupGameScreen();
void drawFramedBox(uint16_t x,uint16_t y,uint16_t w,uint16_t h);
void drawColorSprite(const uint16_t * b, int16_t x, int16_t y);
void drawColorSprite(const uint16_t * b, int16_t x, int16_t y, int16_t h);
void drawGhostColorSprite(const uint16_t * b, int16_t x, int16_t y, uint8_t maskSides);
void drawBigNum(uint16_t);
void showScore();
void showEnemyScore();
void showLevel();
void getNextPiece();
void showStatePlaying();
void clearGhostPiece();
void clearPiece(int8_t x,int8_t y,uint8_t currentRotation);
bool checkCanMove(int8_t x,int8_t y,uint8_t r);
void drawGhostPiece();
bool movePiece(int8_t x,int8_t y,int8_t currentRotation);
void inGame();
void chooseOpponent();
void checkIfNewHighScore();

// Sound functions
void setSong(int8_t s);
void playEffect(int8_t s);


// screens

void showChallenged();
