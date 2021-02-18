/*
  Hello Message.c

  Display a hello message in the serial terminal.

  http://learn.parallax.com/propeller-c-start-simple/simple-hello-message
*/

#include "simpletools.h"                      // Include simpletools header

int main()                                    // main function
{
  wifi_start(31, 30, 115200, WX_ALL_COM);
  print("Leave a Network\r");
 
  // Leaves network where it was a station and sets
  // the Wi-Fi module's mode to AP.

  wifi_leave(AP);

  // Verify mode after leaving the network.

  int mode = wifi_mode(CHECK);

  switch(mode)
  {
    case STA:    //0xf4:
      print("mode=STA\r");
      break;
    case AP:     //0xf3  
      print("mode=AP\r");
      break;
    case STA_AP: //0xf2
      print("mode=STA+AP");
      break;
  }                             // Display a message
}