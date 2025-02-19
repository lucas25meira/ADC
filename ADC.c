/* Programa de capacitação EMBARCATECH
 * Unidade 4 - Microcontroladores 
 * Capítulo 8 - Conversores A/D
 * Tarefa 1 - Aula Síncrona 10/02/2025
 *
 * Programa desenvolvido por:
 *      - Lucas Meira de Souza Leite
 * 
 * Objetivos: 
 *      - Compreender o funcionamento do conversor analógico-digital (ADC) no RP2040
 *      - Utilizar o PWM para controlar a intensidade de dois LEDs RGB com base nos valores do joystick
 *      - Representar a posição do joystick no display SSD1306 por meio de um quadrado móvel.
 *      - Aplicar o protocolo de comunicação I2C na integração com o display.
 */

#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/adc.h"
#include "hardware/i2c.h"
#include "hardware/pwm.h"
#include "ssd1306.h"

//Display
#define I2C_SDA 14
#define I2C_SCL 15
#define I2C_PORT i2c1
#define ENDERECO 0x3c

//LEDs
#define LED_VERDE 11
#define LED_AZUL 12
#define LED_VERMELHO 13

//Botões e Joystick
#define BOTAO_A 5
#define PB_JOYSTICK 22
#define VRX_PIN 26
#define VRY_PIN 27

//ADC
#define ADC_0  0
#define ADC_1  1

uint SLICE_LED_AZUL, SLICE_LED_VERMELHO;
volatile uint64_t DEBOUNCE_TIME;
volatile bool LIGADO1, LIGADO2 = 0;

volatile bool ATIVO1, ATIVO2 = 1;

uint16_t VRX_PIN_VALUE, VRY_PIN_VALUE;
volatile uint8_t BORDA1 = 0;
volatile uint8_t BORDA2 = 2;

bool RETANGULO = false;

ssd1306_t ssd;


void PWM(uint led, uint *slice, uint16_t leveli);

uint16_t leitura(uint8_t input); 

void gpio_irq_handler(uint gpio, uint32_t events){
    uint32_t DEBOUNCE = to_us_since_boot(get_absolute_time());
    if(gpio == BOTAO_A || gpio == PB_JOYSTICK){
        if(DEBOUNCE - DEBOUNCE_TIME > 500000){
            if(gpio == BOTAO_A){
                pwm_set_enabled(SLICE_LED_AZUL, !ATIVO1);
                pwm_set_enabled(SLICE_LED_VERMELHO, !ATIVO2);

                LIGADO1 = !LIGADO1;                
                ATIVO1 = !ATIVO1;
                ATIVO2 = !ATIVO2;
            }
            if (gpio == PB_JOYSTICK){
                gpio_put(11, !gpio_get(11));
                LIGADO2 = !LIGADO2;
                    if(BORDA1 == 0){
                        if(BORDA2 == 1){
                            BORDA1 = 2;
                        }
                        else BORDA1 = 1;
                        BORDA2 = BORDA1;
                    } else BORDA1 = 0;
                }
            }
    }
        DEBOUNCE_TIME = DEBOUNCE;
}

uint16_t leitura(uint8_t input){
uint16_t leitura_adc = 0;
    for(uint8_t i = 0; i <= 9; i++){
        adc_select_input(input);
        sleep_us(2);
        leitura_adc += adc_read();
    }
return leitura_adc / 10;
}

void PWM(uint led, uint *slice, uint16_t leveli){
        gpio_set_function(led, GPIO_FUNC_PWM);
        *slice = pwm_gpio_to_slice_num(led);
        pwm_set_clkdiv(*slice, 16);
        pwm_set_wrap(*slice, 4096);
        pwm_set_gpio_level(led, leveli);
        pwm_set_enabled(*slice, true);          
    
}

void main(){
    
    stdio_init_all();

    //Inicialização dos botões
    gpio_init (BOTAO_A);
    gpio_set_dir(BOTAO_A, false);
    gpio_pull_up(BOTAO_A);
    
    gpio_init (PB_JOYSTICK);
    gpio_set_dir(PB_JOYSTICK, false);
    gpio_pull_up(PB_JOYSTICK);
    
    //Inicialização i2c display
    i2c_init(I2C_PORT, 400*1000);
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);

    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SCL);
    
    //Inicialização dos LEDS
    gpio_init(LED_AZUL);
    gpio_set_dir(LED_AZUL, GPIO_OUT);
    gpio_put(LED_AZUL, 0);

    gpio_init(LED_VERDE);
    gpio_set_dir(LED_VERDE, GPIO_OUT);
    gpio_put(LED_VERDE, 0);

    gpio_init(LED_VERMELHO);
    gpio_set_dir(LED_VERMELHO, GPIO_OUT);
    gpio_put(LED_VERMELHO, 0);

    //ADC Joystick
    adc_init();
    adc_gpio_init(VRY_PIN);
    adc_gpio_init(VRX_PIN);
    
    //Display
    ssd1306_init(&ssd, WIDTH, HEIGHT, false, ENDERECO, I2C_PORT);
    ssd1306_config(&ssd);

    PWM(LED_AZUL, &SLICE_LED_AZUL, 0);
    PWM(LED_VERMELHO, &SLICE_LED_VERMELHO, 0);
    
    gpio_set_irq_enabled_with_callback(BOTAO_A, GPIO_IRQ_EDGE_FALL, true, gpio_irq_handler);
    gpio_set_irq_enabled_with_callback(PB_JOYSTICK, GPIO_IRQ_EDGE_FALL, true, gpio_irq_handler);
    
    while (true) {
        VRX_PIN_VALUE = leitura(ADC_1);
        VRY_PIN_VALUE = 4096 - leitura(ADC_0);
        
        uint16_t SLICE_X = (VRX_PIN_VALUE * 4096) / 4095;
        uint16_t SLICE_Y = (VRY_PIN_VALUE * 4096) / 4095;

        //Ajusta a intensidade dos LEDS VERMELHO E AZUL de acordo com a posição do Joystick
        if(VRX_PIN_VALUE >= 2100 && VRX_PIN_VALUE <= 2300)
            pwm_set_gpio_level(LED_VERMELHO, 0);
        else 
            pwm_set_gpio_level(LED_VERMELHO, SLICE_X);
        if(VRY_PIN_VALUE >= 2030 && VRY_PIN_VALUE <= 2090)
            pwm_set_gpio_level(LED_AZUL, 0);
        else 
            pwm_set_gpio_level(LED_AZUL, SLICE_Y); 

        
        //RETANGULO
        RETANGULO = !RETANGULO;
        uint16_t coluna_x = (VRX_PIN_VALUE * WIDTH) / 4095;
        uint16_t linha_y = (VRY_PIN_VALUE * HEIGHT) / 4095;
        if(coluna_x > WIDTH - 8) 
            coluna_x = WIDTH - 8;
        if(linha_y > HEIGHT - 8) 
            linha_y = HEIGHT - 8;
        ssd1306_fill(&ssd, false);
        ssd1306_rect(&ssd, linha_y, coluna_x, 8, 8, true, true);
        if(BORDA1 == 1) 
            ssd1306_rect(&ssd, 4, 4, 124, 60, LIGADO2, false);
        if(BORDA1 == 2) 
            ssd1306_rect(&ssd, 4, 4, 124, 60, !RETANGULO, false);
        ssd1306_send_data(&ssd);
    }
}

