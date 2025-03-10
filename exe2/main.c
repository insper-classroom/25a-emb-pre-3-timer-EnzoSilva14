#include "hardware/gpio.h"
#include "pico/stdlib.h"
#include <stdio.h>

const int BTN_PIN_R = 28;
const int BTN_PIN_G = 26;

const int LED_PIN_R = 4;
const int LED_PIN_G = 6;

volatile int flag_f_r = 0;
volatile int flag_f_g = 0;

bool timer_callback_r(repeating_timer_t *rt) {
    bool state_r = gpio_get(LED_PIN_R);
    gpio_put(LED_PIN_R , !state_r);
    return true; 
}
bool timer_callback_g(repeating_timer_t *rt) {
    bool state_g = gpio_get(LED_PIN_G);
    gpio_put(LED_PIN_G , !state_g);
    return true; 
}
void btn_callback(uint gpio, uint32_t events) {
    if (events == 0x4) {
        if (gpio == BTN_PIN_R)
            flag_f_r = 1;
        else if (gpio == BTN_PIN_G)
            flag_f_g = 1;
    }
}

int main() {
    gpio_init(LED_PIN_R);
    gpio_set_dir(LED_PIN_R, GPIO_OUT);

    gpio_init(LED_PIN_G);
    gpio_set_dir(LED_PIN_G, GPIO_OUT);

    gpio_init(BTN_PIN_R);
    gpio_set_dir(BTN_PIN_R, GPIO_IN);
    gpio_pull_up(BTN_PIN_R);

    gpio_init(BTN_PIN_G);
    gpio_set_dir(BTN_PIN_G, GPIO_IN);
    gpio_pull_up(BTN_PIN_G);

    gpio_set_irq_enabled_with_callback(BTN_PIN_R, GPIO_IRQ_EDGE_FALL, true,
                                       &btn_callback);
    gpio_set_irq_enabled(BTN_PIN_G, GPIO_IRQ_EDGE_FALL, true);

    repeating_timer_t timer_red;
    bool timer_r = false;
    repeating_timer_t timer_green;
    bool timer_g = false;

    while (true) {
        // Led Vermelho
        if (flag_f_r) {
            flag_f_r = 0;
            if (timer_r){
                gpio_put(LED_PIN_R , 0);
                cancel_repeating_timer(&timer_red);
                timer_r = false;
            }
            else{
                add_repeating_timer_ms(500 , timer_callback_r , NULL , &timer_red);
                timer_r = true;
            }
        }
        // Led Verde
        if (flag_f_g) {
            flag_f_g = 0;
            if (timer_g){
                gpio_put(LED_PIN_G , 0);
                cancel_repeating_timer(&timer_green);
                timer_g = false;
            }
            else{
                add_repeating_timer_ms(250 , timer_callback_g , NULL , &timer_green);
                timer_g = true;
            }
        }
    }
}