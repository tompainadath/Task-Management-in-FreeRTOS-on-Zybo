# Task-Management-in-FreeRTOS-on-Zybo
# Summary
The lab analyses different functions of Vivado’s SDK IP Integrator. For this basic IP integrator was explored. A block design with two GPIO interfaces was created in the Vivado software. An SDK FreeRTOS template was imported and was modified to input switches and buttons,  and output LEDs. Programmed the system to let the LEDs behave certain way according to inputs from switches by switching between tasks. The motivation for this analysis was to understand concept of task management in Embedded Systems. The results were as expected. Successfully synthesized the block design and the system behaved the way desired.
#  Introduction
The objectives of the lab were to control the behavior of LEDs on the zybo board using the switches and buttons using FeeRTOS tasks. The tasks to perform are described below:
1)	Create a FreeRTOS task TaskLED that increments a 4-bit counter and displays the results in the LEDs with an appropriate delay at priority 1. 
2)	Create a FreeRTOS task TaskBTN that reads the BTNs at priority 1. Within the FreeRTOS task TaskBTN 
a)	If BTN0 and BTN1 are depressed at some point together then TaskLED is suspended (vTaskSuspend()). If BTN2 is depressed, regardless of the status of BTN0 and BTN1, then TaskLED is resumed (vTaskResume()). 
b)	If BTN3 is depressed then TaskSW is suspended (vTaskSuspend()). If BTN3 is released then TaskSW is resumed (vTaskResume()). 
3)	Create a FreeRTOS task TaskSW that reads the SWs at priority 1. Within the FreeRTOS task TaskSW:
a)	If SW0 and SW1 are ON together at some point then TaskBTN is suspended (vTaskSuspend()). 
b)	If SW0 and SW1 are OFF then TaskBTN is resumed (vTaskResume()). 
c)	If SW3 is ON then TaskLED is suspended (vTaskSuspend()). If SW3 is OFF TaskLED is resumed (vTaskResume()). 
#  Discussion
Created a block design consisting of ZYNQ7 processing system, processor system reset, AXI Interconnect and two AXI GPIOs. The diagram of the design is shown below:
 
Figure 1. Diagram of the Hardware design
It can be observed that two GPIO blocks are created to control LEDs and get inputs from buttons and switches. HDL wrapper file was created and generated bitstream. The design was then exported to SDK. Imported the template ‘Hello world’ file and modified it to perform the specified tasks. 
Two tasks were created using the xTaskCreate function. They were TaskLED and TaskSW. Both tasks were given the same priority, tskIDLE_PRIORITY+1. The code snippet below shows how this was accomplished
 
Figure 2. Code to create tasks
To accomplish task 1, a TaskLED function was defined. Within this function, an infinite loop was created. Added different LED sequences as previously defined with a 1 second delay between them. The delay was created first by converting seconds to equivalent ticks using the pdMS_TO_TICKS function and using it as a parameter inside vTaskDelay function. A code snippet of the task function TaskLED is provided below:
 
Figure 3. Task TaskLED function
The images below show how the default LEDs behavior. The LEDs count up from 0 to 15 and do it again infinitely
 
Figure 4. Default LED task 

 
Figure 5. Default LED task

 
Figure 6. Default LED task

 
Figure 7. Default LED task
For Task 2, TaskBTN function was defined. Inside the function an infinite loop was created with if-else statements that checks status of the switches. A code snippet of this task function is given below:
 
Figure 8. Task TaskBTN function
The images below shows the behavior of LEDs when BTN0 and BTN1 are depressed together
 
Figure 9. When BTN0 and BTN1 are depressed
It can be seen that the TaskLED is suspended pausing the count up of LEDs. Images below shows how LEDs resume with the sequence when SW1 is turned ON. 
 
Figure 10. When BTN2 is depressed
To do task remaining we would need to define the third task, TaskSW. The code snippet for this is shown below:
 
Figure 11. Task TaskSW function
In the above function, the code suspends TaskBTN when SW0 and SW1 are turned ON. In any other cases the TaskBTN is resumed. Meanwhile, when SW3 is turned ON the TaskLED is suspended and resumed when turned OFF. Checks the input from the switches using the XGpio_DiscreteRead function. The images below shows Task 2b in action
 
Figure 11. When SW3 is ON
 
Figure 12. When BTN3 is depressed
Figure 11 Shows that when SW3 is ON the TaskLED is suspended. But when BTN3 is depressed the TaskSW is suspended and the LED count resumes. This also demonstrates Task 3c, The case were TaskLED is suspended when SW3 is turned ON and resumes otherwise.
For task 3a and 3b, when SW0 and SW1 are turned ON the TaskBTN is suspended. Following images this in action:
 
Figure 13. When SW0 and SW1 are turned ON
 
Figure 14. When SW0 and SW1 are turned ON and BTN0 and BTN1 depressed
 
Figure 15. When SW0 and SW1 are turned ON and BTN0 and BTN1 depressed
 
Figure 16. When SW0 and SW1 are turned ON and BTN0 and BTN1 depressed
 
Figure 16. When SW0 and SW1 are turned OFF and BTN0 and BTN1 depressed
From the above images it can be seen that when SW0 and SW1 are turned ON the TaskBTN is suspended. When the those switches are turned OFF, the TaskBTN resumes. 
#  Conclusions
Successfully generated a hardware design using Vivado software and implemented the tasks on Zybo board by programming in C using SDK. The main object of the lab was to create tasks and manage them based of the specifications. Successfully switched between tasks based on the inputs from the buttons and switches.The project results met the objectives as expected. 
