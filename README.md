# TTGO_TWatch_WordClock
## Wordclock on the lilygo t-watch 2020

This arduino code displays the time in a typical wordclock format using the Liligo T-watch 2020.
But furthermore I tried to implicate more functions using this typical wordclock style. So it is possible to display the date, setting the time and date, change the color of letters and background, switch between english and german version and setting the power management.
This arduino code uses the "TTGO_TWatch_Library-master"-library. 
You will find this library on https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library. Please follow the instructions for installing this library first.

### Requirements

#### Hardware
Liliygo® T-Watch 2020 

#### Software
Arduino IDE (https://www.arduino.cc/en/Main/Software)
##### Libraries
TTGO_TWatch_Library-master  (https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library)

### Installation
Please install the Arduino IDE first. Follow the instructions on https://www.arduino.cc/en/Main/Software.
After installing the Arduino IDE you have to install the TTGO T-watch library from https://github.com/Xinyuan-LilyGO/TTGO_TWatch_Library. You will find a good documentation on this side, too. This library comes with a lot of drivers and also with some good examples for the T-watch.
Connect the T-watch to one of your usb-ports. Please select the port under „tools“ --> „port“. 
For first tries you can load some examples coming up with the library. 
You will find these examples under „file“  --> “examples“ --> „examples for TTGO-T-watch“ (you have to scroll down in the menu)

**NOTE:**
If you have a problem with connecting the T-watch via USB, e.g. you can‘t choose the port in the arduino IDE because it is in gray color, please look here for more informations and help. 
https://github.com/espressif/arduino-esp32/issues/1137 
Thanks to „beegee-tokyo“!

I had this problem. After connecting the watch to one of my USB-Ports I couldn’t choose the port in the arduino IDE. The name of the port was gray colored. After installing the  „CP210x USB to UART Bridge VCP Drivers“ everything worked fine.
You will find the driver on  http://esp32.net/usb-uart/ and also on https://www.silabs.com/products/development-tools/software/usb-to-uart-bridge-vcp-drivers

After installing the Arduino IDE and the library you can download the wordclock-package. Please download the zip-file end extract it in an own folder. Load the file „t-watch_wordclock_V1_0.ino“ into the arduino IDE.
By double-click on the file name. Arduino will start automatically and the file will be loaded
In the Arduino IDE choose“ file“ --> „open“ (or STRG+o) and select the file to open    
After the file has been loaded you will find 4 tabs at the top of the Arduino IDE:

t-watch_wordclock_V1_0:	    the main program of the wordclock
calcMatrix.c:			          an outsourced part of the program
FreeSans12ptWordclock.h:	  The file with the used font. This font is a variation of the FreeSans12pt7p.h of ardufruit_GFX included in the T-watch-library.
config.h:	                  A file to select the hardware. 
                            Please be sure that the line 
                            „#define LILYGO_WATCH_2020_V1    // To use T-Watch2020 , please uncomment this line "
                            is really uncomment. That means, that the two // at the beginning of the line were deleted. 

Now you can upload the code to the watch.
By clicking „sketch“ --> upload
Or by clicking the symbol with the arrow to the right side

**NOTE:**
Sometimes there’s a problem with the upload. Please switch on the watch by pressing the button on the side of the watch for 2 seconds. Try to upload the sketch again.
After the upload finished you can disconnect the watch from the usb-port. If everything is o.k. the program will start automatically by display a time in a typical wordclock style.

### Functions
Starting from the main screen – the typical wordclock - you can call different functions by wiping on the display:
Wiping to the left:	One after the other the date, the month and the year are displayed. At the end, the main screen will be displayed again 
Wiping to the right:	The status of the battery is displayed. After a few seconds the main screen will be displayed again
Wiping to the bottom: 	The menu is displayed. You can choose the different points of the menu by typing the point on the display. If you don’t type anything the main screen will be displayed after a few seconds

### Menu

#### Set time
First of all the actual day will be displayed. By wiping up or down you can increase or decrease the day. When you reached the correct day wipe to the left. The setting for the month appered. Please set the month – and also the year, hour and minute – as same as the day.
After setting the minute you have to wipe to the left once more.  Date and time is now saved. The main screen will be displayed.

#### Change color
In this menu you can change the color of the matrix and the background color.
The name of the color of the matrix will be displayed. By wiping down you chnage the background color. By wiping up you will change the color of the matrix. There are 23 different colors fort he matrix and fort he background.
After setting the colors you have to wipe to the left for saving the colors. The main screen will be displayed.
 
#### Power Management
There are three possibilities for the power management. By touching the display you can choose the method. After touching the screen the choosed method will be saved and the main screen will be displayed.
##### Switch off completly 
When the main screen is displayed for a few seconds and the display wasn’t touched the watch will be powerd off completly. You can wake up the watch by lifting your wrist or double click. After waking up the watch needs a few seconds before displaying the time. This method requires the least battery power. 
##### Only switch off the display
When the main screen is displayed for a few seconds and the display wasn’t touched the display of the watch will shut off.  You can wake up the watch by bringing the watch in a position so that you can read the display.  The time will be displayed much faster but this method needs much more battery power.
##### Don’t switch off the watch
The watch won’t be powerd off – until the battery is empty.

#### Language
At the moment there are two languages: english and german. Choose one language by typing the languange on the display. After touching the screen the choosed language will be saved and the main screen will be displayed.








