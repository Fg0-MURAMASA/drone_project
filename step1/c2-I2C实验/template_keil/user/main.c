#include "stm32f10x.h"
#include <stdio.h>
#include <Delay.h>

// HMC5883L的参数
#define HMC5883L_ADDRESS 0x1E // HMC5883L的I2C地址（7位地址）
#define HMC5883L_REG_CONFA 0x00 // 配置寄存器A
#define HMC5883L_REG_CONFB 0x01 // 配置寄存器B
#define HMC5883L_REG_MODE 0x02 // 模式寄存器
#define HMC5883L_REG_X_OUT_L 0x03 // X轴输出低字节
#define HMC5883L_MODE_CONTINOUS 0x00 // 连续模式
#define HMC5883L_AVERAGING_8 0x40 // 8次平均值
#define HMC5883L_RATE_15 0x03 // 15Hz的数据输出率

//LSM303D的参数
#define LSM303D_ACCEL_ADDR 0x19 // LSM303D的加速度计I2C地址
#define LSM303D_GYRO_ADDR 0x32  // LSM303D的陀螺仪I2C地址
#define LSM303D_REG_ACCEL_CTRL_REG1_A 0x20 // 加速度控制寄存器1
#define LSM303D_REG_ACCEL_OUT_X_L_A 0x28   // 加速度X轴输出低字节
#define LSM303D_REG_GYRO_CTRL_REG1_G 0x20   // 陀螺仪控制寄存器1
#define LSM303D_REG_GYRO_OUT_X_L_G 0x22     // 陀螺仪X轴输出低字节

//BMP180的参数
#define BMP180_ADDRESS 0x77 // BMP180的I2C地址
#define BMP180_REG_CALIBRATION 0xAA // 校准寄存器起始地址
#define BMP180_REG_CONTROL 0xF4     // 控制寄存器地址
#define BMP180_CONTROL_MODE_ULTRALOWPOWER 0x03 // 超低功耗模式
#define BMP180_CONTROL_MODE_STANDARD 0x07 // 标准模式
#define BMP180_CONTROL_MODE_HIGHRES 0x08 // 高分辨率模式
#define BMP180_CONTROL_MODE_ULTRAHIGHRES 0x0B // 超高分辨率模式


void The_i2c_init(void);
void myiic_w_scl(uint8_t value);
void myiic_w_sda(uint8_t value);
uint8_t myiic_r_sda(void);
void myiic_start(void);
void myiic_stop(void);
void myiic_sendbyte(uint8_t byte);
uint8_t myiic_receivebyte(void);
void myiic_sendack(uint8_t byte);
uint8_t myiic_receiveack(void);
void myiic_writereg(uint8_t addr,uint8_t reg,uint8_t data);
void myiic_readreg(uint8_t address, uint8_t reg, uint8_t* data, uint16_t size);
void HMC5883L_Init(void);
void HMC5883L_ReadData(uint16_t *x, uint16_t *y, uint16_t *z);
void LSM303D_Accel_Init(void);
void LSM303D_Gyro_Init(void);
void LSM303D_ReadAccel(uint16_t *x, uint16_t *y, uint16_t *z);
void LSM303D_ReadGyro(uint16_t *x, uint16_t *y, uint16_t *z);
uint16_t ac1, ac2, ac3, b1, b2, mb, mc, md;
void BMP180_ReadCalibration(void);
void BMP180_Init(uint8_t mode);
uint32_t BMP180_ReadUncompensatedTemperature(void);
uint32_t BMP180_ReadUncompensatedPressure(uint8_t mode);

int main(void)
{
	
	while(1)  
	{
	}
}

void The_i2c_init(void){
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	GPIO_InitTypeDef GPIO_InitStruct;
	GPIO_InitStruct.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7;
	GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
	GPIO_InitStruct.GPIO_Speed = GPIO_Speed_2MHz;
	
	GPIO_Init(GPIOB, &GPIO_InitStruct);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, ENABLE);
	RCC_APB1PeriphResetCmd(RCC_APB1Periph_I2C1, DISABLE);
	
	I2C_InitTypeDef I2C_InitStruct;
	
	I2C_InitStruct.I2C_ClockSpeed = 400000;
	I2C_InitStruct.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStruct.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_Init(I2C1, &I2C_InitStruct);
	
	I2C_Cmd(I2C1, ENABLE);
}

void myiic_w_scl(uint8_t value){
	GPIO_WriteBit(GPIOC,GPIO_Pin_6,(BitAction)value);
	Delay(10);
}
void myiic_w_sda(uint8_t value){
	GPIO_WriteBit(GPIOC,GPIO_Pin_7,(BitAction)value);
	Delay(10);
}
uint8_t myiic_r_sda(){
	uint8_t value;
	value=GPIO_ReadInputDataBit(GPIOC,GPIO_Pin_3);
	Delay(10);
 return value;
}
void myiic_start(){
	myiic_w_sda(1);
	myiic_w_scl(1);
	myiic_w_sda(0);
	myiic_w_scl(0);
}
 void myiic_stop(){
	myiic_w_sda(0);
	myiic_w_scl(1);
	myiic_w_sda(1);
 }
  void myiic_sendbyte(uint8_t byte){ 
	int i;
	 for(i=0;i<8;i++){
		myiic_w_sda(byte&(0x80>>i));
		myiic_w_scl(1);
		myiic_w_scl(0);	
		}
}
 
 uint8_t myiic_receivebyte(){
	int i;
	uint8_t byte=0x00;
	myiic_w_sda(1);
	for(i=0;i<8;i++){
	 myiic_w_scl(1);
		if(myiic_r_sda()==1){
			byte|=(0x80>>i);
			}
		myiic_w_scl(0);
		}
	 return byte;
 }
  void myiic_sendack(uint8_t byte){
	myiic_w_sda(byte);
	myiic_w_scl(1);
	myiic_w_scl(0);
} 

 uint8_t myiic_receiveack(void){   
	uint8_t byte;
	myiic_w_sda(1);
	myiic_w_scl(1);
	byte=myiic_r_sda();
	myiic_w_scl(0);
	return byte; 
}
 void myiic_writereg(uint8_t addr,uint8_t reg,uint8_t data){
	myiic_start();
	myiic_sendbyte(addr & 0xfe);
	myiic_receiveack();
	myiic_sendbyte(reg);
	myiic_receiveack();
	myiic_sendbyte(data);
	myiic_receiveack();
	myiic_stop();
 }
void myiic_readreg(uint8_t address, uint8_t reg, uint8_t* data, uint16_t size){
	 myiic_start();
	 myiic_sendbyte(address & 0xfe);
	 myiic_receiveack();
	 myiic_sendbyte(reg);
	 myiic_receiveack();
	 myiic_start();
	 myiic_sendbyte(address|0x01);
	 myiic_receiveack();
	 for(int i=0; i<size-1; i++){
		data[i]=myiic_receivebyte();
		 	 myiic_sendack(0);
	 }
	 data[size-1]=myiic_receivebyte();
	 myiic_sendack(1);
	 Delay(10);
	 myiic_stop();
 }
 

void HMC5883L_Init()
{
    // 配置HMC5883L
    myiic_writereg(HMC5883L_ADDRESS, HMC5883L_REG_CONFA, HMC5883L_AVERAGING_8 | HMC5883L_RATE_15);
    myiic_writereg(HMC5883L_ADDRESS, HMC5883L_REG_CONFB, 0x00);
    myiic_writereg(HMC5883L_ADDRESS, HMC5883L_REG_MODE, HMC5883L_MODE_CONTINOUS);
}

void HMC5883L_ReadData(uint16_t *x, uint16_t *y, uint16_t *z)
{
    uint8_t data[6];
    // 读取X、Y、Z轴的数据
    myiic_readreg(HMC5883L_ADDRESS, HMC5883L_REG_X_OUT_L, data, 6);
    // 将读取到的高低字节组合成16位整数
    *x = (data[1] << 8) | data[0];
    *y = (data[3] << 8) | data[2];
    *z = (data[5] << 8) | data[4];
}


void LSM303D_Accel_Init()
{
    // 初始化加速度计
    myiic_writereg(LSM303D_ACCEL_ADDR, LSM303D_REG_ACCEL_CTRL_REG1_A, 0x78); // 100Hz采样率
}

void LSM303D_Gyro_Init()
{
    // 初始化陀螺仪
    myiic_writereg(LSM303D_GYRO_ADDR, LSM303D_REG_GYRO_CTRL_REG1_G, 0x60); // 100Hz采样率
}
void LSM303D_ReadAccel(uint16_t *x, uint16_t *y, uint16_t *z)
{
    uint8_t data[6];
    // 读取加速度数据
    myiic_readreg(LSM303D_ACCEL_ADDR, LSM303D_REG_ACCEL_OUT_X_L_A, data, 6);
    // 组合数据并转换为16位整数
    *x = (data[1] << 8) | data[0];
    *y = (data[3] << 8) | data[2];
    *z = (data[5] << 8) | data[4];
}

void LSM303D_ReadGyro(uint16_t *x, uint16_t *y, uint16_t *z)
{
    uint8_t data[6];
    // 读取陀螺仪数据
    myiic_readreg(LSM303D_GYRO_ADDR, LSM303D_REG_GYRO_OUT_X_L_G, data, 6);
    // 组合数据并转换为16位整数
    *x = (data[1] << 8) | data[0];
    *y = (data[3] << 8) | data[2];
    *z = (data[5] << 8) | data[4];
}


void BMP180_ReadCalibration()
{
    uint8_t data[24];
    myiic_readreg(BMP180_ADDRESS, BMP180_REG_CALIBRATION, data, 24);
    // 将读取到的16位数据转换为整数
    ac1 = ((int16_t)data[0] << 8) | data[1];
    ac2 = ((int16_t)data[2] << 8) | data[3];
    ac3 = ((int16_t)data[4] << 8) | data[5];
    b1 = ((int16_t)data[6] << 8) | data[7];
    b2 = ((int16_t)data[8] << 8) | data[9];
    mb = ((int16_t)data[10] << 8) | data[11];
    mc = ((int16_t)data[12] << 8) | data[13];
    md = ((int16_t)data[14] << 8) | data[15];
    // ...其他校准参数的读取
}
 
void BMP180_Init(uint8_t mode)
{
    BMP180_ReadCalibration(); // 首先读取校准参数
    // 设置BMP180的工作模式
    uint8_t cmd = BMP180_CONTROL_MODE_ULTRALOWPOWER + mode;
    myiic_writereg(BMP180_ADDRESS, BMP180_REG_CONTROL, cmd);
}
uint32_t BMP180_ReadUncompensatedTemperature()
{
    uint8_t data[2];
    // 读取温度测量值
    myiic_readreg(BMP180_ADDRESS, 0xF6, data, 2);
    int32_t ut = ((int32_t)data[0] << 8) | data[1];
    return ut;
}
uint32_t BMP180_ReadUncompensatedPressure(uint8_t mode)
{
    uint8_t data[3];
    int32_t up;
    // 开始压力测量
    myiic_writereg(BMP180_ADDRESS, BMP180_REG_CONTROL, BMP180_CONTROL_MODE_ULTRALOWPOWER);
    // 根据所选模式等待相应时间
    switch(BMP180_CONTROL_MODE_ULTRALOWPOWER + mode)
    {
        case BMP180_CONTROL_MODE_ULTRALOWPOWER:
            Delay(5);
            break;
        // ...其他模式的延时处理
    }
    // 读取压力测量值
    myiic_readreg(BMP180_ADDRESS, 0xF6, data, 3);
    up = ((uint32_t)data[0] << 12) | ((uint32_t)data[1] << 4) | (data[2] >> 4);
    return up;
}
