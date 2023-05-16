#include "MKL46Z4.h"
#include "lcd.h"


int selector = 0; 	//0-> digito 1
  			//1-> digito 2
  			//2-> digito 3
  			//3-> digito 4
  			//4-> Pausa
  			//5-> Cuenta atrás
int minutos = 0;
int segundos = 0;

uint32_t tiempo = 0;
  
void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); 
}

void clear_LCD(){
  LCD->WF8B[LCD_FRONTPLANE0] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE1] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE2] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE3] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE4] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE5] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE6] = LCD_CLEAR;
  LCD->WF8B[LCD_FRONTPLANE7] = LCD_CLEAR;
}

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}
void short_delay(void)
{
  volatile int i;

  for (i = 0; i < 500000; i++);
}





void actualizar_tiempo(){
    if (segundos == 0 && minutos == 0){
        // El cronómetro ha llegado a 0, hacemos parpadear los LEDs y mostramos 00:00 en el LCD
        TPM0->SC &= ~TPM_SC_CMOD_MASK; // Desactivamos el temporizador TPM0
        TPM0->CNT = 0; // Reiniciamos el contador
        TPM0->SC |= TPM_SC_CMOD(0x01); // Reactivamos el temporizador TPM0

        LCD->WF8B[LCD_FRONTPLANE0] = 0x3F; // Mostramos 0 en el primer dígito
        LCD->WF8B[LCD_FRONTPLANE1] = 0x3F; // Mostramos 0 en el segundo dígito
        LCD->WF8B[LCD_FRONTPLANE2] = 0x3F; // Mostramos 0 en el tercer dígito
        LCD->WF8B[LCD_FRONTPLANE3] = 0x3F; // Mostramos 0 en el cuarto dígito



        while(1){ // Hacemos parpadear los LEDs
            short_delay();
            PTD->PDOR ^= (1 << 5);
            PTE->PDOR ^= (1 << 29);
        }
    } else if (segundos == 0){
        if (minutos > 0){
            segundos = 59;
            minutos--;
        }
    } else {
        segundos--;
    }
}






void sclock_init(){
  SIM->SCGC6 |= SIM_SCGC6_RTC_MASK;
  SIM->SOPT1 |= 0x00000000;
  RTC->CR = RTC_CR_OSCE(1); //Activamos el oscilador
  delay();
  RTC->CR = RTC_CR_CLKO(1);
  RTC->SR |= (1 << 4); //Activamos el reloj 
}



void ini_clock()
{
  SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK; 
  SIM->SOPT2 |= SIM_SOPT2_TPMSRC(0x03); 
  TPM0->SC = TPM_SC_CMOD(0x00); 
  TPM0->SC |= TPM_SC_CPWMS(0x00);

  TPM0->SC |= TPM_SC_PS(0x00); // Seleccionamos que no divida en el Preescaler 
  TPM0->MOD = TPM_MOD_MOD(0x7CFF); 
  TPM0->SC |= TPM_SC_TOF_MASK; 
  TPM0->SC |= TPM_SC_CMOD(0x01); 
}





void sw1_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[3] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  GPIOC->PDDR &= ~(1 << 3);
  PORTC->PCR[3] |= PORT_PCR_IRQC(0xA);    
}


void sw2_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  PORTC->PCR[12] |= PORT_PCR_MUX(1) | PORT_PCR_PE(1);
  GPIOC->PDDR &= ~(1 << 12);
  PORTC->PCR[12] |= PORT_PCR_IRQC(0xA);   
}

void PORTDIntHandler(void) {
  int pressed_switch = PORTC->ISFR;
  PORTC->ISFR = 0xFFFFFFFF; 

  // SW1
  if(pressed_switch == (0x8)) {

	if (selector == 3) {
		minutos = (minutos + 10)%100;
	}
	if (selector == 2) {
		minutos = minutos/10 + ((minutos+ 1) %10);
	}
	if (selector == 1) {
		segundos = (segundos + 10)%60;
	}
	if (selector == 0) {
		segundos = segundos/10 + ((segundos+ 1) %10);
	}
	if (selector == 4) {
	  selector = 5; 

	}
	else if (selector == 5) {
	  selector = 4;
	}
  }
  // SW2
  if(pressed_switch == (0x1000)) {

	  	if (selector == 4){
		  	minutos = 0;
		  	segundos = 0;
		  	selector = 0;
	  	}
	  	else if(selector<4 ){
			selector++;
		}
  }
  
}


int sw1_check()
{
  return( !(GPIOC->PDIR & (1 << 3)) );
}

int sw2_check()
{
  return( !(GPIOC->PDIR & (1 << 12)) );
}




void sws_ini(){
  SIM->COPC = 0;            
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
  NVIC_SetPriority(31, 0);  
  NVIC_EnableIRQ(31);  
}



void led_green_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);
  GPIOD->PDDR |= (1 << 5);
  GPIOD->PSOR = (1 << 5);
}

void led_green_toggle()
{
  GPIOD->PTOR = (1 << 5);
}


void led_red_ini()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  GPIOE->PDDR |= (1 << 29);
  GPIOE->PSOR = (1 << 29);
}

void led_red_toggle(void)
{
  GPIOE->PTOR = (1 << 29);
}


// void leds_ini()
// {
//   SIM->COPC = 0;
//   SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
//   PORTD->PCR[5] = PORT_PCR_MUX(1);
//   PORTE->PCR[29] = PORT_PCR_MUX(1);
//   GPIOD->PDDR |= (1 << 5);
//   GPIOE->PDDR |= (1 << 29);
//   // both LEDS off after init
//   GPIOD->PSOR = (1 << 5);
//   GPIOE->PSOR = (1 << 29);
// }

void leds_ini(){
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
  PORTD->PCR[5] |= PORT_PCR_MUX(1);
  PORTD->PCR[5] |= PORT_PCR_DSE_MASK;
  PORTD->PCR[5] |= PORT_PCR_PE_MASK;
  PORTD->PCR[5] |= PORT_PCR_PS_MASK;
  GPIOD->PDDR |= (1 << 5);

  PORTE->PCR[29] |= PORT_PCR_MUX(1);
  PORTE->PCR[29] |= PORT_PCR_DSE_MASK;
  PORTE->PCR[29] |= PORT_PCR_PE_MASK;
  PORTE->PCR[29] |= PORT_PCR_PS_MASK;
  GPIOE->PDDR |= (1 << 29);
}






int main(void)
{


  irclk_ini(); 
  ini_clock(); //Inicializa el reloj que usaremos para contar.
  leds_ini();
  sws_ini();
  sw1_ini();
  sw2_ini();
  lcd_ini();

  while(1){
  lcd_display_time(minutos,segundos);
  
  //Control overflow
  if ((TPM0->SC & TPM_SC_TOF_MASK) == TPM_SC_TOF_MASK){
  
  	
  	if (selector == 5){
  		actualizar_tiempo();
  		lcd_display_time(minutos,segundos);
  	}
  	
  	//Reseteamos el contador
  	TPM0->SC |= TPM_SC_TOF_MASK;
  	}
  }
  return 0;
}
