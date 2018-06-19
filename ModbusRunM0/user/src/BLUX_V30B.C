#include <stdio.h>
#include "M051Series.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "main.h"
#include "user_mb_app.h"
#include "BLUX_V30B.h"


/*---------------------------------------------------------------------------------------------------------*/
/* Global variables                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
volatile uint8_t g_u8DeviceAddr;
volatile uint8_t g_au8TxData[3];
volatile uint8_t g_u8RxData;
volatile uint8_t g_u8DataLen;
volatile uint8_t g_u8EndFlag = 0;

typedef void (*I2C_FUNC)(uint32_t u32Status);

static I2C_FUNC s_I2C1HandlerFn = NULL;

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C1 IRQ Handler                                                                                       */
/*---------------------------------------------------------------------------------------------------------*/
void I2C1_IRQHandler(void)
{
    uint32_t u32Status;

    u32Status = I2C_GET_STATUS(I2C1);
    if(I2C_GET_TIMEOUT_FLAG(I2C1))
    {
        /* Clear I2C0 Timeout Flag */
        I2C_ClearTimeoutFlag(I2C1);
    }
    else
    {
        if(s_I2C1HandlerFn != NULL)
            s_I2C1HandlerFn(u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Rx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterRx(uint32_t u32Status)
{
    if(u32Status == 0x08)                       /* START has been transmitted and prepare SLA+W */
    {
        I2C_SET_DATA(I2C1, (g_u8DeviceAddr << 1));    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x18)                  /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x20)                  /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_STOP(I2C1);
        I2C_START(I2C1);
    }
    else if(u32Status == 0x28)                  /* DATA has been transmitted and ACK has been received */
    {
        if(g_u8DataLen != 2)
        {
            I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_STA_SI);
        }
    }
    else if(u32Status == 0x10)                  /* Repeat START has been transmitted and prepare SLA+R */
    {
        I2C_SET_DATA(I2C1, ((g_u8DeviceAddr << 1) | 0x01));   /* Write SLA+R to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x40)                  /* SLA+R has been transmitted and ACK has been received */
    {
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x58)                  /* DATA has been received and NACK has been returned */
    {
        g_u8RxData = (unsigned char) I2C_GET_DATA(I2C1);
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_STO_SI);
        g_u8EndFlag = 1;
    }
    else
    {
        /* TO DO */
       // printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

/*---------------------------------------------------------------------------------------------------------*/
/*  I2C Tx Callback Function                                                                               */
/*---------------------------------------------------------------------------------------------------------*/
void I2C_MasterTx(uint32_t u32Status)
{
    if(u32Status == 0x08)                       /* START has been transmitted */
    {
        I2C_SET_DATA(I2C1, g_u8DeviceAddr << 1);    /* Write SLA+W to Register I2CDAT */
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x18)                  /* SLA+W has been transmitted and ACK has been received */
    {
        I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
    }
    else if(u32Status == 0x20)                  /* SLA+W has been transmitted and NACK has been received */
    {
        I2C_STOP(I2C1);
        I2C_START(I2C1);
    }
    else if(u32Status == 0x28)                  /* DATA has been transmitted and ACK has been received */
    {
        if(g_u8DataLen != 3)
        {
            I2C_SET_DATA(I2C1, g_au8TxData[g_u8DataLen++]);
            I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_SI);
        }
        else
        {
            I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_STO_SI);
            g_u8EndFlag = 1;
        }
    }
    else
    {
        /* TO DO */
       // printf("Status 0x%x is NOT processed\n", u32Status);
    }
}

void I2C0_Close(void)
{
    /* Disable I2C0 interrupt and clear corresponding NVIC bit */
    I2C_DisableInt(I2C0);
    NVIC_DisableIRQ(I2C0_IRQn);

    /* Disable I2C0 and close I2C0 clock */
    I2C_Close(I2C0);
    CLK_DisableModuleClock(I2C0_MODULE);

}
void InitBLUX_V30(void)
{

/* Enable  I2C1 module clock */
//CLK_EnableModuleClock(I2C1_MODULE);

/* Configure the SDA1 & SCL1 of I2C1 pins */
//SYS->P4_MFP &= ~(SYS_MFP_P44_Msk | SYS_MFP_P45_Msk);
//SYS->P4_MFP |= SYS_MFP_P44_SCL1  | SYS_MFP_P45_SDA1;

GPIO_SetMode(P2, BIT7,GPIO_PMD_OUTPUT);//EN
GPIO_SetMode(P4, BIT4|BIT5,GPIO_PMD_QUASI );//EN

P27=1;

   return ;
  /* Open I2C module and set bus clock */
    I2C_Open(I2C1, 100000);

  /* Set I2C 4 Slave Addresses */
    I2C_SetSlaveAddr(I2C1, 0, 0x15, 0);   /* Slave Address : 0x15 */
    I2C_SetSlaveAddr(I2C1, 1, 0x35, 0);   /* Slave Address : 0x35 */
    I2C_SetSlaveAddr(I2C1, 2, 0x55, 0);   /* Slave Address : 0x55 */
    I2C_SetSlaveAddr(I2C1, 3, 0x75, 0);   /* Slave Address : 0x75 */

    /* Enable I2C interrupt */
    I2C_EnableInt(I2C1);
    NVIC_EnableIRQ(I2C1_IRQn);




}
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS] ;
extern unsigned long  Systick;
uint32_t BLUX_V3BTime;
void Blux_v3bPoll(void)
{	
    uint32 m_Lux = 0x00;
  	if(Systick-BLUX_V3BTime>=(500))
	{
	     BLUX_V3BTime=Systick;

	   B_LUX_GetLux(&m_Lux);

	  usSRegHoldBuf[17]=m_Lux>>16 ; //��16bit
   	  usSRegHoldBuf[18]=m_Lux ;	  //��16bit

	   //temp=((float)m_Lux/1000);

	  
	}
   




   return ;
   // g_u8DeviceAddr = 0x50;

   //  g_au8TxData[0] = ;
   //  g_au8TxData[1] = 0;
    // g_au8TxData[2] = 0;

     //   g_u8DataLen = 0;
     //   g_u8EndFlag = 0;

        /* I2C function to write data to slave */
     //   s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterTx;

        /* I2C as master sends START signal */
     //   I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_STA);

        /* Wait I2C Tx Finish */
      //  while(g_u8EndFlag == 0);
         g_u8EndFlag = 0;

        /* I2C function to read data from slave */
        s_I2C1HandlerFn = (I2C_FUNC)I2C_MasterRx;

        g_u8DataLen = 0;
        g_u8DeviceAddr = 0x95; //��ȡ���յ�ַ

		 g_au8TxData[0] =0 ;//�ӵ�ַ0��ʼ��ȡ
       //g_au8TxData[1] = 0;
      //g_au8TxData[2] = 0;

        I2C_SET_CONTROL_REG(I2C1, I2C_I2CON_STA);

        /* Wait I2C Rx Finish */
        while(g_u8EndFlag == 0);

 
        g_u8RxData = g_au8TxData[2];
   

    /* Close I2C0 */
    I2C0_Close();

    while(1);
}















extern void DrvSYS_Delay(uint32_t us);

uint8    BUF_0[129];                       //�������ݻ�����      	
uint32   dis_data_0;                     //����

/*---------------------------------------------------------------------
 ��������: ��ʱ���� ��ͬ�Ĺ�������,��Ҫ�����˺���
 ����˵��: ��	
 ��������: ��
 ---------------------------------------------------------------------*/
void B_LUX_delay_nms(uint16 k)	
{						
  uint16 i,j;				
  for(i=0;i<k;i++)
  {			
   	DrvSYS_Delay(1000);
  }						
}					

/*---------------------------------------------------------------------
 ��������: ��ʱ5΢��  ��ͬ�Ĺ�������,��Ҫ�����˺���
 ����˵��: ��	
 ��������: ��
 ---------------------------------------------------------------------*/
void B_LUX_Delay5us()
{
  DrvSYS_Delay(5);
}

/*---------------------------------------------------------------------
 ��������: ��ʱ5����  ��ͬ�Ĺ�������,��Ҫ�����˺���
 ����˵��: ��	
 ��������: ��
 ---------------------------------------------------------------------*/
void B_LUX_Delay5ms()
{
  uint16 n = 20000;
  
  while (n--);
}

/*---------------------------------------------------------------------
 ��������: ��ʼ�ź�
 ����˵��: ��	
 ��������: ��
 ---------------------------------------------------------------------*/
void B_LUX_Start()
{
  B_LUX_SDA0_H;                         //����������
  B_LUX_SCL0_H;                         //����ʱ����
  B_LUX_Delay5us();                     //��ʱ
  B_LUX_SDA0_L;                         //�����½���
  B_LUX_Delay5us();                     //��ʱ
  B_LUX_SCL0_L;                         //����ʱ����
}

/*---------------------------------------------------------------------
 ��������: ֹͣ�ź�
 ����˵��: ��	
 ��������: ��
 ---------------------------------------------------------------------*/
void B_LUX_Stop()
{
  B_LUX_SDA0_L;                         //����������
  B_LUX_SCL0_H;                         //����ʱ����
  B_LUX_Delay5us();                     //��ʱ
  B_LUX_SDA0_H;                         //����������
  B_LUX_Delay5us();                     //��ʱ
  B_LUX_SCL0_L;
  B_LUX_Delay5us();
  
  //B_LUX_delay_nms(1000);                         //��ʱ180ms
}

/*---------------------------------------------------------------------
 ��������: ����Ӧ���ź�
 ����˵��: ack - Ӧ���ź�(0:ACK 1:NAK)
 ��������: ��
 ---------------------------------------------------------------------*/
void B_LUX_SendACK(uint8 ack)
{
  if (ack&0x01)	B_LUX_SDA0_H;		//дӦ���ź�
  else	B_LUX_SDA0_L;
  
  B_LUX_SCL0_H;                         //����ʱ����
  B_LUX_Delay5us();                     //��ʱ
  B_LUX_SCL0_L;                         //����ʱ����
  B_LUX_SDA0_H;
  B_LUX_Delay5us();                     //��ʱ
}

/*---------------------------------------------------------------------
 ��������: ����Ӧ���ź�
 ����˵��: ��
 ��������: ����Ӧ���ź�
 ---------------------------------------------------------------------*/
uint8 B_LUX_RecvACK()
{
  uint8 CY = 0x00;
  B_LUX_SDA0_H;
  
  B_LUX_SDA0_I;
  
  B_LUX_SCL0_H;                         //����ʱ����
  B_LUX_Delay5us();                     //��ʱ
  
  
  CY |= B_LUX_SDA0_DAT;                 //��Ӧ���ź�
  
  B_LUX_Delay5us();                     //��ʱ
  
  B_LUX_SCL0_L;                         //����ʱ����
  
  B_LUX_SDA0_O;
  
  return CY;
}

/*---------------------------------------------------------------------
 ��������: ��IIC���߷���һ���ֽ�����
 ����˵��: dat - д�ֽ�
 ��������: ��
 ---------------------------------------------------------------------*/
void B_LUX_SendByte(uint8 dat)
{
  uint8 i;
  
  for (i=0; i<8; i++)         			//8λ������
  {
    if (dat&0x80)	B_LUX_SDA0_H;
    else	B_LUX_SDA0_L;                   //�����ݿ�
    
    B_LUX_Delay5us();             		//��ʱ
    B_LUX_SCL0_H;                		//����ʱ����
    B_LUX_Delay5us();             		//��ʱ
    B_LUX_SCL0_L;                		//����ʱ����
    B_LUX_Delay5us();             		//��ʱ
    dat <<= 1;              			//�Ƴ����ݵ����λ
  }
  
  B_LUX_RecvACK();
}

/*---------------------------------------------------------------------
 ��������: ��IIC���߽���һ���ֽ�����
 ����˵��: ��
 ��������: �����ֽ�
 ---------------------------------------------------------------------*/
uint8 B_LUX_RecvByte()
{
  uint8 i;
  uint8 dat = 0;
  B_LUX_SDA0_I;
  
  B_LUX_SDA0_H;                         //ʹ���ڲ�����,׼����ȡ����,
  for (i=0; i<8; i++)         	        //8λ������
  {
        dat <<= 1;
    B_LUX_SCL0_H;                       //����ʱ����
    B_LUX_Delay5us();             	//��ʱ
    dat |= B_LUX_SDA0_DAT;              //������               
    B_LUX_SCL0_L;                       //����ʱ����
    B_LUX_Delay5us();             	//��ʱ
    
	
  }
  B_LUX_SDA0_O;
  
  return dat;
}

/*---------------------------------------------------------------------
 ��������: дBH1750
 ����˵��: REG_Address - �Ĵ�����ַ
 ��������: ��
 ---------------------------------------------------------------------*/
void B_LUX_Single_Write(uint8 REG_Address)
{
  /*
  B_LUX_Start();                                //��ʼ�ź�
  B_LUX_SendByte(B_LUX_SlaveAddress);           //�����豸��ַ+д�ź�
  B_LUX_SendByte(REG_Address);                  //�ڲ��Ĵ�����ַ����ο�����pdf22ҳ 
  //  BH1750_SendByte(REG_data);                //�ڲ��Ĵ������ݣ���ο�����pdf22ҳ 
  B_LUX_Stop();                                 //����ֹͣ�ź�
  */
  
}

/*---------------------------------------------------------------------
 ��������: ��������BH1750�ڲ�����
 ����˵��: ��
 ��������: ��
 ---------------------------------------------------------------------*/
void B_LUX_Multiple_read(void)
{  
  /*
  uint8 i;	
  B_LUX_Start();                                //��ʼ�ź�
  B_LUX_SendByte(B_LUX_SlaveAddress+1);         //�����豸��ַ+���ź�
  
  for (i=0; i<3; i++)                           //������ȡ6����ַ���ݣ��洢��BUF
  {
    BUF_0[i] = B_LUX_RecvByte();                //BUF[0]�洢0x32��ַ�е�����
    if (i == 0x02)
    {
      
      B_LUX_SendACK(1);                         //���һ��������Ҫ��NOACK
    }
    else
    {		
      B_LUX_SendACK(0);                         //��ӦACK
    }
  }
  
  B_LUX_Stop();                                 //ֹͣ�ź�
  B_LUX_Delay5ms();
  */
}

/*---------------------------------------------------------------------
 ��������: ��ʼ�����մ�����
 ����˵��: ��
 ��������: ��
 ---------------------------------------------------------------------*/
void B_LUX_Init()
{
	
  B_LUX_SCL0_O;
  B_LUX_SDA0_O;
  
  //B_LUX_delay_nms(100);	                        //��ʱ100ms
  
  //B_LUX_Single_Write(0x01); 
  
}

/*---------------------------------------------------------------------
 ��������: ���ն�ȡ����
 ����˵��: ��
 ��������: ���ع���ֵ
 ---------------------------------------------------------------------*/
uint32 val32 = 0;
void B_LUX_GetLux(uint32 *vLux)
{  
  
  fp32 temp;
  uint8 i = 0;
  

  //---------------------------������
  B_LUX_Start();                                //��ʼ�ź�
  B_LUX_SendByte(B_LUX_SlaveAddress+0);         //�����豸��ַ+���ź�
  B_LUX_SendByte(0x00);
 
  B_LUX_Start();                                //��ʼ�ź�
  B_LUX_SendByte(B_LUX_SlaveAddress+1);         //�����豸��ַ+���ź�
  B_LUX_delay_nms(1);                         //��ʱ180ms
  for (i=0; i<=128; i++)                           //������ȡ6����ַ���ݣ��洢��BUF
  {
    BUF_0[i] = B_LUX_RecvByte();                //BUF[0]�洢0x32��ַ�е�����
    if (i == 128)
    {
      
      B_LUX_SendACK(1);                         //���һ��������Ҫ��NOACK
    }
    else
    {		
      B_LUX_SendACK(0);                         //��ӦACK
    }
  }
  B_LUX_Stop();                                 //ֹͣ�ź�
  
  //---------------------------
  
  
  /*
   //---------------------------д����
  B_LUX_Start();                                //��ʼ�ź�
  B_LUX_SendByte(B_LUX_SlaveAddress+0);         //�����豸��ַ+���ź�
  B_LUX_SendByte(0x10);
  for (i=0; i<=112; i++)                           //������ȡ6����ַ���ݣ��洢��BUF
  {
    B_LUX_SendByte(0x30+i); 
  }
  //B_LUX_SendByte(0x00); 
  B_LUX_Stop();                                 //ֹͣ�ź�
  //---------------------------
*/
  
  
  val32   = BUF_0[3];
  val32 <<= 8;
  val32  |= BUF_0[2];
  val32 <<= 8;
  val32  |= BUF_0[1];
  val32 <<= 8;
  val32  |= BUF_0[0];
  
  temp = (fp32)val32*1.4;			//����͸���Ƚ���ֵ*1.4
  *vLux = (uint32)(temp);
} 













