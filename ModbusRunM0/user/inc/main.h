#ifndef __main_h
#define __main_h

 /*sensor Config */

/*ʹ�ù��մ��������壬��֮*/
//#define USE_LUX
/*ʹ����ʪ�ȴ��������壬��֮*/
//#define USE_DHT11 

/*Module Config*/
/*ʹ��USE_GPRSģ������������CO2/PM25/ESP8266_WIFIֻ�ܿ�һ��,��Ϊ����UART1*/
//#define USE_GPRS 
/*ʹ��USE_PM25ģ������������CO2/GPRS/ESP8266_WIFIֻ�ܿ�һ��,��Ϊ����UART1*/
//#define USE_PM25
/*ʹ��USE_CO2ģ������������PM25/GPRS/ESP8266_WIFIֻ�ܿ�һ��,��Ϊ����UART1*/
//#define USE_CO2
 /*ʹ��USE_ESP8266_WIFIģ������������PM25/GPRS/ESP8266_WIFIֻ�ܿ�һ��,��Ϊ����UART1*/
#define USE_ESP8266_WIFI


/*KEY and RELAY Config */
/*ʹ��KEY�������壬��֮*/
#define USE_KEY 
/*ʹ��RELAY�������壬��֮*/
#define USE_RELAY 



/*Board Config */

/*���������ʾʹ��������õ�ַ�����������CO2+DHT11+����Modbus-RTU*/
//#define USE_CO2DHT11_SW_ADDR_BOARD
/*���������ʾʹ��Ӳ���������õ�ַ�����������CO2+1~8·Modbus-RTU*/
#define USE_CO2DHT11RELAY8_HW_ADDR_BOARD







#ifdef   USE_CO2DHT11_SW_ADDR_BOARD
#define  GET_SW_ADDR   //����ӻ���ַ
#else
#define  GET_HW_ADDR   //Ӳ�����뿪��
#endif



#endif
