#include <reg52.h>
#include <math.h>
#include <delay.h>
#include <common.h>
sbit KG=P1^7;
uchar position[8];//用于记录8个舵机的位置
uchar pick_up[8];           //kouchu[8];各个io口轮流输出是屏蔽其它位
uchar arr[8];               //提供排序空间 paixu_ncha[8]=0;中间过度用      
uchar t0bit=0;			    //定时器0周期相同标志位
uchar N;

//关于扫尾值的参数
uchar zsax=15; //14h,z 平面sa 修正参数				   p da  z  xiao  
uchar psax=10; //15h,p 平面sa 修正参数交互2
uchar zsag=20; //16h,z 平面sa 过渡参数
uchar psag=20;//17h,p 平面sa 过渡参数
/*************************************************************************************
函数名称：void low_level_500u
函数功能：PWM信号低电平时间子程序，控制舵机PWM信号的低电平时间决定舵机转动的速度  积分思想
输入    ：time
输出    ：无
*************************************************************************************/
void low_level_500u(uint time)
{
  uint i; 
  for(i=0;i<time;i++)
  {delay500us(1);}
}
/*************************************************************************************
函数名称：void t0_init
函数功能：t0定时器初始化，用于舵机2.5ms定时
输入    ：无 
输出    ：无
备注    ：没有开启定时
*************************************************************************************/
void t0_init(void)
{
  TMOD  = TMOD & 0xf0;        //初始化定时器1的计数方式为方式1     无用
  TMOD  = TMOD | 0x01;

  TH0=0xee;			          //22.1184MHz,2.5Ms定时ee00
  TL0=0x00;
  ET0=1;						
  EA=1;
  t0bit=1;
}
/*************************************************************************************
函数名称：void low_level_t0
函数功能：同周期定时器0设置及启动程序，使每变化一个变化量的周期相同	 给寄存器填值
输入    ：THTL
输出    ：无
*************************************************************************************/
void low_level_t0(uint THTL)
{
  TH0=THTL>>8;			  //22.1184MHz,2.5Ms定时ee00
  TL0=THTL;
  t0bit=0;
  TR0=1;
}
/*************************************************************************************
函数名称：void T0_Interrupt(void) interrupt 1
函数功能：定时器t0中断服务函数函数	中断一次填一次值 标记一次一个2.5ms到了
输入    ：
输出    ：无
*************************************************************************************/
void T0_Interrupt(void) interrupt 1
{  
   TH0=0xee;		     //22.1184MHz,2.5Ms定时ee00
   TL0=0x00;
   t0bit=1;
}
/*************************************************************************************
函数名称：init_cpu
函数功能：单片机初始化程序
输入    ：无
输出    ：无
*************************************************************************************/
void  init_cpu(void)
{ 											  
	t0_init();						//定时器0初始化
	delay500us(10);
}
/*************************************************************************************
函数名称：array()
函数功能：排序子程序，将各个口的8位根据时间的长短排序 提供arr[i]=arr[i]- arr[i+1]相邻差值
输入    ：无
输出    ：无
*************************************************************************************/
void array()
{ 
	uchar i=0,j=0,x=0;

	pick_up[0]=0xFE;
    pick_up[1]=0xFD;
    pick_up[2]=0xFB;
    pick_up[3]=0xF7;
    pick_up[4]=0xEF;
    pick_up[5]=0xDF;
    pick_up[6]=0xBF;
    pick_up[7]=0x7F;	//赋值

	//冒泡排序
	for(i=0;i<=6;i++)
	{    for(j=i+1;j<=7;j++)
         {
		 	if(arr[i]<arr[j])//交换数据
			{
				x=arr[j];
				arr[j]=arr[i];
				arr[i]=x;
					  
				x=pick_up[j];
				pick_up[j]=pick_up[i];
                pick_up[i]=x;  
             }
         }
	}	
	for(i=0;i<7;i++)
    {
		arr[i]= arr[i]- arr[i+1];
    } 		
}
/*************************************************************************************
函数名称：void PWM_8()
函数功能：8路舵机输出子程序，实现8路舵机的PWM信号在最短的时间内输出
输入    ：position[i]
输出    ：无
备注    ：P0口
*************************************************************************************/
void PWM_8()
{
	uchar i=0,j;

	for(i=0;i<8;i++)          //取P0口舵机对应的值
	{arr[i]=position[i];}
    array();                 //排序计算   
	low_level_t0(0xee00);	  //开启定时器并赋初始值,   22.1184MHz,2.5Ms定时ee00			 	   		   
    P0=0xff;                  //使口P0全部拉高
    delay500us(1);            //调用延时500us函数
    for(i=0;i<8;i++)          //P0口8路同时输出
    {      
		for(j=0;j<arr[7-i];j++)	  //此时arr为（安位置大小）相邻舵机位置相差值
		{delay8us(1);}
        P0=P0&pick_up[7-i];		  //此时pick_up为 代表的是那一位为0 逻辑与后拉低
    } 
    while(t0bit==0);		  	   //等待2.5ms应该说是一个周期的到来  可自定义		
	TR0=0;	 					   //关闭定时器0
}
/*************************************************************************************
函数名称：initial_position();
函数功能：初始位置子程序，根据各个舵机的不同位置设置初始位置
输入    ：无
输出    ：无
*************************************************************************************/
void initial_position(void)                 
{
	uint i,j,x=0;
	for(i=0;i<30;i++)
	{	x++;
		position[0]=253; //0号(舵机)
    position[1]=125; //1号
    position[2]=132; //2号
    position[3]=124; //3号
    position[4]=125; //4号
    position[5]=118; //5号
    position[6]=125; //6号
    position[7]=125; //7号
    										
		PWM_8();
		if(x<10)
        {j=20;j--;
		low_level_500u(j);
		}
		else 
        {	j=10;
		    j++;
           	low_level_500u(j);
        }				       
	}
}
void go_ahead(void)
{
  uchar i,j,t;
  j=9;
	for(i=0;i<10;i++)
	{
		if(i<15)
		{
			position[0]++;
		}
		  PWM_8();
		  if(i<5){j--;}
		  if(i>=5&&i<70){j=2;}
		  if(i>=70&&i<75){j++;}
		  delay100us(8*j);
	  }	  
	delay500ms(4);
		
		for(i=0;i<10;i++)
	{
		if(i<15)
		{
			position[0]--;
		}
		  PWM_8();
		  if(i<5){j--;}
		  if(i>=5&&i<70){j=2;}
		  if(i>=70&&i<75){j++;}
		  delay100us(8*j);
	 }


}

void main()
{
	SP=0x70;     //堆栈指针初始化 
	P0M1 = 0x00;//P0 as Digital OUTPUT
	P0M0 = 0x00;     	
	init_cpu();//初始化cpu  设置定时器  
  delay500us(10);
  P0=0xff;
	delay500ms(1);
	initial_position();//初始位置 
	delay500ms(1);	
	while(KG)   
	{
		if(!KG)
		delay500us(1);            //调用延时500us函数
		if(!KG)
		break;
	}	  							
 

//	 go_ahead();   
   while(1);   
}
