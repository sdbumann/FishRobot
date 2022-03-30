#include "hardware.h"
#include "registers.h"
#include "modes.h"
#include "can.h"
#include "module.h"
#include "robot.h"

#define IMODE_GO 1

#define NB_MOT 4

#define AX 20.
#define AR 20.
#define AP 20.
#define H  27.8e-3 //[s]

const  uint8_t mot_addr[] = {72,73,74,21};

int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data);
void controller(void);


static float frequency[NB_MOT];
static float phase_shift[NB_MOT];
static float amplitude[NB_MOT];
static float offset[NB_MOT];

float x[NB_MOT],   x_prev[NB_MOT], x_prevprev[NB_MOT];
float r[NB_MOT],   r_prev[NB_MOT], r_prevprev[NB_MOT];
float p[NB_MOT],   p_prev[NB_MOT], p_prevprev[NB_MOT];
float phi[NB_MOT], phi_prev[NB_MOT];
float theta[NB_MOT];


int8_t register_handler(uint8_t operation, uint8_t address, RadioData* radio_data) {
  if (operation == ROP_WRITE_MB){
    switch (address){
        case 1:
            for(uint8_t i = 0; i < NB_MOT; i++)
                frequency[i] = DECODE_PARAM_8(radio_data->multibyte.data[i], 0, 1.5);
            return TRUE;
        case 2:
            for(uint8_t i = 0; i < NB_MOT; i++)
                phase_shift[i] = DECODE_PARAM_8(radio_data->multibyte.data[i], -1.5, 1.5);
            return TRUE;
        case 3:
            for(uint8_t i = 0; i < NB_MOT; i++)
                amplitude[i] = DECODE_PARAM_8(radio_data->multibyte.data[i], 0, 0.7);
            return TRUE;
        case 4:
            for(uint8_t i = 0; i < NB_MOT; i++)
                offset[i] = DECODE_PARAM_8(radio_data->multibyte.data[i], -0.35, 0.35);
            return TRUE;
    }
  }
  return FALSE;
}

void controller() {
    uint8_t i, j;
    // uint32_t cycletimer;

    for (i=0;i<NB_MOT;i++){
        x_prev[i] = 0;
        x_prevprev[i] = 0;
        r_prev[i] = 0;
        r_prevprev[i] = 0;
        p_prev[i] = 0;
        p_prevprev[i] = 0;
        phi_prev[i] = 0;
        theta[i] = 0;
    }

    for (i=0; i<NB_MOT; i++){
        start_pid(mot_addr[i]);
    }

    while(reg8_table[REG8_MODE] == IMODE_GO) {
        // cycletimer = getSysTICs();

        for (i=0;i<NB_MOT;i++){ // send to motors
            bus_set(mot_addr[i], MREG_SETPOINT, RAD_TO_OUTPUT_BODY(theta[i]));
        }

        for (i=0;i<NB_MOT;i++){ // calculation loop 1
            x[i] = (AX*AX/4. * offset[i]      + (AX/H+2./H/H)*x_prev[i] - x_prevprev[i]/H/H) / (1./H/H + AX*AX/4. + AX/H);
            r[i] = (AR*AR/4. * amplitude[i]   + (AR/H+2./H/H)*r_prev[i] - r_prevprev[i]/H/H) / (1./H/H + AR*AR/4. + AR/H);
            p[i] = (AP*AP/4. * phase_shift[i] + (AP/H+2./H/H)*p_prev[i] - p_prevprev[i]/H/H) / (1./H/H + AP*AP/4. + AP/H);

            x_prevprev[i] = x_prev[i];
            x_prev[i]     = x[i];
            r_prevprev[i] = r_prev[i];
            r_prev[i]     = r[i];
            p_prevprev[i] = p_prev[i];
            p_prev[i]     = p[i];
        }

        for (i=0;i<NB_MOT;i++){ // calculation loop 2
            phi[i] = H*frequency[i];
            for (j=0;j<NB_MOT;j++){
                phi[i] += H * ((i==j)?0:.5) * r[j] * sin(phi_prev[j] - phi_prev[i]);
            }
            phi[i] += phi_prev[i];
            theta[i] = x[i] + r[i]*sin(phi[i]+p[i]);

            phi_prev[i] = phi[i];
        }

        // while ( (float) getElapsedSysTICs(cycletimer) < H * sysTICSperSEC){
            pause(ONE_MS);
        // }

    }

    for (i=0;i<NB_MOT;i++){
        bus_set(mot_addr[i], MREG_SETPOINT, DEG_TO_OUTPUT_BODY(0.0));
    }

    pause(ONE_SEC);

    for (i=0;i<NB_MOT;i++){
        bus_set(mot_addr[i], MREG_MODE, MODE_IDLE);
    }
}



int main(void)
{
    hardware_init();
    registers_init();

    // Changes the color of the led (red) to show the boot
    set_color_i(4, 0);

    init_body_module(72);
    init_limb_module(73);
    init_limb_module(74);
    init_body_module(21);

    reg8_table[REG8_MODE] = 0;

    // turn off LEDs for tracking
    set_reg_value_dw(72, MREG32_LED, 0);
    set_reg_value_dw(21, MREG32_LED, 0);

    // Registers the register handler callback function
    radio_add_reg_callback(register_handler);

    // Changes the color of the led (green) to show the boot
    set_color_i(2, 0);

    // control loop
    while (1) {
        if (reg8_table[REG8_MODE] == IMODE_GO){
            set_rgb(0, 127, 0);
            controller();
        }
    }


    return 0;
}
