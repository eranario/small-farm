/* 
Program just to sample humidity and temperature sensor CM2302
and test wifi module along with other peripherals
*/

#include "simpletools.h"
#include "abvolts.h"
#include "wifi.h"

int event, id, handle;
int getFromPageId;

int main()
{
  int wifi_timer = 0, dt = 0, dt2 = 0, RH_timer = 0;
  int mlevel = 0;
  
  //initialize analog signal
  ad_init(21,20,19,18);
  
  wifi_start(31,30,115200,WX_ALL_COM);
  
  getFromPageId = wifi_listen(HTTP, "/bot");
  print("getFromPageId = %d\n", getFromPageId);
  
  wifi_timer = CNT;
  dt = CLKFREQ/5;
  dt2 = CLKFREQ;
  
  while(1)
  {
    
   if(CNT - RH_timer > dt2)
   {
     mlevel = (ad_in(0) * 500/4096);
   }
    
   if(CNT - wifi_timer > dt)
   {
     wifi_poll(&event, &id, &handle);
     print("event = %c, id = %d, handle = %d\r", event, id, handle);
     
     if(event == 'G')
     {
       if(id == getFromPageId)
       {
         print("Incoming GET request, sending %d\n", mlevel);
         wifi_print(GET, handle, "%d", mlevel);
       }         
     }       
     wifi_timer = CNT;
     pause(500);
    }
        
   }   
}