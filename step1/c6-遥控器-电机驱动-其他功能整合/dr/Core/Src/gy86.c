#include "gy86.h"
// HMC5883L的配置命令
#define HMC5883L_ADDRESS 0x1E // HMC5883L的I2C地址（7位地址）
#define HMC5883L_REG_CONFA 0x00 // 配置寄存器A
#define HMC5883L_REG_CONFB 0x01 // 配置寄存器B
#define HMC5883L_REG_MODE 0x02 // 模式寄存器
#define HMC5883L_REG_X_OUT_L 0x03 // X轴输出低字节
#define HMC5883L_MODE_CONTINUOUS 0x00 // 连续模式
#define HMC5883L_AVERAGING_8 0x40 // 8次平均值
#define HMC5883L_RATE_15 0x03 // 15Hz的数据输出率

// MPU6050的配置命令
#define MPU6050_ADDRESS 0xD0
#define MPU6050_PWR_MGMT_1 0x6B //电源管理寄存器1
#define MPU6050_PWR_MGMT_2 0x6C //电源管理寄存器2
#define MPU6050_SMPRT_DIV 0x19 //采样频率分频器
#define MPU6050_CONFIG 0x1A //配置寄存器
#define MPU6050_GYRO_CONFIG 0x1B //陀螺仪配置寄存器
#define MPU6050_ACCEL_CONFIG 0x1C //加速度传感器配置寄存器
#define MPU6050_USER_CTRL 0x6A //用户控制寄存器
#define MPU6050_INT_PIN_CFG 0x37 //旁路and引脚启用配置
#define MPU6050_ACCEL_XOUT_H 0x3B
#define MPU6050_ACCEL_XOUT_L 0x3C
#define MPU6050_ACCEL_YOUT_H 0x3D
#define MPU6050_ACCEL_YOUT_L 0x3E
#define MPU6050_ACCEL_ZOUT_H 0x3F
#define MPU6050_ACCEL_ZOUT_L 0x40
#define MPU6050_GYRO_XOUT_H 0x43
#define MPU6050_GYRO_XOUT_L 0x44
#define MPU6050_GYRO_YOUT_H 0x45
#define MPU6050_GYRO_YOUT_L 0x46
#define MPU6050_GYRO_ZOUT_H 0x47
#define MPU6050_GYRO_ZOUT_L 0x48

// MS5611的配置命令


void HMC5883L_Init(){
	HAL_I2C_Mem_Write(&hi2c1, HMC5883L_ADDRESS, HMC5883L_REG_CONFA, I2C_MEMADD_SIZE_8BIT, HMC5883L_AVERAGING_8 | HMC5883L_RATE_15, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, HMC5883L_ADDRESS, HMC5883L_REG_CONFB, I2C_MEMADD_SIZE_8BIT, 0x00, 1, HAL_MAX_DELAY);
	HAL_I2C_Mem_Write(&hi2c1, HMC5883L_ADDRESS, HMC5883L_REG_MODE, I2C_MEMADD_SIZE_8BIT, HMC5883L_MODE_CONTINUOUS, 1, HAL_MAX_DELAY);
}
void HMC5883L_ReadData(int16_t *x, int16_t *y, int16_t *z)
{
    uint8_t data[6];
    // 读取X、Y、Z轴的数据
    HAL_I2C_Mem_Read(&hi2c1, HMC5883L_ADDRESS, HMC5883L_REG_X_OUT_L, I2C_MEMADD_SIZE_8BIT, data, 6, HAL_MAX_DELAY);
    // 将读取到的高低字节组合成16位整数
    *x = (data[1] << 8) | data[0];
    *y = (data[3] << 8) | data[2];
    *z = (data[5] << 8) | data[4];
    // 应用校准和滤波处理（省略具体实现）
    // ...
}



void MPU6050_Init(){
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, MPU6050_PWR_MGMT_1, I2C_MEMADD_SIZE_8BIT, 0x01, 1, HAL_MAX_DELAY); //解除睡眠, 选择陀螺仪时钟
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, MPU6050_PWR_MGMT_2, I2C_MEMADD_SIZE_8BIT, 0x00, 1, HAL_MAX_DELAY); //六个轴都不待机
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, MPU6050_SMPRT_DIV, I2C_MEMADD_SIZE_8BIT, 0x09, 1, HAL_MAX_DELAY); //10分频
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, MPU6050_CONFIG, I2C_MEMADD_SIZE_8BIT, 0x06, 1, HAL_MAX_DELAY); //滤波参数最大
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, MPU6050_GYRO_CONFIG, I2C_MEMADD_SIZE_8BIT, 0x18, 1, HAL_MAX_DELAY); //陀螺仪最大量程
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, MPU6050_ACCEL_CONFIG, I2C_MEMADD_SIZE_8BIT, 0x18, 1, HAL_MAX_DELAY); //加速度计最大量程
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, MPU6050_USER_CTRL, I2C_MEMADD_SIZE_8BIT, 0x80, 1, HAL_MAX_DELAY); //控制I2C主模式使能?
	HAL_I2C_Mem_Write(&hi2c1, MPU6050_ADDRESS, MPU6050_INT_PIN_CFG, I2C_MEMADD_SIZE_8BIT, 0x02, 1, HAL_MAX_DELAY); //旁路模式?

}

void MPU6050_GetData_old( int16_t* AccX, int16_t* AccY, int16_t* AccZ, int16_t* GyroX, int16_t* GyroY, int16_t* GyroZ){
	uint8_t DataH, DataL;
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, DataH, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_L, I2C_MEMADD_SIZE_8BIT, DataL, 2, HAL_MAX_DELAY);
	*AccX = (DataH << 8)|DataL;

	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_ACCEL_YOUT_H, I2C_MEMADD_SIZE_8BIT, DataH, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_ACCEL_YOUT_L, I2C_MEMADD_SIZE_8BIT, DataL, 2, HAL_MAX_DELAY);
	*AccY = (DataH << 8)|DataL;

	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_ACCEL_ZOUT_H, I2C_MEMADD_SIZE_8BIT, DataH, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_ACCEL_ZOUT_L, I2C_MEMADD_SIZE_8BIT, DataL, 2, HAL_MAX_DELAY);
	*AccZ = (DataH << 8)|DataL;

	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_GYRO_XOUT_H, I2C_MEMADD_SIZE_8BIT, DataH, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_GYRO_XOUT_L, I2C_MEMADD_SIZE_8BIT, DataL, 2, HAL_MAX_DELAY);
	*GyroX = (DataH << 8)|DataL;

	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_GYRO_YOUT_H, I2C_MEMADD_SIZE_8BIT, DataH, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_GYRO_YOUT_L, I2C_MEMADD_SIZE_8BIT, DataL, 2, HAL_MAX_DELAY);
	*GyroY = (DataH << 8)|DataL;

	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_GYRO_ZOUT_H, I2C_MEMADD_SIZE_8BIT, DataH, 2, HAL_MAX_DELAY);
	HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_GYRO_ZOUT_L, I2C_MEMADD_SIZE_8BIT, DataL, 2, HAL_MAX_DELAY);
	*GyroZ = (DataH << 8)|DataL;

}


void MPU6050_GetData(int16_t* AccX, int16_t* AccY, int16_t* AccZ, int16_t* GyroX, int16_t* GyroY, int16_t* GyroZ) {
    uint8_t buffer[14]; // 加速度计+温度+陀螺仪共14字节

    // 一次性读取所有传感器数据（0x3B开始，连续14字节）
    HAL_I2C_Mem_Read(&hi2c1, MPU6050_ADDRESS, MPU6050_ACCEL_XOUT_H, I2C_MEMADD_SIZE_8BIT, buffer, 14, HAL_MAX_DELAY);

    // 解析加速度计数据（X/Y/Z各占2字节）
    *AccX  = (buffer[0]  << 8) | buffer[1];
    *AccY  = (buffer[2]  << 8) | buffer[3];
    *AccZ  = (buffer[4]  << 8) | buffer[5];

    // 跳过温度数据（buffer[6]和buffer[7]）

    // 解析陀螺仪数据（X/Y/Z各占2字节）
    *GyroX = (buffer[8]  << 8) | buffer[9];
    *GyroY = (buffer[10] << 8) | buffer[11];
    *GyroZ = (buffer[12] << 8) | buffer[13];
}

