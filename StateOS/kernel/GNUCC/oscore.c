/******************************************************************************

    @file    StateOS: oscore.c
    @author  Rajmund Szymanski
    @date    12.11.2016
    @brief   StateOS port file for ARM Cotrex-M uC.

 ******************************************************************************

    StateOS - Copyright (C) 2013 Rajmund Szymanski.

    This file is part of StateOS distribution.

    StateOS is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published
    by the Free Software Foundation; either version 3 of the License,
    or (at your option) any later version.

    StateOS is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program. If not, see <http://www.gnu.org/licenses/>.

 ******************************************************************************/

#if defined(__GNUC__) && !defined(__ARMCC_VERSION)

#include <stddef.h>
#include <oskernel.h>

/* -------------------------------------------------------------------------- */

__attribute__((naked))
void PendSV_Handler( void )
{
	__asm volatile
	(
#if __CORTEX_M < 3
"	mrs   r0,    PSP               \n"
"	mov   r3,    lr                \n"
"	lsl   r3,   #29                \n"
"	bmi   priv_tsk_switch          \n"
"	mov   r0,    sp                \n"
"	sub   sp,   #36                \n"
"priv_tsk_switch:                  \n"
"	sub   r0,   #36                \n"
"	stm   r0!, { r4  - r7 }        \n"
"	mov   r3,    r8                \n"
"	mov   r4,    r9                \n"
"	mov   r5,    r10               \n"
"	mov   r6,    r11               \n"
"	mov   r7,    lr                \n"
"	stm   r0!, { r3  - r7 }        \n"
"	sub   r0,   #36                \n"
#else
"	tst   lr,   #4                 \n"
"	itee  ne                       \n"
"	mrsne r0,    PSP               \n"
"	moveq r0,    sp                \n"
#if __FPU_USED
"	subeq sp,   #100               \n"
"	tst   lr,   #16                \n"
"	it    eq                       \n"
"vstmdbeq r0!, { s16 - s31 }       \n"
#else
"	subeq sp,   #36                \n"
#endif
"	stmdb r0!, { r4  - r11, lr }   \n"
#endif
"	bl    core_tsk_handler         \n"
#if __CORTEX_M < 3
"	add   r0,   #16                \n"
"	ldm   r0!, { r3  - r7 }        \n"
"	mov   r8,    r3                \n"
"	mov   r9,    r4                \n"
"	mov   r10,   r5                \n"
"	mov   r11,   r6                \n"
"	mov   lr,    r7                \n"
"	sub   r0,   #36                \n"
"	ldm   r0!, { r4  - r7 }        \n"
"	add   r0,   #20                \n"
"	mov   r3,    lr                \n"
"	lsl   r3,   #29                \n"
"	bmi   priv_tsk_enter           \n"
"	mov   sp,    r0                \n"
"	bx    lr                       \n"
"priv_tsk_enter:                   \n"
"	msr   PSP,   r0                \n"
"	bx    lr                       \n"
#else
"	ldmia r0!, { r4  - r11, lr }   \n"
#if __FPU_USED
"	tst   lr,   #16                \n"
"	it    eq                       \n"
"vldmiaeq r0!, { s16 - s31 }       \n"
#endif
"	tst   lr,   #4                 \n"
"	ite   ne                       \n"
"	msrne PSP,   r0                \n"
"	moveq sp,    r0                \n"
"	bx    lr                       \n"
#endif

:::	"memory"
	);
}

/* -------------------------------------------------------------------------- */

#endif // __GNUC__ && !__ARMCC_VERSION
