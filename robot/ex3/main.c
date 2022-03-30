#include "hardware.h"
#include "registers.h"
#include "module.h"
#include "robot.h"



const  uint8_t dof_addr[] = {72,73,74,21};
static uint8_t dof_buffer[sizeof(dof_addr)];

static int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data) {
    if ((operation == ROP_READ_MB) && (address == 0)) {
        radio_data->multibyte.size = sizeof(dof_addr);
        for (uint8_t i = 0; i < sizeof(dof_addr); i++) {
          radio_data->multibyte.data[i] = dof_buffer[i]; // CAN't directly read because CAN is also interrupt-based
        }
        return TRUE;
    }
    return FALSE;
}


int main(void) {
    hardware_init();

    // Changes the color of the led (red) to show the boot
    set_color_i(4, 0);

    // Initialises the body module with the specified address (but do not start
    // the PD controller)
    uint8_t i, toggle = 0;

    init_body_module(72);
    init_limb_module(73);
    init_limb_module(74);
    init_body_module(21);

    // Registers the register handler callback function
    radio_add_reg_callback(register_handler);


    // Keeps the LED blinking in green to demonstrate that the main program is
    // still running and registers are processed in background.
    while (1) {
        for(i=0; i<sizeof(dof_addr); i++)
            dof_buffer[i] = bus_get(dof_addr[i], MREG_POSITION); // read DOFs into buffer 
        set_color_i(toggle?2:0, 0);
        toggle = !toggle;
        pause(FIFTY_MS);
    }

    return 0;
  }
