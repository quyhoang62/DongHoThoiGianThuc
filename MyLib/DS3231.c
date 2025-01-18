#include "DS3231.h"

// dia chi i2c chi co 7 bit nen phai dich trai 1
#define RTC_ADDR (0x68<<1)
#define HOUR_REGISTER  0x02      // Thanh ghi giờ
extern I2C_HandleTypeDef hi2c1;

uint8_t Decimal2BCD(uint8_t num)
{
	return (num/10)<<4|(num%10);
}
uint8_t BCD2Decimal(uint8_t num)
{
	return (num >> 4)*10 + (num&0x0F);
}


// ghi thoi gian moi
// su dung con tro de giam kich thuoc bo nho
// vi con tro thuong chi chiem 4byte (phu thuoc vao kien truc vxl)
void rtc_write_time(DateTime *dt)
{
	uint8_t data[8];
	data[0] = 0x00; // dia chi thanh ghi dau tien
	data[1] = Decimal2BCD(dt->sec);
	data[2] = Decimal2BCD(dt->min);
	data[3] = Decimal2BCD(dt->hour);
	data[4] = Decimal2BCD(dt->day);
	data[5] = Decimal2BCD(dt->date);
	data[6] = Decimal2BCD(dt->mon);
	data[7] = Decimal2BCD(dt->year);
	HAL_I2C_Master_Transmit(&hi2c1, RTC_ADDR, data, 8, 100);
}

// doc thoi gian 
void rtc_read_time(DateTime *dt)
{
	uint8_t data[7];
	uint8_t add_reg = 0x00;
	HAL_I2C_Master_Transmit(&hi2c1, RTC_ADDR, &add_reg, 1, 100); // truyen du lieu dia chi muon doc
	HAL_I2C_Master_Receive(&hi2c1, RTC_ADDR, data, 7, 100);
	dt->sec = BCD2Decimal(data[0]);
	dt->min = BCD2Decimal(data[1]);
	dt->hour = BCD2Decimal(data[2]);
	dt->day = BCD2Decimal(data[3]);
	dt->date = BCD2Decimal(data[4]);
	dt->mon = BCD2Decimal(data[5]);
	dt->year = BCD2Decimal(data[6]);
}



float rtc_read_temp(DateTime *dt)
{
    uint8_t data_temp[2];
    uint8_t add_reg = 0x11;
    HAL_I2C_Master_Transmit(&hi2c1, RTC_ADDR, &add_reg, 1, 100);
    HAL_I2C_Master_Receive(&hi2c1, RTC_ADDR, data_temp, 2, 100);

    // Phần nguyên
    int8_t temp_integer = (int8_t)data_temp[0]; // Dữ liệu có dấu
    // Phần thập phân (2 bit cao của byte thứ hai)
    float temp_fraction = (data_temp[1] >> 6) * 0.25f;
    return temp_integer + temp_fraction;
}
uint8_t read_hour_register() {
    uint8_t hour;
		uint8_t add_hour = 0x02;
    HAL_I2C_Master_Transmit(&hi2c1, RTC_ADDR, &add_hour, 1, 100); // truyen du lieu dia chi muon doc
		HAL_I2C_Master_Receive(&hi2c1, RTC_ADDR, &hour, 1, 100);
    return hour;
}


