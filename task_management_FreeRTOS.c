/* FreeRTOS includes. */
#include "FreeRTOS.h"
#include "xgpio.h"
#include "task.h"
#include "timers.h"
/* Xilinx includes. */
#include "xil_printf.h"
#include "xparameters.h"

/* Definitions */
//#define GPIO_DEVICE_ID  XPAR_AXI_GPIO_0_DEVICE_ID	/* GPIO device that LEDs are connected to */
int switch_read;
#define SW 0x9
#define LED 0x9										/* Initial LED value - X00X */
#define LED_DELAY 10000000							/* Software delay length */
#define CHANNEL 1								/* GPIO port for LEDs */
#define SW_CHANNEL 2								/* GPIO port for LEDs */
#define printf xil_printf							/* smaller, optimised printf */

#define SW_DEVICE_ID  XPAR_AXI_GPIO_0_DEVICE_ID	 /* GPIO device that SWs are connected to */
#define BTN_DEVICE_ID  XPAR_AXI_GPIO_0_DEVICE_ID  /* GPIO device that BTNs are connected to */
#define LEDS_DEVICE_ID XPAR_AXI_GPIO_1_DEVICE_ID  /* GPIO device that LEDs are connected to */

int status;  // Initialize status variable
/* define delay count */
#define TIMER_ID	1
#define DELAY_10_SECONDS	10000UL
#define DELAY_1_SECOND		1000UL
#define TIMER_CHECK_THRESHOLD	9
/*-----------------------------------------------------------*/

/* The TaskLED, TaskBTN and TaskSW tasks */
static void TaskLED( void *pvParameters );
static void TaskBTN( void *pvParameters );
static void TaskSW( void *pvParameters );
/*-----------------------------------------------------------*/

/* The task handles  */
static TaskHandle_t xLEDTask;
static TaskHandle_t xBTNTask;
static TaskHandle_t xSWTask;

int suspended  = 0;
int btn_suspended = 0;
int btn_read = 0;
int sw_read = 0;
int led_suspended = 0;
XGpio LEDInst, SWInst, BTNInst;

int main( void )
{
	 // Initialize LEDs
	 status = XGpio_Initialize(&LEDInst, LEDS_DEVICE_ID);
	 if (status != XST_SUCCESS) return XST_FAILURE;

	 // Initialize buttons
	 status = XGpio_Initialize(&BTNInst, BTN_DEVICE_ID);
	 if (status != XST_SUCCESS) return XST_FAILURE;

	 // Initialize slide switches
	 status = XGpio_Initialize(&SWInst, SW_DEVICE_ID);
	 if (status != XST_SUCCESS) return XST_FAILURE;

	 // Set LEDs direction to outputs
	 XGpio_SetDataDirection(&LEDInst, 1, 0x00);

	 // Set button direction to inputs
	 XGpio_SetDataDirection(&BTNInst, 1, 0xFF);

	 // Set slide switch direction to inputs
	 XGpio_SetDataDirection(&SWInst, 1, 0xFF);

	const TickType_t x10seconds = pdMS_TO_TICKS( DELAY_10_SECONDS );

	/* Create LED task with priority 1*/
	xTaskCreate( 	TaskLED, /* The function that implements the task. */
				( const char * ) "TaskLED",  /* Text name for the task, provided to assist debugging only. */
				configMINIMAL_STACK_SIZE, 	/* The stack allocated to the task. */
				NULL, 						/* The task parameter is not used, so set to NULL. */
				tskIDLE_PRIORITY+1,			/* The task runs at the idle priority. */
				&xLEDTask);

	/* Create BTN task with priority 1*/
	xTaskCreate( TaskBTN,
				( const char * ) "BTN",
				configMINIMAL_STACK_SIZE,
				NULL,
				tskIDLE_PRIORITY + 1,
				&xBTNTask );

	/* Create SW task with priority 1*/
	xTaskCreate( TaskSW,
				 ( const char * ) "TaskSW",
				 configMINIMAL_STACK_SIZE,
				 NULL,
				 tskIDLE_PRIORITY + 1,
				 &xSWTask );



	/* Start the tasks and timer running. */
	vTaskStartScheduler();

	/* If all is well, the scheduler will now be running, and the following line
	will never be reached.  If the following line does execute, then there was
	insufficient FreeRTOS heap memory available for the idle and/or timer tasks
	to be created.  See the memory management section on the FreeRTOS web site
	for more details. */
	for( ;; );
}


/*-----------------------------------------------------------*/

/* Led Task function */
static void TaskLED( void *pvParameters )
{
	int led = 0;  // Define and initialize led variable

	const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );

	for( ;; )  // Infinite loop
	{
				// for loop to count up LED
				for(int i=0; i<=15;i++){
					XGpio_DiscreteWrite(&LEDInst, CHANNEL, led);  // Write to LEDs
					vTaskDelay( x1second );  // Delay of 1 second
					led = led +1;  // increment led count
					if (led>15){  // rest count once reached 15
						led = 0;
					}
				}
	}
}


/* TaskBTN function */
static void TaskBTN( void *pvParameters )
{

	const TickType_t x1second = pdMS_TO_TICKS( DELAY_1_SECOND );  // assign delay variable
	for( ;; )  // Infinite loop
	{

		btn_read = XGpio_DiscreteRead(&BTNInst, CHANNEL);  // Read button inputs

		if (btn_read == 0x3){  // if BTN0 and BTN1 is depressed
			vTaskSuspend(xLEDTask);  // Suspend LED Task
			suspended = 1;
		}
		else if (btn_read == 0x4){  // If BTN2 is depressed resume LED Task
			vTaskResume(xLEDTask);
			suspended = 0;
		}
		else if (btn_read == 0x8){  // If BTN3 is depressed suspend Switch Task
			vTaskSuspend(xSWTask);
			if(led_suspended == 1){  // If both BTN3 is depressed and led was suspended resume LED Task
				vTaskResume(xLEDTask);
				led_suspended = 0;
			}
		}
		else{  // Else if no buttons are pressed resume Switch task
			vTaskResume(xSWTask);
		}

	}
}


/* Switch Task function*/
static void TaskSW( void *pvParameters )
{

	for( ;; )
	{
		sw_read = XGpio_DiscreteRead(&SWInst, SW_CHANNEL);  // Read switch inputs

		if (sw_read == 0x3){  // If SW1 and SW0 are ON suspend Button Task
			vTaskSuspend(xBTNTask);
			btn_suspended = 1;
		}
		else if (sw_read == 0x8){  // If SW3 is ON, suspend LED task and resume Button Task
			vTaskSuspend(xLEDTask);
			led_suspended = 1;
			vTaskResume(xBTNTask);
		}

		else if (suspended == 0 && sw_read != 0x8){  // Else resume LED and Button tasks
			vTaskResume(xLEDTask);
			vTaskResume(xBTNTask);

		}

	}
}



