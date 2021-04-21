#include "simpletools.h"
#include "wifi.h"

int main()
{
  wifi_start(9, 8, 115200, USB_PGM_TERM);

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
  }     
}