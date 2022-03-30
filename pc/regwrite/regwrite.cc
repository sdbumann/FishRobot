#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <windows.h>

#include "utils.h"
#include "remregs.h"
#include "robot.h"
#include "regdefs.h"

using namespace std;

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface

int main(int argc, char *argv[])
{
    if (argc < 3)
        cout << "too few arguments" << endl;

    CRemoteRegs regs;
    if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
      return 1;
    }

    int reg;
    uint8_t buffer[argc-2];

    sscanf(argv[1], "%d", &reg);
    for (int i = 0; i < argc-2; i++){
        if(!sscanf(argv[i+2], "%d", (int*) (buffer+i))) {
            cout << "ERROR! for some reason" << endl;
            return 1;
        }
    }

    // cout << "reg " << reg << ": ";
    // for (int i = 0; i < argc-2; i++){
    //     cout << (int) buffer[i] << ((i<argc-3) ? ", " : "\n");
    // }

     regs.set_reg_mb((uint8_t) reg, buffer, argc-2);
}
