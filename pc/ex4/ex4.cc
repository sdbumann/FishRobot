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
    double dt;

    if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
      return 1;
    }

    // Reboots the head microcontroller to make sure it is always in the same state
    reboot_head(regs);

    // // motor demo
    // regs.set_reg_b(REG8_MODE, 1); // start
    // while(!kbhit())
    //   Sleep(100);
    // regs.set_reg_b(REG8_MODE, 0); // stop
    // ext_key();

    // Sleep(2000);  // wait 

    // cout << "strting nxt op" << endl;

    // tracking a 1 Hz sine wave
    regs.set_reg_b(REG8_MODE, 2); // start
    while (!kbhit()){
        dt = time_d();
        regs.set_reg_b(2, ENCODE_PARAM_8(sin(2*M_PI*dt), -1, 1));
    }
    regs.set_reg_b(REG8_MODE, 0); // stop
}
