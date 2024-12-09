/*
 * UART.h
 *
 *  Created on: Dec 3, 2024
 *      Author: samsolano
 */

#ifndef SRC_UART_H_
#define SRC_UART_H_

#include "stdio.h"
#include "string.h"

void UART_init(void);
void USART2_IRQHandler(void);
void UART_print(char *str);
void UART_print_int(char *string, int32_t value);
void UART_print_float(char *string, float value);

//initial position of paddle from left most character
uint8_t paddle_position = 37;

void UART_print_float(char *string, float value) {
    char buffer[100];
    int integer_part = (int)value; //separate integer from decimal for easy printing
    int decimal_part = (int)((value - integer_part) * 1000); // Keep 3 decimal places
    sprintf(buffer, "%s%d.%03d V\r\n\r\n", string, integer_part, decimal_part);
    UART_print(buffer);
}

//function to print a formatted string with an int to uart
void UART_print_int(char *string, int32_t value)
{
	char stringify[100] = {0};
	sprintf(stringify,string ,value); //take value and format it into char array
	UART_print(stringify);
}

void UART_init(void)
{
	 // Enable clocks for GPIOA and USART2
	RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;   // Enable GPIOA clock
	RCC->APB1ENR1 |= RCC_APB1ENR1_USART2EN; // Enable USART2 clock

	// Configure PA2 as USART2_TX and PA3 as USART2_RX
	GPIOA->MODER &= ~(GPIO_MODER_MODE2 | GPIO_MODER_MODE3); // Clear mode bits
	GPIOA->MODER |= (GPIO_MODER_MODE2_1 | GPIO_MODER_MODE3_1); // Set alternate function mode

	GPIOA->AFR[0] |= (0x7 << GPIO_AFRL_AFSEL2_Pos); // Set AF7 (USART2) for PA2
	GPIOA->AFR[0] |= (0x7 << GPIO_AFRL_AFSEL3_Pos); // Set AF7 (USART2) for PA3

	// Configure USART2 for 115200 baud rate (assuming 4 MHz clock)
	USART2->BRR = 208; // Set baud rate divisor for 115200 baud (4 MHz / (16 * 115200) ≈ 35)

	// Enable USART2, transmitter, and receiver and receive data register
	USART2->CR1 = (USART_CR1_TE | USART_CR1_RE | USART_CR1_UE | USART_CR1_RXNEIE);
	//enable interrupts
	NVIC_EnableIRQ(USART2_IRQn);
}

void UART_print(char *str)
{
	while (*str != '\0') 		// Loop until the end of the string
	{
		while (!(USART2->ISR & USART_ISR_TXE)) {} //finish receiving
		USART2->TDR = *str++; 				//write it out
	}
}

void USART2_IRQHandler(void)
{

	//if receiving data interrupt
	if(USART2->ISR & USART_ISR_RXNE)
	{
		char single[2];
		//put input anded with the read register into a variable
		sprintf(single,"%c", (char)(USART2->RDR & USART_RDR_RDR));

		if(single[0] == 'a') //when a is pressed move paddle left by three
		{
			if(paddle_position >= 3)
			{
				paddle_position -= 3;
			}
		}
		if(single[0] == 'd') //when d is pressed move paddle right by three
		{
			if(paddle_position <= 76)
			{
				paddle_position += 3;
			}
		}
		USART2->ISR &= ~(USART_ISR_RXNE);
	}
}

#endif /* SRC_UART_H_ */
