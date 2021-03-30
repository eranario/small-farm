/* 
Program just to sample humidity and temperature sensor CM2302
and test wifi module along with other peripherals
*/

#include "simpletools.h"
#include "wifi.h"

#define ACRELAY 12
#define DCRELAY 13

//wifi poll variables
int event = 0, id, handle;
int getFromPageId;
char buttonCmd = 0;

int main()
{
  int wifi_timer = 0, dt = 0, dt2 = 0;
  
  wifi_start(31,30,115200,WX_ALL_COM);
  
  getFromPageId = wifi_listen(HTTP, "/bot");
  print("getFromPageId = %d\n", getFromPageId);
  
  wifi_timer = CNT;
  dt = CLKFREQ/5;
  dt2 = CLKFREQ;
  
  while(1)
  {
    
   if(CNT - wifi_timer > dt)
   {
     wifi_poll(&event, &id, &handle);
     print("event = %c, id = %d, handle = %d\r", event, id, handle);
     
     switch(event)
     {
       case 'N': //None
                break;
       case 'P': //HTTP POST
                if(id == getFromPageId)
                {
                  wifi_scan(POST, handle, "go%c", &buttonCmd); //look for web page button event
                  print("go=%c \n", buttonCmd); //troubleshooting on Terminal
                  
                  if(buttonCmd != 0)
                  {
                    
                    switch(buttonCmd)
                    {
                      
                      case 'R': 
                                high(DCRELAY);
                                break;
                      
                      case 'O':
                                low(DCRELAY);
                                break;
                      
                    }                      
                    
                  }                    
                }
        //case 'G': //HTTP GET
                  //inset temp_humidity code here                      
     }              
     wifi_timer = CNT;
     pause(500);
    }
        
   }   
}