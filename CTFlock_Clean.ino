//***CTF Cipher lock with button pressed  ***
//***Used in 2019                         ***
//***Teams can play  on 1 device          ***
//***Version 1.0                          ***
//*** Kudos to the Arduino Rotary Encoder Tutorial by Dejan Nedelkovski, www.HowToMechatronics.com
//*** Kudos to Adafruit for the display driver software

//*** START Declerations and includes***********
//*** Don't know why but do NOT use D7 as an input on a NodeMCU board.. just don't try
//*** HARDWARE
//Pin D2 = SDC of OLED.096 display
//Pin D3 = SDA of OLED.096 display

#define outputA D6    // Rotary encoder output
#define outputB D5    // Rotary encoder output
#define rotarsw D4    // Rotary button output and lights up the onboard LED on NodeMCU board

//*** Encoder stuff ****
int counter = 0; 
int aState;
int aLastState;
int pressed;
int pressedLastState; 

//*** CTF FLAG ****
const char *FLAG = "ABCD1234"; //Not more than 10 positions - shared by team A and B

//For Display the Adafruit driver files were used
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#define OLED_RESET LED_BUILTIN
Adafruit_SSD1306 display(OLED_RESET);
#if (SSD1306_LCDHEIGHT != 64)
#error("Height incorrect, please fix Adafruit_SSD1306.h!");
#endif

//For CTFLock program
int Sequence [] = { -1,2,-3,-4,5,-6 } ; //change if you like
const int Sequence_SIZE = sizeof(Sequence) / sizeof(int);
int Stage = 0;
int ticker;                            // variable with the rotary ticks
int sensitivity =3;                    // divide counter by sensitivity for smoother counting
long measuretick = 0            ;      // the last time the range was checked
long refresh_measuretick = 300  ;      // time in milliseconds before next display refresh call
boolean newtick = false         ;      // variable to indicate a tick
boolean armed = true            ;      // variable to indicate if the challenge is still on
//**END Declerations and includes***********


//****************RUN once*****************
void setup() {
  delay(1000);
  Serial.begin(115200); // let this run also in production mode Never know where you will need this
  Serial.println();
  //Rotary stuff
  pinMode (outputA,INPUT);
  pinMode (outputB,INPUT);
  pinMode (rotarsw,INPUT);
  aLastState = digitalRead(outputA);
  pressedLastState = digitalRead(rotarsw);
  pressed = pressedLastState ;
  
  //Start Display 
  // by default, we'll generate the high voltage from the 3.3v line internally! (neat!)
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3C (for the 128x32)
  delay(1000);
  Welcome();
  delay(3000);
  Detailscreen();
 
  //Reset measuretick
  measuretick = millis();
}
//****************END of RUN once*************


//**START***MAIN LOOP*****************************************************************************
void loop() {
  aState = digitalRead(outputA); // Reads the "current" state of the outputA
  // If the previous and the current state of the outputA are different, that means a Pulse has occured
  if (aState != aLastState){     
     // If the outputB state is different to the outputA state, that means the encoder is rotating clockwise
     if (digitalRead(outputB) != aState) { 
         ticker ++;
         counter = (ticker / sensitivity);
     } else {
         ticker --;
         counter = (ticker / sensitivity);
     }
  } 
  aLastState = aState; // Updates the previous state of the outputA with the current state
  
  pressed = digitalRead(rotarsw);
     if ( (pressed == 0) && (pressedLastState == 1) && (armed == true) ) { 
        Serial.println("pressed");
        pressedLastState = 0;
        if (counter == Sequence[Stage]){
           Stage ++;
           }
        if (Stage == Sequence_SIZE){
           armed = false;
           flagscreen();
        }
     }
     if ( (pressed == 1)&&(pressedLastState == 0)) { 
         Serial.println("unpressed");
         pressedLastState = 1;
     }
 
  if ((millis() - measuretick) > refresh_measuretick) {
       newtick = true ;
       measuretick = millis();  //update last measurement time
     }

  if ( (newtick == true) && (armed == true) ) {
     // this block is handy for debug only as you can read stuff in the serial port debug stream
     // no need to take it out in real life CTF 
     Detailscreen();      // If challenge is active then update screen
     Serial.print("Stage = ");
     Serial.print(Stage);
     Serial.print(" and Sequence_SIZE = ");
     Serial.println(Sequence_SIZE);
     Serial.print(" NEXT number = ");
     Serial.print(Sequence[Stage]); Serial.print(" Button status ");
     Serial.print(pressed);Serial.print(pressedLastState);  Serial.print(" Counter: "); Serial.println(counter);
     newtick = false ; //reset the boolean
    }    
}
//**END***MAIN LOOP******************************************************************************


//*********Display Stuff*********
void Welcome() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("----------");
  display.println("The Rotary");
  display.println(" CTF-Lock");
  display.println("__________");
  display.display();
}

void Detailscreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("Number:");
  display.setTextSize(4);
  display.print("  ");
  display.println(counter);
  display.setTextSize(2);
  display.print("# ");
  display.print(Stage);
  display.print(" of ");
  display.println(Sequence_SIZE);
  display.display();
}

void flagscreen() {
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,0);
  display.println("**!FLAG!**");
  display.println("----------");
  display.println(FLAG);
  display.println("----------");
  display.display();
  delay(20000);
  Stage = 0;
  counter = 0;
  Welcome();
  delay(3000);
  Detailscreen();
  armed = true;
}

