// This sketch has been Refurbished by BUHOSOFT
// IMPORTANT: Adafruit_TFTLCD LIBRARY MUST BE SPECIFICALLY
// CONFIGURED FOR EITHER THE TFT SHIELD OR THE BREAKOUT BOARD.
// SEE RELEVANT COMMENTS IN Adafruit_TFTLCD.h FOR SETUP.
//#define DEBUG
#include <Adafruit_GFX.h>     // Core graphics library
#include <Adafruit_TFTLCD.h>  // Hardware-specific library
#include <SPI.h>              // SPI general
#include <SD.h>               // SD Card libraries

// The control pins for the LCD can be assigned to any digital or
// analog pins...but we'll use the analog pins as this allows us to
// double up the pins with the touch screen (see the TFT paint example).
#define LCD_CS A3 // Chip Select goes to Analog 3
#define LCD_CD A2 // Command/Data goes to Analog 2
#define LCD_WR A1 // LCD Write goes to Analog 1
#define LCD_RD A0 // LCD Read goes to Analog 0

#define LCD_RESET A4 // Can alternately just connect to Arduino's reset pin

// When using the BREAKOUT BOARD only, use these 8 data lines to the LCD:
// For the Arduino Uno, Duemilanove, Diecimila, etc.:
//   D0 connects to digital pin 8  (Notice these are
//   D1 connects to digital pin 9   NOT in order!)
//   D2 connects to digital pin 2
//   D3 connects to digital pin 3
//   D4 connects to digital pin 4
//   D5 connects to digital pin 5
//   D6 connects to digital pin 6
//   D7 connects to digital pin 7
// For the Arduino Mega, use digital pins 22 through 29
// (on the 2-row header at the end of the board).

// Assign human-readable names to some common 16-bit color values:
#define	BLACK   0x0000
#define	BLUE    0x001F
#define	RED     0xF800
#define	GREEN   0x07E0
#define CYAN    0x07FF
#define MAGENTA 0xF81F
#define YELLOW  0xFFE0
#define WHITE   0xFFFF

// Variables I added.
  const int hall = A5;
  const int chipSelect = 4;
  File dataFile;
  int preState=0;
  int rev = 0;
  int intTime;
  int rots = 0;
  long avgSpd = 0;
  long rotSpd = 0;
  long dist =0;
  int diameter = 0.000426261; //in miles, so already in miles/(time)
  int splits[] = [0,0,0,0,0];

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
// If using the shield, all control and data lines are fixed, and
// a simpler declaration can optionally be used:
// Adafruit_TFTLCD tft;

//void setup(void) {
void setup(){
  pinMode(SS, OUTPUT);

 #ifdef DEBUG
  Serial.begin(9600);
  Serial.println(F("TFT LCD test"));
  Serial.print("TFT size is ");
  Serial.print(tft.width());
  Serial.print("x");
  Serial.println(tft.height());
#endif // DEBUG
  tft.reset();

  uint16_t identifier = tft.readID();

if(identifier == 0x9325) {
#ifdef DEBUG
    Serial.println(F("Found ILI9325 LCD driver"));
  } else if(identifier == 0x9328) {

    Serial.println(F("Found ILI9328 LCD driver"));
  } else if(identifier == 0x7575) {

    Serial.println(F("Found HX8347G LCD driver"));
  } else if(identifier == 0x9341) {

    Serial.println(F("Found ILI9341 LCD driver"));
  } else if(identifier == 0x8357) {

    Serial.println(F("Found HX8357D LCD driver"));
#endif // DEBUG
    } else {
    #ifdef DEBUG
    Serial.print(F("Unknown LCD driver chip: "));
    Serial.println(identifier, HEX);
    Serial.print(F("I try use ILI9341 LCD driver "));
    Serial.println(F("If using the Adafruit 2.8\" TFT Arduino shield, the line:"));
    Serial.println(F("  #define USE_ADAFRUIT_SHIELD_PINOUT"));
    Serial.println(F("should appear in the library header (Adafruit_TFT.h)."));
    Serial.println(F("If using the breakout board, it should NOT be #defined!"));
    Serial.println(F("Also if using the breakout, double-check that all wiring"));
    Serial.println(F("matches the tutorial."));
    #endif // DEBUG
    identifier = 0x9341;
  }


  tft.begin(identifier);
#ifdef DEBUG
  Serial.println(F("Benchmark                Time (microseconds)"));

  Serial.print(F("Screen fill              "));
  Serial.println(testFillScreen());
  delay(500);

  Serial.print(F("Text                     "));
  Serial.println(testText());
  delay(3000);

  Serial.print(F("Lines                    "));
  Serial.println(testLines(CYAN));
  delay(500);

  Serial.print(F("Horiz/Vert Lines         "));
  Serial.println(testFastLines(RED, BLUE));
  delay(500);

  Serial.print(F("Rectangles (outline)     "));
  Serial.println(testRects(GREEN));
  delay(500);

  Serial.print(F("Rectangles (filled)      "));
  Serial.println(testFilledRects(YELLOW, MAGENTA));
  delay(500);

  Serial.print(F("Circles (filled)         "));
  Serial.println(testFilledCircles(10, MAGENTA));

  Serial.print(F("Circles (outline)        "));
  Serial.println(testCircles(10, WHITE));
  delay(500);

  Serial.print(F("Triangles (outline)      "));
  Serial.println(testTriangles());
  delay(500);

  Serial.print(F("Triangles (filled)       "));
  Serial.println(testFilledTriangles());
  delay(500);

  Serial.print(F("Rounded rects (outline)  "));
  Serial.println(testRoundRects());
  delay(500);

  Serial.print(F("Rounded rects (filled)   "));
  Serial.println(testFilledRoundRects());
  delay(500);

  Serial.println(F("Done!"));
#endif // DEBUG
  tft.setRotation(0);
  if (!SD.begin(chipSelect)) {
    errorSD();
    while (1) ;
  }
  Serial.println("card initialized.");
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (! dataFile) {
    errorFile();
    // Wait forever since we cant write data
    while (1) ;
  }
  intTime = millis();
}

void loop(void) {
  printDebug();
  String dataString="";
  int sensor = analogRead(hall);
  dataString+=String(millis()-intTime);
  dataString+=String(",");
  dataString+=String(sensor);
  dataFile.println(dataString);
  Serial.println(dataString);
  dataFile.flush();
  delay(50);
  /*for(uint8_t rotation=0; rotation<4; rotation++) {
    tft.setRotation(rotation);
    testText();
    delay(2000);
  }
  testFilledRoundRects();
  testRoundRects();
  testFilledTriangles();
  testTriangles();
  testCircles(10, WHITE);
  testFillScreen();
  testText();
  testLines(CYAN);
  testFastLines(RED, BLUE);
  testRects(GREEN);
  testFilledRects(YELLOW, MAGENTA);
  testFilledCircles(10, MAGENTA);*/
}

unsigned long testFillScreen() {
  unsigned long start = micros();
  tft.fillScreen(BLACK);
  tft.fillScreen(RED);
  tft.fillScreen(GREEN);
  tft.fillScreen(BLUE);
  tft.fillScreen(BLACK);
  return micros() - start;
}
unsigned long errorSD(){
 tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("Insert SD card and restart.");
}
unsigned long errorFile(){
 tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("We can't open the file, try a different SD card.");
}

unsigned long printDebug(){
  int curState = analogRead(A5);
  //tft.fillScreen(BLACK);
  int x,y,w = tft.width(),h = tft.height();
  for(y=24;y<47;y++) tft.drawFastHLine(0,y,w,BLACK);
  for(x=0;x<w;x++) tft.drawFastHLine(x/4,0,h/4,BLACK);
  for(y=74;y<100;y++) tft.drawFastHLine(0,y,w,BLACK);
  for(x=0;x<w;x++) tft.drawFastHLine(x,0,h/4,BLACK);
  unsigned long start = micros();
  tft.setCursor(0,0);
  tft.setTextColor(WHITE); tft.setTextSize(3);
  tft.print("Pin A5 reads: ");
  tft.println(curState);
  //tft.println("");
  tft.print("Is triggered? ");
  if(curState==0)
  tft.println("yes");else
  tft.println("no");
  /*Following code block is for total number of increments
  if(curState!=preState && curState == 0){
  rev++;
  }
  tft.println("Number of revolutions: " + rev);
  preState = curState;
  //end*/
  return micros() - start;
}

unsigned long run(){
  int curState = analogRead(A5);
  unsigned long curTime = micros();
  if(curState==0 && curState!=preState){
    rots++;
  }
  dist = diameter*3.14159265*rots;
  avgSpd = dist/(((curTime-intTime)*.001)/360)
  //
  //attempt to calculate speed very number of revolutions
  //
  int preState = curState;
}


