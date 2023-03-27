#include "MKL46Z4.h"

#define LED_GREEN_POS (5)
#define LED_RED_POS (29)
#define SW1_POS (3)
#define SW2_POS (12)

int SW1_ACUM = 0;
int SW2_ACUM = 0;
int CONTROL = -1;

void led_green_init()
{
  SIM->COPC = 0;                  
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;       
  PORTD->PCR[LED_GREEN_POS] = PORT_PCR_MUX(1);  
  GPIOD->PDDR |= (1 << LED_GREEN_POS);      
  GPIOD->PSOR |= (1 << LED_GREEN_POS);    
}

void led_green_on(void)
{
  GPIOD->PCOR |= (1 << LED_GREEN_POS);
}

void led_green_off(void) {
  GPIOD->PSOR |= (1 << LED_GREEN_POS);
}

void led_red_init() {
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[LED_RED_POS] = PORT_PCR_MUX(1);
  GPIOE->PDDR |= (1 << LED_RED_POS);
  GPIOE->PSOR |= (1 << LED_RED_POS);
}

void led_red_on(void) {
  GPIOE->PCOR |= (1 << LED_RED_POS);
}

void led_red_off(void) {
  GPIOE->PSOR |= (1 << LED_RED_POS);
}


void sw1_init() {
  
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[SW1_POS] |= PORT_PCR_MUX(1);
  PORTC->PCR[SW1_POS] |= PORT_PCR_PE_MASK;// Pull enable
  PORTC->PCR[SW1_POS] |= PORT_PCR_PS_MASK;// Pull select

  PORTC->PCR[SW1_POS] |= PORT_PCR_IRQC(0xA); // IRQ en el flanco de bajada
  NVIC_SetPriority(31, 0);  // Interrupcion 31
  NVIC_EnableIRQ(31);     // La activa
}

void sw2_init() {

  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[SW2_POS] |= PORT_PCR_MUX(1);
  PORTC->PCR[SW2_POS] |= PORT_PCR_PE_MASK;
  PORTC->PCR[SW2_POS] |= PORT_PCR_PS_MASK;

  // IRQ
  PORTC->PCR[SW2_POS] |= PORT_PCR_IRQC(0xA);
  NVIC_SetPriority(31, 0);
  NVIC_EnableIRQ(31);
}

void PORTDIntHandler(void) {
  int pressed = PORTC->ISFR;
  PORTC->ISFR = 0xFFFFFFFF; // Limpiamos

  if(pressed == (0x8)) 
  {
    SW1_ACUM += 1;
    if(SW1_ACUM > 1) 
    {
      SW1_ACUM = 0;
    }
  }

  if(pressed == (0x1000)) 
  {
    SW2_ACUM += 1;
    if(SW2_ACUM > 1) 
    {
      SW2_ACUM = 0;
    }
  }

  CONTROL = SW1_ACUM + (SW2_ACUM * 2);
}


int main(void)
{
  led_green_init();
  led_red_init();
  sw1_init();
  sw2_init();

  while(1) {
     
    // 00 = 0 Ambas cerradas
    // 01 = 1 Puerta 2 abierta
    // 10 = 2 Puerta 1 abierta
    // 11 = 3 Ambas abiertas
     
    switch(CONTROL) {
      case 0:
        led_green_on();
        led_red_off();
        break;
      case 1:
        led_green_off();
        led_red_on();
        break;
      case 2:
        led_green_off();
        led_red_on();
        break;
      case 3:
        led_green_off();
        led_red_on();
        break;
    }
  }
  return 0;
}
