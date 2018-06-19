#ifndef __main_h
#define __main_h

 /*sensor Config */

/*使用光照传感器则定义，反之*/
//#define USE_LUX
/*使用温湿度传感器则定义，反之*/
//#define USE_DHT11 

/*Module Config*/
/*使用USE_GPRS模块则开启，他和CO2/PM25/ESP8266_WIFI只能开一个,因为都用UART1*/
//#define USE_GPRS 
/*使用USE_PM25模块则开启，他和CO2/GPRS/ESP8266_WIFI只能开一个,因为都用UART1*/
//#define USE_PM25
/*使用USE_CO2模块则开启，他和PM25/GPRS/ESP8266_WIFI只能开一个,因为都用UART1*/
//#define USE_CO2
 /*使用USE_ESP8266_WIFI模块则开启，他和PM25/GPRS/ESP8266_WIFI只能开一个,因为都用UART1*/
#define USE_ESP8266_WIFI


/*KEY and RELAY Config */
/*使用KEY输入则定义，反之*/
#define USE_KEY 
/*使用RELAY输入则定义，反之*/
#define USE_RELAY 



/*Board Config */

/*定义这个表示使用软件配置地址，这个板子是CO2+DHT11+光照Modbus-RTU*/
//#define USE_CO2DHT11_SW_ADDR_BOARD
/*定义这个表示使用硬件薄码配置地址，这个板子是CO2+1~8路Modbus-RTU*/
#define USE_CO2DHT11RELAY8_HW_ADDR_BOARD







#ifdef   USE_CO2DHT11_SW_ADDR_BOARD
#define  GET_SW_ADDR   //软件从机地址
#else
#define  GET_HW_ADDR   //硬件薄码开关
#endif



#endif
