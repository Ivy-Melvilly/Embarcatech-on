#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "ssd1306.h"

#define LED_R_PIN 13
#define LED_G_PIN 11
#define LED_B_PIN 12
#define BTN_A_PIN 5
#define I2C_PORT i2c1
#define PINO_SCL 14
#define PINO_SDA 15

// Inicialização do I2C

int A_state = 0;    //Botao A está pressionado?

// Funções para exibir mensagens no OLED
ssd1306_t disp;

void inicializa(){
    stdio_init_all();
    i2c_init(I2C_PORT, 400*1000);// I2C Inicialização. Usando 400Khz.
    gpio_set_function(PINO_SCL, GPIO_FUNC_I2C);
    gpio_set_function(PINO_SDA, GPIO_FUNC_I2C);
    gpio_pull_up(PINO_SCL);
    gpio_pull_up(PINO_SDA);
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, I2C_PORT);
    ssd1306_clear(&disp);
}
void displayMensagem(const char* mensagem) {
    ssd1306_clear(&disp);
    ssd1306_set_cursor(0, 0);
    ssd1306_puts(mensagem, true);
    ssd1306_refresh_display();
}
void SinalAberto(){
    gpio_put(LED_R_PIN, 0);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);
    displayMensagem("SINAL ABERTO - ATRAVESSAR COM CUIDADO");   
}

void SinalAtencao(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 1);
    gpio_put(LED_B_PIN, 0);
    displayMensagem("SINAL DE ATENÇÃO - PREPARE-SE");
}

void SinalFechado(){
    gpio_put(LED_R_PIN, 1);
    gpio_put(LED_G_PIN, 0);
    gpio_put(LED_B_PIN, 0);
    displayMensagem("SINAL FECHADO - AGUARDE");
}

int WaitWithRead(int timeMS){
    for(int i = 0; i < timeMS; i = i+100){
        A_state = !gpio_get(BTN_A_PIN);
        if(A_state == 1){
            return 1;
        }
        sleep_ms(100);
    }
    return 0;
}
int main(){
    
    // INICIANDO LEDS
    gpio_init(LED_R_PIN);
    gpio_set_dir(LED_R_PIN, GPIO_OUT);
    gpio_init(LED_G_PIN);
    gpio_set_dir(LED_G_PIN, GPIO_OUT);
    gpio_init(LED_B_PIN);
    gpio_set_dir(LED_B_PIN, GPIO_OUT);

    // INICIANDO BOTÄO
    gpio_init(BTN_A_PIN);
    gpio_set_dir(BTN_A_PIN, GPIO_IN);
    gpio_pull_up(BTN_A_PIN);
    
    inicializa();

    while(true){

        SinalFechado();
        A_state = WaitWithRead(8000);   //espera com leitura do botäo
        //sleep_ms(8000);


        if(A_state){               //ALGUEM APERTOU O BOTAO - SAI DO SEMAFORO NORMAL
            //SINAL AMARELO PARA OS CARROS E PEDESTRES POR 5s
            SinalAtencao();
            sleep_ms(5000);

            //SINAL VERDE PARA OS PEDESTRES POR 10s
            SinalAberto();
            sleep_ms(10000);

        }else{                     //NINGUEM APERTOU O BOTAO - CONTINUA NO SEMAFORO NORMAL
                                      
            SinalAtencao();
            sleep_ms(2000);

            //SINAL VERDE PARA OS PEDESTRES POR 15s
            SinalAberto();
            sleep_ms(8000);
        }
                
    }

    return 0;

}