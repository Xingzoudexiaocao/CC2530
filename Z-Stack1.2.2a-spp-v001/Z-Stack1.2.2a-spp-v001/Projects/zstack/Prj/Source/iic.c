
/*��������ʾ���ٶȼƺ������ǵ�6λԭʼ����
****************************************/
#include "ioCC2530.h"
#include "iic.h"
#include "OnBoard.h"
#include "hal_types.h"
 

/*
  ���Ŷ���
*/
#define SDA P0_4
#define SCL P0_5




 

 
 
 
 
void ALLInit(void)
{
//   Init_IO();
   InitMPU6050();
}
 
 
/****************us��ʱ���� 32M���� ������MCU********************/
void delay_us(unsigned int u)
{
   u=4*u;
   MicroWait(u);
}
 
 
/*****************************MPU6050*******************************/
void WriteSDA_0(void) 
{
    P0SEL&=~0x10;
    P0DIR|=0x10;
    SDA=0;
}
 
 
void WriteSDA_1(void) 
{
    P0SEL&=~0x10;
    P0DIR|=0x10;
    SDA=1;
}
    
void WriteSCL_0(void)  
{
    P0SEL&=~0x20;
    P0DIR|=0x20;
    SCL=0;
}
 
 
void WriteSCL_1(void)  
{
    P0SEL&=~0x20;
    P0DIR|=0x20;
    SCL=1;
}

 
void ReadSDA(void)//sda����,=0
{
    P0SEL&=~0x10;P0DIR&=~0x10;
}

void SDA_OUT(void)//sda���,=1
{
    P0SEL&=~0x10;
    P0DIR|=0x10;
}
 
//void Init_IO(void)
//{
//  P0DIR|=0xc0;
//  P0SEL&=0x3f;
//}
// 
/**************************************
��ʼ�ź�
**************************************/
void MPU6050_Start(void)  
{
    WriteSDA_1();
    WriteSCL_1();
    delay_us(5);
    WriteSDA_0();
    delay_us(5);
    WriteSCL_0();
    delay_us(5);
}
 
 
/**************************************
ֹͣ�ź�
**************************************/
void MPU6050_Stop()  
{    
   WriteSDA_0();
   WriteSCL_1();
   delay_us(5);
   WriteSDA_1();
}
 
 
//I2C����Ӧ���ź�
//��ڲ���:ack (0:ACK 1:NAK)
void MPU6050_SendACK(unsigned char ack)
{
    
    SDA=ack;                     //дӦ���ź�
    WriteSCL_1();                   //����ʱ����
    delay_us(5);//Delay5us();    //��ʱ
    WriteSCL_0();                   //����ʱ����
    delay_us(5);//Delay5us();    //��ʱ
}
 
 
/**************************************
����Ӧ���ź�
**************************************/
unsigned char MPU6050_RecvACK()
{
    ReadSDA();
    WriteSCL_1();                       //����ʱ����
    delay_us(5);//Delay5us();    //��ʱ
    CY=SDA;                      //��Ӧ���ź�
    WriteSCL_0();                       //����ʱ����
    delay_us(5);//Delay5us();    //��ʱ
    P0DIR|=0x00;
    return CY;
}
 
 
/**************************************
��IIC���߷���һ���ֽ�����
**************************************/
void MPU6050_SendByte(unsigned char data)
{
    unsigned char i;
    SDA_OUT();
    
    for(i=0;i<8;i++)         //8λ������
    {       
        if(data&0x80)
        {WriteSDA_1();}
        else 
        {WriteSDA_0();}
        
        data<<=1;
        WriteSCL_1();            //����ʱ����
        delay_us(5);          //��ʱ
        WriteSCL_0();            //����ʱ����
        delay_us(5);          //��ʱ
    }
    MPU6050_RecvACK();
}
 
 
/**************************************
��IIC���߽���һ���ֽ�����
**************************************/
unsigned char MPU6050_RecvByte()
{
    unsigned char i;
    unsigned char dat = 0;
    WriteSDA_1();                  //ʹ���ڲ�����,׼����ȡ����,
    for (i=0; i<8; i++)         //8λ������
    {
        dat <<= 1;
        SCL = 1;                //����ʱ����
        delay_us(5);                 //��ʱ
        dat |= SDA;             //������               
        SCL = 0;                //����ʱ����
         delay_us(5);                 //��ʱ
    }
    return dat;
 
}
 
 
//******���ֽ�д��*******************************************
void Single_WriteI2C(unsigned char REG_Address,unsigned char REG_data)
{
    MPU6050_Start();                  //��ʼ�ź�
    MPU6050_SendByte(0xD0);   //�����豸��ַ+д�ź�
    MPU6050_SendByte(REG_Address);    //�ڲ��Ĵ�����ַ��
    MPU6050_SendByte(REG_data);       //�ڲ��Ĵ������ݣ�
    MPU6050_Stop();                   //����ֹͣ�ź�
}
 
 
//********���ֽڶ�ȡ*****************************************
unsigned char Single_ReadI2C(unsigned char REG_Address)
{
unsigned char REG_data;
MPU6050_Start();                   //��ʼ�ź�
MPU6050_SendByte(0xD0);    //�����豸��ַ+д�ź�
MPU6050_SendByte(REG_Address);     //���ʹ洢��Ԫ��ַ����0��ʼ	
MPU6050_Start();                   //��ʼ�ź�
MPU6050_SendByte(0xD0+1);  //�����豸��ַ+���ź�
REG_data=MPU6050_RecvByte();       //�����Ĵ�������
MPU6050_SendACK(1);                //����Ӧ���ź�
MPU6050_Stop();                    //ֹͣ�ź�
return REG_data;
}

void InitMPU6050()
{
	Single_WriteI2C(PWR_MGMT_1, 0x00);	//�������״̬
	Single_WriteI2C(SMPLRT_DIV, 0x07);
	Single_WriteI2C(CONFIG, 0x06);
	Single_WriteI2C(GYRO_CONFIG, 0x18);
	Single_WriteI2C(ACCEL_CONFIG, 0x01);
}

int GetData(unsigned char REG_Address)
{
	unsigned char H,L;
	H=Single_ReadI2C(REG_Address);
	L=Single_ReadI2C(REG_Address+1);
	return (H<<8)+L;   //�ϳ�����
}

unsigned int absi( int val) 
{
  if (val<0) return -1*val;
  else return val;
  
}


