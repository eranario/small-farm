/* IoT Program to allow HTTP POST and GET commands from a web page
   interact & control a Parallax Feedback 360° High Speed Servo
   and communicate with backpacks mounted on chickens using XBee
*/

#include "simpletools.h"
#include "wifi.h"
#include "dht22.h"

// pin definitions for using Jed's board w/Charlieplexing
#define LEDPIN 1
#define LOWPIN 2
#define INPUTPIN 3
#define RED_BTN 4
#define GREEN_BTN 5
#define BLUE_BTN 6
#define WHITE_BTN 7
#define LOAD_DAT 8
#define LOAD_CLK 9
//Xbee pins
#define RXPIN 10
#define TXPIN 11

#define ACRELAY 12
#define DCRELAY 13
#define TEMPRH 14
#define BUZZER_PIN 15
#define KERNEL_MOTOR 17

#define MYADDR ACE
#define NUM_BKPACKS 4
#define LOAD_SLOPE 10  // Load Cell slope


#define MODE 0
#define BAUDRATE 9600

// Use static volatile to declare global variables for sharing between functions running in different cogs. 
// static volatile int loadcellzero = 0; // shared
 static volatile int mass_A = 0; // shared
 
 unsigned int stack[44 + 128]; 

 int event = 0, id, handle, navId ;  // wifi poll variables
 char buttonCmd = 0, treat = 0;

 void loadcell(void *par);


int main()
{
  
  int wifi_timer = 0, dt = 0, RH_timer = 0, dt2 = 0;
  int Temperature=0, TempDecimal, Humidity=0, HumidDecimal;


  cogstart(loadcell, NULL, stack, sizeof(stack));  // Start the load cell code in a new cog.

  wifi_start(31, 30, 115200, WX_ALL_COM); // all communication via WiFi. Requires the SEL line tied to 3.3 V.
//  pause(1000);

  low(0); // Set pin 0 to low. This is to suppress any possible noise due to Wifi on this pin.
  
  wifi_timer = CNT; // Mark current time by storing in variable 
  dt = CLKFREQ/5;    // Pick a timeout, 200 milliseconds in this case
  dt2 = CLKFREQ;    // Pick a timeout, 1 second in this case

  navId = wifi_listen(HTTP, "/bot"); // Setup listener for HTTP communications
  
  // CLKFREQ stores number of system clock ticks in 1 second.
  // CNT stores current number of system clock ticks elapsed.
  
  while(1)
  {
    
   if(CNT - RH_timer > dt2) // check temperature & humidity once every second;
    {

      dht22_read(TEMPRH);
      Temperature = dht22_getTemp(FAHRENHEIT);
      Temperature = (Temperature+5)/10;
      Humidity = dht22_getHumidity();
      Humidity = (Humidity+5)/10;
      RH_timer = CNT;
//                 print("T= %d RH= %d \n", Temperature, Humidity);
    }

   if(CNT - wifi_timer > dt) // check wifi once every 200 ms;
    {
                          
     wifi_poll(&event, &id, &handle); 
//                 print("event %c  mass %d \n", event, mass_A);
     switch(event)
      { 
       case 'N': //None
//                 print("event %c  mass %d \n", event, mass_A);
                 break;
       case 'P': //HTTP POST
                if(id == navId)
                 {
                   wifi_scan(POST, handle, "go%c", &buttonCmd); //look for web page button event
                   print("go=%c \n", buttonCmd);  // troubleshooting on Terminal
                   if(buttonCmd != 0)
                    {
                     switch(buttonCmd)
                      {
                       case 'R': 
                                 high(DCRELAY); // Turn relay on
 //                                print(" relay on \n");
                                 break;
                       case 'O':
                                 low(DCRELAY);  // Turn relay off
                                 break;
                      } 
                   }  // end if buttonCmd          
                 }        
                 break;
        case 'G': //HTTP GET
                 if(id == navId)
                  {
                   wifi_print(GET, handle, "%d,%d,%d", mass_A, Temperature, Humidity); // send mass to web page for display
//                   print("event: %c\n", event);
                  } 
                 break;
      }    
     wifi_timer = CNT;
    } // end wifi-timer check    
                        

//   if(mass_A < 51) {}
  

//if(CNT - mass_timer > 30*CLKFREQ) mass_timer = CNT;
    
  }// end of while(1)
} // end of main




void loadcell(void *par)
{
  int Bits, i, Sum, startcount=0, loadcellzero=0;

  // initialize the HX711
  high(LOAD_CLK); // set pin SCK high
  // CLKFREQ stores number of system clock ticks in 1 second.
  // CNT stores current number of system clock ticks elapsed.
  waitcnt(CLKFREQ/10000 + CNT);  // wait 100 microseconds to power down the HX711
  low(LOAD_CLK); // set pin SCK low, reset chip and enter normal operation mode.
  // At power on or reset the input selection defaults to Channel A with a gain of 128

  while(1)
   {
    while( input(LOAD_DAT) ) 	;  // DOUT is high when data is not ready
    Bits = 0;
    for(i=0; i<24; i++)  // clock out the 24 data bits starting with the MSB
     {
      Bits = Bits << 1;  // multiply by 2
      high(LOAD_CLK); // set pin SCK high, 
      if (input(LOAD_DAT) ) { if(i==0) Bits = 510;  Bits += 1; } // The 510 value is the 2's complement
                                                             // for negative values when converting 
                                                             // from 24 bits to a 32 bit integer.
      low(LOAD_CLK); // set pin SCK low
     }
    Sum = (Bits >> 5); // shift 5 bits, divide by 32, to reduce the 24 bit value
                       // to something that is not noisy.  
    if(startcount < 8) { loadcellzero += Sum >> 3 ; startcount++; } // get the average of 8 readings for the initial zero.
    else Sum -= loadcellzero;                     //Then subtract to zero the load cell.
    if(Sum < 0) Sum = 0;
    mass_A = 2 * Sum / LOAD_SLOPE ;
    
    
    // send the 25th, 26th & 27th pulses to stay with channel A and gain=64 
     for(i=0; i<3; i++) {
      waitcnt(CLKFREQ/100000 + CNT);  // wait 10 microseconds
      high(LOAD_CLK); // set pin SCK high
      waitcnt(CLKFREQ/100000 + CNT);  // wait 10 microseconds
      low(LOAD_CLK); // set pin SCK low
     }    

   }
}