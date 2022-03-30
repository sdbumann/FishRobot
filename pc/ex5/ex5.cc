#include <math.h>
#include <iostream>
#include "remregs.h"
#include "robot.h"
#include "utils.h"
#include "regdefs.h"

using namespace std;

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface

int main(){
  CRemoteRegs regs;

  if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
    return 1;
  }

  // Reboots the head microcontroller to make sure it is always in the same state
  reboot_head(regs);

  // motor demo
  // regs.set_reg_b(REG8_MODE, 2); // start
  // while(!kbhit())
  //   Sleep(100);
  // regs.set_reg_b(REG8_MODE, 0); // stop
  // ext_key();

  // Sleep(2000);  // wait 

  // cout << "strting nxt op" << endl;
  double amplitude = 10, frequency = .5;
  // tracking a 1 Hz sine wave
  regs.set_reg_b(REG8_MODE, 2); // start
  while (!kbhit()){
    regs.set_reg_b(1, ENCODE_PARAM_8(amplitude, 0, 60));
    regs.set_reg_b(2, ENCODE_PARAM_8(frequency, 0, 2));
    if (amplitude < 60)
      amplitude += 10;
    else {
      amplitude = 10;
      frequency += .2;
      if (frequency >= 2)
        frequency = .5;
    }
    cout << "f: " << frequency << "\ta: " << amplitude << endl;

    Sleep(3000);

  }
  regs.set_reg_b(REG8_MODE, 0); // stop
} 

