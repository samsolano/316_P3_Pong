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
void Print_Screen(int8_t *x_position, int8_t *y_position);
void GAME_init(Ball *ball);
void Check_Collisions(int8_t *dx, int8_t *dy, int8_t *x_position, int8_t *y_position);


int main(void)
{
	HAL_Init();
	SystemClock_Config();
	UART_init();

	Ball ball;
	GAME_init(&ball);

	while (1)
	{
		Print_Screen(&(ball.x), &(ball.y));
		Check_Collisions(&(ball.dx), &(ball.dy), &(ball.x), &(ball.y));
	}
}


void Check_Collisions(int8_t *dx, int8_t *dy, int8_t *x_position, int8_t *y_position)
{
	*x_position += *dx;
	*y_position += *dy;

	UART_print_int("\r\n x_position: %d", *x_position);
	UART_print_int("\r\n dx: %d", *dx);

	if((*x_position == 79) && (*dx > 0))
	{
		*dx = -1;
	}
	if((*x_position == 0) && (*dx < 0))
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
	for(uint32_t i = 0; i < 500000; i++) {}
	UART_print("\033[2J\033[H");

	for(uint8_t row = 0; row < VIEW_HEIGHT; row++)
	{

		for(uint8_t col = 0; col < VIEW_WIDTH; col++)
		{
			if((row == *y_position) && (col == *x_position))	// for printing ball
			{
				UART_print("o");
			}
			if((row == 15) && (col == paddle_position))		// for printing paddle
			{
				UART_print("_____");
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
	ball->x = 5;
	ball->y = BALL_Y_START;
	ball->dx = -1;
	ball->dy = -1;
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
