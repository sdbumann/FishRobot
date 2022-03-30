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
    if (argc < 2){
        cout << "too few arguments" << endl;
        return 1;
    }

    if (argc > 2){
        cout << "too many arguments" << endl;
        return 1;
    }

    CRemoteRegs regs;
    if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
      return 1;
    }

    char command[20];

    sscanf(argv[1], "%s", command);

    if (strstr(command, "start") || strstr(command, "Start") || strstr(command, "START")){
        cout << "starting robot" << endl;
        regs.set_reg_b(0, 1);
        return 0;
    }

    if (strstr(command, "stop") || strstr(command, "Stop") || strstr(command, "STOP")){
        cout << "stopping robot" << endl;
        regs.set_reg_b(0, 0);
        return 0;
    }

}
