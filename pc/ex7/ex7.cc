#include <iostream>
#include <cstdlib>
#include <stdint.h>
#include <windows.h>
#include <iostream>
#include <fstream>

using namespace std;
#include "trkcli.h"
#include "utils.h"

#include "remregs.h"
#include "robot.h"
#include "regdefs.h"

using namespace std;

const uint8_t RADIO_CHANNEL = 201;         ///< robot radio channel
const char* INTERFACE = "COM1";            ///< robot radio interface

const char* TRACKING_PC_NAME = "biorobpc6";   ///< host name of the tracking PC
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
  uint32_t frame_time, frame_time_prev = 0;
  double start_time = time_d();
  double time;

  //open or create data.csv
  ofstream file;
  file.open ("data.csv");
  file<<"time,id,x,y\n";

  uint8_t frequency[4]={ENCODE_PARAM_8(1,0,1.5), ENCODE_PARAM_8(1,0,1.5), ENCODE_PARAM_8(1,0,1.5), ENCODE_PARAM_8(1,0,1.5)};
  uint8_t phase_shift[4]={ENCODE_PARAM_8(0,90, 90), ENCODE_PARAM_8(0,90, 90), ENCODE_PARAM_8(0,90, 90), ENCODE_PARAM_8(0,90, 90)};
  uint8_t amplitude[4]={ENCODE_PARAM_8(20,0,40), ENCODE_PARAM_8(20,0,40), ENCODE_PARAM_8(20,0,40), ENCODE_PARAM_8(40,0,40)};
  uint8_t offset[4]=   {ENCODE_PARAM_8(0,-20,20), ENCODE_PARAM_8(0,-20,20), ENCODE_PARAM_8(0,-20,20), ENCODE_PARAM_8(0,-20,20)};

  frequency[0]=frequency[1]=frequency[2]=frequency[3]=ENCODE_PARAM_8(1.5,0,1.5);

  phase_shift[0]=ENCODE_PARAM_8(0,-90,90);
  phase_shift[3]=ENCODE_PARAM_8(20,-90,90);
  phase_shift[1]=ENCODE_PARAM_8(-90,-90,90);
  phase_shift[2]=ENCODE_PARAM_8(90,-90,90);

  amplitude[0]=amplitude[3]=ENCODE_PARAM_8(20,0,40);
  amplitude[1]=amplitude[2]=ENCODE_PARAM_8(0,0,40);

  offset[0]=offset[1]=offset[2]=offset[3]=ENCODE_PARAM_8(0,-20,20);


  Sleep(10);

  regs.set_reg_mb(1, frequency, 4);
  regs.set_reg_mb(2, phase_shift, 4);
  regs.set_reg_mb(3, amplitude, 4);
  regs.set_reg_mb(4, offset, 4);
  cout<<"intialized"<<endl;

  regs.set_reg_b(REG8_MODE, 1);
  cout<<"started"<<endl;

  //Gets the current position
  if (!trk.update(frame_time)) {
    return 1;
  }

  while (!kbhit()) {

    // Waits 10 ms before getting the info next time (anyway the tracking runs at 15 fps)
   do {
      //Gets the current position
      if (!trk.update(frame_time)) {
        return 1;
      }
      Sleep(10);
    } while (frame_time_prev == frame_time);
    frame_time_prev = frame_time;

    time = (time_d() - start_time);

    cout.precision(2);

    // Gets the ID of the first spot (the tracking system supports multiple ones)
    int id = trk.get_first_id();

    // Reads its coordinates (if (id == -1), then no spot is detected)
    if (id != -1 && trk.get_pos(id, x, y)) {
      // print to cmd
      cout << "(" << x << ", " << y << ")" << " m      \n";

      //print to csv file
      file<<time<<","<<id<<","<<x<<","<<y<<"\n";//time,id,x,y
    }
    else {
      cout << "(not detected)" << '\r';
    }

  }

  regs.set_reg_b(REG8_MODE, 0);
  cout<<"stopped"<<endl;

  //close file data.csv
  file.close();

  // Clears the console input buffer (as kbhit() doesn't)
  FlushConsoleInputBuffer(GetStdHandle(STD_INPUT_HANDLE));
}
