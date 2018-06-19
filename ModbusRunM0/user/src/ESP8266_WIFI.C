
 

/*
*
*   USR-GM3 module
*	2018-5-28
*	GRPS module
*
*
*/
#include "main.h"


#include <stdio.h>
#include "port.h"
/* ----------------------- Modbus includes ----------------------------------*/
#include "mb.h"
#include "mbport.h"
#include "user_mb_app.h"


#include "M051Series.h"
#include "Uart3th.h"
#include "ESP8266_WIFI.h"
void rt_thread_entry_Weight(void* parameter);
extern USHORT   usSRegHoldBuf[S_REG_HOLDING_NREGS] ;
extern unsigned long  Systick;
extern uint32_t UID;
//The ADC conver chip is <HX711>  -=-HW

 //The ADC Data defined
 #define  ADDO_UP 
 #define  ADDO_DOWN 
 #define  ADDO		 P03
//The ADC Clock defined
 #define  ADSK_UP 	   
 #define  ADSK_DOWN 
 #define  ADSK		 P01



#define NONE    255
#define	CMD1    1   
#define	CMD2    2   
#define	CMD3    3    


#define CMD4    4
#define CMD5    5
#define CMD6    6
#define CMD7    7
#define CMD8    8
#define CMD9    9
#define CMD10   10

#define CMDTIME  5000/*100 *1ms =100ms*/
#define SMARTCONFIGTIMEOUT  24  // ���ü�����CMDTIMEʱ����Ϊ������ʱֵ�� 4�Ļ�����4*CMDTIMEms
#define GETIDTIMEOUT        1*60*10  //����gitid����ʱ��10���� �ղ���getid������
static uint32_t WeightAD1,WeightAD2;//the RAW weight AD value
static float    WeightADCalibration=440;//Calibration adc =440
static uint32_t WeightADCSignBit=0;//SignBit 0:+ 1:-
static int Heavy=0,HeavyAlarm=1000;//Wet=1KG


extern unsigned char UartReciveBuf[16*8]; //uart reciver buff 
extern unsigned int  UartLen;		   //uart reciver byte lenth 
extern unsigned char RecivEvent;// 1:recived UART byte to buff 0:empty UART buff

uint8_t  wifi_cmd_state = NONE;
uint8_t  wifi_net_state =0;//�������� �ɹ� 1  ʧ��0
uint8_t  wifirep=1; 		 //�豸�ظ�
uint32_t wifireptimeoput=1000;//�豸�ظ���ʱʱ��Ĭ��1000ms��


uint32_t wifiTime;	
 int wificmdtimeout;
uint8_t startsend=0; 
 uint8_t smartwifi=0; 
 static uint32_t configtimeout;	
int32_t getidtiomeout=GETIDTIMEOUT; 
void SendData2Sever()
{

   
					/*		  
					FF AA 01 01	   ��ͷ
					
					CE 94 17	��id
					 
					02 88 88 00 00 00 
					
					wet
					
					
					AA FF 24 0D 0A 	���*/
				   char temp[24];

				   temp[0]=0xFF;
				   temp[1]=0xAA;
				   temp[2]=1;
				   temp[3]=1;


				   temp[4]=((UID>>24)+(UID>>16));
				   temp[5]=UID>>8;
				   temp[6]=UID;

				   temp[7]=2;
				   temp[8]=0x88;
				   temp[9]=0x88;
				   temp[10]=0;
				   temp[11]=0;
				   temp[12]=0;

				   temp[13]=0x88;
				   temp[14]=0;
				   temp[15]=0;
				   temp[16]=0;


				   temp[17]=0xAA;
				   temp[18]=0xFF;
				   temp[19]=0x24;
				   temp[20]=0x0d;
				   temp[21]=0x0a;

			     wifiTime=Systick;
		 rt_thread_entry_Weight((void*)0);
		           temp[13]=Heavy>>24;
				   temp[14]=Heavy>>16;
				   temp[15]=Heavy>>8;	  //��������
				   temp[16]=Heavy>>0;

				   if(startsend)
				   {

				   	 startsend=0;

				   temp[0]=0xFF;
				   temp[1]=0xAA;
				   temp[2]=1;
				   temp[3]=1;


				   temp[4]=((UID>>24)+(UID>>16));
				   temp[5]=UID>>8;
				   temp[6]=UID;

				   temp[7]=1;
				   temp[8]=0x00;
				   temp[9]=0x26;
				   temp[10]=0;
				   temp[11]=0;
				   temp[12]=0;

				   temp[13]=0x00;


				   temp[14]=0xAA;
				   temp[15]=0xFF;
				   temp[16]=0x24;
				   temp[17]=0x0d;
				   temp[18]=0x0a;

				UART_Write(UART1,temp,19);//
	 			   }
				   else
				   {
			 UART_Write(UART1,temp,22);//	   
				   }




}


void WifiConnectNetMode(void)
{
		 
		//���ֻ�ܷ������ܴ����ɺ���Ľ��ܺ�������
	  switch(wifi_cmd_state)
	  {
			  		case  NONE:
					{
					 
						break;
					
					}

					 
			  		case CMD1:
					{
					
					   if(wifirep==1)
					   {
				 UART_Write(UART1,"AT+CWMODE_DEF=1\r\n",17);//��������ȴ��ظ�
						 				
					   	 	wifirep=0; 
					  }
					   break;
					
					}
			  	
			  		case CMD2:
					{
					
					   if(wifirep==1)
					   {
				//  UART_Write(UART1,"AT+CIPSTART=\"TCP\",\"192.168.0.6\",2301\r\n",38);//��������ȴ��ظ�
						 				
					    UART_Write(UART1,"AT+CIPSTART=\"TCP\",\"47.93.188.246\",5679\r\n",40);//��������ȴ��ظ�
						 	wifirep=0; 
					  }
					   break;
					
					}
					case CMD3:
					{
					
					   if(wifirep==1)
					   {
								
					        UART_Write(UART1,"AT+CIPMODE=1\r\n",14);//��������ȴ��ظ� 
						 	wifirep=0; 
					  }
					   break;
					
					}
			  	   case CMD4:
					{
					
					   if(wifirep==1)
					   {
								
					        UART_Write(UART1,"AT+CIPSEND\r\n",12);//��������ȴ��ظ�
						 	wifirep=0; 
					  }
					   break;
					
					}

   /*������wifi���ò���ָ�������TCP���Ӳ���ָ��*/

					 case CMD5:
					{
					
					   if(wifirep==1)
					   {
								
					        UART_Write(UART1,"AT+RESTORE\r\n",12);//�ָ����������ã���Ҫ���ԭ����wifi��Ϣ
						 	wifirep=0; 
					  }
					   break;
					
					}

					 case CMD6:
					{
					
					   if(wifirep==1)
					   {
								
					        UART_Write(UART1,"AT+CWMODE_DEF=1\r\n",17);//ģʽ
						 	wifirep=0; 
					  }
					   break;
					
					}
					 case CMD7:
					{
					
					   if(wifirep==1)
					   {
								
					        UART_Write(UART1,"AT+CWAUTOCONN=1\r\n",17);//�Զ�����Ap
						 	wifirep=0; 
					  }
					   break;
					
					}
						 case CMD8:
					{
					
					   if(wifirep==1)
					   {
								
					        UART_Write(UART1,"AT+CWSTARTSMART=3\r\n",19);//������������ģʽ
						 	wifirep=0; 
					  }
					   break;
					
					}
		
	  }






	 

}
				                          
void WifiSentEventPoll(int time)  //wifiӦ�ú����̣߳�
{
		 


			if((smartwifi)||((Systick-configtimeout>=(CMDTIME*SMARTCONFIGTIMEOUT))&&(usSRegHoldBuf[27]==88)))
			{
					  //�������������ɻ�������������ʱ����
						  if(smartwifi)
						  {
						  	usSRegHoldBuf[27]=55;//�����������
						  }
						  else
						  {
						  
						  	usSRegHoldBuf[27]=33;//���ó�ʱ
						  }

		
						 wifi_net_state=0;
						 wifi_cmd_state = CMD1;	//�����л���TCP����������������
						 startsend=0;
						 wifirep=1;
						 wificmdtimeout=0;
						 smartwifi=0;
						 P16=0;		//��λwifiģ��
					     DrvSYS_Delay(100000);
					 	 P16=1;
					
		}


	  	if((wifirep==0)&&(wificmdtimeout==0)) //�������ظ����
		{
			 wificmdtimeout=Systick;//��¼��ʼʱ��
		
		}
		else if(wifirep==1)
		{
		
		  wificmdtimeout=0;
		}

		if(wificmdtimeout)
		{
		
			if(Systick-wificmdtimeout>=(CMDTIME*2))
			{
				 if(usSRegHoldBuf[27]==88)
				 {
				   	wifi_cmd_state=CMD5;//���������ģʽ��ʱ��ô���½�������ģʽ
				    wifirep=1; //���¿�ʼ
					wificmdtimeout=0;

				 }
				 else
				 {

					    wifi_net_state=0;
						 wifi_cmd_state = CMD1;	//�����л���TCP����������������
						 startsend=0;
						 wifirep=1;
						 wificmdtimeout=0;
					P16=0;		//��λwifiģ��
				    DrvSYS_Delay(100000);
				    P16=1;
				 }

			
			
				
			}

		}


	 if(wifi_net_state==0) //���û�н���������ôִ��
	 {
	 
		if( Systick-wifiTime>=CMDTIME) //100MSִ��һ��
		{	wifiTime=Systick;
			
	     WifiConnectNetMode();//����ģ��ע�Ტ���ӵ�����
	 	
		}
		
							 
	 }
	 else	  //��������Ѿ���������ôִ��
	 {

	 if(Systick-wifiTime>=1000) //100MSִ��һ��
		{
		   SendData2Sever();
		   if((getidtiomeout>0)&&(( usSRegHoldBuf[29]==0)))
		   {
		     getidtiomeout--;	//����һ������--
		   }
		}
		
	 	   

	   //�շ�Ӧ�ò�����
	 
	 }


}
	  							     
void WifiReciveEventPoll(void)
{
		uint8_t sum;
	 if(RecivEvent)
	 {

				  //���յ���ATָ��ظ������ﴦ��



				     switch(wifi_cmd_state)
				   {
				   	  case  NONE:
						{
						 
							break;
						
						}
				  		 case CMD1:
						{
						  if(strstr(UartReciveBuf,"OK")!=0)
						 {
			
			
						 	wifi_cmd_state = CMD2;//�����ȴ�OKָ��
						 	  wifirep=1;
			
			
			
						 }
			
						  break;
						}
				   	   	case CMD2:
						{
						  if(strstr(UartReciveBuf,"AT+CIPSTART")!=0)
						 {
			
			
						 	wifi_cmd_state = CMD3;//�����ȴ�OKָ��
						 	  wifirep=1;
			
			
			
						 }
			
						  break;
						}
							case CMD3:
						{
						  if(strstr(UartReciveBuf,"AT+CIPMODE")!=0)
						 {
			
			
						 	wifi_cmd_state = CMD4;//�����ȴ�OKָ��
						 	  wifirep=1;
			
			
			
						 }
			
						  break;
						}
						case CMD4:
						{
						  if((strstr(UartReciveBuf,"AT+CIPSEND")!=0))
						 {
			
			
						 	 wifi_cmd_state = NONE;//��һ��ָ��
						 	  wifirep=1;
							  wifi_net_state=1;//����TCP����״̬
			
			
			
						 }
			
						  break;
						}
						 case CMD5:
						{
						  if((strstr(UartReciveBuf,"AT+RESTORE")!=0))
						 {
			
			
						 	 wifi_cmd_state = CMD6;//��һ��ָ��
						 	  wifirep=1;
			
						 }
			
						  break;
						}
						 case CMD6:
						{
						  if((strstr(UartReciveBuf,"AT+CWMODE_DEF=1")!=0))
						 {
			
			
						 	 wifi_cmd_state = CMD7;//��һ��ָ��
						 	  wifirep=1;
			
						 }
			
						  break;
						}
							 case CMD7:
						{
						  if((strstr(UartReciveBuf,"AT+CWAUTOCONN=1")!=0))
						 {
			
			
						 	 wifi_cmd_state = CMD8;//��һ��ָ��
						 	  wifirep=1;
			
						 }
			
						  break;
						}
						 	 case CMD8:
						{
						  if((strstr(UartReciveBuf,"AT+CWSTARTSMART=3")!=0))
						 {
			
			
						 	 wifi_cmd_state = NONE;//��һ��ָ��
						 	  wifirep=1;
			
						 }
			
						  break;
						}



		
	   }

		if(strstr(UartReciveBuf,"recvok")!=0)
		{
		
		   getidtiomeout=GETIDTIMEOUT;//

		}		 		  	
	   	  	
	   	 if(strstr(UartReciveBuf,"ERROR")!=0)
		 {

				getidtiomeout=GETIDTIMEOUT;//

			  
			             wifi_net_state=0;
						 wifi_cmd_state = CMD1;	//�����л���TCP����������������
						 startsend=0;
						 wifirep=1;
						 wificmdtimeout=0;
				P16=0;		//��λwifiģ��
			    DrvSYS_Delay(100000);
			    P16=1;
		

		 }
		 else if(strstr(UartReciveBuf,"getid")!=0)
		 {
		 		 getidtiomeout=GETIDTIMEOUT;//
				  startsend=1;//������������
		 }

		 else if(strstr(UartReciveBuf,"smartconfig connected wifi")!=0)
		 {
		 		  smartwifi=1;
				 
		 }



		memcpy(UartReciveBuf,0,16*8);
		//process  reciver byte
		UartLen=0;	//clear reciver byte lenth 
		RecivEvent=0;//reset


	  }
}

void WifiMain(uint32_t t)
{


			 usSRegHoldBuf[30]=getidtiomeout; 	
	   	 if((getidtiomeout==0))
		 {

				getidtiomeout=GETIDTIMEOUT;//

			  
			wifi_net_state=0;
			wifi_cmd_state = CMD1;	//�����л���TCP����������������
			startsend=0;
			wifirep=1;
			wificmdtimeout=0;
				P16=0;		//��λwifiģ��
			    DrvSYS_Delay(100000);
			    P16=1;
		

		 }
       
			usSRegHoldBuf[28]= wifi_cmd_state;//��¼������ǰ��ָ��


		   if(usSRegHoldBuf[27]==1)	//���д��1��������ģʽ
		    {
			   
		   		wifi_net_state=0; //���·�������
				usSRegHoldBuf[27]=88;//��������ģʽ
				configtimeout=	Systick;
				wifi_cmd_state=CMD5;	//�л�AT�������ģʽͷ
					P16=0;		//��λwifiģ��
				    DrvSYS_Delay(100000);
				    P16=1;
					
		    }

	  (void)WifiSentEventPoll(t);
	  (void)WifiReciveEventPoll();

}



void ConfigHWADCHX711(void);
void WifiInit(void)
{
  ConfigHWADCHX711();
 
 
  x3THPortSerialInit(0,115200,8,MB_PAR_NONE);
 x3THPortTimersInit(( 7UL * 220000UL ) / ( 2UL * /*ulBaudRate*/115200 ))	;
 wifi_net_state=0;
 wifi_cmd_state = CMD1;
 startsend=0;
 wifirep=1;
 wificmdtimeout=0;
 smartwifi=0;
 	P16=0;		//��λwifiģ��
	DrvSYS_Delay(100000);
	P16=1;

 //UART_Write(UART1,"AT\r\n",4);
 
}

 void delayp()
 {
		 	 //K=20 that mean's the physical time is about 1us by softwave
		   int k=20;
		while(k)k--;
 }


/* 
Sampling external ADC Start implementation 
The chip is HX711 by DATA and SCLK line
20160501 testing OK!!!
*/
uint32_t ReadADCHX711(void)	 //����128
{


		  uint32_t Count;
		  uint8_t i;

		  ADSK=0;
		  Count=0;
		  delayp();
		  while(ADDO);  
		  for(i=0;i<24;i++)
		  {
		    delayp();
		    ADSK=1; 
			Count=Count<<1;
			delayp();
			ADSK=0;
			if(ADDO)Count++;
		  }
		  ADSK=1;
		  Count=Count^0x800000;
		  delayp();
		  ADSK=0;
	
		 return  Count;
}
 void ConfigHWADCHX711(void)
{
	   
 	GPIO_SetMode(P0, BIT3, GPIO_PMD_INPUT);
	GPIO_SetMode(P0, BIT1, GPIO_PMD_OUTPUT );
	 
     WeightAD1= ReadADCHX711(); //Sampling ADC  1
     DrvSYS_Delay(1000000); //500ms
     WeightAD1= ReadADCHX711(); //Sampling ADC  2
	 DrvSYS_Delay(1000000);  //500ms
     WeightAD1= ReadADCHX711(); //Sampling ADC  3 init and save it to WeightAD that it's effective
     WeightAD2=WeightAD1;
	
} 
void CleanWeight(void)
{
   WeightAD1= ReadADCHX711();
   WeightAD2=WeightAD1;
}
 

 void UpdataWeight(uint32_t wet)
{

//����RS485-MODBUS HOLD �Ĵ���
  usSRegHoldBuf[23]= wet>>16;//��16��λg
  usSRegHoldBuf[24]= wet>>0;//��16��λg
  usSRegHoldBuf[25]= WeightADCSignBit;//������־
  if(usSRegHoldBuf[26]==0x01)//ȥƤ��־
  {
  	 CleanWeight();//ȥƤ
	 usSRegHoldBuf[26]=0x02;//ȥƤ���
  }



}

void rt_thread_entry_Weight(void* parameter)
{  



/******************���ش���������*****************************************************/
		
	
          WeightAD1 = ReadADCHX711(); //Sampling ADC

		if(WeightAD1>=WeightAD2)//��ǰ�ɼ����������ϴβ���
		{
		   WeightAD1=(WeightAD1-WeightAD2);//��������ֵ
		}
		else
		{
		  
		   WeightAD1=(WeightAD2-WeightAD1);//���㸺��ֵ

		   
		}


	   Heavy=((float)WeightAD1/(float)WeightADCalibration);//��������

	  	UpdataWeight(Heavy);//updata DATA
		

}





























































