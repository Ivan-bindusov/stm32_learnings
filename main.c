#include "stm32f1xx.h"

#define LED_PC13_OFF() GPIOC->BSRR |= GPIO_BSRR_BS13;
#define LED_PC13_ON() GPIOC->BSRR |= GPIO_BSRR_BR13;

uint8_t Button_state = 0;

void SetSysClockTo72(void)
{
  RCC->CR |= RCC_CR_HSEON;
  while(READ_BIT(RCC->CR, RCC_CR_HSERDY) == RESET) {}
  
  FLASH->ACR &= ~FLASH_ACR_PRFTBE;
  FLASH->ACR |= FLASH_ACR_PRFTBE;
  FLASH->ACR &= ~FLASH_ACR_LATENCY;
  FLASH->ACR |= FLASH_ACR_LATENCY_2;
  
  RCC->CFGR &= ~RCC_CFGR_HPRE;
  RCC->CFGR |= RCC_CFGR_HPRE_DIV1;
  
  RCC->CFGR &= ~RCC_CFGR_PPRE2;
  RCC->CFGR |= RCC_CFGR_PPRE2_DIV1;
  
  RCC->CFGR &= ~RCC_CFGR_PPRE1;
  RCC->CFGR |= RCC_CFGR_PPRE1_DIV2;
  
  RCC->CFGR &= (uint32_t) ((uint32_t) ~ (RCC_CFGR_PLLSRC | RCC_CFGR_PLLXTPRE | RCC_CFGR_PLLMULL));
  RCC->CFGR |= (0x01<<RCC_CFGR_PLLSRC_Pos) | (0x07<<RCC_CFGR_PLLMULL_Pos);
  RCC->CR |= RCC_CR_PLLON;
  
  while(READ_BIT(RCC->CR, RCC_CR_PLLRDY) != RCC_CR_PLLRDY) {}
  
  RCC->CFGR &= ~RCC_CFGR_SW;
  RCC->CFGR |= RCC_CFGR_SW_PLL;
  
  while(READ_BIT(RCC->CFGR, RCC_CFGR_SWS) != RCC_CFGR_SWS_PLL){}
}

void PORTA_0_INIT(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  
  GPIOA->CRL &= ~GPIO_CRL_MODE0_0;
  GPIOA->CRL &= ~GPIO_CRL_MODE0_1;
  
  GPIOA->CRL &= ~GPIO_CRL_CNF0_0;
  GPIOA->CRL |= GPIO_CRL_CNF0_1;
}

void PORTC_13_INIT(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  
  GPIOC->CRH &= ~GPIO_CRH_MODE13;
  GPIOC->CRH &= ~GPIO_CRH_CNF13;
  GPIOC->CRH |= GPIO_CRH_MODE13_1;
}

int main(void)
{
  SetSysClockTo72();
  
  PORTA_0_INIT();
  PORTC_13_INIT();
  
  while(1)
  {
    Button_state = READ_BIT(GPIOA->IDR, GPIO_IDR_IDR0);
    
    if(Button_state == 1)
    {
      LED_PC13_ON();
    }
    else
    {
      LED_PC13_OFF();
    }
    
//    GPIOC->BSRR |= GPIO_BSRR_BS13;
//    for(int i=0;i<1000000;i++) {};
//    GPIOC->BSRR |= GPIO_BSRR_BR13;
//    for(int i=0;i<1000000;i++) {};
  }
  
}
