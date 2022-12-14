// Console IO is a wrapper between the actual in and output and the console code
// In an embedded system, this might interface to a UART driver.

#include "consoleIo.h"
#include <stdio.h>
#include "usart.h"
#include <stdint.h>
#include <string.h>
#include <stdlib.h>

// Retargetting write and read:

uint32_t elapsed_time = 0;
int flag_training = 0;

int _read(int file, char *result, size_t len){
	HAL_StatusTypeDef status;

	int retcode = 0;

	if (len != 0){
		status = HAL_UART_Receive(&huart2, (uint8_t *) result, len, HAL_MAX_DELAY);

		if (status == HAL_OK){

			retcode = len;
		} else {
			retcode = -1;
		}
	}

	return retcode;
}

int _write(int file, char *outgoing, int len){
	HAL_UART_Transmit(&huart2, (uint8_t*) outgoing, len, 100);
	return len;
}

eConsoleError ConsoleIoInit(void)
{
	MX_USART2_UART_Init();
	return CONSOLE_SUCCESS;
}
eConsoleError ConsoleIoReceive(uint8_t *buffer, const uint32_t bufferLength, uint32_t *readLength)
{
    uint32_t i = 0;
    char ch;

    ch = getchar();
    while ( ( '\n' != ch ) && ( i < bufferLength ) )
    {
        buffer[i] = (uint8_t) ch;
        i++;
        printf("%c", ch);
        ch = getchar();
    }
    *readLength = i;
    return CONSOLE_SUCCESS;
}

eConsoleError ConsoleIoSendString(const char *buffer)
{
	printf("%s", buffer);
	return CONSOLE_SUCCESS;
}

eConsoleError print_counter(){
	if (flag_training == 0){
		printf("Starting training\n");
		flag_training = 1;

	}else{
		printf("Stop training\n");
		uint32_t current_ticks = HAL_GetTick();
		elapsed_time = (current_ticks - elapsed_time)/1000;
		printf("Training time: %ld\n", elapsed_time);
		elapsed_time = current_ticks;
		flag_training = 0;
	}

	return CONSOLE_SUCCESS;

}
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_pin){
	if(GPIO_pin == BUTTON_Pin){
		//int val = (int) HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin);
		//printf("Val %d\n", val);
		if(HAL_GPIO_ReadPin(BUTTON_GPIO_Port, BUTTON_Pin) == 1){
			HAL_GPIO_TogglePin(GREEN_GPIO_Port, GREEN_Pin);
			print_counter();
		}
	}
}


