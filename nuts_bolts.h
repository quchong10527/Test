/*
	nuts_bolts.h - Header file for shared definitions, variables, and functions
	Part of Grbl

	The MIT License (MIT)

	GRBL(tm) - Embedded CNC g-code interpreter and motion-controller
	Copyright (c) 2009-2011 Simen Svale Skogsrud
	Copyright (c) 2011-2012 Sungeun K. Jeon

	Permission is hereby granted, free of charge, to any person obtaining a copy
	of this software and associated documentation files (the "Software"), to deal
	in the Software without restriction, including without limitation the rights
	to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
	copies of the Software, and to permit persons to whom the Software is
	furnished to do so, subject to the following conditions:

	The above copyright notice and this permission notice shall be included in
	all copies or substantial portions of the Software.

	THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
	IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
	FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
	AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
	LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
	OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
	THE SOFTWARE.
*/

#ifndef nuts_bolts_h
#define nuts_bolts_h

#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include "config.h"
#include "defaults.h"
#include "pin_map.h"

#define false       0
#define true        1

#define N_AXIS      3 // Number of axes
#define X_AXIS      0 // Axis indexing value
#define Y_AXIS      1
#define Z_AXIS      2

#define MM_PER_INCH (25.40)
#define INCH_PER_MM (0.0393701)

// Useful macros
#define clear_vector(a) memset(a, 0, sizeof(a))
#define clear_vector_float(a) memset(a, 0.0, sizeof(float)*N_AXIS)
#define max(a,b) (((a) > (b)) ? (a) : (b))
#define min(a,b) (((a) < (b)) ? (a) : (b))

// Bit field and masking macros
#define bit(n)              (1 << n) 
#define bit_true(x,mask)    (x |= mask)
#define bit_false(x,mask)   (x &= ~mask)
#define bit_toggle(x,mask)  (x ^= mask)
#define bit_istrue(x,mask)  ((x & mask) != 0)
#define bit_isfalse(x,mask) ((x & mask) == 0)

// Define system executor bit map. Used internally by runtime protocol as runtime command flags, 
// which notifies the main program to execute the specified runtime command asynchronously.
// NOTE: The system executor uses an unsigned 8-bit volatile variable (8 flag limit.) The default
// flags are always false, so the runtime protocol only needs to check for a non-zero value to 
// know when there is a runtime command to execute.
#define EXEC_STATUS_REPORT   bit(0) // bitmask 00000001
#define EXEC_CYCLE_START     bit(1) // bitmask 00000010
#define EXEC_CYCLE_STOP      bit(2) // bitmask 00000100
#define EXEC_FEED_HOLD       bit(3) // bitmask 00001000
#define EXEC_RESET           bit(4) // bitmask 00010000
#define EXEC_ALARM           bit(5) // bitmask 00100000
#define EXEC_CRIT_EVENT      bit(6) // bitmask 01000000
// #define                  bit(7) // bitmask 10000000

// Define system state bit map. The state variable primarily tracks the individual functions
// of Grbl to manage each without overlapping. It is also used as a messaging flag for
// critical events.
#define STATE_IDLE       0 // Must be zero.
#define STATE_INIT       1 // Initial power up state.
#define STATE_QUEUED     2 // Indicates buffered blocks, awaiting cycle start.
#define STATE_CYCLE      3 // Cycle is running
#define STATE_HOLD       4 // Executing feed hold
#define STATE_HOMING     5 // Performing homing cycle
#define STATE_ALARM      6 // In alarm state. Locks out all g-code processes. Allows settings access.
#define STATE_CHECK_MODE 7 // G-code check mode. Locks out planner and motion only. (Locks out在...之外)
                           // G代码检查模式.只运动不执行预处理
// #define STATE_JOG     8 // Jogging mode is unique like homing.

// Define global system variables
typedef struct {
	uint8_t  abort;                // System abort flag. Forces exit back to main loop for reset.
	                               // 系统异常标志,异常时强制退出主循环并复位 
	uint8_t  state;                // Tracks the current state of Grbl.
	                               // GRBL当前状态
	volatile uint8_t execute;      // Global system runtime executor bitflag variable. See EXEC bitmasks.
	                               // 系统运行时实位标志
	int32_t  position[N_AXIS];     // Real-time machine (aka home) position vector in steps. 
	                               // NOTE: This may need to be a volatile variable, if problems arise.
	                               // 以步的方式表示机床时实位置向量.当出现问题时需要一个易变的数值
	uint8_t  auto_start;           // Planner auto-start flag. Toggled off during feed hold. Defaulted by settings.
	                               // 预处理器自动启动标志,当暂停时关掉预处理器.默认状态由settings设置
} system_t;
extern system_t sys;

// Read a floating point value from a string. Line points to the input buffer, char_counter 
// is the indexer pointing to the current character of the line, while float_ptr is 
// a pointer to the result variable. Returns true when it succeeds
// line: 字符输入缓冲  char_counter: 程序行当前字符的索引 float_ptr: 计算的结果
int read_float(char *line, uint8_t *char_counter, float *float_ptr);

// Delays variable-defined milliseconds. Compiler compatibility fix for _delay_ms().
void delay_ms(uint16_t ms);

// Delays variable-defined microseconds. Compiler compatibility fix for _delay_us().
void delay_us(uint32_t us);

// Syncs Grbl's gcode and planner position variables with the system position.
void sys_sync_current_position();

#endif
