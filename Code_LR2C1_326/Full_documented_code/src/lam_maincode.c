/* WRITTEN BY TEAM LOGIC_LLAMS */
 // model STM32 Nucleo F4_446RE

#include "main.h"// main.h for including stm hal header file and defining
 // HEADER file for display fonts and pixel, as we can use oled display library said in webinar 
#include "ssd1306.h"
 #include "fonts.h"
 
I2C_HandleTypeDef hi2c1; // defined in  header file of i2c communication
TIM_HandleTypeDef htim1;// function in header file of timer for sensor communication
TIM_HandleTypeDef htim3;// function in header file of timer for pwm for servo

// Configuration functions for internal clk of STM, GPIO pins and for connectivity timers and i2C (oled display)
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C1_Init(void);
static void MX_TIM1_Init(void);
static void MX_TIM3_Init(void);


// pins for defined for hx711 and various variables for weight measurement function
#define SCK_PIN GPIO_PIN_7
#define SCK_PORT GPIOC
#define DATA_PIN GPIO_PIN_10
#define DATA_PORT GPIOB
int weight_avg =0;// it will be shown in display directly weight
int  weightA = 0; // for doing average the value 
uint32_t tare_weight = 888025;
float Original = 15000;  // A known callibration values in milli gram !!
float sensor_value = 32405; // some random value for calibiration comparision with known 

void microDelay(uint16_t delay)
{
  __HAL_TIM_SET_COUNTER(&htim1, 0);
  while (__HAL_TIM_GET_COUNTER(&htim1) < delay);
}
 
 // sensor read values function for load cell 
int32_t SENSOR(void)
{
  uint32_t data = 0;
  uint32_t startTime = HAL_GetTick();
  while(HAL_GPIO_ReadPin(DATA_PORT, DATA_PIN) == GPIO_PIN_SET)
  {
    if(HAL_GetTick() - startTime > 200)
      return 0;
  }
  for(int8_t len=0; len<24 ; len++)
  {
  // pin PC7 turning on and off so that we can read the value of sensor
    HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);
    microDelay(1);
    data = data << 1; 
    HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
    microDelay(1);
    if(HAL_GPIO_ReadPin(DATA_PORT, DATA_PIN) == GPIO_PIN_SET)
      data ++;
  }
  data = data ^ 0x800000; // xoring the values according to data sheet 
  HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);
  microDelay(1);
  HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
  microDelay(1);
  return data;
}
 // defined a functions for returning weight milligrams 
int weight_in_grams()
{
  int32_t  total = 0;
  int32_t  samples = 25;
  int milligram;
  float coefficient;
  for(uint16_t i=0 ; i<samples ; i++)
  {
      total += SENSOR();  // taking sensor values about 25 times for averaging the value
  }
  int32_t average = (int32_t)(total / samples);
  coefficient = Original / sensor_value; // assuming the bend curve for weight is approx linear for load cell precision is in 0 to 1 KG
  milligram = (int)(average-tare_weight)*coefficient;
  return milligram;
}

//main part of code inside int main

int main(void)
{
int count = 0;
int num = 2;//  num will select what will be threshold value of weight for each cycle... 
// what num  does is as we are measuring it continuously the total weight thresholds  measured by load cell should be 10gm, 15gm , 25gm .. num makes it factor of 5 
//HAL INITIALISATION FOR STM
// INTIALISING CONNECTIVITY, TIMERS , GPIO PINS AND OLED DISPLAY

  HAL_Init();
  SystemClock_Config();
  MX_GPIO_Init();
  MX_I2C1_Init();
  MX_TIM3_Init();
  MX_TIM1_Init();
  SSD1306_Init();
 
 // WE NEED TO START TIMERS CHANNEL AND &htim3 &htim2 refrencing  
 HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_1);
 HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_2);
 HAL_TIM_PWM_Start(&htim3,TIM_CHANNEL_3);
 HAL_TIM_Base_Start(&htim1);
 HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_SET);
 HAL_Delay(10);
 HAL_GPIO_WritePin(SCK_PORT, SCK_PIN, GPIO_PIN_RESET);
 HAL_Delay(10);
 
 
// this function is created for diplaying values of weight and measuring load as soon as the dispensing cycle starts , it take arguments as threshold weight after which dispensing should stop
int process_start1(int factor){

while(1){
// b
	 SSD1306_Clear();
 	 SSD1306_GotoXY (0,0);
 	 SSD1306_Puts (" WEIGHT in", &Font_11x18, 1);
 	 SSD1306_GotoXY (0,15);
 	 SSD1306_Puts (" milli grams ", &Font_11x18, 1);
     SSD1306_UpdateScreen();
  	 HAL_Delay(1000);
      // measuring weight again for average 
     for(int n=0; n<5; n++){
    	  weightA = weight_in_grams();
    	  weight_avg = weight_avg + weightA;
            }
     weight_avg =  weight_avg/5;
   //  printing the values 
     char snum[7];
 	 SSD1306_GotoXY (10,35);
 	 itoa(weight_avg, snum, 10);
 	 SSD1306_Puts (snum, &Font_11x18, 1);
 	 SSD1306_UpdateScreen();
	 HAL_Delay(4000);
     if(weight_avg > 4975*factor) { // the factor is chosen to be such that we can get approximate weight of each dispensed solution around 10 and 5 gm respectively
     	break;
     	} 
    }
       return 0;
 }
//// turing low and low for so that according to circuit, motor can run in reverse direction , for pump to reverse liquid back to source
void process_rev1(){
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
	 HAL_Delay(3000);
   __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1, 500);
	 HAL_Delay(40000);
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
}
// the process two is for different dispenser to return to its storage 
void process_rev2(){
	 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);
		 HAL_Delay(3000);
	   __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2, 1000);
		 HAL_Delay(40000);
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_SET);
		 HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);
}

 while (1)
  {
//Setting up the values for servo so that it selects the outlet and pinch valve 
 __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1, 500);
 __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2, 500);
 __HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3, 500);
 HAL_Delay(2000);
SSD1306_Clear();
// clear and set display
// turing high and low for so that according to circuit, motor can run in forward direction..... :) used relay instead of motor driver for forward and backward dir of motor pump
// high low cause relay follow inverting logic 
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);//IN1
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);//IN2
// The Hal delay is given so that proces of reading weight can wait untill drop of liquid drops
HAL_Delay(30000);
//diplaying values of weight and measuring load 
process_start1(num);
__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1, 1000);// pinch valve off :( as threshold is reached
HAL_Delay(3000);
//now the liquid trapped in liquid  should be reversed

process_rev1();
HAL_Delay(1000);

//shifting to different dispenser changing servo angles for pinch and outlet 
__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_1, 1000);
__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_3, 1000);
__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2, 1000);

HAL_Delay(2000);
// turing high and low for so that according to circuit, motor can run in forward direction again 
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_4, GPIO_PIN_RESET);//IN4
HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);//IN2
HAL_Delay(30000);

num = num +1;// as there is new threshold value for next dispenser 

process_start1(num);
__HAL_TIM_SET_COMPARE(&htim3,TIM_CHANNEL_2, 500);// pinch valve off :( again
HAL_Delay(3000);
    //now the liquid trapped in water should be reversed
process_rev2();
HAL_Delay(3000);

count++;// counter for only five cycles 
num = num +2;
if(count>=5){
	break;
}
}

}

//// Initialize HSE oscillator and PLL settings for the main system clock, Advanced Peripheral Bus 1 and set to 90 MHZ 
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE3);
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 4;
  RCC_OscInitStruct.PLL.PLLN = 90;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 2;
  RCC_OscInitStruct.PLL.PLLR = 2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

//Initializing  I2C1 peripheral with settings for clock speed 400k 
// ALSO AS we don't need for disabling  specific features like general call mode and stretch mode.

static void MX_I2C1_Init(void)
{

  hi2c1.Instance = I2C1;
  hi2c1.Init.ClockSpeed = 400000;
  hi2c1.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c1.Init.OwnAddress1 = 0;
  hi2c1.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c1.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c1.Init.OwnAddress2 = 0;
  hi2c1.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c1.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c1) != HAL_OK)
  {
    Error_Handler();
  }

}



//TIMER 3 INTITIALISATION FOR 3 CHANNELS PWM FOR SERVO MOTORS WITH PRESCALER OF 180 
// AS INTERNAL CLOCK IS SET FOR 90 MHZ AND COUNTER PERIOD IS FOR 10K CAUSE OF 50HZ LIMIT OF PWM FOR SERVO
static void MX_TIM3_Init(void)
{

  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  TIM_OC_InitTypeDef sConfigOC = {0};
  htim3.Instance = TIM3;
  htim3.Init.Prescaler = 179;
  htim3.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim3.Init.Period = 10000-1;
  htim3.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim3.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim3, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_Init(&htim3) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim3, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sConfigOC.OCMode = TIM_OCMODE_PWM1;
  sConfigOC.Pulse = 0;
  sConfigOC.OCPolarity = TIM_OCPOLARITY_HIGH;
  sConfigOC.OCFastMode = TIM_OCFAST_DISABLE;
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_1) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_2) != HAL_OK)
  {
    Error_Handler();
  }
  if (HAL_TIM_PWM_ConfigChannel(&htim3, &sConfigOC, TIM_CHANNEL_3) != HAL_OK)
  {
    Error_Handler();
  }
  HAL_TIM_MspPostInit(&htim3);
}


  // configurations for TIMER FOR HX711 SENSOR BASICALLY WE WANT 90 PRESCALER FOR 90 MHZ CLOCK FREQUENCY counter mode is up and , Auto preload and master slave is disabled...
static void MX_TIM1_Init(void)
{



  TIM_ClockConfigTypeDef sClockSourceConfig = {0};
  TIM_MasterConfigTypeDef sMasterConfig = {0};
  htim1.Instance = TIM1;
  htim1.Init.Prescaler = 89;
  htim1.Init.CounterMode = TIM_COUNTERMODE_UP;
  htim1.Init.Period = 65535;
  htim1.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
  htim1.Init.RepetitionCounter = 0;
  htim1.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
  if (HAL_TIM_Base_Init(&htim1) != HAL_OK)
  {
    Error_Handler();
  }
  sClockSourceConfig.ClockSource = TIM_CLOCKSOURCE_INTERNAL;
  if (HAL_TIM_ConfigClockSource(&htim1, &sClockSourceConfig) != HAL_OK)
  {
    Error_Handler();
  }
  sMasterConfig.MasterOutputTrigger = TIM_TRGO_RESET;
  sMasterConfig.MasterSlaveMode = TIM_MASTERSLAVEMODE_DISABLE;
  if (HAL_TIMEx_MasterConfigSynchronization(&htim1, &sMasterConfig) != HAL_OK)
  {
    Error_Handler();
  }


}
  
  
  
  // KIND OF GENERATED GPIO CONFIGURATIONS , SETTING UP STMCUBE_IDE FOR GPIO PINS USED IN PROJECT AND ALSO CLOCK ENABBLE 
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};


  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3|GPIO_PIN_4, GPIO_PIN_RESET);

 
///////// FOR CONNECTION BETWEEN HX711 AND STM ////////////////
  /*Configure GPIO pin : PB10 */
  GPIO_InitStruct.Pin = GPIO_PIN_10;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
  
  
   /*Configure GPIO pin : PC7 */
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStruct);
////////////////////////////////////////////////

  /*Configure GPIO pins : PB3 PB4 for pins D3 AND D5 RELAY SWITCHING  */
  GPIO_InitStruct.Pin = GPIO_PIN_3|GPIO_PIN_4;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pin : PB6 */
  GPIO_InitStruct.Pin = GPIO_PIN_6;
  GPIO_InitStruct.Mode = GPIO_MODE_ANALOG;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);


}

////////BASICALLY BELOW LINES OF CODE IS FOR ERROR HANDLING  for HAL_ TIMERS AND i2c  IN STM32 CUBE IDE FOR INTERNAL CLOCK /////////
void Error_Handler(void)
{
  
  __disable_irq();
  while (1)
  {
  }
  
}

#ifdef  USE_FULL_ASSERT

void assert_failed(uint8_t *file, uint32_t line)
{
  
}
#endif 
