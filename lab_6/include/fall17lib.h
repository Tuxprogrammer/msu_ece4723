#ifndef FALL17LIB_H
#define FALL17LIB_H

#include "esos.h"

// Gets the number of elements in an array x (NOT in an array pointed to by x!)
#define NELEM(x) (sizeof(x)/sizeof(x[0]))

// Converts an integer to a string in the specified buffer and base.
char* itoa(int value, char* result, int base);

char* utoa(unsigned int value, char* result, int base);

char* utoar(unsigned int value, char* result, int base, unsigned int width);

// Converts the given null-terminated string to uppercase.
char* upperstring(char* psz_str);

// ESOS provides but does not declare the following API.
// To work around this bug, we declare it here.
ESOS_CHILD_TASK( __esos_OutUint8AsDecString, uint8_t u8_x);

// llocate and launch a child task, all in one go.
#define ESOS_TASK_CALL_CHILD(...) do { \
	static ESOS_TASK_HANDLE th_child; \
	ESOS_ALLOCATE_CHILD_TASK(th_child); \
	ESOS_TASK_SPAWN_AND_WAIT(th_child, __VA_ARGS__); \
} while(0)

// Minimum and maximum.  Standard stuff.
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#define MAX(x,y) ((x) > (y) ? (x) : (y))

// Conversion between decimal and 3p5 fixed point
uint8_t dtou3p5(uint8_t ipart, uint8_t fpart);
uint8_t u3p5toipart(uint8_t fixedpt);
uint8_t u3p5tofpart(uint8_t fixedpt);

#endif