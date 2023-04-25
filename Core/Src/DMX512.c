#include "DMX512.h"
#include "uart.h"
#include "main.h"
#include "stdio.h"
#define __root
//solt 24.752ms  40HZ refresh
//-----------------------------------------------------------------------------
// Global Variables
//-----------------------------------------------------------------------------
//uint16_t DMX_model; //IC number
//uint8_t DMX_Chn;    //channel number

//static uint16_t DMX_model;
//static uint8_t DMX_Chn;
//static unsigned char dmxData[513];// Data pool
//static uint8_t fac_us = 0;					// DMX_Delay_us used
__root const char MenuStr[]={"Version :"__DATE__ __TIME__ __FILE__};

/* Clear DMX Buffer */
void clrDmxData(uint8_t* dmxData)
{
	int i;
	for(i=1;i<513;i++)
	{
		dmxData[i] = 0;
	}
}
/* Set Tx_GPIO_Mode */
//iomode 0:OUTPUT_PP
//iomode 1:AF_PP
void GPIO_Tx_Config_OUT(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;
      /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = DMX_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DMX_TX_GPIO_Port, &GPIO_InitStruct);
}
void GPIO_Tx_Config_AF(void)
{
    GPIO_InitTypeDef GPIO_InitStruct;

      /*Configure GPIO pin : PtPin */
    GPIO_InitStruct.Pin = DMX_TX_Pin;
    GPIO_InitStruct.Mode = GPIO_MODE_AF_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(DMX_TX_GPIO_Port, &GPIO_InitStruct);
}

/* Delay nus for break */
void DMX_Delay_us(uint32_t nus)
{
/*    Software Delay  */
    uint32_t i=8*nus;
    while(i--);
//      uint32_t delay_curr;
//      int32_t x=0,delay_goal,delay_start;
//      delay_start = SysTick->VAL;
//      //nus *= US_RATIO;
//      x = delay_start - nus;
//      if (x < 0)
//      {
//          delay_goal = x + (SystemCoreClock / (1000U / uwTickFreq));
//          do{
//                  delay_curr = SysTick->VAL;
//          }while(((delay_curr>delay_goal)||(delay_curr<delay_start);
//      }
//      else
//      {
//            delay_goal = x;
//            do{
//                    delay_curr = SysTick->VAL;
//            }while(((delay_curr>delay_goal)&&(delay_curr<delay_start)));
//      }

/*    Systick reload Delay  */
//    uint32_t temp;
////  temp=HAL_RCC_GetSysClockFreq();
//    //Get System Clock Frequency(in MHz)
//    SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
//    fac_us = HAL_RCC_GetSysClockFreq() / 1000000 / 8;
//    SysTick->LOAD = nus*fac_us;          
//    SysTick->VAL = 0x00;
//    SysTick->CTRL |= SysTick_CTRL_ENABLE_Msk;      
//    do
//    {
//        temp = SysTick->CTRL;
//    }while((temp&0x01) && !(temp&(1<<16)));   
//    //SysTick->CTRL &= ~SysTick_CTRL_ENABLE_Msk;
//    SysTick->LOAD = 0x01193F;
//    SysTick->VAL = 0X00;     
}
/* Send Break sign and 00 Code */
void DMX_Break()
{
    GPIO_Tx_Config_OUT();     //Set UART TX pin mode to OUTPUT
    DMX_TX_High;			  //DMX512 Mark before break MBB >88us
    DMX_Delay_us(150);
    DMX_TX_Low;
    DMX_Delay_us(150);        //DMX512 BREAK >88us
    DMX_TX_High;
    DMX_Delay_us(15);         //DMX512 Mark after break MAB >8us
    GPIO_Tx_Config_AF();
    /* Send Start Code 00 */
    DMX_Send_8Data(0x00);
}
/* Send 8bit data and 8bit always set */
void DMX_Send_8Data(uint8_t tempdata)
{
    if(DMX_UART->SR & (1<<6))
    {
        DMX_UART->DR = 0x0100 | tempdata;
    }
    while((DMX_UART->SR&0X40)==0);//waiting for Send data over
}
/* Send packet data,tempnum must <512 */
void DMX_Send_Packet(uint16_t tempnum, unsigned char* dmxData1 )
{
	DMX_UART_INIT_SEND_DATA;
    uint16_t i=0;
    DMX_Break();        //Break and Start Code
    if(tempnum==0)      //set to default
        tempnum=512;
    while(i < tempnum)  //1-512
    {
        DMX_Send_8Data(dmxData1[i]);
        i++; 
    }
}
/* Init DMX parameter */
//void DMX_Init()
//{
///* Init HardWare Info */
//    if((DMX_model*DMX_Chn)>512) //data must <512
//       printf("ERROR Data too long !!\r\n");
///* Init by HAL_Init() */
//    clrDmxData();       //Clear Data
//}
/* Write Address For IC,tempadd is First IC add */
//void DMX_Write_Add(uint16_t tempadd)
////{
////	DMX_model = 3;
////    uint8_t i;
////    uint16_t address;
////    address=tempadd;
//////    if(tempadd == 0)    //DMX IC start at 1
//////        address=1;
////    DMX_Reset();        //Send 2S Reset sign
////    for(i = 0; i < DMX_model; ++i)
////    {
////         DMX_Send_ADD_Data(address);
////         address+=DMX_Chn;
////    }
////    address=tempadd;
////    DMX_Reset();
////    for(i = 0; i < DMX_model; ++i)
////    {
////         DMX_Send_ADD_Data(address);
////         address+=DMX_Chn;
////    }
////}
void DMX_Send_Add(unsigned char add )
{
	DMX_UART_INIT_SEND_ADD;
	uint8_t*  dmxData = DMX_ADD_Data(add);
    uint16_t i=0;
    DMX_Break();        //Break and Start Code
    for( i = 0 ; i < 513 ; i++ )  //1-512
    {
        DMX_Send_8Data(dmxData[i]);
        i++;
    }
}
/*Change IC Address Order */
uint8_t* DMX_ADD_Data(uint16_t tempdata)
{
    uint8_t data_H,data_L,tempadd[512];
	data_H=(tempdata>>6)&63;     //Get High 8bit
	data_L=tempdata&63;		    //Get Low 8bit
	tempadd[0]=DMX_Transposition(0xc3);
    tempadd[1]=DMX_Transposition(0xf5);
    tempadd[2]=DMX_Transposition(data_H+1);
    tempadd[3]=DMX_Transposition(data_H);
	tempadd[4]=DMX_Transposition(data_L);
    tempadd[5]=DMX_Transposition(data_L-1);
    tempadd[6]=DMX_Transposition(0xf5);
    tempadd[7]=DMX_Transposition(0x5f);
    tempadd[8]=DMX_Transposition(0x00);
    return tempadd;
} 
/* Send Reset sign and 00Code */
void DMX_Reset()
{
    GPIO_Tx_Config_OUT();     //Set UART TX pin mode to OUTPUT
    DMX_TX_Low;       
    HAL_Delay(2000);              //UCS512 RESET >2s
    DMX_TX_High;
    //HAL_Delay(1);                 
    DMX_Delay_us(150);           //UCS512 Mark after RESET MAB >50us
    GPIO_Tx_Config_AF();
    /* Send Start Code 00 */
    DMX_Send_8Data(0x00);
}
/* UCS512C Datasheet */
void DMX_W()
{
    GPIO_Tx_Config_OUT();     //Set UART TX pin mode to OUTPUT
    DMX_TX_High;
    DMX_Delay_us(100);           //UCS512 Mark after RESET MAB >50us
    GPIO_Tx_Config_AF();
}
/* Transposition LSB to MSB */
uint8_t DMX_Transposition(uint8_t tempchar)
{
	uint8_t i=0;
	uint8_t tempnum=0;	
	for(i=0;i<8;i++)
	{
		tempnum=tempnum>>1; 
		tempnum=((tempchar<<i)&0x80)|tempnum; 
	}
	return tempnum;
}
