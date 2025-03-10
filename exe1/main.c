#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int LED_PIN_R = 4;

volatile int flag_f_r = 0;
volatile int timer_state = 0;

bool timer_callback(repeating_timer_t *rt) {
    timer_state = 1;
    return true; 
}

void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) { // fall edge
        flag_f_r = !flag_f_r;
    } else if (events == 0x8) { // rise edge
    }
}

int main() {

    stdio_init_all();
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);

    repeating_timer_t timer;

    while (true) {
        if (flag_f_r && !timer_state) {
            add_repeating_timer_ms(500, timer_callback, NULL, &timer);
        }
        if (timer_state){
            gpio_put(LED_PIN_R, !gpio_get(LED_PIN_R) );
            timer_state = 0;
        }
        if (!flag_f_r) {
            cancel_repeating_timer(&timer);
            gpio_put(LED_PIN_R, 0);
        }
    }
}