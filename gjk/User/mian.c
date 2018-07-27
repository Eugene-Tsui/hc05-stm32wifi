#include "stm32f10x.h"
#include "./usart/bsp_usart.h"
#include "./usart/bsp_usart_blt.h"
#include "./SysTick/bsp_SysTick.h"
#include "./hc05/bsp_hc05.h"
#include "bsp_esp8266.h"
#include "test.h"
#include "bsp_led.h"
#include <string.h>
#include <stdlib.h>


unsigned int Task_Delay[3]; 


BLTDev bltDevList;




char sendData[1024];
char linebuff[1024];

int main(void)
{
	 char* redata;
		char cStr [ 100 ] = { 0 };
	uint16_t len;
	static uint8_t hc05_role=1;
	unsigned long count;
	
	char hc05_mode[10]="SLAVE";
	char hc05_name[30]="HC05_SLAVE";
	char hc05_nameCMD[40];
	char str[20];
	
	  //初始化systick
	SysTick_Init();
	SysTick->CTRL|=SysTick_CTRL_ENABLE_Msk;

	USART_Config();
	
	ESP8266_Init (); 
	HC05_INFO("**********HC05模块实验************");
	
	if(HC05_Init() == 0)
		HC05_INFO("HC05模块检测正常。");
	else
	{
		HC05_ERROR("HC05模块检测不正常，请检查模块与开发板的连接，然后复位开发板重新测试。");
		while(1);
	}


	/*复位、恢复默认状态*/
	HC05_Send_CMD("AT+RESET\r\n",1);	
	delay_ms(800);
	
	HC05_Send_CMD("AT+ORGL\r\n",1);
	delay_ms(200);

	
	/*各种命令测试演示，默认不显示。
	 *在bsp_hc05.h文件把HC05_DEBUG_ON 宏设置为1，
	 *即可通过串口调试助手接收调试信息*/	
	
	HC05_Send_CMD("AT+VERSION?\r\n",1);
	
	HC05_Send_CMD("AT+ADDR?\r\n",1);
	
	HC05_Send_CMD("AT+UART?\r\n",1);
	
	HC05_Send_CMD("AT+CMODE?\r\n",1);
	
	HC05_Send_CMD("AT+STATE?\r\n",1);	

	HC05_Send_CMD("AT+ROLE=1\r\n",1);
	
	/*初始化SPP规范*/
	HC05_Send_CMD("AT+INIT\r\n",1);
	HC05_Send_CMD("AT+CLASS=0\r\n",1);
	HC05_Send_CMD("AT+INQM=1,9,48\r\n",1);
	
	/*设置模块名字*/
	//sprintf(hc05_nameCMD,"AT+NAME=%s\r\n",hc05_name);
	//HC05_Send_CMD(hc05_nameCMD,1);

	HC05_INFO("本模块名字为:%s ,模块已准备就绪。",hc05_name);

	while(1)
	{

		//搜索蓝牙模块，并进行连接
		if(Task_Delay[2]==0 && !IS_HC05_CONNECTED()) 
		{
			if(hc05_role == 1)	//主模式 AT+STATE?
			{
				HC05_INFO("正在扫描蓝牙设备...");
				linkHC05();
				Task_Delay[2]=3000; //此值每1ms会减1，减到0才可以重新进来这里，所以执行的周期是3s
			}
		}				
		
			//连接后每隔一段时间检查接收缓冲区
		if(Task_Delay[0]==0 && IS_HC05_CONNECTED())  
		{
				uint16_t linelen;

				/*获取数据*/
				redata = get_rebuff(&len); 
				linelen = get_line(linebuff,redata,len);
			
				/*检查数据是否有更新*/
				if(linelen<200 && linelen != 0)
				{
					 
					sprintf ( cStr,redata);
		
					ESP8266_SendString ( ENABLE, cStr, 0, Single_ID_0 );               //发送数据
					
					delay_ms ( 100 );
						//HC05_INFO("receive:\r\n%s",redata);
					/*处理数据后，清空接收蓝牙模块数据的缓冲区*/
					clean_rebuff();
					
				}
			Task_Delay[0]=500;//此值每1ms会减1，减到0才可以重新进来这里，所以执行的周期是500ms
		}
	}
	
	/*ESP8266_Init ();                                                               //初始化WiFi模块使用的接口和外设

	
	printf ( "\r\n野火 WF-ESP8266 WiFi模块测试例程\r\n" );                          //打印测试例程提示信息

	
  ESP8266_StaTcpClient_UnvarnishTest ();
	
	
  while ( 1 );*/
	 
}