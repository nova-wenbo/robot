#include <common.h>

void delay8us(uchar num)
{uchar i,j;
for(j=0;j<num;j++)
for (i=0;i<18;i++)
;
}		

void delay10us(uchar num)
{uchar i,j;
for(j=0;j<num;j++)
for (i=0;i<23;i++)
;
}
 
void delay100us(uchar num)
{uchar i,j;
for(j=0;j<num;j++)
for (i=0;i<1;i++)
delay10us(11);
}
 
void delay500us(uchar num)
{uchar i,j;
for(j=0;j<num;j++)
for (i=0;i<1;i++)
delay10us(55);
}


void delay1ms(uchar num)
{uchar i,j;
for(j=0;j<num;j++)
for (i=0;i<1;i++)
delay10us(110);
}

void delay10ms(uchar num)
{uchar i,j;
for(j=0;j<num;j++)
for (i=0;i<1;i++)
delay1ms(10);
}

void delay500ms(uchar num)
{uchar i,j;
for(j=0;j<num;j++)
for (i=0;i<50;i++)
delay1ms(10);
}

void delay1s(uchar num)
{uchar i,j;
for(j=0;j<num;j++)
for (i=0;i<1;i++)
delay10ms(100);
}