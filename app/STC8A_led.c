#include "STC8A_led.h"
#include "STC8A_Delay.h"

struct LED_s LED;

static void LED_Flash(uint16_t onLight_cycle,uint16_t offLight_cycle)
{
	LED_ON;
	delay_ms(onLight_cycle);
	LED_OFF;
	delay_ms(offLight_cycle);
}

void Run_LED(void)
{
	LED_Flash(10,1000);
}

void SystemError_LED(void)
{
	LED_Flash(1000,30);
}

void Stop_LED(void)
{
	LED_OFF;
}

void Init_LED(void)
{
//	GPIO_MODE_OUT_PP(GPIO_P2,Pin_0);  //P20 
	
    LED.Flash = Run_LED;
	LED.Stop = Stop_LED;
	
	LED.Flash();
}
