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
  
  GPIOA->CRL  &= ~GPIO_CRL_CNF0_0;
  GPIOA->CRL  |= GPIO_CRL_CNF0_1;
  
  GPIOA->BSRR = GPIO_BSRR_BS0;
}

void Interrupt_EXTI_PA0_init(void)
{
  EXTI->PR |= EXTI_PR_PR0;
  EXTI->IMR |= EXTI_IMR_MR0;
  AFIO->EXTICR[0] &= ~AFIO_EXTICR1_EXTI0_PA;
  RCC->APB2ENR |= RCC_APB2ENR_IOPAEN;
  EXTI->FTSR |= EXTI_FTSR_TR0;
  NVIC_EnableIRQ(EXTI0_IRQn);
  NVIC_SetPriority(EXTI0_IRQn, 0);
}

void PORTC_13_INIT(void)
{
  RCC->APB2ENR |= RCC_APB2ENR_IOPCEN;
  
  GPIOC->CRH &= ~GPIO_CRH_MODE13;
  GPIOC->CRH &= ~GPIO_CRH_CNF13;
  GPIOC->CRH |= GPIO_CRH_MODE13_1;
}

void EXTI0_IRQHandler(void)
{
  EXTI->PR |= EXTI_PR_PR0;
  if(Button_state == 1)
  {
    for(int i=0; i < 6; i++) {
      LED_PC13_ON();
      for(int j=0; j < 500000; j++) {};
      LED_PC13_OFF();
      for(int j=0; j < 500000; j++) {};
    }
  }
}

int main(void)
{
  SetSysClockTo72();
  
  PORTA_0_INIT();
  PORTC_13_INIT();
  Interrupt_EXTI_PA0_init();
  
  while(1)
  {
    Button_state = READ_BIT(GPIOA->IDR, GPIO_IDR_IDR0);
    LED_PC13_OFF();
  }
  
}
