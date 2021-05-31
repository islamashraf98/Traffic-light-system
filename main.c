#include "stdint.h"
#include "stdbool.h"
#include "inc/hw_ints.h"
#include "inc/hw_memmap.h"
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/sysctl.h"
#include "driverlib/timer.h"
#include "driverlib/interrupt.h"
#include "driverlib/systick.h"
#include "uart.h"

#define FREQUENCY 16000000U
#define RED 0
#define YELLOW 1
#define GREEN 2
#define ON 1
#define OFF 0
#define NORTH_TO_SOUTH 0
#define EAST_TO_WEST 1
#define BUTTON_NORTH_TO_SOUTH 0
#define BUTTON_EAST_TO_WEST 1
#define SIZE 15

volatile uint8_t north_to_south_status = GREEN, east_to_west_status = RED,
peds_north_to_south_status = RED, peds_east_to_west_status = GREEN,
north_to_south_status_before, east_to_west_status_before,
peds_north_to_south_status_before, peds_east_to_west_status_before,
north_to_south_status_before2, east_to_west_status_before2,
peds_north_to_south_status_before2, peds_east_to_west_status_before2,
button_pressed,
int_status,
sequence = NORTH_TO_SOUTH,
time = 0,
peds_count=0,
enable_pressing = 1;
 
uint8_t uart_status [SIZE];

void cars_traffic_ISR();
void peds_button_ISR();
void peds_traffic_ISR();

int main()
{
                                            //--------------Clock configurations-----------//

    //Enabling clock to portA, PORTD, PORTF and timer0
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOA);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOF);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_GPIOD);
    SysCtlPeripheralEnable(SYSCTL_PERIPH_TIMER0);

    //waiting in a loop checking that the clock is enabled
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOA));
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOF));
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_GPIOD));
    while (!SysCtlPeripheralReady(SYSCTL_PERIPH_TIMER0));

                                            //--------------GPIO configurations-----------//
    
    //Making PA2, PA3, PA4, PA5, PA6, PA7 as output (Cars traffic lights)
    GPIOPinTypeGPIOOutput(GPIO_PORTA_BASE,GPIO_PIN_2 | GPIO_PIN_3 | GPIO_PIN_4 | GPIO_PIN_5 | GPIO_PIN_6 | GPIO_PIN_7);
    //Making PD0, PD1, PD2, PD3 as output (Pedestrians traffic lights)
    GPIOPinTypeGPIOOutput(GPIO_PORTD_BASE,GPIO_PIN_0 | GPIO_PIN_1 | GPIO_PIN_2 | GPIO_PIN_3);
    
    //Making PF4 as input (button for pedestrian_north_to_south)
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_4);
    //Making PF0 as input (button for pedestrian_east_to_west)
    GPIOPinTypeGPIOInput(GPIO_PORTF_BASE,GPIO_PIN_0);

    //Unlocking PF0 in order to work properly
    GPIOUnlockPin(GPIO_PORTF_BASE,GPIO_PIN_0);
    
    //Configuring as pull up for PF4 (button for pedestrian_north_to_south)
    GPIOPadConfigSet(GPIO_PORTF_BASE ,GPIO_PIN_4,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);
    //Configuring as pull up for PF0 (button for pedestrian_east_to_west)
    GPIOPadConfigSet(GPIO_PORTF_BASE ,GPIO_PIN_0,GPIO_STRENGTH_2MA,GPIO_PIN_TYPE_STD_WPU);

    //Enabling interrupt for PF4 and PF0
    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_4);
    GPIOIntEnable(GPIO_PORTF_BASE,GPIO_INT_PIN_0);
    
    //Making the interrupt type falling edge for the onboard switches PF0 and PF4 (internal pull up)
    GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_4,GPIO_FALLING_EDGE);    
    GPIOIntTypeSet(GPIO_PORTF_BASE,GPIO_PIN_0,GPIO_FALLING_EDGE);    
    
    //Passing the address of the function (peds_north_to_south_ISR) to the GPIO Int register
    //as this will be our interrupt handler when there is an interrupt caused by portF
    GPIOIntRegister(GPIO_PORTF_BASE,peds_button_ISR);
    
    
    //--------------Systick timer to count 2 seconds when a pedestrian presses the push button-----------//
    
    //Start by disabling the timer
    SysTickDisable();
    
    //Reloading it with 16Million (elapse every 1 second)
    SysTickPeriodSet(FREQUENCY);
    
    //Enabling interrupt for systick
    SysTickIntEnable();
    
    //Passing the address of the ISR
    SysTickIntRegister(peds_traffic_ISR);
    

    //--------------Timer0 to count the 8 seconds (5 then 7 then 8) for the sequence-----------//
    
    //Setting timer clock source to be clock system
    TimerClockSourceSet(TIMER0_BASE,TIMER_CLOCK_SYSTEM);

    //Starting timer0 by disabling it first
    TimerDisable(TIMER0_BASE,TIMER_A);
    
    //Configuring the timer to work periodic down counting
    TimerConfigure(TIMER0_BASE,TIMER_CFG_PERIODIC);
    
    //reloading it with the value that will make it elapse every 1 second
    TimerLoadSet(TIMER0_BASE, TIMER_A, FREQUENCY);
    
    //Enabling the interrupts for timer0 when timerA timeout
    TimerIntEnable(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
    
    //Passing the address of the function (north_to_south_ISR) to the timer Int register as this will
    //be our interrupt handler when there is an interrupt caused by timeout of timer0 and timer1
    TimerIntRegister(TIMER0_BASE,TIMER_A,cars_traffic_ISR);
    
    //enabling timer0 only to start counting (Starting from the north_to_south cars green)
    TimerEnable(TIMER0_BASE,TIMER_A);
    
    //enabling the uart
    uart_init();
    
    while (1){
      
                    //--------------North_to_south traffic---------------------//
      
      //Checking If the north_to_south traffic status is RED, light it RED
      if (north_to_south_status == RED){
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,GPIO_PIN_2);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,OFF);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,OFF);
      }
      
      //Checking If the north_to_south traffic status is Yellow, light it Yellow
      else if (north_to_south_status == YELLOW){
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,OFF);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,GPIO_PIN_3);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,OFF);
      }
      
      //Checking If the north_to_south traffic status is Green, light it GREEN
      else if (north_to_south_status == GREEN){
        
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_2,OFF);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_3,OFF);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_4,GPIO_PIN_4);   
        
      //Checking if it is a new status for the north to south (to become green) then transmit it through uart0 and receive it through loopback mode
        
      if (north_to_south_status_before2 != north_to_south_status){
          uart_transmit_string("CARS NORTH SOUTH.", UART0_TRANSMIT);       
          uart_receive_string(uart_status, UART0_RECEIVE);
        }
      }
      

                    //--------------East_to_west traffic---------------------//
      
      //Checking If the east_to_west traffic status is RED, light it RED
      if (east_to_west_status == RED){
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_5,GPIO_PIN_5);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_6,OFF);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_7,OFF);
        
      }
      
      //Checking If the east_to_west traffic status is Yellow, light it Yellow
      else if (east_to_west_status == YELLOW){
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_5,OFF);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_6,GPIO_PIN_6);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_7,OFF);
        
      }
      
      //Checking If the east_to_west traffic status is Green, light it GREEN
      else if (east_to_west_status == GREEN){
        
                               
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_5,OFF);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_6,OFF);
        GPIOPinWrite(GPIO_PORTA_BASE,GPIO_PIN_7,GPIO_PIN_7);
        
      //Checking if it is a new status for the east to west (to become green) then transmit it through uart0 and receive it through loopback mode
        if (east_to_west_status_before2 != east_to_west_status){
          uart_transmit_string("CARS EAST WEST.", UART0_TRANSMIT);
          uart_receive_string(uart_status, UART0_RECEIVE);
        }
      }

      
              //--------------North_to_south peds---------------------//
      
      
      //Checking If the north_to_south pedestrians status is RED, light it RED
      if (peds_north_to_south_status == RED){
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0,GPIO_PIN_0);
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_1,OFF);
      }
      
      //Checking If the north_to_south pedestrians status is Green, light it GREEN
      else if (peds_north_to_south_status == GREEN){
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_0,OFF);
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_1,GPIO_PIN_1);
        
      //Checking if it is a new status for the north to south pedestrians (to become green) then transmit it through uart0 and receive it through loopback mode
        if (peds_north_to_south_status_before2 != peds_north_to_south_status){
          uart_transmit_string("PEDS NORTH SOUTH.", UART0_TRANSMIT);
          uart_receive_string(uart_status, UART0_RECEIVE);
        } 
      }

      
              //--------------East_to_west peds---------------------//
      
      //Checking If the east_to_south pedestrians status is RED, light it RED
      if (peds_east_to_west_status == RED){
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_2,GPIO_PIN_2);
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,OFF);
        
      }
      
      //Checking If the east_to_south pedestrians status is GREEN, light it GREEN
      else if (peds_east_to_west_status == GREEN){

        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_2,OFF);
        GPIOPinWrite(GPIO_PORTD_BASE,GPIO_PIN_3,GPIO_PIN_3);
        
      //Checking if it is a new status for the east to west pedestrians (to become green) then transmit it through uart0 and receive it through loopback mode
        if (peds_east_to_west_status_before2 != peds_east_to_west_status){
          uart_transmit_string("PEDS EAST WEST.", UART0_TRANSMIT);
          uart_receive_string(uart_status, UART0_RECEIVE);
        }
        
      }          
      
      //Saving last status for each traffic light
      north_to_south_status_before2 = north_to_south_status;
      east_to_west_status_before2 = east_to_west_status;
      peds_north_to_south_status_before2 = peds_north_to_south_status;
      peds_east_to_west_status_before2 = peds_east_to_west_status;
                          
    }
}

void cars_traffic_ISR(){
  
 
  //Clearing the timer0A interrupt flag
  TimerIntClear(TIMER0_BASE, TIMER_TIMA_TIMEOUT);
  
  //incrementing the time variable (represents how many seconds elapsed)
  time++;

  //Checking which direction is handling the sequence (is lighted GREEN)
  if (sequence == NORTH_TO_SOUTH){
    
    //If the sequence is NORTH_TO_SOUTH (lighted GREEN) then the other direction is lighted RED

    
    //Checking if 5 seconds elapsed then the traffic light is switched to YELLOW and if 7 seconds elapsed (another 2 seconds),
    //the traffic light is switched to RED and the pedestrians light is switched to GREEN
    
    if (time == 5){
      north_to_south_status = YELLOW;
    }
    else if (time == 7){
      north_to_south_status = RED;
      peds_north_to_south_status = GREEN;
    }
    else if (time == 8){
      
      //then at the last second (after total 8 seconds) the time variable is initalized again by 0
      time = 0;
      
      //Switching the sequence to EAST_TO_wEST
      sequence = EAST_TO_WEST;
      
      //east_to_west direction is become green to start the sequence and its pedestrians becomes RED
      east_to_west_status = GREEN;
      peds_east_to_west_status = RED;
    }
  }
  else if (sequence == EAST_TO_WEST){
    //Checking if 5 seconds elapsed then the traffic light is switched to YELLOW and if 7 seconds elapsed (another 2 seconds),
    //the traffic light is switched to RED and the pedestrians light is switched to GREEN
    if (time == 5){
      east_to_west_status = YELLOW;
    }
    else if (time == 7){
      east_to_west_status = RED;
      peds_east_to_west_status = GREEN;
    }
    
    //then at the last second (after total 8 seconds) the time variable is initalized again by 0
    else if (time == 8){
      time = 0;
      
      //Switching the sequence to NORTH_TO_SOUTH
      sequence = NORTH_TO_SOUTH;
      
      //north_to_south direction is become green to start the sequence and its pedestrians becomes RED
      north_to_south_status = GREEN;
      peds_north_to_south_status = RED;
    }
  }
}

void peds_traffic_ISR (){
  
  //incrementing peds_count by 1 (represents no of seconds elapsed)
  peds_count++;
  
  //Checking if 2 seconds elapsed from the time the button pressed
  if (peds_count == 2){
    
    //Checking which button was pressed in order to know which direction was holding the sequence
    if (button_pressed == BUTTON_NORTH_TO_SOUTH){
      
      //If the north to south was holding the sequence then its status is restored before the button pressed and the timer is enabled again
      north_to_south_status = north_to_south_status_before;
      peds_north_to_south_status = peds_north_to_south_status_before;
      TimerDisable(TIMER0_BASE,TIMER_A);
      TimerEnable(TIMER0_BASE,TIMER_A);
    }
    
    else if (button_pressed == BUTTON_EAST_TO_WEST){
      
      //If the east to west was holding the sequence then its status is restored before the button pressed and the timer is enabled again
      east_to_west_status = east_to_west_status_before;
      peds_east_to_west_status = peds_east_to_west_status_before;
      TimerDisable(TIMER0_BASE,TIMER_A);
      TimerEnable(TIMER0_BASE,TIMER_A);
    }
  }
  
  //Checking if 1 second from the end of the Period of pedestrian crossing is over
  else if (peds_count == 3){
    //initialzing the seconds back to 0 and disabling the systick timer as the Period of pedestrian crossing is done
    peds_count=0;
    
    //Disabling the systick timer of the pedestrian crossing
    SysTickDisable();
    
    //Enabling the button to be pressed again
    enable_pressing = 1;
    
  }
}

void peds_button_ISR (){
  
  //Getting the number of the interrupt in portF
  int_status = GPIOIntStatus(GPIO_PORTF_BASE,true);
  
  //Clearing the interrupt flag
  GPIOIntClear(GPIO_PORTF_BASE,int_status);
  
  //Checking which pin caused the interrupt (either pin 0 or 4) as the int_status should be the same number of the interrupt pin
  //also checking if it is enabled to press it
  if (((int_status & GPIO_INT_PIN_4) == GPIO_INT_PIN_4) && enable_pressing == 1){
    
    //If pin 4 caused the interrupt then the button pressed is identified as the pedestrian traffic belonging to north to south cars
    button_pressed = BUTTON_NORTH_TO_SOUTH;
    
    
    //The button is only effective if the cars traffic is GREEN so that if the 2 buttons were pressed together
    //only the button with the corresponding cars traffic GREEN is held
    if (north_to_south_status == GREEN){
      
      //current status of north_to_south direction is reserved and the new status becomes RED so that pedestrians can cross
      north_to_south_status_before = north_to_south_status;
      peds_north_to_south_status_before = peds_north_to_south_status;
      north_to_south_status = RED;
      peds_north_to_south_status = GREEN;
              
      //disabling the pressing of the button until 1 second is elapsed from the end of the Period of pedestrian crossing
      enable_pressing = 0;
      
      //Disabling the cars traffic timer in order to pause until pedestrians cross (2 seconds)
      TimerDisable(TIMER0_BASE,TIMER_A);
    }
  }
  
  else if (((int_status & GPIO_INT_PIN_0) == GPIO_INT_PIN_0) && enable_pressing==1){
    
    //If pin 0 caused the interrupt then the button pressed is identified as the pedestrian traffic belonging to east to west cars
    button_pressed = BUTTON_EAST_TO_WEST;
    
    //The button is only effective if the cars traffic is GREEN
    if (east_to_west_status == GREEN){
      
    //current status of east_to_west direction is reserved and the new status becomes RED so that pedestrians can cross
    east_to_west_status_before = east_to_west_status;
    peds_east_to_west_status_before = peds_east_to_west_status;
    peds_east_to_west_status = GREEN;
    east_to_west_status = RED;
    

    //disabling the pressing of the button until 1 second is elapsed from the end of the Period of pedestrian crossing
    enable_pressing = 0;
    
    //Disabling the cars traffic timer in order to pause until pedestrians cross (2 seconds)
    TimerDisable(TIMER0_BASE,TIMER_A);
    }
  }
  //Enabling the systickTimer to start counting 2 seconds
  SysTickEnable(); 
}
