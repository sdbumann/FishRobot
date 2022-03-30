#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <windows.h>
#include "trkcli.h"
#include "utils.h"

#include "remregs.h"
#include "robot.h"
#include "regdefs.h"

using namespace std;

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface

const char* TRACKING_PC_NAME = "biorobpc11";   ///< host name of the tracking PC
const uint16_t TRACKING_PORT = 10502;          ///< port number of the tracking PC

int main()
{
  CTrackingClient trk;


  // Connects to the tracking server
  if (!trk.connect(TRACKING_PC_NAME, TRACKING_PORT)) {
    return 1;
  }

  CRemoteRegs regs;
  if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
    return 1;
  }

  // Reboots the head microcontroller to make sure it is always in the same state
  reboot_head(regs);


  double x=0, y=0;
  uint32_t rgb;

  while (!kbhit()) {
    uint32_t frame_time;
    // Gets the current position
    if (!trk.update(frame_time)) {
      return 1;
    }


    cout.precision(2);

    // Gets the ID of the first spot (the tracking system supports multiple ones)
    int id = trk.get_first_id();

    // Reads its coordinates (if (id == -1), then no spot is detected)
    if (id != -1 && trk.get_pos(id, x, y)) {
      cout << "(" << fixed << x << ", " << y << ")" << " m      \r";
      rgb = ((uint32_t) (uint8_t) (64*x/6) << 16) | ((uint32_t) 64 << 8) | (uint8_t) (64*y/2) ;
      regs.set_reg_dw(0, rgb);
    } else {
      cout << "(not detected)" << '\r';
    }

    // Waits 10 ms before getting the info next time (anyway the tracking runs at 15 fps)
    Sleep(10);
  }

  // Clears the console input buffer (as kbhit() doesn't)
  FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}
