/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    stm32f4xx_it.c
  * @brief   Interrupt Service Routines.
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "stm32f4xx_it.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN TD */

/* USER CODE END TD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

static uint16_t takty_melodii[] = {
	330,
	330,
	330,
	330,
	330,
	330,
	1,
	1,
	494,
	494,
	494,
	494,
	494,
	494,
	1,
	1,
	660,
	660,
	660,
	660,
	660,
	660,
	1,
	1,
	590,
	590,
	590,
	590,
	590,
	590,
	660,
	660,
	660,
	660,
	660,
	660,
	494,
	494,
	494,
	494,
	494,
	494,
	494,
	494,
	523,
	523,
	523,
	523,
	523,
	523,
	1,
	1,
	450,
	450,
	450,
	450,
	450,
	450,
	1,
	1,
	494,
	494,
	494,
	494,
	494,
	494,
	1,
	1,
	392,
	392,
	392,
	392,
	392,
	392,
	392,
	392,
	450,
	450,
	450,
	450,
	450,
	450,
	370,
	370,
	370,
	370,
	370,
	370,
	1,
	1,
	392,
	392,
	392,
	392,
	392,
	392,
	1,
	1,
	330,
	330,
	330,
	330,
	330,
	330,
	330,
	330,
	370,
	370,
	370,
	370,
	370,
	370,
	1,
	1,
	294,
	294,
	294,
	294,
	294,
	294,
	1,
	1,
	330,
	330,
	330,
	330,
	330,
	330,
	247,
	247,
	247,
	247,
	247,
	247,
	247,
	247,
	261,
	261,
	261,
	261,
	261,
	261,
	1,
	1,
	311,
	311,
	311,
	311,
	311,
	311,
	1,
	1,
	330,
	330,
	330,
	330,
	330,
	330,
	1,
	1,
	247,
	247,
	247,
	247,
	247,
	247,
	247,
	247,
	262,
	262,
	262,
	262,
	262,
	262,
	1,
	1,
	370,
	370,
	370,
	370,
	370,
	370,
	1,
	1,
	330,
	330,
	330,
	330,
	330,
	330,
	1,
	1,
	494,
	494,
	494,
	494,
	494,
	494,
	1,
	1,
	660,
	660,
	660,
	660,
	660,
	660,
	1,
	1,
	590,
	590,
	590,
	590,
	590,
	590,
	660,
	660,
	660,
	660,
	660,
	660,
	494,
	494,
	494,
	494,
	494,
	494,
	494,
	494,
	523,
	523,
	523,
	523,
	523,
	523,
	1,
	1,
	450,
	450,
	450,
	450,
	450,
	450,
	1,
	1,
	494,
	494,
	494,
	494,
	494,
	494,
	1,
	1,
	392,
	392,
	392,
	392,
	392,
	392,
	392,
	392,
	450,
	450,
	450,
	450,
	450,
	450,
	370,
	370,
	370,
	370,
	370,
	370,
	1,
	1,
	392,
	392,
	392,
	392,
	392,
	392,
	1,
	1,
	330,
	330,
	330,
	330,
	330,
	330,
	330,
	330,
	370,
	370,
	370,
	370,
	370,
	370,
	1,
	1,
	294,
	294,
	294,
	294,
	294,
	294,
	1,
	1,
	330,
	330,
	330,
	330,
	330,
	330,
	247,
	247,
	247,
	247,
	247,
	247,
	247,
	247,
	261,
	261,
	261,
	261,
	261,
	261,
	1,
	1,
	311,
	311,
	311,
	311,
	311,
	311,
	1,
	1,
	330,
	330,
	330,
	330,
	330,
	330,
	1,
	1,
	247,
	247,
	247,
	247,
	247,
	247,
	247,
	247,
	262,
	262,
	262,
	262,
	262,
	262,
	1,
	1,
	370,
	370,
	370,
	370,
	370,
	370,
	1,
	1,
	330,
	330,
	330,
	330,
	330,
	330,
	1,
	1
};

static unsigned pozostale_klatki = 1;
static unsigned biezacy_takt = 0;

extern uint8_t muzyka_wlaczona;

/* USER CODE END 0 */

/* External variables --------------------------------------------------------*/
extern TIM_HandleTypeDef htim3;
/* USER CODE BEGIN EV */

/* USER CODE END EV */

/******************************************************************************/
/*           Cortex-M4 Processor Interruption and Exception Handlers          */
/******************************************************************************/
/**
  * @brief This function handles Non maskable interrupt.
  */
void NMI_Handler(void)
{
  /* USER CODE BEGIN NonMaskableInt_IRQn 0 */

  /* USER CODE END NonMaskableInt_IRQn 0 */
  /* USER CODE BEGIN NonMaskableInt_IRQn 1 */
  while (1)
  {
  }
  /* USER CODE END NonMaskableInt_IRQn 1 */
}

/**
  * @brief This function handles Hard fault interrupt.
  */
void HardFault_Handler(void)
{
  /* USER CODE BEGIN HardFault_IRQn 0 */

  /* USER CODE END HardFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_HardFault_IRQn 0 */
    /* USER CODE END W1_HardFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Memory management fault.
  */
void MemManage_Handler(void)
{
  /* USER CODE BEGIN MemoryManagement_IRQn 0 */

  /* USER CODE END MemoryManagement_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_MemoryManagement_IRQn 0 */
    /* USER CODE END W1_MemoryManagement_IRQn 0 */
  }
}

/**
  * @brief This function handles Pre-fetch fault, memory access fault.
  */
void BusFault_Handler(void)
{
  /* USER CODE BEGIN BusFault_IRQn 0 */

  /* USER CODE END BusFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_BusFault_IRQn 0 */
    /* USER CODE END W1_BusFault_IRQn 0 */
  }
}

/**
  * @brief This function handles Undefined instruction or illegal state.
  */
void UsageFault_Handler(void)
{
  /* USER CODE BEGIN UsageFault_IRQn 0 */

  /* USER CODE END UsageFault_IRQn 0 */
  while (1)
  {
    /* USER CODE BEGIN W1_UsageFault_IRQn 0 */
    /* USER CODE END W1_UsageFault_IRQn 0 */
  }
}

/**
  * @brief This function handles System service call via SWI instruction.
  */
void SVC_Handler(void)
{
  /* USER CODE BEGIN SVCall_IRQn 0 */

  /* USER CODE END SVCall_IRQn 0 */
  /* USER CODE BEGIN SVCall_IRQn 1 */

  /* USER CODE END SVCall_IRQn 1 */
}

/**
  * @brief This function handles Debug monitor.
  */
void DebugMon_Handler(void)
{
  /* USER CODE BEGIN DebugMonitor_IRQn 0 */

  /* USER CODE END DebugMonitor_IRQn 0 */
  /* USER CODE BEGIN DebugMonitor_IRQn 1 */

  /* USER CODE END DebugMonitor_IRQn 1 */
}

/**
  * @brief This function handles Pendable request for system service.
  */
void PendSV_Handler(void)
{
  /* USER CODE BEGIN PendSV_IRQn 0 */

  /* USER CODE END PendSV_IRQn 0 */
  /* USER CODE BEGIN PendSV_IRQn 1 */

  /* USER CODE END PendSV_IRQn 1 */
}

/**
  * @brief This function handles System tick timer.
  */
void SysTick_Handler(void)
{
  /* USER CODE BEGIN SysTick_IRQn 0 */

  /* USER CODE END SysTick_IRQn 0 */
  HAL_IncTick();
  /* USER CODE BEGIN SysTick_IRQn 1 */

  /* USER CODE END SysTick_IRQn 1 */
}

/******************************************************************************/
/* STM32F4xx Peripheral Interrupt Handlers                                    */
/* Add here the Interrupt Handlers for the used peripherals.                  */
/* For the available peripheral interrupt handler names,                      */
/* please refer to the startup file (startup_stm32f4xx.s).                    */
/******************************************************************************/

/**
  * @brief This function handles TIM3 global interrupt.
  */
void TIM3_IRQHandler(void)
{
  /* USER CODE BEGIN TIM3_IRQn 0 */
	#define DZIELNIK_1 13
	#define DZIELNIK_2 2
	#define OKRES_BAZOWY 999
	#define CZESTOTLIWOSC_ZASTEPCZA 20

	pozostale_klatki--;
	if (pozostale_klatki == 0) {
		uint16_t okres;
		uint16_t wypelnienie;
		if (takty_melodii[biezacy_takt] < DZIELNIK_1) {
			pozostale_klatki = CZESTOTLIWOSC_ZASTEPCZA / DZIELNIK_2;
			okres = OKRES_BAZOWY / CZESTOTLIWOSC_ZASTEPCZA;
		} else {
			pozostale_klatki = takty_melodii[biezacy_takt] / (DZIELNIK_1*DZIELNIK_2);
			okres = OKRES_BAZOWY * DZIELNIK_1 / takty_melodii[biezacy_takt];
		}
		if (muzyka_wlaczona) {
			wypelnienie = okres / 2;
		} else {
			wypelnienie = 0;
		}
		biezacy_takt++;
		biezacy_takt %= (sizeof(takty_melodii) / sizeof(*takty_melodii));
		TIM3->ARR = okres;
		TIM3->CCR1 = wypelnienie;
	}
  /* USER CODE END TIM3_IRQn 0 */
  HAL_TIM_IRQHandler(&htim3);
  /* USER CODE BEGIN TIM3_IRQn 1 */

  /* USER CODE END TIM3_IRQn 1 */
}

/* USER CODE BEGIN 1 */

/* USER CODE END 1 */
