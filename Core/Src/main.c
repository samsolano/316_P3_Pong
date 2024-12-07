#include "main.h"
#include "UART.h"

#define VIEW_HEIGHT 24
#define VIEW_WIDTH  80
#define BALL_X_START 40
#define BALL_Y_START 10

typedef struct {
    int8_t x, y;        // Position
    int8_t dx, dy;      // Velocity
} Ball;

void SystemClock_Config(void);
void RNG_init(void);
uint32_t Get_Random_Number(void);
void Print_Screen(int8_t *x_position, int8_t *y_position);
void GAME_init(Ball *ball);
void Check_Screen_Collisions(int8_t *dx, int8_t *dy, int8_t *x_position, int8_t *y_position);
void Check_Paddle_Collisions(int8_t *dx, int8_t *dy, int8_t *x_position, int8_t *y_position);


int main(void)
{
	HAL_Init();
	SystemClock_Config();
	UART_init();
	RNG_init();

	Ball ball;
	GAME_init(&ball);





	UART_print("\033[2J\033[H");

	while (1)
	{

		UART_print_int("random: %d \r\n", (Get_Random_Number() && 0xff));
		for(uint32_t i = 0; i < 100000; i++) {}



		if(0)
		{
		Print_Screen(&(ball.x), &(ball.y));
		Check_Screen_Collisions(&(ball.dx), &(ball.dy), &(ball.x), &(ball.y));
		Check_Paddle_Collisions(&(ball.dx), &(ball.dy), &(ball.x), &(ball.y));

		if(ball.y >= 24)
		{
			UART_print("\033[2J\033[H");
			for(uint16_t i = 0; i < 900; i++)
			{
				UART_print("GAME OVER ");
			}
			break;
		}
		}
	}
}

uint32_t Get_Random_Number(void)
{
	while(!(RNG->SR && RNG_SR_DRDY))
	if(RNG->DR != 0)
	{
		return RNG->DR;
	}

	return 69;
}

void RNG_init(void)
{
	RCC->AHB2ENR |= RCC_AHB2ENR_RNGEN;
	RNG->CR |= RNG_CR_RNGEN;
}


void Check_Paddle_Collisions(int8_t *dx, int8_t *dy, int8_t *x_position, int8_t *y_position)
{

	if((*x_position <= (paddle_position + 3)) && (*x_position >= paddle_position) && (*y_position == 20))
	{
		*dy = -1;
		*dx = -1;
	}
	if((*x_position > (paddle_position + 3)) && (*x_position <= paddle_position + 7) && (*y_position == 20))
	{
		*dy = -1;
		*dx = 1;
	}


}


void Check_Screen_Collisions(int8_t *dx, int8_t *dy, int8_t *x_position, int8_t *y_position)
{
	*x_position += *dx;
	*y_position += *dy;

	if((*x_position == 77) && (*dx > 0))
	{
		*dx = -1;
	}
	if((*x_position == 1) && (*dx < 0))
	{
		*dx = 1;
	}

	if(*y_position == 1)
	{
		*dy = 1;
	}
}


void Print_Screen(int8_t *x_position, int8_t *y_position)
{
	for(uint32_t i = 0; i < 100000; i++) {}
	UART_print("\033[2J\033[H");

	for(uint8_t row = 0; row < VIEW_HEIGHT; row++)
	{

		for(uint8_t col = 0; col < VIEW_WIDTH; col++)
		{
			if(row == 0 && col == 0)
			{
				UART_print("|-------------------------------------------------------------------------------");
			}
			if(col == 0)
			{
				UART_print("|");
			}
			if((col == (VIEW_WIDTH - 2)) && (row == *y_position)) //if row where ball is
			{
				UART_print("|");
			}
			else if(col == (VIEW_WIDTH - 1) && (row != *y_position) && (row != 21)) // if row where no ball is
			{
				UART_print("|");
			}
			else if((col == (VIEW_WIDTH - 3)) && (row == 21)) // if row where paddle is
			{
				UART_print("|");
			}
			if((row == *y_position) && (col == *x_position))	// for printing ball
			{
				UART_print("o");
			}
			if((row == 21) && (col == paddle_position))		// for printing paddle
			{
				UART_print("_______");
				col += 4;
			}
			else
			{
				UART_print(" ");
			}
		}
		UART_print("\r\n");
	}
}


void GAME_init(Ball *ball)
{
	ball->x = BALL_X_START;
	ball->y = BALL_Y_START;
	ball->dx = 1;
	ball->dy = 1;
}


































/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
  RCC_OscInitStruct.MSIState = RCC_MSI_ON;
  RCC_OscInitStruct.MSICalibrationValue = 0;
  RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_9;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
