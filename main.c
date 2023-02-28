
#include "MKL46Z4.h"

// Switches
#define SW1_MASK (1 << 3)
#define SW2_MASK (1 << 12)

// // LED (RG)
// // LED_GREEN = PTD5
// // LED_RED = PTE29

// // Sensor SW1 (puerto C bit 3)
// // Sensor SW2 (puerto C bit 12)

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}


void switch_init()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
  PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
  GPIOC->PDDR &= ~(SW1_MASK | SW2_MASK);
}

// LED_GREEN = PTD5
void led_green_init()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);
  GPIOD->PDDR |= (1 << 5);
  GPIOD->PSOR = (1 << 5);
}

void led_green_on()
{
  GPIOD->PCOR = (1 << 5);
}

void led_green_off()
{
  GPIOD->PSOR = (1 << 5);
}

void led_green_toggle()
{
  GPIOD->PTOR = (1 << 5);
}

// LED_RED = PTE29
void led_red_init()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOE->PDDR |= (1 << 29);
  GPIOE->PSOR = (1 << 29);
}

void led_red_on(void)
{
  GPIOE->PCOR = (1 << 29);
}

void led_red_off(void)
{
  GPIOE->PSOR = (1 << 29);
}

void led_red_toggle(void)
{
  GPIOE->PTOR = (1 << 29);
}

int main(void)
{
  led_green_init();
  led_red_init();
  switch_init();

  int sw1_count = 0;
  int sw2_count = 0;

  while (1) {
    if (!(GPIOC->PDIR & SW1_MASK)) {
      delay(); 
      if (!(GPIOC->PDIR & SW1_MASK)) {
        sw1_count++;
        if (sw1_count >= 2) {
          sw1_count = 0;
          led_red_on();
        }
      }
    }

  
    if (!(GPIOC->PDIR & SW2_MASK)) {
      delay(); 
      if (!(GPIOC->PDIR & SW2_MASK)) { 
        sw2_count++;
        if (sw2_count >= 2) {
          sw2_count = 0;
          led_red_on();
        }
      }
    }

    
    if (sw1_count == 1 || sw2_count == 1) {
      led_green_off();
      led_red_on();
    } else {
      led_green_on();
      led_red_off();
    }
  }

  return 0;
}
