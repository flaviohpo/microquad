#ifndef __PORTS_H
#define __PORTS_H

#include "mytypes.h"

#define LOW_TO_HIGH                 0
#define HIGH_TO_LOW                 1

#define Port1GetInterruptEdge(x) (P1IES & x)
#define Port1SetInterruptEdge(x) (P1IES|=(1<<x))    // config como HIGH_TO_LOW
#define Port1ClrInterruptEdge(x) (P1IES&=~(1<<x))   // config como LOW_TO_HIGH

#define Port1GetInterruptFlag(x) (P1IFG & x)
#define Port1ClrInterruptFlag(x) (P1IFG&=~(1<<x))

void port_init_p1(uint8 InitialCondition, uint8 InterruptEnable, uint8 Outputs, uint8 Select);
void port_init_p2(uint8 InitialCondition, uint8 InterruptEnable, uint8 Outputs, uint8 Select);
void port_init_p3(uint8 InitialCondition, uint8 Outputs, uint8 Select);
void port_init_p4(uint8 InitialCondition, uint8 Outputs, uint8 Select);
void port_init_p5(uint8 InitialCondition, uint8 Outputs, uint8 Select);
void port_init_p6(uint8 InitialCondition, uint8 Outputs, uint8 Select);

void port_set_interrupt_P1(void(*interrupt_p1)(void));

#endif // __PORTS_H
