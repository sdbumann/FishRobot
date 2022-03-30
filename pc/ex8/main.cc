#include <iostream>
#include <math.h>
#include <cstdlib>
#include <stdint.h>
#include <windows.h>
#include <conio.h>
#include <fstream>

//#include "utils.h"
#include "remregs.h"
#include "robot.h"
#include "regdefs.h"

using namespace std;

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface


int main(){

  CRemoteRegs regs;
  if (!init_radio_interface(INTERFACE, RADIO_CHANNEL, regs)) {
    return 1;
  }

  int speed=0;
  int rcl=0;

  uint8_t frequency[4] = {ENCODE_PARAM_8(0,0,1.5), ENCODE_PARAM_8(0,0,1.5), ENCODE_PARAM_8(0,0,1.5),ENCODE_PARAM_8(0,0,1.5)};
  uint8_t phase_shift[4]={0,0,0,0};
  uint8_t amplitude[4] = {ENCODE_PARAM_8(0,0,1), ENCODE_PARAM_8(0,0,1), ENCODE_PARAM_8(0,0,1), ENCODE_PARAM_8(0,0,1)};
  uint8_t offset[4]    = {ENCODE_PARAM_8(0,-1,1),ENCODE_PARAM_8(0,-1,1),ENCODE_PARAM_8(0,-1,1), ENCODE_PARAM_8(0,-1,1)};

        for (int i=0; i<4; i++){
          frequency[i] = ENCODE_PARAM_8(0,0,1.5);
        }


  // Reboots the head microcontroller to make sure it is always in the same state
  reboot_head(regs);

  regs.set_reg_b(REG8_MODE, 1);



  while(1){
    int ch = getch();
    // cout << ch << endl;

    if(ch==13 || ch==32 || ch==3){//if key press enter, space or c -> exit program
      // cout << ch << endl;
      cout << "enter = exit\n" << endl;
      break;
    }
    if(ch==224){
        ch=getch();
        switch (ch){
          case 72:
            cout << "up\n" <<endl;
            speed++;
            if (speed > 5)
              speed=5;

            // frequency[0]=frequency[1]=frequency[2]=frequency[3]=ENCODE_PARAM_8(1,0,1.5);

            amplitude[0]=amplitude[3]=ENCODE_PARAM_8(20,0,40);
            amplitude[1]=amplitude[2]=ENCODE_PARAM_8(0,0,40);

            offset[0]=offset[1]=offset[2]=offset[3]=ENCODE_PARAM_8(0,-20,20);
            break;

          case 80:
            cout << "down\n" <<endl;

            speed--;
            if (speed < 0)
              speed=0;


            // frequency[0]=frequency[1]=frequency[2]=frequency[3]=ENCODE_PARAM_8(0,0,1.5);


            amplitude[0]=amplitude[3]=ENCODE_PARAM_8(20,0,40);
            amplitude[1]=amplitude[2]=ENCODE_PARAM_8(0,0,40);


            break;

          case 77:
            cout << "right\n" <<endl;
            //write stuff in fish register

            rcl++;
            if (rcl >= 1){
              rcl = 1;
            }


            amplitude[0]=amplitude[3]=ENCODE_PARAM_8(20,0,40);
            amplitude[1]=amplitude[2]=ENCODE_PARAM_8(0,0,40);

            break;

          case 75:
            cout << "left\n" <<endl;
            //write stuff in fish register


            rcl--;
            if (rcl <= -1){
              rcl = -1;
            }



            amplitude[0]=amplitude[3]=ENCODE_PARAM_8(20,0,40);
            amplitude[1]=amplitude[2]=ENCODE_PARAM_8(0,0,40);

            break;

          default:
            cout << "not defined\n" << endl;
        }

        phase_shift[0]=ENCODE_PARAM_8(0,-90,90);
        phase_shift[3]=ENCODE_PARAM_8(15*speed,-90,90);
        phase_shift[1]=ENCODE_PARAM_8(-90,-90,90);
        phase_shift[2]=ENCODE_PARAM_8(90,-90,90);

        offset[0]=offset[1]=offset[2]=offset[3]=ENCODE_PARAM_8(0,-20,20);
              switch (rcl){
                case 1:
                  offset[1]=ENCODE_PARAM_8(-20,-20,20);
                  break;
                case -1:
                  offset[2]=ENCODE_PARAM_8(20,-20,20);
                  break;
                default:
                  break;
              }

        for (int i=0; i<4; i++){
          frequency[i] = ENCODE_PARAM_8(1.5/5*speed,0,1.5);
        }

        regs.set_reg_mb(1, frequency, 4);
        regs.set_reg_mb(2, phase_shift, 4);
        regs.set_reg_mb(3, amplitude, 4);
        regs.set_reg_mb(4, offset, 4);
    }
  }
  cout << "exit programp\n" <<endl;
  regs.set_reg_b(REG8_MODE, 0);
  cout<<"stopped"<<endl;
}
