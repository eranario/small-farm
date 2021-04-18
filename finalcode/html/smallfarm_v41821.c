/* 
Program just to sample humidity and temperature sensor CM2302
and test wifi module along with other peripherals
*/

#include "simpletools.h"
#include "wifi.h"
#include "dht22.h"

#define TEMPRH 14

int event, id, handle;
int getFromPageId;

int main()
{
  int wifi_timer = 0, dt = 0, dt2 = 0, RH_timer = 0;
  int Temperature = 0, Humidity = 0;
  
  wifi_start(31,30,115200,WX_ALL_COM);
  
  getFromPageId = wifi_listen(HTTP, "/bot");
  print("getFromPageId = %d\n", getFromPageId);
  
  wifi_timer = CNT;
  dt = CLKFREQ/5;
  dt2 = CLKFREQ;
  
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
     print("T= %d RH= %d \n", Temperature, Humidity);
     
   }
    
   if(CNT - wifi_timer > dt)
   {
     wifi_poll(&event, &id, &handle);
     print("event = %c, id = %d, handle = %d\r", event, id, handle);
     
     if(event == 'G')
     {
       if(id == getFromPageId)
       {
         print("Incoming GET request, sending %d,%d\r", Temperature, Humidity);
         //print("Incoming GET request, sending %d\r", Humidity);
         wifi_print(GET, handle, "%d,%d", Temperature, Humidity);
         //wifi_print(GET, handle, "%d", Humidity);
       }         
     }       
     wifi_timer = CNT;
     pause(500);
    }
        
   }   
}