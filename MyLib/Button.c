#include "Button.h"

//  Từ khóa __weak được dùng để khai báo rằng hàm đó là một hàm yếu. 
//  một hàm yếu có thể được ghi đè bởi một hàm khác có cùng tên trong chương trình.


// extern: chỉ được dùng để khai báo rằng một hàm (hoặc biến) đã được định nghĩa ở nơi khác.

__weak void btn_pressing_callback(Button_Typedef * ButtonX)
{
	
}
	
__weak void btn_release_callback(Button_Typedef * ButtonX)
{

}
__weak void btn_press_short_callback(Button_Typedef * ButtonX)
{

}
__weak void btn_press_timeout_short_callback(Button_Typedef * ButtonX)
{

}
//extern void btn_presing_callback()
//{
//	
//}

void button_handle(Button_Typedef *ButtonX)
{
	// loc nhieu
	uint8_t sta = HAL_GPIO_ReadPin(ButtonX->GPIOx, ButtonX->GPIO_Pin);
	if(sta != ButtonX->btn_filter)
	{
		ButtonX->btn_filter = sta;
		ButtonX->is_debouncing = 1;
		ButtonX->time_decoune = HAL_GetTick();
	}
	// tin hieu da xac lap
	if(ButtonX->is_debouncing && (HAL_GetTick() - ButtonX->time_decoune >= 15))
	{
		ButtonX->btn_curent = ButtonX->btn_filter;
		ButtonX->is_debouncing = 0;
	}
	// xu li
	if(ButtonX->btn_curent != ButtonX->btn_last)
	{
		if(ButtonX->btn_curent == 0) //			nhan xuong
		{
			ButtonX->is_press_timeout = 1;
			btn_pressing_callback(ButtonX);
			ButtonX->time_start_press = HAL_GetTick();
		}
		// nha nut
		else
		{
			if(HAL_GetTick() - ButtonX->time_start_press <= 1000)
			{
				btn_press_short_callback(ButtonX);
			}
			btn_release_callback(ButtonX);
			ButtonX->is_press_timeout = 0;
		}
		ButtonX->btn_last = ButtonX->btn_curent;
	}
	if(ButtonX->is_press_timeout && ((HAL_GetTick() - ButtonX->time_start_press) >= 2000))
	{
		btn_press_timeout_short_callback(ButtonX);
		ButtonX->is_press_timeout = 0;
	}
}

void Button_Init(Button_Typedef *ButtonX, GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
		ButtonX->GPIOx = GPIOx;
		ButtonX->GPIO_Pin = GPIO_Pin;
}
