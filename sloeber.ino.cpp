#ifdef __IN_ECLIPSE__
//This is a automatic generated file
//Please do not modify this file
//If you touch this file your change will be overwritten during the next build
//This file has been generated on 2021-01-16 23:55:23

#include "Arduino.h"
#include <Arduino_FreeRTOS.h>
#include <semphr.h>

int serial_putchar(char c, FILE* f) ;
static void lock(int i, UBaseType_t pr, UBaseType_t tsk);
static void unlock(int i, UBaseType_t tsk);
static void work(int x);
void setup() ;
void loop() ;
void TaskBlink0(void *pvParameters) ;
void TaskBlink1(void *pvParameters) ;
void TaskBlink2(void *pvParameters) ;


#include "PCP-FreeRTOS.ino"

#endif
