/******************************* (C) LiteMesh **********************************
 * @module  Console
 * @file    portConsole.c
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   Port functions console lib
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#include "stm32f4xx.h" 
#include "port_microrl.h"
#include "microrl.h"
#include "config.h"
//#include "gpio_io.h"
//#include <stdio.h>
#ifdef OS_TNKERNEL
#include "printf.h"
#endif
#include <stdlib.h>
#include <string.h>

//#include "dac.h"

microrl_t rl;
microrl_t *prl = &rl;

/*
AVR platform specific implementation routines (for Atmega8, rewrite for your MC)
*/
#define _AVR_DEMO_VER "1.0"

// definition commands word
#define _CMD_HELP        "help"
#define _CMD_CLEAR       "clear"
#define _CMD_CLR         "clear_port"
#define _CMD_SET         "set_port"
#define _CMD_PPS_ON      "pps_on"     // Включить ИИП [pps_on]
#define _CMD_PPS_OFF     "pps_off"    // Отключить ИИП [pps_off]
#define _CMD_CON_TOGGLE  "con_tog"    // Переключить контактор
#define _CMD_PPS_SET     "pps"        // Устанавливает напряжение на источнике
#define _CMD_DAC_SET     "dac"        // Устанавливает напряжение на DAC
#define _CMD_LOAD_OFF    "load_off"
#define _CMD_LOAD_ON     "load_on"

// arguments for set/clear
#define _SCMD_PB  "port_b"
#define _SCMD_PD  "port_d"

#define _NUM_OF_CMD 
#define _NUM_OF_SETCLEAR_SCMD 2

//available  commands
char * keyworld [] = {
  _CMD_HELP,
  _CMD_CLEAR,
  _CMD_SET,
  _CMD_CLR,
  _CMD_PPS_ON,
  _CMD_PPS_OFF,
  _CMD_CON_TOGGLE,
  _CMD_PPS_SET,
  _CMD_DAC_SET,
  _CMD_LOAD_OFF,
  _CMD_LOAD_ON,
};

// 'set/clear' command argements
char * set_clear_key [] = {_SCMD_PB, _SCMD_PD};

// array for comletion
char * compl_world [_NUM_OF_CMD + 1];

/**
  * @brief  
  * @retval 
  */
void MICRORL_Init(void)
{
  microrl_init(prl, print);
  microrl_set_execute_callback (prl, execute);
  
}

/**
  * @brief  Print callback for microrl library
  * @retval 
  */
void print (const char *str)
{
  printf(str);
}

/**
  * @brief  
  * @retval 
  */
void MICRORL_GetChar(uint8_t ch)
{
  microrl_insert_char(prl, ch);
}

//*****************************************************************************
// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
int execute (int argc, const char * const * argv)
{
	int i = 0;
	// just iterate through argv word and compare it with your commands
	while (i < argc) {
		if (strcmp (argv[i], _CMD_HELP) == 0) 
        {
 		  print ("microrl library v");
		  print (MICRORL_LIB_VER);
		  print ("\n\r");
		  print_help ();        // print help
		} 
        else if (strcmp (argv[i], _CMD_CLEAR) == 0) 
        {
		  print ("\033[2J");    // ESC seq for clear entire screen
		  print ("\033[H");     // ESC seq for move cursor at left-top corner
		}  
                
        /*  Подключить импульсный источник питания */
        else if (strcmp (argv[i], _CMD_PPS_ON) == 0) 
        {
          //IO_IpsOn();
          print("\n\r IPS On\n\r");
		  return 0;
		}
                
        /* Отключить импульсный источник питания */
        else if (strcmp (argv[i], _CMD_PPS_OFF) == 0) 
        {
          //IO_IpsOff();
		  print("\n\r IPS Off\n\r");
		  return 0;
		}
         
		/* Отк/подкл аккумуляторов от источника питания*/
		else if (strcmp (argv[i], _CMD_CON_TOGGLE) == 0) 
        {
          //IO_BatToSupplytogle();
		  return 0;
		}		
		
		/* установить новое значение напряжения на источнике */
		else if (strcmp (argv[i], _CMD_PPS_SET) == 0) 
        {
		  float value = 0.0;
          if (++i < argc)
          {
            value = atof(argv[i]);
            //DAC_SetNewUpps(value);
            print("\n\r");
		    return 0;
          }
		}
		
		/* установить новое значение напряжения на DAC */
		else if (strcmp (argv[i], _CMD_DAC_SET) == 0) 
        {
		  float value = 0.0;
          if (++i < argc)
          {
            value = atof(argv[i]);
            //DAC_SetDesireVoltage(value);
            print("\n\r");
		    return 0;
          }
		}
        
        /*  Подключить нагрузку */
        else if (strcmp (argv[i], _CMD_LOAD_ON) == 0) 
        {
          //IO_LoadOn();
          print("\n\r Load On\n\r");
		  return 0;
		}
                
        /* Отключить нагрузку */
        else if (strcmp (argv[i], _CMD_LOAD_OFF) == 0) 
        {
          //IO_LoadOff();
		  print("\n\r Load Off\n\r");
		  return 0;
		}
        
        else 
        {
		  print ("command: '");
		  print ((char*)argv[i]);
		  print ("' Not found.\n\r");
		}
		i++;
	}
	return 0;
}

void print_help (void)
{
	print ("Use TAB key for completion\n\rCommand:\n\r");
	print ("\tclear               - clear screen\n\r");
        print ("\tdac                 - send test value\n\r");
}

/******************************* (C) LiteMesh *********************************/
