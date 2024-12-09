#include "main.h"
#include "UART.h"

#define VIEW_HEIGHT 24
#define VIEW_WIDTH  80
#define BALL_X_START 40
#define BALL_Y_START 10
#define ONE_BYTE 0xff
#define NUM_POSSIBLE_DIRECTIONS 4
#define PADDLE_LENGTH 6
#define PADDLE_ROW 21
#define NUM_OF_GAMEOVERS 900
#define RIGHT_BORDER 77
#define LEFT_BORDER 1
#define TOP_BORDER 1
#define SLOW_REFRESH_DELAY 100000
#define BALL_BORDER_OFFSET 2
#define NO_BALL_BORDER_OFFSET 1
#define PADDLE_BORDER_OFFSET 3
#define RIGHT_DIRECTION 1
#define LEFT_DIRECTION -1
#define UP_DIRECTION -1
#define DOWN_DIRECTION 1
#define MIDDLE_PADDLE_OFFSET 3
#define FAR_END_OF_PADDLE 7
#define BOTTOM_OF_SCREEN 24

typedef struct {
    int8_t x, y;        // Position
    int8_t dx, dy;      // Velocity
} Ball;

void SystemClock_Config(void);
void Print_Screen(int8_t *x_position, int8_t *y_position);
void GAME_init(Ball *ball);
void Check_Screen_Collisions(int8_t *dx, int8_t *dy, int8_t *x_position, int8_t *y_position);
void Check_Paddle_Collisions(int8_t *dx, int8_t *dy, int8_t *x_position, int8_t *y_position);
void RNG_init(void);
uint32_t Get_Random_Number(void);

int main(void)
{
	HAL_Init();
	SystemClock_Config();
	UART_init();
	RNG_init();

	Ball ball;
	GAME_init(&ball);
	UART_print("\033[2J\033[H"); //Clear and reset screen

	while (1)
	{
		//Print screen with current positions
		Print_Screen(&(ball.x), &(ball.y));
		//check for collisions and reassign values
		Check_Screen_Collisions(&(ball.dx), &(ball.dy), &(ball.x), &(ball.y));
		Check_Paddle_Collisions(&(ball.dx), &(ball.dy), &(ball.x), &(ball.y));

		if(ball.y >= BOTTOM_OF_SCREEN) //if ball hits bottom of screen print game over
		{
			UART_print("\033[2J\033[H");
			for(uint16_t i = 0; i < NUM_OF_GAMEOVERS; i++)
			{
				UART_print("GAME OVER ");
			}
			break;
		}
	}
}

uint32_t Get_Random_Number(void){
	while(!(RNG->SR & RNG_SR_DRDY)); //wait for number to be ready
	return RNG->DR;   //return random number
}

void Check_Paddle_Collisions(int8_t *dx, int8_t *dy, int8_t *x_position, int8_t *y_position)
{
	//if ball is hitting the left side of the paddle, then make the ball move to the left and up
	if((*x_position <= (paddle_position + MIDDLE_PADDLE_OFFSET)) && (*x_position >= paddle_position) && (*y_position == PADDLE_ROW - 1))
	{
		*dy = UP_DIRECTION;
		*dx = LEFT_DIRECTION;
	}
	//if ball is hitting the right side of the paddle, then make the ball move to the right and up
	if((*x_position > (paddle_position + MIDDLE_PADDLE_OFFSET)) && (*x_position <= paddle_position + FAR_END_OF_PADDLE) && (*y_position == PADDLE_ROW - 1))
	{
		*dy = UP_DIRECTION;
		*dx = RIGHT_DIRECTION;
	}
}

void Check_Screen_Collisions(int8_t *dx, int8_t *dy, int8_t *x_position, int8_t *y_position)
{
	//update ball position
	*x_position += *dx;
	*y_position += *dy;

	//if hitting right side of screen change x direction to left
	if((*x_position == RIGHT_BORDER) && (*dx > 0))
	{
		*dx = LEFT_DIRECTION;
	}
	//if hitting left side of screen change x direction to right
	if((*x_position == LEFT_BORDER) && (*dx < 0))
	{
		*dx = RIGHT_DIRECTION;
	}
	//i f hitting top of screen bounce down
	if(*y_position == TOP_BORDER)
	{
		*dy = DOWN_DIRECTION;
	}
}

void Print_Screen(int8_t *x_position, int8_t *y_position)
{
	//alter refresh rate to make game more stable
	for(uint32_t i = 0; i < SLOW_REFRESH_DELAY; i++) {}
	UART_print("\033[2J\033[H");

	for(uint8_t row = 0; row < VIEW_HEIGHT; row++)
	{
		for(uint8_t col = 0; col < VIEW_WIDTH; col++)
		{
			//print top border
			if(row == 0 && col == 0)
			{
				UART_print("|-------------------------------------------------------------------------------");
			}
			//print left border
			if(col == 0)
			{
				UART_print("|");
			}
			//logic to print right border of row where the ball currently is
			if((col == (VIEW_WIDTH - BALL_BORDER_OFFSET)) && (row == *y_position))
			{
				UART_print("|");
			}
			//logic to print right border of row where no ball or paddle is
			else if(col == (VIEW_WIDTH - NO_BALL_BORDER_OFFSET) && (row != *y_position) && (row != PADDLE_ROW))
			{
				UART_print("|");
			}
			//logic to print right border where paddle is
			else if((col == (VIEW_WIDTH - PADDLE_BORDER_OFFSET)) && (row == PADDLE_ROW))
			{
				UART_print("|");
			}
			if((row == *y_position) && (col == *x_position))   // for printing ball
			{
				UART_print("o");
			}
			if((row == PADDLE_ROW) && (col == paddle_position)) // for printing paddle
			{
				UART_print("_______");
				col += PADDLE_LENGTH;
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

	// grab a byte and mod it with 4 to get 4 possible values: 0,1,2,3
	uint8_t random = (Get_Random_Number() & ONE_BYTE) % NUM_POSSIBLE_DIRECTIONS;

	//depending on which value assign an initial ball direction
	switch(random)
	{
		case 0:
			ball->dx = RIGHT_DIRECTION;
			ball->dy = UP_DIRECTION;
			break;
		case 1:
			ball->dx = LEFT_DIRECTION;
			ball->dy = UP_DIRECTION;
			break;
		case 2:
			ball->dx = RIGHT_DIRECTION;
			ball->dy = DOWN_DIRECTION;
			break;
		case 3:
			ball->dx = LEFT_DIRECTION;
			ball->dy = DOWN_DIRECTION;
			break;
		default:
			ball->dx = RIGHT_DIRECTION;
			ball->dy = UP_DIRECTION;
	}
}

void RNG_init(void){
    RCC->PLLCFGR &= ~RCC_PLLCFGR_PLLSRC;
    //multi speed internal oscillator
    RCC->PLLCFGR |= RCC_PLLCFGR_PLLSRC_MSI;

    RCC->PLLCFGR &= ~(RCC_PLLCFGR_PLLM | RCC_PLLCFGR_PLLN | RCC_PLLCFGR_PLLQ);
    //set division factor
    RCC->PLLCFGR |= (RCC_PLLCFGR_PLLM_0);
    //set multiplication factor
    RCC->PLLCFGR |= (RCC_PLLCFGR_PLLN_2 | RCC_PLLCFGR_PLLN_3); //might have to reset these
    //set division factor
    RCC->PLLCFGR |= (RCC_PLLCFGR_PLLQ_1);

    RCC->PLLCFGR |= RCC_PLLCFGR_PLLQEN; // enable pll clock
	RCC->CR |= RCC_CR_PLLON;            // enable pll
	while(!(RCC->CR & RCC_CR_PLLRDY));  // wait for pll

	RCC->CCIPR &= ~RCC_CCIPR_CLK48SEL;
	RCC->CCIPR |= (2 << RCC_CCIPR_CLK48SEL_Pos); // set 48MHz pll clk

    RCC->AHB2ENR|= RCC_AHB2ENR_RNGEN; // enable RNG clk
    RNG->CR |= RNG_CR_RNGEN;		  // enable RNG

    RNG->SR &= ~(RNG_SR_CEIS | RNG_SR_SEIS); // clear flags
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
