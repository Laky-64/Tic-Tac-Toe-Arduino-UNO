#include <SPFD5408_Adafruit_GFX.h>
#include <SPFD5408_Adafruit_TFTLCD.h>
#include <SPFD5408_TouchScreen.h>

#define YP A1
#define XM A2
#define YM 7
#define XP 6
#define LCD_CS A3
#define LCD_CD A2
#define LCD_WR A1
#define LCD_RD A0
#define LCD_RESET A4

#define BLACK   0x0000
#define BLUE    0x001F
#define RED     0xF800
#define GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

#define MINPRESSURE 1
#define MAXPRESSURE 1000

#define TS_MINX 125
#define TS_MINY 85
#define TS_MAXX 965
#define TS_MAXY 905

TouchScreen ts = TouchScreen(XP, YP, XM, YM, 2000);
Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);

int X,Y;
boolean buttonEnabled=true;
int gameScreen = 1;
int moves = 1;
int winner = 0; 
int board[]={0,0,0,0,0,0,0,0,0};
extern uint8_t circle[];
extern uint8_t x_bitmap[];

void setup() {
  Serial.begin(9600);
  tft.reset();
  tft.begin(0x9325);
  tft.setRotation(3);
  drawStartScreen();
}

void loop() {
  TSPoint p = waitTouch();
  X = p.y; Y = p.x;
  if(gameScreen==3){
    buttonEnabled =true;
  }
  //Serial.print("X:"); Serial.print(X); Serial.print(",Y:"); Serial.println(Y);
  if(Y>60 & Y<260 & X>180 & X<220 & buttonEnabled){
    Serial.println("Button Pressed");
    resetGame();
    drawGameScreen();
    playGame();
  }
  delay(300);
}
TSPoint waitTouch() {
  TSPoint p;
  do {
    p = ts.getPoint(); 
    pinMode(XM, OUTPUT);
    pinMode(YP, OUTPUT);
  } while((p.z < MINPRESSURE )|| (p.z > MAXPRESSURE));
  p.x = map(p.x, TS_MINX, TS_MAXX, 0, 320);
  p.y = map(p.y, TS_MINY, TS_MAXY, 0, 240);
  return p;
}
void drawStartScreen(){
  tft.fillScreen(WHITE);
  //tft.drawRect(0,0,319,240,WHITE);
  tft.setCursor(30,100);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.print("Tic Tac Toe");
  tft.setCursor(80,30);
  tft.setTextColor(GREEN);
  tft.setTextSize(4);
  tft.print("Arduino");
  createStartButton();
}
void createStartButton(){
  tft.fillRect(60,180, 200, 40, RED);
  tft.drawRect(60,180,200,40,BLACK);
  tft.setCursor(72,188);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("Start Game");
}
void resetGame(){
  buttonEnabled = false;
  for(int i=0;i<9;i++){
   board[i]=0;
  }
  moves = 1;
  winner = 0;
  gameScreen=2;
}
void drawGameScreen(){
   tft.fillScreen(WHITE);
   //tft.drawRect(0,0,319,240,WHITE);
   drawVerticalLine(125);
   drawVerticalLine(195);
   drawHorizontalLine(80);
   drawHorizontalLine(150);
}
void drawHorizontalLine(int y){
  int i=0;
  for(i=0;i<7;i++){
    tft.drawLine(60,y+i,270,y+i,BLACK);
  }
}
void drawVerticalLine(int x){
  int i=0;
  for(i=0;i<7;i++){
    tft.drawLine(x+i,20,x+i,220,BLACK);
  }
}
void drawGameOverScreen(){
  tft.fillScreen(WHITE);
  //Draw frame
  //tft.drawRect(0,0,319,240,WHITE);
  //Print "Game Over" Text
  tft.setCursor(50,30);
  tft.setTextColor(BLACK);
  tft.setTextSize(4);
  tft.print("GAME OVER");
 if(winner == 0){
  //Print "DRAW!" text 
  tft.setCursor(110,100);
  tft.setTextColor(YELLOW);
  tft.setTextSize(4);
  tft.print("DRAW");
 }
 if(winner == 1){
  //Print "HUMAN WINS!" text 
  tft.setCursor(60,100);
  tft.setTextColor(BLUE);
  tft.setTextSize(4);
  tft.print("YOU WINS");
 }
 if(winner == 2){
  //Print "CPU WINS!" text 
  tft.setCursor(60,100);
  tft.setTextColor(RED);
  tft.setTextSize(4);
  tft.print("CPU WINS");
 }
 createPlayAgainButton();
 pinMode(XM, OUTPUT);
 pinMode(YP, OUTPUT); 
}

void createPlayAgainButton(){
   //Create Red Button
  tft.fillRect(60,180, 200, 40, RED);
  tft.drawRect(60,180,200,40,BLACK);
  tft.setCursor(72,188);
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.print("Play Again");
}
void playGame(){
  do{
    if(moves%2==1)
    {
     arduinoMove();
     printBoard();
     checkWinner();
    }else
    {
      playerMove(); 
      printBoard();
      checkWinner();  
    }
    moves++;
  }while (winner==0 && moves<10); 
  if(winner == 1){
    Serial.println("YOU WINS");
    delay(2000);
    gameScreen=3;
    drawGameOverScreen();
  }else if(winner ==2){
     Serial.println("CPU WINS");
     delay(2000);
     gameScreen=3;
     drawGameOverScreen();
  }else{
    Serial.println("DRAW");
    delay(2000);
    gameScreen=3;
    drawGameOverScreen();
  }
}

void playerMove(){
  pinMode(XM, OUTPUT);
  pinMode(YP, OUTPUT);
  TSPoint p;
  boolean validMove = false;
  do{    
    p = ts.getPoint();  //Get touch point  
    if (p.z > ts.pressureThreshhold){
      p.x = map(p.x, TS_MAXX, TS_MINX, 0, 320);
      p.y = map(p.y, TS_MAXY, TS_MINY, 0, 240);
      p.x=320-p.x;
      p.y=240-p.y;
      if((p.x<115)&& (p.y>=150)) //6
      {
        if(board[6]==0)
        {
          Serial.println("\nPlayer Move: 6");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[6]=1;
          drawPlayerMove(6);  
          validMove=true;
        }
      }
       else if((p.x>0 && p.x<115)&& (p.y<150 && p.y>80)) //3
      {
       
        if(board[3]==0)
        {
         Serial.println("\nPlayer Move: 3");
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[3]=1;
          drawPlayerMove(3);  
          validMove=true;
        }
      }
       else if((p.x<125)&& (p.y<80)) //0
      {
        if(board[0]==0)
        {
          Serial.println("\nPlayer Move: 0");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[0]=1;
          drawPlayerMove(0); 
          validMove=true; 
        }
      }

    else if((p.x>125 && p.x<=195)&& (p.y<80)) //1
      {
        if(board[1]==0)
        {
          Serial.println("\nPlayer Move: 1");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[1]=1;
          drawPlayerMove(1);  
          validMove=true;
        }
      }

       else if((p.x>195)&& (p.y<80)) //2
      {
        if(board[2]==0)
        {
          Serial.println("\nPlayer Move: 2");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[2]=1;
          drawPlayerMove(2);  
          validMove=true;
        }
      }

      else if((p.x>125 && p.x<=195)&& (p.y<150 && p.y>80)) //4
      {
        if(board[4]==0)
        {
          Serial.println("\nPlayer Move: 4");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[4]=1;
          drawPlayerMove(4);  
          validMove=true;
        }
      }

       else if((p.x>195)&& (p.y<150 && p.y>80)) //5
      {
        if(board[5]==0)
        {
          Serial.println("\nPlayer Move: 5");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[5]=1;
          drawPlayerMove(5);  
          validMove=true;
        }
      }

        else if((p.x>125 && p.x<=195)&& (p.y>150)) //7
      {
        if(board[7]==0)
        {
          Serial.println("\nPlayer Move: 7");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[7]=1;
          drawPlayerMove(7);  
          validMove=true;
        }
      }

       else if((p.x>195)&& (p.y>150)) //8
      {
        if(board[8]==0)
        {
          Serial.println("\nPlayer Move: 8");          
          pinMode(XM, OUTPUT);
          pinMode(YP, OUTPUT);
          board[8]=1;
          drawPlayerMove(8);  
          validMove=true;
        }
      }
      
    }
   }while(validMove==false); 
}
void printBoard(){
  int i=0;
  Serial.print("Board: [");
  for(i=0;i<9;i++)
  {
    Serial.print(board[i]);
    Serial.print(",");
  }
  Serial.print("]");
}
int cid = 0;
int checkOpponent(int cnew){
  cid = cnew;
  Serial.print("\nChecking: ");Serial.print(cid);
  if(check(board[0])+check(board[1])+check(board[2])==2 && findoperand(0,1)!=100){
     return findoperand(0,1); //H1
  }else if(check(board[3])+check(board[4])+check(board[5])==2 && findoperand(3,1)!=100){
     return findoperand(3,1); //H2
  }else if(check(board[6])+check(board[7])+check(board[8])==2 && findoperand(6,1)!=100){
     return findoperand(6,1); //H3
  }else if(check(board[0])+check(board[3])+check(board[6])==2 && findoperand(0,3)!=100){
     return findoperand(0,3); //V1
  }else if(check(board[1])+check(board[4])+check(board[7])==2 && findoperand(1,3)!=100){
     return findoperand(1,3); //V2
  }else if(check(board[2])+check(board[5])+check(board[8])==2 && findoperand(2,3)!=100){
     return findoperand(2,3); //V3
  }else if(check(board[2])+check(board[4])+check(board[6])==2 && findoperand(2,2)!=100){
     return findoperand(2,2); //D1
  }else if(check(board[0])+check(board[4])+check(board[8])==2 && findoperand(0,4)!=100){
     return findoperand(0,4); //D2
  }else{
     if(cid==2){
      return checkOpponent(1);
     }else{
      return 100;
     }
  }
}
int check(int i){
  if(i==cid){
    return 1;
  }else{
    return 0;
  }
}
int findoperand(int startf,int inc){
  int retnum=100;
  int addr=inc*2;
  for(int i=startf;i<=startf+addr;i+=inc){
    if(board[i]==0){
      retnum=i;
    }
  }
  Serial.print("\nFinded :"); Serial.println(retnum);
  return retnum;
}
void arduinoMove(){
  int b = 0;
  int counter =0;
  int movesPlayed = 0;
  int firstMoves[]={0,2,6,8};
  for(counter=0;counter<4;counter++){
    if(board[firstMoves[counter]]!=0){
      movesPlayed++;
    }
  }  
  do{
    if(moves<=2){
      int randomMove =random(4); 
      int c=firstMoves[randomMove];
      if (board[c]==0){  
        delay(1000);
        board[c]=2;
        Serial.print("\nArduino Move:");
        Serial.print(firstMoves[randomMove]);
        Serial.println();
        drawCpuMove(firstMoves[randomMove]);
        b=1;
      }    
    }else{
      int nextMove = checkOpponent(2);
      if(nextMove == 100){  
        if(movesPlayed == 4){
           int randomMove =random(9); 
           if(board[randomMove]==0){  
             delay(1000);
             board[randomMove]=2;
             Serial.print("\nArduino Move:");
             Serial.print(randomMove);
             Serial.println();
             drawCpuMove(randomMove);
             b=1;
          }   
        }else{
          int randomMove =random(4); 
          int c=firstMoves[randomMove];
          if(board[c]==0){  
            delay(1000);
            board[c]=2;
            Serial.print("\nArduino Move:");
            Serial.print(firstMoves[randomMove]);
            Serial.println();
            drawCpuMove(firstMoves[randomMove]);
            b=1;
          }   
        }
     }else{
       delay(1000);
       board[nextMove]=2;
       drawCpuMove(nextMove);
       b=1;
    }
   }
  }
  while (b<1);
}
void drawCircle(int x, int y)
{
  drawBitmap(x,y,circle,65,65,RED);
}

void drawX(int x, int y)
{
  drawBitmap(x,y,x_bitmap,65,65,BLUE);
}

void drawCpuMove(int move)
{
  switch(move)
  {
    case 0: drawCircle(55,15);  break;
    case 1: drawCircle(130,15); break;
    case 2: drawCircle(205,15); break;
    case 3: drawCircle(55,85);  break;
    case 4: drawCircle(130,85); break;
    case 5: drawCircle(205,85); break;
    case 6: drawCircle(55,155); break;
    case 7: drawCircle(130,155);break;
    case 8: drawCircle(205,155);break;
  }
}

void drawPlayerMove(int move)
{
  switch(move)
  {
    case 0: drawX(55,15);  break;
    case 1: drawX(130,15); break;
    case 2: drawX(205,15); break;
    case 3: drawX(55,85);  break;
    case 4: drawX(130,85); break;
    case 5: drawX(205,85); break;
    case 6: drawX(55,155); break;
    case 7: drawX(130,155);break;
    case 8: drawX(205,155);break;
  }
}

void checkWinner() 
// checks board to see if there is a winner
// places result in the global variable 'winner'
{
  int qq=0;
  // noughts win?
  if (board[0]==1 && board[1]==1 && board[2]==1) { 
    winner=1; 
  }
  if (board[3]==1 && board[4]==1 && board[5]==1) { 
    winner=1; 
  }
  if (board[6]==1 && board[7]==1 && board[8]==1) { 
    winner=1; 
  }  
  if (board[0]==1 && board[3]==1 && board[6]==1) { 
    winner=1; 
  }
  if (board[1]==1 && board[4]==1 && board[7]==1) { 
    winner=1; 
  }
  if (board[2]==1 && board[5]==1 && board[8]==1) { 
    winner=1; 
  }  
  if (board[0]==1 && board[4]==1 && board[8]==1) { 
    winner=1; 
  }
  if (board[2]==1 && board[4]==1 && board[6]==1) { 
    winner=1; 
  }
  // crosses win?
  if (board[0]==2 && board[1]==2 && board[2]==2) { 
    winner=2; 
  }
  if (board[3]==2 && board[4]==2 && board[5]==2) { 
    winner=2; 
  }
  if (board[6]==2 && board[7]==2 && board[8]==2) { 
    winner=2; 
  }  
  if (board[0]==2 && board[3]==2 && board[6]==2) { 
    winner=2; 
  }
  if (board[1]==2 && board[4]==2 && board[7]==2) { 
    winner=2; 
  }
  if (board[2]==2 && board[5]==2 && board[8]==2) { 
    winner=2; 
  }  
  if (board[0]==2 && board[4]==2 && board[8]==2) { 
    winner=2; 
  }
  if (board[2]==2 && board[4]==2 && board[6]==2) { 
    winner=2; 
  }
 
}
void drawBitmap(int16_t x, int16_t y,
 const uint8_t *bitmap, int16_t w, int16_t h, uint16_t color) {

  int16_t i, j, byteWidth = (w + 7) / 8;
  uint8_t byte;

  for(j=0; j<h; j++) {
    for(i=0; i<w; i++) {
      if(i & 7) byte <<= 1;
      else      byte   = pgm_read_byte(bitmap + j * byteWidth + i / 8);
      if(byte & 0x80) tft.drawPixel(x+i, y+j, color);
    }
  }
}
