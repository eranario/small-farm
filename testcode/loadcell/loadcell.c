
#include "simpletools.h"
#include "wifi.h"

#define LOAD_DAT 8
#define LOAD_CLK 9
#define LOAD_SLOPE 10

static volatile int mass_A = 0; //shared

unsigned int stack[44+128];

int event = 0, id, handle;
int getFromPageId;

void loadcell(void *par);

int main()
{
  int wifi_timer = 0, dt = 0, dt2 = 0, RH_timer = 0;
  
  cogstart(loadcell,NULL,stack,sizeof(stack));
  
  wifi_start(31,30,115200,WX_ALL_COM);
  
  getFromPageId = wifi_listen(HTTP, "/bot");
  print("getFromPageId = %d\n", getFromPageId);
  
  // CLKFREQ stores number of system clock ticks in 1 second
  // CNT stores current number of system clock ticks elapsed
  
  wifi_timer = CNT;
  dt = CLKFREQ/5;
  dt2 = CLKFREQ;
  
  while(1)
  {
    
   if(CNT - wifi_timer > dt)
   {
     wifi_poll(&event, &id, &handle);
     print("event = %c, id = %d, handle = %d\r", event, id, handle);
     
     if(event == 'G')
     {
       if(id == getFromPageId)
       {
         print("Incoming GET request, sending %d\n", mass_A);
         wifi_print(GET, handle, "%d", mass_A);
       }         
     }       
     wifi_timer = CNT;
     pause(500);
    }
        
   }   
}

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