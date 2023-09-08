
#include"config.h"
#include <Adafruit_GFX.h>

// "FreeSans12ptWordClock.h" is the filename of the font. This font is a variation of FreeSans12pt7p.h of ardufruit_GFX. Only the capital letters were used and "O'", "Ä", "Ö" and "Ü" were added 
// This font is a variation of FreeSans12pt7p.h of ardufruit_GFX. Only the capital letters were used and "O'", "Ä", "Ö" and "Ü" were added 
// Thanks to DenSyo for his Excel fonts editor-converter. You will find it here
// https://forum.arduino.cc/index.php?topic=447956.msg3082388#msg3082388
#include "FreeSans12ptWordclock.h"     

#include "calcMatrix.c"               // part of the program which has been outsourced for more clarity

#include <EEPROM.h>                   // EEPROM was used to save settings as language, color and power management


// for this project I used the TTGO Watch library
// You will find it on github.com
// https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library
// used parts of the "TTGO_TWatch_Library-master"
TTGOClass*ttgo;                      
TFT_eSPI *tft;
AXP20X_Class *power;
BMA *sensor;

String matrix[11];    // the array of the differnt matrixes used for the different functions of the clock

char chrMatrix[111];                      // the array of the letters of the actuel used matrix
char letterAct;                           // the actual letter while printing the Matrix
int colBackground, setColBackground;      // the colors of background and letters

String formatTime;                                    // you will get date and time of the RTC as a string, this string is used for converting part of the string into integer
int day, month, year, minute, hour ;                  // the date and time calculated from the string formatTime
int hourView, minuteView, minuteSave;                 // xxView is calculated for display the time as words, minuteSave is used for calculate the refreshing of the display
int setDay, setMonth, setYear, setMinute, setHour;    // setX is use for setting date and time

int battPerc, battPercView, battLoad;     // percent of battery status; xView ist calculated for display the battery status; battLoad indicates that battery is loading      

bool look;                            // indicates that the position of the clock is such that it can be read
int16_t xTouch, yTouch;               // the coordinates while touching the display 
int xASave, yASave, xBSave, yBSave;   // used to calculated if display touched at one position or if display wiped
byte touch;                           // shows if display touched at one position or shows the direction of wiping
int luX, luY, rdX, rdY;               // were used for asking if a special area of the display has been touched
int countDisplayTouch;                // used for limiting the time while waiting for touching the display

byte counterToPowOff=1;               // counter for the time to shut off the watch

byte powManagement;                   // the kind of power management (completly shut off, only shut off the display or don't shut off



void setup()
{   
  ttgo=TTGOClass::getWatch();         // get watch instance, see also the documentation for the TTGO Watch library
  ttgo->begin();                      // initialize the hardware
  ttgo->openBL();                   // turn on the backlight                    
 
  ttgo->power->adc1Enable(AXP202_VBUS_VOL_ADC1 | AXP202_VBUS_CUR_ADC1 | AXP202_BATT_CUR_ADC1 | AXP202_BATT_VOL_ADC1, true);  // settings for using the power managemnt of the watch

  // Accel parameter structure
  Acfg cfg;
  cfg.odr = BMA4_OUTPUT_DATA_RATE_100HZ;  
  cfg.range = BMA4_ACCEL_RANGE_2G;  
  cfg.bandwidth = BMA4_ACCEL_NORMAL_AVG4;
  cfg.perf_mode = BMA4_CONTINUOUS_MODE;  
  // Configure the BMA423 accelerometer
  ttgo->bma->accelConfig(cfg);
  // Enable BMA423 accelerometer
  // Warning : Need to use feature, you must first enable the accelerometer  
  ttgo->bma->enableAccel();
  // Disable BMA423 isStepCounter feature
  ttgo->bma->enableFeature(BMA423_STEP_CNTR, false);
  // Enable BMA423 isTilt feature
  ttgo->bma->enableFeature(BMA423_TILT, true);
  // Enable BMA423 isDoubleClick feature
  ttgo->bma->enableFeature(BMA423_WAKEUP, true);
  // Reset steps
  ttgo->bma->resetStepCounter();
  // Turn off feature interrupt
  // sensor->enableStepCountInterrupt();
  ttgo->bma->enableTiltInterrupt();
  // It corresponds to isDoubleClick interrupt
  ttgo->bma->enableWakeupInterrupt(); 
  
  minuteSave=60;
  
  
  // adresses of the EEPROM and what they are used for
  // 0:  color of letter
  // 1:  color of background
  // 2:  kind of power management (0 = completly shut off; 1 = only shut off the display; 2 = don't shut off the watch)
  // 3:  language (0 = german; 1 = english)  
  

  EEPROM.begin(4);
  
  setColMatrix = EEPROM.read(0);                  // get the color of the letters out of the EEPROM  
  setColBackground = EEPROM.read(1);              // get the color of the background out of the EEPROM  
  if (setColMatrix==setColBackground)             // if the color for letter and background is the same, set different values for background and letter
  {
    setColMatrix=1;
    setColBackground=0;
  }
  colMatrix=codeColor(setColMatrix);
  colBackground=codeColor(setColBackground);
   
  powManagement = EEPROM.read(2);                 // get the kind of power management out of the EEPORM
  if(powManagement > 2) {powManagement = 2;}

  language = EEPROM.read(3);                      // get the used language out of the EEPORM
  if (language > 1) {language = 1;}               // if the value of the EEprom is larger than 1, language is 1 (english)
  setLangMatrix();                                // load the matrix for the used language
}



void lightPowerOff()
// shut off the display and the backlight
{
  ttgo->tft->fillScreen (TFT_BLACK);
  ttgo->displaySleep();
  ttgo->closeBL();  
 
}


void waitWatchMoved()
// waiting for moving the watch in a position so that it can be read, signal for switch on the display
{
  Accel acc;
  look=false;
  do
  {          
    ttgo->bma->getAccel(acc);    
    if (((acc.x>(-50)) && (acc.x<50)) && ((acc.y>400) && (acc.y<800)) && ((acc.z>(-1000)) && (acc.z<(-700)))) {look=true;}  
    // these are the coordinates of x-, y- and z-axis of the accelerator indicates the reading position of the watch
  }
  while (look==false);        // loop until the right position is reached (look == true)
  ttgo->displayWakeup();      // ...then wake up the display
  ttgo->openBL();             // ...and turn on the backlight
}



void powerOff()
// shut off the watch completly
{
  ttgo->tft->fillScreen (TFT_BLACK);
  ttgo->displaySleep();
  ttgo->closeBL();
  ttgo->powerOff();
  // Use ext1 for external wakeup
  esp_sleep_enable_ext1_wakeup(GPIO_SEL_39, ESP_EXT1_WAKEUP_ANY_HIGH);
  esp_deep_sleep_start();  
}



void clearMatrix()
// clear the matrix, set color of all letters darkgrey (0x10A2) and set the background color
{
  ttgo->tft->fillScreen (colBackground);
  for (int i=0; i <110; i++) {letterCol[i]=0x10A2;}     
}



void printMatrix(byte mtx)
// the letters of the choosed matrix (mtx) were displayed
{
  ttgo->tft->setTextSize(1);                                          // set the size of the letters
  ttgo->tft->setFreeFont(&FreeSans12ptWordClock);                     // this is the name of the font used in the file (maybe it is different from the file name)
  for (int letterRow=0; letterRow <=10; letterRow =letterRow+1)       // counter for the rows of the wordclock
  {
     for (int letterCount=0; letterCount <=10; letterCount =letterCount+1)            // counter for the letter in the row
    {
      ttgo->tft->setTextColor(letterCol[(letterRow*11)+letterCount],colBackground);   // set the color of the actual printed letter
      // there are 110 letters in each matrix, the number of the actual letter in the array is calculated by row*11 + number of letter in the row
      matrix[mtx].toCharArray(chrMatrix,111);    // copied each letter of the string "matrix[mtx]" in an array called chrMatrix (chrMatrix[0] to chrMatrix[109])                                         
      letterAct=chrMatrix[(letterRow*11)+letterCount];                         // this ist the actual letter printed
      ttgo->tft->drawChar(letterAct,8+(letterCount*20),25+(23*letterRow));     // the actual letter is printed, coordinates for each letter ware calculated
    }  
  }
}  



boolean chkMatrixTouch (int firstLet, int numLet)
// checked if a special position of the display is touched
// the position is defined by the number of the letter (firstLet) of the matrix and the lenght (numLet)
{  
  firstLet=firstLet;
  // a rectangle was calculated by the number of the letter and the lenght, lu = left up, rd = right down
  luX=5+((firstLet%11)*20);
  rdX=luX+(numLet*20);
  luY=(((firstLet/11)-1)*23)+27;
  rdY=luY+23;  
  if (((xASave > luX) && (xASave < rdX)) && ((yASave > luY) && (yASave < rdY)))   // checked if the display was touched in the rectangle
  {
     return true;
  }
  else return false;
}


int waitDisplayTouch()
// waiting for touching the display and calculating if display was touched at one position or calculating the direction of wiping the display
{
  xTouch = 0;   // the actual position while touching the display
  yTouch = 0;
  xASave = 0;   // xxSave were used for calculation
  yASave = 0; 
  xBSave = 0;
  yBSave = 0;     
  countDisplayTouch =0;   // used as a timer
  // check if display is topuched
  do
  {
    countDisplayTouch =countDisplayTouch +1;    // increase the timer
  }
  while ((ttgo->getTouch(xTouch, yTouch)==false) && (countDisplayTouch<5000));  // wait for touching the display or for reaching the end of the timer
  
  if (ttgo->getTouch(xTouch, yTouch)==false) {return 0;}      // if display wasn't touch return 0
  xASave = xTouch;    // when display was touched save the cordinates
  yASave = yTouch;     
  do                  // wait until the display is no more touched
  {
    xTouch=0;             
    yTouch=0;
    ttgo->getTouch(xTouch, yTouch);             
    if (xTouch != 0)       // while the display is still touched save the coordinates               
    {
      xBSave=xTouch;
      yBSave=yTouch;
    }   
  }
  while (xTouch != 0 );   // display is no more touched
  // compare the saved coordiantes to calculate the direction of wiping
  if ((xBSave > xASave-20) && (xBSave < xASave +20) && (yBSave > yASave-20) && (yBSave < yASave +20)){return 1;}  // no wiping, only touched the display at one position
  if (xBSave < xASave-50){return 2;}    // wiping to the left side
  if (xBSave > xASave+50){return 3;}    // wiping to the right side
  if (yBSave < yASave-50){return 4;}    // wiping to the upper side
  if (yBSave > yASave+50){return 5;}    // wiping to the bottom side
}
  



void setTimeDate ()
// setting the date an the time 
{
  formatTime = ttgo->rtc->formatDateTime(PCF_TIMEFORMAT_DD_MM_YYYY);        // the time was requested, the format of this string is DD-MM-YYYY 
  setDay = (formatTime.substring (0,2)).toInt();                            // the strings for day, month and year were converted to integer
  setMonth = (formatTime.substring (3,5)).toInt();
  setYear = (formatTime.substring (6,11)).toInt();   
  clearMatrix();              
  calcMatrix (3,setDay,0);      // calculate and show the matrix for setting the day
  printMatrix(3);
  do
  {
    touch=waitDisplayTouch();  
    switch (touch)    
    {
      case 4: setDay=setDay+1; break;  // wiping up increases the day            
      case 5: setDay=setDay-1; break;   // wiping down decreases the day                
    }
    if (setDay>31) {setDay=1;}
    if (setDay<1) {setDay=31;}
    if ((touch==4) || (touch==5))     // if it was wiped to the top or to the buttom...
    {
      clearMatrix();                  // ...display the matrix with the new day
      calcMatrix (3,setDay,0);
      printMatrix(3);      
    }
  }  
  while (touch!=2);                   // if it was wiped to the left...
  clearMatrix();                      // do the same to set month, year, hour and minute
  calcMatrix (4,setMonth,0);
  printMatrix(4);
  do
  {
    touch=waitDisplayTouch();  
    switch (touch)    
    {
      case 4: setMonth=setMonth+1; break;       
      case 5: setMonth=setMonth-1; break;
    }
    if (setMonth>12) {setMonth=1;}
    if (setMonth<1) {setMonth=12;}
    if ((touch==4) || (touch==5))
    {
      clearMatrix();
      calcMatrix (4,setMonth,0);    
      printMatrix(4);
    }
  }  
  while (touch!=2);  
  clearMatrix();
  calcMatrix (5,setYear,0);  
  printMatrix(5);
  do
  {
    touch=waitDisplayTouch();  
    switch (touch)    
    {
      case 4: setYear=setYear+1; break;       
      case 5: setYear=setYear-1; break;
    }
    if (setYear>2039) {setYear=2020;}
    if (setYear<2020) {setYear=2039;}
    if ((touch==4) || (touch==5))
    {
      clearMatrix();
      calcMatrix (5,setYear,0);    
      printMatrix(5);
    }
  }  
  while (touch!=2);   
  formatTime = ttgo->rtc->formatDateTime(PCF_TIMEFORMAT_HM);        // the time was requested, the format of this string ist HH:MM
  setHour = (formatTime.substring (0,2)).toInt();                   // the strings for the hour and for the minute were converted to Integer
  setMinute = (formatTime.substring (3)).toInt();  
  clearMatrix();
  calcMatrix (6, setHour,0);  
  printMatrix(6);
  do
  {
    touch=waitDisplayTouch();  
    switch (touch)    
    {
      case 4: setHour=setHour+1; break;       
      case 5: setHour=setHour-1; break;
    }
    if (setHour>23) {setHour=0;}
    if (setHour<0) {setHour=23;}
    if ((touch==4) || (touch==5))
    {
      clearMatrix();
      calcMatrix (6,setHour,0);    
      printMatrix(6);
    }
  }  
  while (touch!=2);   
  clearMatrix();
  calcMatrix (7,setMinute,0);  
  printMatrix(7);
  do
  {
    touch=waitDisplayTouch();  
    switch (touch)    
    {
      case 4:{setMinute=setMinute+1;} break;       
      case 5: {setMinute=setMinute-1;} break;
    }
    if (setMinute>59) {setMinute=0;}
    if (setMinute<0) {setMinute=59;}
    if ((touch==4) || (touch==5))
    {
      clearMatrix();
      calcMatrix (7,setMinute,0);
      printMatrix(7);
    }    
  }  
  while (touch!=2);   
  ttgo->rtc->setDateTime(setYear,setMonth,setDay,setHour,setMinute,00);    // setting the date and time of the RTC
  clearMatrix();
  calcMatrix(7,60,0);
  printMatrix(7);
  delay (2000);
}




int codeColor(int code)
// there a 24 different colors
// for setting the color of letter or the background color you need the rgb555-code
// a cool website for calculating the rgb565-code is the following one:
// http://www.barth-dev.de/online/rgb565-color-picker/  
// thanks to Thomas Barth
// but most codes were taken from TFT_eSPI.h of the "TTGO_TWatch_Library-master"
{
  //0: SCHWARZ / BLACK       
  //1: MARINEBLAU / NAVY        
  //2: DUNKELGRÜN / DARKGREEN   
  //3: DUNKELZYAN / DARKCYAN    
  //4: KASTANIENBRAUN / MAROON      
  //5: LILA / PURPLE      
  //6: OLIV / OLIVE       
  //7: HELLGRAU / LIGHTGREY   
  //8: DUNKELGRAU / DARKGREY    
  //9: BLAU / BLUE        
  //10: GRÜN / GREEN       
  //11: ZYAN / CYAN        
  //12: ROT / RED         
  //13: MAGENTA / MAGENTA     
  //14: GELB / YELLOW      
  //15: WEISS / WHITE       
  //16: ORANGE / ORANGE      
  //17: GRÜNGELB / GREENYELLOW 
  //18: ROSA / PINK        
  //19: BRAUN / BROWN       
  //20: GOLD / GOLD        
  //21: SILBER / SILVER      
  //22: HIMMELBLAU / SKYBLUE     
  //23: VIOLETT / VIOLET      

  switch (code)
  {
    case 0: return 0x0000; break;
    case 1: return 0x000F; break;
    case 2: return 0x03E0; break;
    case 3: return 0x03EF; break;
    case 4: return 0x7800; break;
    case 5: return 0x780F; break;
    case 6: return 0x7BE0; break;
    case 7: return 0xD69A; break;
    case 8: return 0x7BEF; break;
    case 9: return 0x001F; break;
    case 10: return 0x07E0; break;
    case 11: return 0x07FF; break;
    case 12: return 0xF800; break;
    case 13: return 0xF81F; break;
    case 14: return 0xFFE0; break;
    case 15: return 0xFFFF; break;
    case 16: return 0xFDA0; break;
    case 17: return 0xB7E0; break;
    case 18: return 0xFE19; break;
    case 19: return 0x9A60; break;
    case 20: return 0xFEA0; break;
    case 21: return 0xC618; break;
    case 22: return 0x867D; break;
    case 23: return 0x915C; break;
  }
}



void setColor()
// setting the color for the letters and for the background
{  
  clearMatrix();
  calcMatrix(8,setColMatrix,0);
  printMatrix(8);
  do
  {
    touch=waitDisplayTouch();  
    switch (touch)    
    {
      case 4:   // wiping to the top changes the color of the letters
      {
        setColMatrix=setColMatrix+1;
        if (setColMatrix>23) {setColMatrix=0;} 
        break;       
      }
      case 5:   // wiping to the bottom changes the color of the background
      {
        setColBackground=setColBackground+1; 
        if (setColBackground>23) {setColBackground=0;} 
        break;
      }
    }
    if ((touch==4) || (touch==5))                   // if it was wiped to the top or wiped to the bottom...
    {    
      colMatrix=codeColor(setColMatrix);            // the matrix was displayed using the new colors and displayd the name of the letter color
      colBackground=codeColor(setColBackground);      
      clearMatrix();
      calcMatrix (8,setColMatrix,0);
      printMatrix(8);   
    }
  }  
  while (touch!=2);                                 // when wiped to the left settings were saved
  clearMatrix();
  calcMatrix (8,24,0);
  printMatrix(8);   
  EEPROM.write(0, setColMatrix);                    // color for the letters and for the background will be saved in EEProm so the informations can be read after shutting off the watch
  EEPROM.commit();
  delay(50);
  EEPROM.write(1,setColBackground);
  EEPROM.commit();
  delay(1500);
}



void setPowerMangemant()
// setting the püower management
{  
  clearMatrix();      
  calcMatrix(9,powManagement,1);        // display the matrix for seeting the power management, 1 means that all choosable possibilities were displayed
  printMatrix(9);
  if (waitDisplayTouch()==1)
  { 
    if ((chkMatrixTouch (0, 11) == true) || (chkMatrixTouch (11, 11) == true))  // check if "shut off completly" was pressed
    // in the first line "switch of" was displayed, in the second line "completly" was displayed
    // 0,11 and 11,11 means, that it will be checked if a touch on the display was in the first row (letters 0 to 10) or in the second row (letters 11 to 21) 
    {
      powManagement=0;      
    }
    if ((chkMatrixTouch (44, 11) == true) || (chkMatrixTouch (55, 11) == true))   // check if "shut off display only" was pressed
    {
      powManagement=1;   
    }
    if ((chkMatrixTouch (88, 11) == true) || (chkMatrixTouch (99, 11) == true))   // check if "don't shut off" was pressed
    {
      powManagement=2;   
    }     
    clearMatrix();
    calcMatrix(9,powManagement,0);      // display the matrix for seeting the power management and display the choosen point
    printMatrix(9);
    EEPROM.write(2, powManagement);     // save the settings for power management in EEProm
    EEPROM.commit();
    delay(1500);
    clearMatrix();
    calcMatrix(9,3,0);                  // display that the settings were saved
    printMatrix(9);  
    delay(1500);
  }
}

void setLangMatrix()
// different matrixes for different languages

// "="="Ä"; ">"="Ö"; "?"="Ü"; "@" ="O'"
// Matrix 0 for the time (the typical wordclock)
// Matrix 1 for the battery status
// Matrix 2 for the menu
// Matrix 3 for setting the date (day)
// Matrix 4 for setting the date (month)
// Matrix 5 for setting the date (year)
// Matrix 6 for setting the time (hour)
// Matrix 7 for setting the time (minute);
// Matrix 8 for setting the colors
// Matrix 9 for setting the power management
// Matrix 10 for setting the language

{
  if (language == 0)    // the german matrixes
  {    
    matrix[0]="ESKISTAF?NFZEHNZWANZIGDOEIVIERTELVORFUNKNACHHALBAELF?NFEINSXAMZWEIDREIPMJVIERSECHSNLACHTSIEBENZW>LFZEHNEUNKUHR";
    matrix[1]="DIEQWERTZIUGHJBATTERIEISTNMEHRHJKWENIGERPALSEINFFASTGVBDREIVIERTELHALBAMSVOLLLEERRIGZUNDLKNBGEKWIRDGELADENZVBM";
    matrix[2]="UHRZEITWRZUSTELLENPKGVAEZBNMASDFGXDRTGFARBENZBMANPASSENTFGCVBNGUJSPOWERTDGZHJMANAGEMENTLJHGFDSARZUILKVDSPRACHE";
    matrix[3]="ESGISTGDERVVIERSECHSRAZWEIDRITZIOF?NFSIEBENXZW>LFELFLKMEINZWEIDREIACHTNEUNDSANEUNZEHNTERDREISSVCUNDZWANZIGSTER";
    matrix[4]="MONATEGHNBVJANUARRUIKFAFEBRUARRCBM=RZUOAPRILMAISDHNJUNIJULIFAUGUSTSEPTEMBERWEGHKLOKTOBERNOVEMBERWDVDEZEMBERKFC";
    matrix[5]="JAHREGHNBVAZWANZIGACHTEINZWEIDREIVIERF?NFRCBSECHSIEBENUNEUNRGIKFUOTFGCVBNGUJSJHGFDSARZUIUNDXZWANZIGDREISSIGWDV";
    matrix[6]="STUNDENULLMVIERSECHSRAZWEIDREIZIOF?NFSIEBENXZW>LFELFLKMEINZWEIDREIACHTNEUNDSANEUNZEHNTERDREISSVCUNDZWANZIGSUHR";
    matrix[7]="SICHEREVIEREINSECHSELFZWEIDREIUHRF?NFSIEBENWZW>LFACHTRANEUNZEHNUNDZWANZIGLKMVDREISSIGDSAVIERZIGZEITF?NFZIGNULL";
    matrix[8]="SCHWARZLILAGOLDUNKELMSHELLGRAUROTROSASILBERUZYANBRAUNVBOLIVIOLETTGMARINEBLAUVORANGEGR?NLMAGENTAGELBWEISSICHERT";
    matrix[9]="VOLLST=NDIGABSCHALTENFERTNBGEKAMSNURVDISPLAYABSCHALTENTFTGCVBNGUJSEINSTELLUNGNICHTRIGZATABSCHALTENQGESICHERTER";
    matrix[10]="ERTNBGEKAMSENGLISCHIGZENGLISHWERTTFGCVBNGUJSGESPEICHERTSAVEDHERTJSXDRTGEGHNBVDEUTSCHTNBGGERMANTZIUDAEZBNMASDFG";    
  }
  
  if (language == 1)    // the english matrixes
  {    
    matrix[0]="ITLISASAMPMACQUARTERDCTWENTYFIVEXHALFSTENFTOPASTERUNINEONESIXTHREEFOURFIVETWOEIGHTELEVENSEVENTWELVETENSE@CLOCK";
    matrix[1]="THEQWERTZIUGHJBATTERYHISXNMOREHJKLESSMTHANLAALMOSTHREEMQUARTERSGZHHALFAMSFULLEMPTYIGZGFDLKNBGEKQISPCHARGINGVBM";
    matrix[2]="SETFGCVBRZUTIMEARZPKGVAEZBNMASDFGZBMDHCHANGEJHGFDYCOLORTFGCVBNGUJSPOWERTDGZHJMANAGEMENTLXDRTGEGHNBVLKVLANGUAGE";
    matrix[3]="DAYEGTWENTYTHIRTYNINTHSECONDSIXTHFIRSTHIRDHOFOURTHFIFTHSEVENTHNINEEIGHTHTENTHELEVENTHDHNTWELVETHIKFTIETHTEENTH";
    matrix[4]="MONTHEGHNBVJANUARYUIKFAFEBRUARYCBMARCHOAPRILMAYSDHNJUNEJULYFAUGUSTSEPTEMBERWEGHKLOCTOBERNOVEMBERWDVDECEMBERKFC";
    matrix[5]="YEAREGHNBVFYUIKFTWENTYTWENTYBNMASTHIRTYJHGFDONEEUITHREEFIVEGCVFOURYTWODFGNINEGHKLSIXDGZHSEVENUIMAERTDGZHJEIGHT";
    matrix[6]="HOURSWAMOPMONEASDFTENGTWOVCELEVENTHREELKMNHGFOURXTWELVEFIVEJHGFSIXUZFVNISEVENEIGHTWSCFTZWENINETDGZHVCDSE@CLOCK";
    matrix[7]="MINASTWENTYTHIRTYFORTYFIFTYJHZEROONETWOTHREEFOURFIVESIXSEVENEIGHTWNINETENTDGZELEVENUZFVNTWELVESAVEDTHIRTEENDSE";
    matrix[8]="BLACKNAVYBMREDARKGREENCYANMAROONXPURPLEOLIVEVIOLETDGREYSILVERSKYWTMAGENTAGOLDYELLOWHITEZORANGEBROWNBLUEPINKSET";
    matrix[9]="SWITCHSOFDCCOMPLETLYVEXDRTGEGHNBVSWITCHXOFFRSCREENAONLYGEKQVNBGEKTSETTINGGUJSDONTMSWITCHOFFRAMSZFVNSAVEDJHGFDY";
    matrix[10]="ERTNBGEKAMSENGLISCHIGZENGLISHWERTTFGCVBNGUJSGESPEICHERTSAVEDHERTJSXDRTGEGHNBVDEUTSCHTNBGGERMANTZIUDAEZBNMASDFG";  
  }
}



void setLanguage()
// setting the language (0 = germnan, 1= english)
{  
  clearMatrix();
  calcMatrix(99,language,1);    //display the matrix for seeting the language, 1 means that all choosable possibilities were displayed
  printMatrix(10);
  if (waitDisplayTouch()==1)
  {     
    if ((chkMatrixTouch (11, 11) == true) || (chkMatrixTouch (22, 11) == true))  // check if "englisch" was pressed  
    {
      language=1;           
    }
    if ((chkMatrixTouch (77, 11) == true) || (chkMatrixTouch (88, 11) == true))  // check if "german" was pressed
    {
      language=0;        
    }
    
    setLangMatrix();              // load the matrix for the choosen language
    clearMatrix();                
    calcMatrix(99,language,0);    // display the matrix for seeting the language and display the choosen language
    printMatrix(10);
    EEPROM.write(3, language);
    EEPROM.commit();
    delay(1500);
    clearMatrix();
    calcMatrix(99,2,0);           // display that the settings were saved
    printMatrix(10);  
    delay(1500);
  }
}



void printDate()
// displays the actual date
{
  formatTime = ttgo->rtc->formatDateTime(PCF_TIMEFORMAT_DD_MM_YYYY);        // the time was requested, the format of this string is DD-MM-YYYY 
  day = (formatTime.substring (0,2)).toInt();                               // the strings for day, month and year were converted to integer
  month = (formatTime.substring (3,5)).toInt();
  year = (formatTime.substring (6,11)).toInt();   
  // in the follow the different matrixes for day, month and year were displayed
  clearMatrix();
  calcMatrix(3,day,1);
  printMatrix(3);
  delay(1500);
  clearMatrix();
  calcMatrix(4,month,0);
  printMatrix(4);
  delay(1500);
  clearMatrix();
  calcMatrix(5,year,0);
  printMatrix(5);
  delay(1500);  
}




void printTime()
// the typical wordclock will be displayed
{
    formatTime = ttgo->rtc->formatDateTime(PCF_TIMEFORMAT_HMS);      // the time was requested, the format of this string ist HH:MM:SS
    hour = (formatTime.substring (0,2)).toInt();                     // the strings for the hour and for the minute were converted to Integer
    minute = (formatTime.substring (3,5)).toInt();    
    // calculate the matrix for minutes
    // in one hour there are 12 different states to display the minutes
    // state      minutes
    // 0 or 12    58 to 2
    // 1          3  to 7
    // 2          8  to 7
    // 3          13  to 17
    // 4          18  to 22
    // 5          23  to 27
    // 6          28  to 32
    // 7          33  to 37
    // 8          38  to 42
    // 9          43  to 47
    // 10         48  to 52
    // 11         53  to 57    
    minuteView = ((minute+2)/5);              // calculate the different states for displying the minutes
    // calculate the matrix for hours
    if (hour==0) {hour=12;}             
    if (language==0)                          // for the german version                 
    {
      if (minute>22) {hourView=hour+1;}       // for example: time is 2:19 --> "es ist zwanzig nach ZWEI"; 2:25 --> "es ist fünf vor halb DREI" 
      else {hourView=hour;} 
    }     
    if (language==1)                          // for the english     
    {
      if (minute>32) {hourView=hour+1;}       // for example: time is 2:29 --> "it is half past TWO"; 2:35 --> "it is twentyfive to THREE" 
      else {hourView=hour;}
    }     
    
    if (hourView>12) {hourView=hourView-12;}  // Conversion of hours from 24-hour to 12-hour display      
    clearMatrix();
    calcMatrix(0,minuteView, hourView);       // display the WORDCLOCK
    printMatrix(0);
}




void printBattery()
// display the status of the battery
{
  battLoad=0;
  if (ttgo->power->isVBUSPlug())                // check if battery is charging    
  {
    battLoad=1;    
  }     
  battPerc=ttgo->power->getBattPercentage();    // get the status of the battery
  battPercView=((battPerc+5)/10);               // there are 10 different states to display the percentage of battery charge
  Serial.print(battPerc);
  Serial.print(battPercView); 
  clearMatrix();
  calcMatrix(1,battPercView,battLoad);          // display the battery charge 
  printMatrix(1);
  delay(2500);
}





void menu()
// displays the menu 
{
  clearMatrix();
  calcMatrix(2,0,1);          //display the matrix for the menu, 1 means all choosable possibilities were displayed
  printMatrix(2);
  if (waitDisplayTouch()==1)
  { 
    if ((chkMatrixTouch (0, 11) == true) || (chkMatrixTouch (11, 11) == true))  //check if "set time" was pressen
    {
      clearMatrix();
      calcMatrix(2,1,0);       //display "set time"
      printMatrix(2);
      delay (1000);
      setTimeDate();      
    }
    if ((chkMatrixTouch (33, 11) == true) || (chkMatrixTouch (44, 11) == true))  //Prüfen, ob "Farben anpassen" gedrückt wurde
    {
      clearMatrix();
      calcMatrix(2,2,0);       //display "change color"
      printMatrix(2);
      delay (1000);
      setColor();
    }
    if ((chkMatrixTouch (66, 11) == true) || (chkMatrixTouch (77, 11) == true))  //Prüfen, ob "Powermanagement" gedrückt wurde
    {
      clearMatrix();
      calcMatrix(2,3,0);       //display "power management"
      printMatrix(2);
      delay (1000);
      setPowerMangemant();
    }
    if (chkMatrixTouch (99, 11) == true)  //Prüfen, ob "Sprache" gedrückt wurde
    {
      clearMatrix();
      calcMatrix(2,4,0);       //display "language"
      printMatrix(2);
      delay (1000);
      setLanguage();
    }
  }       
}



void loop()
{
  // Anzeigen der Uhrzeit  
  formatTime = ttgo->rtc->formatDateTime(PCF_TIMEFORMAT_HMS); 
  if ((((formatTime.substring (3,5)).toInt()) != minuteSave ) || (counterToPowOff == 1))   // if time changed ("minute" is not "minuteSave" or void loop() was just started (counterToPowOff == 1)
  {
    printTime();                                        // display the wordclock
    minuteSave= (formatTime.substring (3,5)).toInt();   // save the actual minute to recognize if time has changed 
  }  
  touch=waitDisplayTouch();                             // wait for display touches
  if (touch>0) 
  {    
    counterToPowOff=0;
    switch(touch)
    {
      case 1: printTime(); break;                   // no display touch     --> wordclock was displayed
      case 2: printDate(); break;                   // wiping to the left   --> date was displayed
      case 3: printBattery(); break;                // wiping to the right  --> battery status was displaye
      case 4: break;                                // wiping to the top    --> not used at the moment
      case 5: menu(); break;                        // wiping to the bottom --> call menu
    }        
  }    
  counterToPowOff=counterToPowOff+1;                // if there is no display touch the timer for shut off the watch is increased
  if (counterToPowOff > 5)                          // if the timer for shut off the watch is higher then 5 the watch will be shut off - if choosen
  {
    Serial.print ("PowerManagement: ");
    Serial.println (powManagement);
    switch (powManagement)
    {
      case 0: powerOff(); break;    // switch off the watch completly
      case 1: 
      {
        lightPowerOff();            // switch off only the display
        waitWatchMoved();           // and wait for a movement of the watch in a position so that it can be read, this is the signal for switch on the display
        counterToPowOff=1;          // this means, that the wordclock will be displayed, wenn loop() will be started again
      }  
    }
  }
}
