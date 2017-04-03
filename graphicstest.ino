
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
  int preState = 0;
  int rev = 0;
  int intTime;
  int rots = 0;
  long avgSpd = 0;
  long rotSpd = 0;
  long dist =0;
  long diameter = 2.2507;
  int splits[] = {0,0,0,0,0};
  int sDist[] = {0,0,0,0,0};
  int s = 4;
  long rpm = 0;
  long shtSpd = 0;
  long lstUpdate;
  int x,y,w;
  int curState;
  long curTime;
  

Adafruit_TFTLCD tft(LCD_CS, LCD_CD, LCD_WR, LCD_RD, LCD_RESET);
void setup(){
  pinMode(SS, OUTPUT);
  tft.reset();
  uint16_t identifier = tft.readID();
  tft.begin(identifier);
  tft.setRotation(0);
  //Begin SD Testing Block
  if (!SD.begin(chipSelect)) {
    errorSD();
    while (1) ;
  }
  dataFile = SD.open("datalog.txt", FILE_WRITE);
  if (! dataFile) {
    errorFile();
    while (1) ;
  }
  //End SD Testing Block
  //Initialized Vars
  intTime = millis();
  lstUpdate = intTime;
  int x,y,w = tft.width(),h = tft.height();
  tft.setTextColor(WHITE); tft.setTextSize(3);
  tft.fillScreen(BLACK);
  preState = 0;
  curTime = intTime;
  String header = "Time,On,Distance,RPM,Speed";
  dataFile.println(header);
  dataFile.flush();
}

void loop(void) {
  printRun();
  if(curState == 1){
  
  }
}

unsigned long errorSD(){//If there is no SD card, halt and request one.
 tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("Insert SD card and restart.");
}
unsigned long errorFile(){ //If there is a problem with the SD card, halt and request a different one.
 tft.fillScreen(BLACK);
  unsigned long start = micros();
  tft.setCursor(0, 0);
  tft.setTextColor(WHITE);  tft.setTextSize(1);
  tft.println("We can't open the file, try a different SD card.");
}

unsigned long printRun(){ //Function to run under normal operation
  int sens = analogRead(A5);
  curTime = millis();
  if(sens<15)
    curState = 1;
  else
    curState = 0;
  if(curState==1){//See if sensor is triggered.
  if(curState!=preState){//Make sure the sensor wasn't previously triggered.
  rots++;
  dist = diameter*3.14159265*rots;//Calculate distance from diameter*pi*(# of rotations)
  avgSpd = dist/(((curTime-intTime)*.001)/360);//Find total avg speed from distance over time in hours.
  //The following block records the last five sets to memory speed calcs.
  for(int i = 0;i<=3;i++){
    splits[i] = splits[i+1]; //splits is time (ms)
    sDist[i] = sDist[i+1];  //sDist is distance (ft)
  }
  splits[4] = curTime;
  sDist[4]=dist;
  //END PREVIOUS BLOCK
  //Short means based on last 5 rotations.
  rpm = ((sDist[4]-sDist[1])/(diameter*3.14159265))/(.001/60*(splits[4]-splits[1])); //Find short RPM (based on last 5 revs).
  long shtSpd = .681818*((sDist[4]-sDist[1])/(.001*(splits[4]-splits[1]))); //Find short speed, convert to mph.
  long tDelta = curTime-lstUpdate; // Calculate time between the last time the screen updated and now
  /*
   * Writes to screen at most every 350 ms
   * This allows for more data to be gathered
   * This is due to a limitation of the screen's
   * refresh rate.
   * 
   * This code only writes to the screen.
   * The data is still calculated and saved, even if
   * the screen is not written to.
   */
  if(tDelta>350){ 
  int x,y,w = tft.width(),h = tft.height();
  //The for loops clear out parts of text that change.
  for(y=24;y<46;y++) tft.drawFastHLine(0,y,w,BLACK);
  for(x=0;x<w/4;x++) tft.drawFastHLine(x/4,0,h/4,BLACK);
  for(y=71;y<94;y++) tft.drawFastHLine(0,y,w,BLACK);
  for(x=0;x<w/4;x++) tft.drawFastHLine(x,0,h/4,BLACK);
  for(y=120;y<146;y++) tft.drawFastHLine(0,y,w,BLACK);
  for(x=0;x<w/4;x++) tft.drawFastHLine(x,0,h/4,BLACK);
  tft.setCursor(0,0);
  //Prints distance (ft.), RPM (rpm) and Speed (mph)
  tft.println("Distance: ");
  tft.print(dist); tft.println(" ft.");
  tft.println("Avg. RPM: ");
  tft.println(rpm);
  tft.println("Speed: ");
  if(splits[0]!=0){
  tft.print(shtSpd); tft.println(" mph");}
  lstUpdate = curTime; //Records the time the screen was updated for future use.
  }}
  String dataString = "";
  dataString+=String(curTime-intTime);
  dataString+=String(",");
  dataString+=String(curState);
  dataString+=String(",");
  dataString+=String(dist);
  dataString+=String(",");
  dataString+=String(rpm);
  dataString+=String(",");
  dataString+=String(shtSpd);
  dataFile.println(dataString);
  Serial.println(dataString);
  dataFile.flush();
  }
  preState = curState; //Records current state for future use.
}

