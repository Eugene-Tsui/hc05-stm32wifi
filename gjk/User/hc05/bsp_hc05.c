#include "bsp_hc05.h"
#include "../usart/bsp_usart_blt.h"
#include <string.h>
#include <stdio.h>

//蓝牙设备列表，在main文件中定义
extern  BLTDev bltDevList;

 /**
  * @brief  初始化控制LED的IO
  * @param  无
  * @retval 无
  */
static void HC05_GPIO_Config(void)
{		
		GPIO_InitTypeDef GPIO_InitStructure;

		/*开启GPIO时钟*/
		RCC_APB2PeriphClockCmd( BLT_INT_GPIO_CLK|BLT_KEY_GPIO_CLK, ENABLE); 

		GPIO_InitStructure.GPIO_Pin = BLT_INT_GPIO_PIN;	
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING; 
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(BLT_INT_GPIO_PORT, &GPIO_InitStructure);	
	
		
		GPIO_InitStructure.GPIO_Pin = BLT_KEY_GPIO_PIN;
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
		GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
		GPIO_Init(BLT_KEY_GPIO_PORT, &GPIO_InitStructure);	

}



						   
	
 /**
  * @brief  向HC05模块发送命令并检查OK。只适用于具有OK应答的命令
	* @param  cmd:命令的完整字符串，需要加\r\n。
	* @param	clean 命令结束后是否清除接收缓冲区，1 清除，0 不清除
	* @template  复位命令：	HC05_Send_CMD("AT+RESET\r\n",1);	
  * @retval 0,设置成功;其他,设置失败.	
  */
uint8_t HC05_Send_CMD(char* cmd,uint8_t clean)
{	 		 
	uint8_t retry=5;
	uint8_t i,result=1;
	
	while(retry--)
	{
		BLT_KEY_HIGHT;
		delay_ms(10);
		Usart_SendString(HC05_USART,(uint8_t *)cmd);
		
			for(i=0;i<20;i++)
			{ 
				uint16_t len;
				char * redata;
				
				delay_ms(10);
				
				redata = get_rebuff(&len); 
				if(len>0)
				{
					if(redata[0]!=0)
					{
						HC05_DEBUG("send CMD: %s",cmd);

						HC05_DEBUG("receive %s",redata);
					}
					if(strstr(redata,"OK"))				
					{
						
						if(clean==1)
							clean_rebuff();
						return 0;
					}
					else
					{
						//clean_rebuff();
					}
				}
				else
				{					
					delay_ms(100);
				}		
			}
			HC05_DEBUG("HC05 send CMD fail %d times",retry);
		}
	
	HC05_DEBUG("HC05 send CMD fail ");
		
	if(clean==1)
		clean_rebuff();

	return result ;

}


 /**
  * @brief  使用HC05透传字符串数据
	* @param  str,要传输的字符串
  * @retval 无
  */
void HC05_SendString(char* str)
{
		BLT_KEY_LOW;
	
		Usart_SendString(HC05_USART,(uint8_t *)str);

}



 /**
  * @brief  初始化GPIO及检测HC05模块
  * @param  无
  * @retval HC05状态，0 正常，非0异常
  */
uint8_t HC05_Init(void)
{
	uint8_t i;
	
	HC05_GPIO_Config();

	BLT_USART_Config();
	
	for(i=0;i<BLTDEV_MAX_NUM;i++)
	{
		sprintf(bltDevList.unpraseAddr[i]," ");
		sprintf(bltDevList.name[i]," ");

	}	
		bltDevList.num = 0;

	return HC05_Send_CMD("AT\r\n",1);
}





 /**
  * @brief  把接收到的字符串转化成16进制形式的数字变量(主要用于转化蓝牙地址)
	* @param  纯粹的数字字符串
  * @retval 转化后的数字变量	
  */
unsigned long htoul(const char *str)
{


  long result = 0;

  if (!str)
    return 0;

  while (*str)
  {
    uint8_t value;

    if (*str >= 'a' && *str <= 'f')
      value = (*str - 'a') + 10;
    else if (*str >= 'A' && *str <= 'F')
      value = (*str - 'A') + 10;
    else if (*str >= '0' && *str <= '9')
      value = *str - '0';
    else
      break;

    result = (result * 16) + value;
    ++str;
  }

  return result;
}


 /**
  * @brief  在str中，跳过它前面的prefix字符串,
						如str为"abcdefg",prefix为"abc"，则调用本函数后返回指向"defg"的指针
	* @param  str,原字符串
	* @param  str_length，字符串长度
	* @param 	prefix，要跳过的字符串
  * @retval 跳过prefix后的字符串指针
  */
char *skipPrefix(char *str, size_t str_length, const char *prefix)
{

  uint16_t prefix_length = strlen(prefix);

  if (!str || str_length == 0 || !prefix)
    return 0;

  if (str_length >= prefix_length && strncmp(str, prefix, prefix_length) == 0)
    return str + prefix_length;

  return 0;
}

 /**
  * @brief  从stream中获取一行字符串到line中
	* @param  line,存储获得行的字符串数组
	* @param  stream，原字符串数据流
	* @param 	max_size，stream的大小 
  * @retval line的长度，若stream中没有‘\0’，'\r'，'\n'，则返回0
  */
int get_line(char* line, char* stream ,int max_size)  
{  
		char *p;	
    int len = 0;  
		p=stream;
    while( *p != '\0' && len < max_size ){  
        line[len++] = *p;  
				p++;
        if('\n' == *p || '\r'==*p)  
            break;  
    }
	
		if(*p != '\0' && *p != '\n' && *p != '\r')
			return 0;
  
      
    line[len] = '\0';  
    return len;  
} 


 /**
  * @brief  向HC05写入命令，不检查模块的响应
	* @param  command ，要发送的命令
	* @param  arg，命令参数，为0时不带参数，若command也为0时，发送"AT"命令
  * @retval 无
  */
void writeCommand(const char *command, const char *arg)
{
		char str_buf[50];

		BLT_KEY_HIGHT;
		delay_ms(10);

		if (arg && arg[0] != 0)
			sprintf(str_buf,"AT+%s%s\r\n",command,arg);
		else if (command && command[0] != 0)
		{
			sprintf(str_buf,"AT+%s\r\n",command);
		}
		else
			sprintf(str_buf,"AT\r\n");

		HC05_DEBUG("CMD send:%s",str_buf);
		
		Usart_SendString(HC05_USART,(uint8_t *)str_buf);

}


 
 /**
  * @brief  扫描周边的蓝牙设备，并存储到设备列表中。
	* @param  bltDev ，蓝牙设备列表指针
  * @retval 是否扫描到设备，0表示扫描到，非0表示没有扫描到
  */
uint8_t parseBluetoothAddress(BLTDev *bltDev)
{
  /* Address should look like "+ADDR:<NAP>:<UAP>:<LAP>",
   * where actual address will look like "1234:56:abcdef".
   */

	char* redata;
	uint16_t len;
	
	char linebuff[50];
	uint16_t linelen;
	
	uint16_t getlen=0;
	uint8_t linenum=0;	
	
	
	uint8_t i;
	
	char *p;
	

	HC05_Send_CMD("AT+INQ\r\n",0);

	redata =get_rebuff(&len);
	
	if(redata[0] != 0 && strstr(redata, "+INQ:") != 0)
	{
		HC05_DEBUG("rebuf =%s",redata);

getNewLine:
		while(getlen < len-2*linenum )
		{	
			linelen = get_line(linebuff,redata+getlen+2*linenum,len);
			if(linelen>50 && linelen != 0)
			{
				HC05_Send_CMD("AT+INQC\r\n",1);//退出前中断查询
				return 1;
			}
			
			getlen += linelen;
			linenum++;			
					
			p = skipPrefix(linebuff,linelen,"+INQ:");
			if(p!=0)
			{
				uint8_t num ;
				num = bltDev->num;
				
				strBLTAddr(bltDev,':');
				
				for(i=0;i<=num;i++)
				{
					if(strstr(linebuff,bltDev->unpraseAddr[i]) != NULL)	
					{
						goto getNewLine;	//!=null时，表示该地址与解码语句的地址相同
					}
				}							
					
				/*若蓝牙设备不在列表中，对地址进行解码*/	
				bltDev->addr[num].NAP = htoul(p);			
				p = strchr(p,':');

				if (p == 0)
				{
					HC05_Send_CMD("AT+INQC\r\n",1);//退出前中断查询
					return 1;
				}

				bltDev->addr[num].UAP = htoul(++p);
				p = strchr(p,':');

				if (p == 0)
				{
					HC05_Send_CMD("AT+INQC\r\n",1);//退出前中断查询
					return 1;
				}

				bltDev->addr[num].LAP = htoul(++p);
				
				/*存储蓝牙地址(字符串形式)*/
				sprintf(bltDev->unpraseAddr[num],"%X:%X:%X",bltDev->addr[num].NAP,bltDev->addr[num].UAP,bltDev->addr[num].LAP);

				bltDev->num++;
				
			}


		}
		
		clean_rebuff();
		
		HC05_Send_CMD("AT+INQC\r\n",1);//退出前中断查询
		return 0;
	}
	
	else
	{
		clean_rebuff();
		
		HC05_Send_CMD("AT+INQC\r\n",1);//退出前中断查询
		return 1;	
	}

}

 /**
  * @brief  把蓝牙地址转化成字符串形式
	* @param  bltDev ，蓝牙设备列表指针
	*	@param  delimiter, 分隔符。 根据需要使用':'或','。
  * @retval 无
  */
void strBLTAddr(BLTDev *bltDev,char delimiter)  
{
	uint8_t i;
	

	if(bltDev->num==0)
	{
		HC05_DEBUG("/*******No other BLT Device********/");
	}
	else
	{
		for(i=0;i<bltDev->num;i++)
		{
			sprintf(bltDev->unpraseAddr[i],"%X%c%X%c%X",bltDev->addr[i].NAP,delimiter,bltDev->addr[i].UAP,delimiter,bltDev->addr[i].LAP);
		}
	}

}


 /**
  * @brief  获取远程蓝牙设备的名称
	* @param  bltDev ，蓝牙设备列表指针
  * @retval 0获取成功，非0不成功
  */
uint8_t getRemoteDeviceName(BLTDev *bltDev)
{
	uint8_t i;
	char *redata;
	uint16_t len;
	
	char linebuff[50];
	uint16_t linelen;
	char *p;
	
	char cmdbuff[100];
	
	strBLTAddr(bltDev,',');

	HC05_DEBUG("device num =%d",bltDev->num);
	
	for(i=0;i<bltDev->num;i++)
	{
		sprintf(cmdbuff,"AT+RNAME?%s\r\n",bltDev->unpraseAddr[i]);
		HC05_Send_CMD(cmdbuff,0);
		
		redata =get_rebuff(&len);
		if(redata[0] != 0 && strstr(redata, "OK") != 0)
		{
			
			linelen = get_line(linebuff,redata,len);
			if(linelen>50 && linelen !=0 ) linebuff[linelen]='\0';	//超长截断
					
			p = skipPrefix(linebuff,linelen,"+RNAME:");
			if(p!=0)
			{
				strcpy(bltDev->name[i],p);
			}

		}
		else
		{
			clean_rebuff();
			return 1;	
		}
		
		clean_rebuff();
	
	}
	
	return 0;

}

 /**
  * @brief  输出蓝牙设备列表
	* @param  bltDev ，蓝牙设备列表指针
  * @retval 无
  */
void printBLTInfo(BLTDev *bltDev)  
{
	uint8_t i;

	if(bltDev->num==0)
	{
		HC05_DEBUG("/*******No remote BLT Device or in SLAVE mode********/");
	}
	else
	{
		HC05_DEBUG("扫描到 %d 个蓝牙设备",bltDev->num);

		for(i=0;i<bltDev->num;i++)
		{
			HC05_INFO("/*******Device[%d]********/",i);	
			HC05_INFO("Device Addr: %s",bltDev->unpraseAddr[i]);
			HC05_INFO("Device name: %s",bltDev->name[i]);
		}
	}

}



 /**
  * @brief  扫描蓝牙设备，并连接名称中含有"HC05"的设备
	* @param  无
  * @retval 0获取成功，非0不成功
  */
uint8_t linkHC05(void)
{
	uint8_t i=0;
	char cmdbuff[100];
	
	parseBluetoothAddress(&bltDevList);
	getRemoteDeviceName(&bltDevList);
	printBLTInfo(&bltDevList);
	
	
	for(i=0;i<=bltDevList.num;i++)
	{
		if(strstr(bltDevList.name[i],"HC-05") != NULL) //非NULL表示找到有名称部分为HC05的设备
		{
			HC05_INFO("搜索到远程HC-05模块，即将进行配对连接...");
			
			strBLTAddr(&bltDevList,',');		
			
			//配对
			sprintf(cmdbuff,"AT+PAIR=%s,20\r\n",bltDevList.unpraseAddr[i]);
			HC05_Send_CMD(cmdbuff,0);
				
			//连接	
			sprintf(cmdbuff,"AT+LINK=%s\r\n",bltDevList.unpraseAddr[i]);
						
			return HC05_Send_CMD(cmdbuff,0);		
		}
	
	}
	
	return 1;

}
