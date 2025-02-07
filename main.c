#include "gd32vf103.h"
#include "drivers.h"
#include "adc.h"
#include "pwm.h"

// Define meaningful names for key mappings
#define MAX_DIGIT 9
#define KEY_A 10
#define KEY_C 12
#define KEY_D 13
#define KEY_STAR 14
#define KEY_HASH 15

void Load_value(int keyPress, int *Prev_value, int *PendingApply);
void Display(int value);

int main(void) {
    int key, pKey = -1, Prev_value = 0, PendingApply = 0;
    int lookUpTbl[16] = {1, 4, 7, 14, 2, 5, 8, 0, 3, 6, 9, 15, 10, 11, 12, 13};

    // Initialize peripherals
    t5omsi();  // Timer 5 (1 kHz)
    colinit(); // Column toolbox
    l88init(); // 8x8 LED toolbox
    keyinit(); // Keyboard toolbox

    // PWM Initialization
    T1powerUpInitPWM(0x2);  // Timer #1, Channel #2 for PWM on A0
    T1setPWMch1(0); // Set initial PWM duty cycle to 0

    while (1) {
        if (t5expq()) {
            l88row(colset());

            if ((key = keyscan()) >= 0) {
                int keyPress = lookUpTbl[key];
                if (keyPress <= MAX_DIGIT) {  // Ensure value is between 0 and 9
                    Load_value(keyPress, &Prev_value, &PendingApply);
                    Display(Prev_value);
                } else if (keyPress == KEY_A) {  // 'A' key for setting PWM to 100%
                    T1setPWMch1(100);
                } else if (keyPress == KEY_C) {  // 'C' key for setting PWM to 0%
                    T1setPWMch1(0);
                } else if (keyPress == KEY_D) {  // 'D' key for applying stored brightness
                    T1setPWMch1(Prev_value);
                } else if (keyPress == KEY_STAR) {  // '*' key for deleting last digit
                    Prev_value /= 10;
                    Display(Prev_value);
                } else if (keyPress == KEY_HASH) {  // '#' key for resetting value
                    Prev_value = 0;
                    Display(Prev_value);
                }
            }

            // Clear LED rows 1 to 6
            for (int i = 1; i <= 6; i++) {
                l88mem(i, 0);
            }
        }
    }
}

void Display(int value) {
    l88mem(0, value);
}

void Load_value(int keyPress, int *Prev_value, int *PendingApply) {
    int new_value = (*Prev_value * 10) + keyPress;
    if (new_value <= 100) {
        *Prev_value = new_value;
    }
}
