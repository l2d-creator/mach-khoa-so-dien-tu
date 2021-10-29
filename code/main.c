#include <AT89X51.H>
#include <stdio.h>
#include<string.h>
#define RS P1_0//RS=0 =>Cho Phép SD Thanh Ghi Lenh || RS=1 => CHo Phep Su Dung Thanh Ghi Du Lieu
#define RW P1_1//RW=0 =>Cho Phép Ghi Du Lieu Len LCD || RW=1 =>Doc Thong Tin Tu LCD   
#define EN P1_2// Enable Chân cho phép
#define LCD_PORT P0
#define H1	 P2_0
#define H2	 P2_1
#define H3	 P2_2
#define H4	 P2_3  
#define C1	 P2_4
#define C2	 P2_5
#define C3	 P2_6
#define C4	 P2_7
#define LOA  P3_7
#define ON   P1_3
int i,t;
int KEY[8]={0,0,0,0,0,0,0,0};//CODE BAN DAU
int KEYT[20];			   //CODE TAM
int KEYT1[8];			   //CODE TAM DE LUU MAT KHAU KHI DOI MA	
int SLNS=0;				   //SOLAN NHAP SAI
int TTL=0;				   //TRANG THAI LOCK															f
int TTC=0;				   //TRANG THAI CHANGE
int NM=0;
int KEYI=0;
int n[5] = {0xfe,0xfd,0xfb,0xf7};

void delay_ms(int n)			  //Ham DELAY_MS	
{	int k,j;									 
	for(k=0;k<n;k++)
	{	for(j=0;j<500;j++);		}
}
void LCDWriteCmd(unsigned char c) //Ham Ghi Lenh -> LCD
{	RS=0;  //ngo vao lenh
 	RW=0;
	LCD_PORT=c;	//bat hien thi tat con tro
	EN=1;
	EN=0;
 	delay_ms(5);
}
void LCDWriteData(unsigned char c)//Ham Ghi Du Lieu -> LCD
{	RS=1;  //ngo vao du lieu
 	RW=0;	// ghi du lieu
	LCD_PORT=c;
 	EN=1;//LCD hoat dong binh thuong
 	EN=0;//Cam truy xuat
 	delay_ms(5);
}
void LCDcursorxy(int x, int y)	  //Ham Dua Con Tro Den Toa Do (x,y) LCD
{	if((x<1||x>2)&&(y<1||y>16))
	{	x=1;	y=1;   	}
	if(x == 1)	  LCDWriteCmd(0x7F+y);
	else		  LCDWriteCmd(0xBF+y);
}
void LCD_init() 				  // Ham Khoi Tao LCD
{	RS=0;
	RW=0;//ghi du lieu
	EN=0;//cam truy xuat
	delay_ms(15);
 	LCDWriteCmd(0x38);//Hai Dong Ma Tran 16x2
	LCDWriteCmd(0x0C);//Dua Con Tro Ve Dau Dong 2
 	LCDWriteCmd(0x01);//Xoa man hinh LCD
}
void LCD_putstr(unsigned char *s) // Ham Xuat Chuoi Ky Tu -> LCD
{ while (*s)	{	 LCDWriteData(*s);	s++;	}  }
void BEEP(int i)
{	LOA=1;
	delay_ms(i);
	LOA=0;
} 	
void HAMCHINH()
{	for(i=0;i<=3;i++)
	{	P2 = n[i];
//PHIM UNLOCK
	if(P2==0x7e&&TTL==0&TTC==0&&NM==0)
		{	
			delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	BEEP(25);
				LCD_init() ;//goi ham khoi tao LCD
				LCDWriteCmd(0x01);
				LCDcursorxy(1,1);
				LCD_putstr("MO KHOA ?");		  
				LCDcursorxy(2,1);
				LCDWriteCmd(0x0f);
				NM=1;//co nhap ma
				KEYI=0;
			}
		}
//PHIM LOCK
	if(P2==0x7d)
		{	delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{
			BEEP(20);
			LCDWriteCmd(0x01); //Xoa Man Hinh
			LCD_putstr("    DA KHOA");
			LCDcursorxy(2,1); //tao 2 dong
			LCD_putstr("  VAN TRUNG	");
			LCDWriteCmd(0x0C);
			TTL=0;//cam ko cho nhap
			ON=1;
			NM=0;//ko cho tac dong phim
			}
		}
//PHIM OK
	if(P2==0xe7&&NM==1)
	{	delay_ms(10);
		if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1&&TTC==0)
		{	if(KEY[0]==KEYT[0]&&KEY[1]==KEYT[1]&&KEY[2]==KEYT[2]&&KEY[3]==KEYT[3]&&
			   KEY[4]==KEYT[4]&&KEY[5]==KEYT[5]&&KEY[6]==KEYT[6]&&KEY[7]==KEYT[7])
			   {	BEEP(20);	   
			   		LCDWriteCmd(0x01);		LCD_putstr("DA MO KHOA!");
					LCDcursorxy(2,1);		LCD_putstr("DienTu L2D!");
					LCDWriteCmd(0x0C);
					ON=0;//DUNG MK MO KHOA
					NM=2;
					SLNS=0;
					KEYT[0]=KEYT[1]=KEYT[2]=KEYT[3]=KEYT[4]=KEYT[5]=KEYT[6]=KEYT[7]=10;
			   }
			else
			{
				if(SLNS>3)
				{
					LCDWriteCmd(0x01);		LCD_putstr(" SAI MAT KHAU!");
					LCDcursorxy(2,1);		LCD_putstr("HE THONG BI KHOA");
					for(t=0;t<50;t++)
					{	BEEP(100);	delay_ms(100);	 }
					SLNS=0;
					break;	
				}
				else
				{
					LCDWriteCmd(0x01);		LCD_putstr(" SAI MAT KHAU! ");
					LCDcursorxy(2,1);		LCD_putstr("HAY THU LAI!");
					delay_ms(100);			BEEP(100);
					LCDWriteCmd(0x01);		LCD_putstr("MO KHOA ?");
					LCDcursorxy(2,1);
					LCDWriteCmd(0x0f);
					KEYI=0;
					SLNS=SLNS+1;
				}
			}
		}
		if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1&&TTC==1) //Lap Trinh Phim OK Khi Nhan CHANGE
			if(KEY[0]==KEYT[0]&&KEY[1]==KEYT[1]&&KEY[2]==KEYT[2]&&KEY[3]==KEYT[3]&&
			   KEY[4]==KEYT[4]&&KEY[5]==KEYT[5]&&KEY[6]==KEYT[6]&&KEY[7]==KEYT[7])
			   {	BEEP(20);
					KEYI=0;
			   		LCDWriteCmd(0x01);		LCD_putstr("MAT KHAU MOI ?");
					LCDcursorxy(2,1);
					LCDWriteCmd(0x0f);
					TTC=2;
					NM=1;
					SLNS=0;
					KEYT[0]=KEYT[1]=KEYT[2]=KEYT[3]=KEYT[4]=KEYT[5]=KEYT[6]=KEYT[7]=10;
					KEYI=0;
					break;
			   }
			else  
			{	if(SLNS>3)
				{	LCDWriteCmd(0x01);		LCD_putstr("   SAI MAT KHAU!");
					LCDcursorxy(2,1);		LCD_putstr("HE THONG BI KHOA");
					for(t=0;t<50;t++)
					{	BEEP(100);	 delay_ms(300);	   }
					SLNS=0;
					break;
				}
			   else
			   {	LCDWriteCmd(0x01);		LCD_putstr("   MAT KHAU SAI!");
					LCDcursorxy(2,1);   	LCD_putstr("HE THONG BI KHOA");
					delay_ms(100);			BEEP(400);
					LCDWriteCmd(0x01);		LCD_putstr("MAT KHAU CU ?");
					LCDcursorxy(2,1);
					LCDWriteCmd(0x0f);
					KEYI=0;	TTC=1;
					SLNS=SLNS+1;
					break;				
			   }
			}
		if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1&&TTC==2)//OK Nhan Sau Khi Nhap Ma Moi	 //Yeu Cau Xac Nhan Ma Moi
		{	if(KEYI==8)	
			{		BEEP(20);
					KEYT1[0]=KEYT[0];KEYT1[1]=KEYT[1];KEYT1[2]=KEYT[2];KEYT1[3]=KEYT[3];
					KEYT1[4]=KEYT[5];KEYT1[5]=KEYT[5];KEYT1[6]=KEYT[6];KEYT1[7]=KEYT[7]; //LUU MA MOI
					KEYT[0]=KEYT[1]=KEYT[2]=KEYT[3]=KEYT[4]=KEYT[5]=KEYT[6]=KEYT[7]=10;  //XOA MA TAM
					KEYI=0;
			   		LCDWriteCmd(0x01);		LCD_putstr("XAC NHAN LAI");
					LCDcursorxy(2,1);
					LCDWriteCmd(0x0f);
					TTC=3;
					NM=1;
					SLNS=0;break;
			}
			if(KEYI!=8&&TTC==2)
			{  LCDWriteCmd(0x0C);
		 	   LCDWriteCmd(0x01);		LCD_putstr("    LOI!  ");
			  LCDcursorxy(2,1);			LCD_putstr(" 8 KI TU ");
			  BEEP(400);
			  delay_ms(400);	 	 LCDWriteCmd(0xC0);
			  BEEP(20);	  TTC=1;
			  LCDWriteCmd(0x01);		LCD_putstr("MAT KHAU CU ?");
			  LCDcursorxy(2,1);
			  LCDWriteCmd(0x0f);
			  KEYI=0; break;
			}
		}

		if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1&&TTC==3)//Neu Xac Nhan Ma Moi DUNG
			if(KEYT1[0]==KEYT[0]&&KEYT1[1]==KEYT[1]&&KEYT1[2]==KEYT[2]&&KEYT1[3]==KEYT[3]&&
			   KEYT1[4]==KEYT[4]&&KEYT1[5]==KEYT[5]&&KEYT1[6]==KEYT[6]&&KEYT1[7]==KEYT[7])
			   {	BEEP(20);
			   		LCDWriteCmd(0x01);		LCD_putstr("   MAT KHAU ");
					LCDcursorxy(2,1);		LCD_putstr("DA THAY DOI!"); 
					LCDWriteCmd(0x0C);
					NM=0;
					TTC=0;
					KEY[0]=KEYT[0];KEY[1]=KEYT[1];KEY[2]=KEYT[2];KEY[3]=KEYT[3];
					KEY[4]=KEYT[4];KEY[5]=KEYT[5];KEY[6]=KEYT[6];KEY[7]=KEYT[7];
			   }
			 else
			 	{
				BEEP(20);
				NM=1;
				LCDWriteCmd(0x01);		LCD_putstr("    LOI !             ");
				BEEP(400);				delay_ms(400);
				LCDWriteCmd(0x01);
				LCDcursorxy(1,1);
				LCD_putstr("Old code?");		  
				LCDcursorxy(2,1);
				LCDWriteCmd(0x0f);
				TTC=1;
				}	
	}
//PHIM CHANGE
	if(P2==0x7b)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{  	BEEP(20);
				NM=1;
				LCDWriteCmd(0x01);
				LCDcursorxy(1,1);
				LCD_putstr("MAT KHAU CU?");		  
				LCDcursorxy(2,1);
				LCDWriteCmd(0x0f);
				TTC=1;																				 
			}
	}
//PHIM CANCEL
	if(P2==0x77)
	{	delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{
			BEEP(20);
			LCDWriteCmd(0x01);
			LCD_putstr("   DA KHOA!");
			LCDcursorxy(2,1);
			LCD_putstr("DienTuL2D");
			LCDWriteCmd(0x0C);
			TTL=0;
			TTC=1;
			ON=1;
			NM=0;
			}
	}
//Phim Clear
	if(P2==0xb7&&NM==1)
	{	delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{
			BEEP(20);
			
			LCDcursorxy(2,1);
			LCDWriteCmd(0x0C);
			LCD_putstr("                ");
			LCDcursorxy(2,1);
			LCDWriteCmd(0x0f);
			KEYI=0;
			}
	}
//PHIM 0
	if(P2==0xd7&&NM==1)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	LCD_putstr("*");		  
				LCDWriteCmd(0x0f);
				LCDWriteCmd(0x06);
				KEYT[KEYI]=0;
				KEYI=KEYI+1;
				BEEP(20);
			}
	}

//PHIM 1
	if(P2==0xee&&NM==1)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	LCD_putstr("*");		  
				LCDWriteCmd(0x0f);
				LCDWriteCmd(0x06);
				KEYT[KEYI]=1;
				KEYI=KEYI+1;
				BEEP(20);	
			}
	}
//PHIM 2
	if(P2==0xde&&NM==1)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	LCD_putstr("*");		  
				LCDWriteCmd(0x0f);
				LCDWriteCmd(0x06);
				KEYT[KEYI]=2;
				KEYI=KEYI+1;
				BEEP(20);
			}
	}
//PHIM 3
	if(P2==0xbe&&NM==1)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	LCD_putstr("*");		  
				LCDWriteCmd(0x0f);
				LCDWriteCmd(0x06);
				KEYT[KEYI]=3;
				KEYI=KEYI+1;
				BEEP(20);
			}
	}
//PHIM 4
	if(P2==0xed&&NM==1)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	LCD_putstr("*");		  
				LCDWriteCmd(0x0f);
				LCDWriteCmd(0x06);
				KEYT[KEYI]=4;
				KEYI=KEYI+1;
				BEEP(20);
			}
	}
//PHIM 5
	if(P2==0xdd&&NM==1)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	LCD_putstr("*");		  
				LCDWriteCmd(0x0f);
				LCDWriteCmd(0x06);
				KEYT[KEYI]=1;
				KEYI=KEYI+1;
				BEEP(20);	
			}
	}
//PHIM 6
	if(P2==0xbd&&NM==1)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	LCD_putstr("*");		  
				LCDWriteCmd(0x0f);//tat con tro nhap nhay con tro 
				LCDWriteCmd(0x06);//dich con tro sang phai
				KEYT[KEYI]=6;
				KEYI=KEYI+1;
				BEEP(20);	
			}
	}
//PHIM 7
	if(P2==0xeb&&NM==1)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	LCD_putstr("*");		  
				LCDWriteCmd(0x0f);
				LCDWriteCmd(0x06);
				KEYT[KEYI]=7;
				KEYI=KEYI+1;
				BEEP(20);	
			}
	}
//PHIM 8
	if(P2==0xdb&&NM==1)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	LCD_putstr("*");		  
				LCDWriteCmd(0x0f);
				LCDWriteCmd(0x06);
				KEYT[KEYI]=8;
				KEYI=KEYI+1;
				BEEP(20);
			}
	}
//PHIM 9
	if(P2==0xbb&&NM==1)
	{		delay_ms(10);
			if(P2_4==1&&P2_5==1&&P2_6==1&&P2_7==1)
			{	LCD_putstr("*");		  
				LCDWriteCmd(0x0f);
				LCDWriteCmd(0x06);
				KEYT[KEYI]=9;
				KEYI=KEYI+1;
				BEEP(20);
			}
	}
}
}
void main(void)					  //CHUONG TRINH CHINH
{
LOA=0;	  ON=1;
LCD_init();
LCDWriteCmd(0x01);
LCDWriteCmd(0x38);
LCD_putstr("    DA KHOA");
LCDcursorxy(2,1);
LCD_putstr(" VAN TRUNG K36");
LCDWriteCmd(0xC0);
RS=1;  EN=0;	RW=0;
KEYT[0]=KEYT[1]=KEYT[2]=KEYT[3]=KEYT[4]=KEYT[5]=KEYT[6]=KEYT[7]=10;	//xoa gia tri trong bien tam
while (1)
	{
	HAMCHINH();
	}
}
