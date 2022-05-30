/**************************************************************************//**
 * @file     main.c
 * @version  V3.00
 * @brief    Show the usage of GPIO interrupt function.
 *
 * SPDX-License-Identifier: Apache-2.0
 * @copyright (C) 2018 Nuvoton Technology Corp. All rights reserved.
 ******************************************************************************/
#include <stdio.h>
#include "NuMicro.h"
#include "smarttimer.h"
#include <stdlib.h>
void SYS_Init(void)
{
    /* Unlock protected registers */
    SYS_UnlockReg();

    /* Enable HIRC */
    CLK_EnableXtalRC(CLK_PWRCTL_HIRCEN_Msk);

    /* Waiting for HIRC clock ready */
    CLK_WaitClockReady(CLK_STATUS_HIRCSTB_Msk);

    /* Switch HCLK clock source to HIRC */
    CLK_SetHCLK(CLK_CLKSEL0_HCLKSEL_HIRC, CLK_CLKDIV0_HCLK(1));

    /* Set both PCLK0 and PCLK1 as HCLK/2 */
    CLK->PCLKDIV = (CLK_PCLKDIV_APB0DIV_DIV2 | CLK_PCLKDIV_APB1DIV_DIV2);

    /* Switch UART0 clock source to HIRC */
    CLK_SetModuleClock(UART0_MODULE, CLK_CLKSEL1_UART0SEL_HIRC, CLK_CLKDIV0_UART0(1));

    /* Enable UART peripheral clock */
    CLK_EnableModuleClock(UART0_MODULE);

    /* Update System Core Clock */
    /* User can use SystemCoreClockUpdate() to calculate PllClock, SystemCoreClock and CycylesPerUs automatically. */
    SystemCoreClockUpdate();

    /*----------------------------------------------------------------------*/
    /* Init I/O Multi-function                                              */
    /*----------------------------------------------------------------------*/
    /* Set GPB multi-function pins for UART0 RXD and TXD */
    SYS->GPB_MFPH = (SYS->GPB_MFPH & ~(SYS_GPB_MFPH_PB12MFP_Msk | SYS_GPB_MFPH_PB13MFP_Msk)) |
                    (SYS_GPB_MFPH_PB12MFP_UART0_RXD | SYS_GPB_MFPH_PB13MFP_UART0_TXD);

    /* Lock protected registers */
    SYS_LockReg();
}

/*----------------------------------------------------------------------*/
/* Init UART0                                                           */
/*----------------------------------------------------------------------*/
void UART0_Init(void)
{
    /* Reset UART0 */
    SYS_ResetModule(UART0_RST);

    /* Configure UART0 and set UART0 baud rate */
    UART_Open(UART0, 115200);
}



struct date{
  uint16_t year;
  uint8_t  month;
  uint8_t  day;
  uint8_t hour;
  uint8_t minute;
	uint8_t second;
};




struct date m_date;

const char datestr[] = "2016:06:23#17:16:00";
static int8_t sysled_id;


static void sys_ledflash(void);
static void simulation_rtc(void);

static void remove_sysled(void)
{
  stim_kill_event(sysled_id);
}

static void sys_init(void)
{
	sysled_id = stim_loop(500,sys_ledflash,STIM_LOOP_FOREVER);
	stim_runlater(20000,remove_sysled);
	
}



static void set_timetick ( void)
{
    //xxxx:xx:xx#xx:xx:xx

	  m_date.year = atoi(datestr);
	  m_date.month = atoi(datestr + 5);
	  m_date.day = atoi(datestr + 8);
	  m_date.hour = atoi(datestr + 11);
	  m_date.minute = atoi(datestr + 14);
	  m_date.second = atoi(datestr + 17);
    
	  stim_loop(1000,simulation_rtc,STIM_LOOP_FOREVER);
    
}		/* -----  end of static function time_tick  ----- */




static uint8_t isLeap(uint16_t year)
{
  if(year % 4 == 0 && 
      year %100 != 0 &&
      year % 400 == 0){
    return 1;
  }else{
    return 0;
  }
}


static void increaseday(uint8_t limit)
{
  if(++m_date.day > limit){
    m_date.day = 1;
    if(++m_date.month > 12){
      m_date.month = 1;
      m_date.year++;
    }
  }
}

static void changeday ( void )
{
  switch(m_date.month){
    case 1:
    case 3:
    case 5:
    case 7:
    case 8:
    case 10:
    case 12:
      increaseday(31);
      break;
    case 4:
    case 6:
    case 9:
    case 11:
      increaseday(30);
      break;
    case 2:
      if(isLeap(m_date.year) == 1){
        increaseday(29);
      }else{
        increaseday(28);
      }
			break;
  }

  
}


static void simulation_rtc(void)
{

  if(++m_date.second == 60){
    m_date.second = 0;
    if(++m_date.minute == 60){
      m_date.minute = 0;
      if(++m_date.hour == 24){
        m_date.hour = 0;
        changeday();
      }
    }
  }
	printf("time===>[%02d:%02d:%02d]\r\n",m_date.hour,m_date.minute,m_date.second);
}

static void sys_ledflash(void)
{
	static uint8_t flag = 0;
	
	if(flag == 0){

		printf("led off\r\n");
	}else{

		printf("led on\r\n");
	}
	
	flag = !flag;

}


static void runlater_test(void)
{

   printf("after runlater===>[%02d:%02d:%02d]\r\n",m_date.hour,m_date.minute,m_date.second);
	 printf("before delay===>[%02d:%02d:%02d]\r\n",m_date.hour,m_date.minute,m_date.second);
	 stim_delay(2000);
	 printf("after delay===>[%02d:%02d:%02d]\r\n",m_date.hour,m_date.minute,m_date.second);
}
void SysTick_Handler(void)
{
	  stim_tick();
}
int main(void)
{
    /* Init System, IP clock and multi-function I/O. */
    SYS_Init();

    /* Init UART0 for printf */
    UART0_Init();

    printf("\n\nCPU @ %d Hz\n", SystemCoreClock);
 stim_init();
  sys_init();

  set_timetick();

	printf("before runlater===>[%02d:%02d:%02d]\r\n",m_date.hour,m_date.minute,m_date.second);
	stim_runlater(1000,runlater_test);
	
	#ifdef STIM_DEBUG
  stim_loop(10000,stim_print_status,STIM_LOOP_FOREVER);
	#endif

  while(1){
    stim_mainloop();
  };
    /* Waiting for interrupts */
    while(1);
}
