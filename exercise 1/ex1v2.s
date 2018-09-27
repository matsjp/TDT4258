        .syntax unified
	
	      .include "efm32gg.s"

	/////////////////////////////////////////////////////////////////////////////
	//
  // Exception vector table
  // This table contains addresses for all exception handlers
	//
	/////////////////////////////////////////////////////////////////////////////
	
        .section .vectors
	
	      .long   stack_top               /* Top of Stack                 */
	      .long   _reset                  /* Reset Handler                */
	      .long   dummy_handler           /* NMI Handler                  */
	      .long   dummy_handler           /* Hard Fault Handler           */
	      .long   dummy_handler           /* MPU Fault Handler            */
	      .long   dummy_handler           /* Bus Fault Handler            */
	      .long   dummy_handler           /* Usage Fault Handler          */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* SVCall Handler               */
	      .long   dummy_handler           /* Debug Monitor Handler        */
	      .long   dummy_handler           /* Reserved                     */
	      .long   dummy_handler           /* PendSV Handler               */
	      .long   dummy_handler           /* SysTick Handler              */

	      /* External Interrupts */
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO even handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   gpio_handler            /* GPIO odd handler */
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler
	      .long   dummy_handler

	      .section .text

	/////////////////////////////////////////////////////////////////////////////
	//
	// Reset handler
  // The CPU will start executing here after a reset
	//
	/////////////////////////////////////////////////////////////////////////////

	      .globl  _reset
	      .type   _reset, %function
        .thumb_func
_reset:
	bl gpio_clock_setup
	
	bl gpio_led_setup
	
	bl gpio_button_setup
	
	bl gpio_interrupt_setup
	
	bl nvic_setup
	
	bl sleep_setup
	
	bl energy_optimization_low_drive_strength
	
	bl energy_optimization_turn_off_sram
	
	//bl energy_optimization_adjust_peripheral_clock
	
	bl energy_optimization_adjust_core_clock
	wfi
	
	
	//enable GPIO clock
gpio_clock_setup:
	ldr r1, =CMU_BASE
	ldr r2, =CMU_HFPERCLKEN0
	ldr r3, [r1, r2]
	mov r4, 1
	ldr r5, =CMU_HFPERCLKEN0_GPIO
	lsl r4, r4, r5
	orr r3, r3, r4
	str r3, [r1, r2]
	bx lr
	
	//Enable LEDS for port A
gpio_led_setup:
	ldr r1, =GPIO_PA_BASE
	//Set pins 8-15 to output
	ldr r2, =GPIO_MODEH
	mov r3, 0x55555555
	str r3, [r1, r2]
	//Turn off LEDS
	ldr r2, =GPIO_DOUT
	mov r3, 0xffffffff
	str r3, [r1, r2]
	bx lr
	
	//Enable port c buttons for input on pins 0-7
gpio_button_setup:
	ldr r1, =GPIO_PC_BASE
	ldr r2, =GPIO_MODEL
	mov r3, 0x33333333
	str r3, [r1, r2]
	ldr r2, =GPIO_DOUT
	mov r3, 0xff
	str r3, [r1, r2]
	bx lr
	
	//Configure GPIO control registers to allow for interrupts
gpio_interrupt_setup:
	ldr r1, =GPIO_BASE
	ldr r2, =GPIO_EXTIPSELL
	mov r3, 0x22222222
	str r3, [r1, r2]
	mov r3, 0xff
	ldr r2, =GPIO_EXTIRISE
	str r3, [r1, r2]
	ldr r2, =GPIO_EXTIFALL
	str r3, [r1, r2]
	ldr r2, =GPIO_IEN
	str r3, [r1, r2]
	bx lr
	
	//Enable NVIC
nvic_setup:
	ldr r1, =ISER0
	ldr r2, =0x802
	str r2, [r1]
	bx lr
	
	//Enable deep sleep and exit to sleep
sleep_setup:
	ldr r1, =SCR
	mov r2, 0b110
	str r2, [r1]
	bx lr
	
	//Set low drive strength for leds
energy_optimization_low_drive_strength:
	ldr r1, =GPIO_PA_BASE
	mov r2, 1
	str r2, [r1]
	bx lr
	
	//Turn off sram
energy_optimization_turn_off_sram:
	ldr r1, =EMU_BASE
	mov r2, 0x4//EMU_MEMCTRL offset
	mov r3, 0x7
	str r3, [r1, r2]
	bx lr
	
	//Adjust peripheral clock
energy_optimization_adjust_peripheral_clock:
	ldr r1, =CMU_BASE
	mov r2, 0x8//CMU_HFPERCLKDIV offset
	ldr r3, [r1, r2]//The value that gets loaded in here contins an enable bit
	mov r4, 5//Value I want written to CMU_HFPERCLKDIV
	orr r3, r3, r4//Need to ensure that I don't overwrite the enable bit
	str r3, [r1, r2]
	bx lr
	
	//Adjust core clock
energy_optimization_adjust_core_clock:
	ldr r1, =CMU_BASE
	mov r2, 0x4//CMU_HFCORECLKDIV offset
	ldr r3, [r1, r2]//The value that gets loaded in here contins a bit that shouldn't get overwritten
	mov r4, 2//Value I want written to CMU_HFCORECLKDIV
	orr r3, r3, r4//Need to ensure that I don't overwrite the bit
	str r3, [r1, r2]
	bx lr

	/////////////////////////////////////////////////////////////////////////////
	//
  // GPIO handler
  // The CPU will jump here when there is a GPIO interrupt
	//
	/////////////////////////////////////////////////////////////////////////////

        .thumb_func
gpio_handler:
	ldr r1, =GPIO_BASE
	ldr r2, =GPIO_IF
	ldr r3, =GPIO_IFC
	ldr r4, [r1, r2]
	str r4, [r1, r3]
	
	ldr r1, =GPIO_PC_BASE
	ldr r2, =GPIO_DIN
	ldr r3, [r1, r2]
	mov r4, 8
	lsl r3, r3, r4
	ldr r1, =GPIO_PA_BASE
	ldr r2, =GPIO_DOUT
	str r3, [r1, r2]
	bx lr	
	/////////////////////////////////////////////////////////////////////////////
	
        .thumb_func
dummy_handler:  
        b .  // do nothing

