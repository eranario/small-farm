#include "simpletools.h"

int pin, state;

int main()
{
  set_direction(26, 1);        // 1 -> output
  set_direction(27, 1);

  print("Type a 26/27 1/0 variations on:\r");
  print("io=27state=1\r");
  print("Then, press Enter\r\r");

  while(1)
  {
    scan("%d%d", &pin, &state);
    print("pin = %d, state = %d\r\r", pin, state);
    set_output(pin, state);
  }
}
