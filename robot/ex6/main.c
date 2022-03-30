#include "hardware.h"
#include "registers.h"
#include "modes.h"
#include "can.h" 
#include "module.h"
#include "robot.h"

int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data);

int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data)
{
  switch (operation)
  {
    case ROP_WRITE_32:
      if (address == 1) {
          set_rgb(0xFF&(radio_data->dword>>16), 0xFF&(radio_data->dword>>8), 0xFF&(radio_data->dword));
      }
      break;
  }
  return FALSE;
}


int main(void)
{
    hardware_init();
    registers_init();

    set_reg_value_dw(72, MREG32_LED, 0);
    set_reg_value_dw(21, MREG32_LED, 0);

    // Registers the register handler callback function
    radio_add_reg_callback(register_handler);

    // Changes the color of the led (green) to show the boot
    set_color_i(2, 0);

    while (1){
        pause(ONE_SEC);
    }

    return 0;
}
