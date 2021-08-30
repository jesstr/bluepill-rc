/**
  ******************************************************************************
  * @file    usb_endp.c
  * @author  MCD Application Team
  * @version V4.1.0
  * @date    26-May-2017
  * @brief   Endpoint routines
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2017 STMicroelectronics</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of STMicroelectronics nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */


/* Includes ------------------------------------------------------------------*/
#include "usb_lib.h"
#include "usb_desc.h"
#include "usb_mem.h"
#include "hw_config.h"
#include "usb_istr.h"
#include "usb_pwr.h"

#include "ring_buf.h"
#include "tinystdio.h"

#include "FreeRTOS.h"
#include "semphr.h"

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
extern SemaphoreHandle_t vcp_rx_sem;
extern rbuf_t vcp_rx_buf;
extern rbuf_t vcp_tx_buf;

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/
/*******************************************************************************
* Function Name  : SOF_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void SOF_Callback(void)
{
    static uint8_t buf[VIRTUAL_COM_PORT_DATA_SIZE];
    uint8_t len = 0;

    while (rbuf_get(&vcp_tx_buf, &buf[len]) &&
        (len++ < VIRTUAL_COM_PORT_DATA_SIZE)) {
        ;
    }
    if (len > 0) {
        UserToPMABufferCopy((unsigned char *)buf, ENDP1_TXADDR, len);
        SetEPTxCount(ENDP1, len);
        SetEPTxValid(ENDP1);
    }
}

/*******************************************************************************
* Function Name  : EP1_IN_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/

void EP1_IN_Callback(void)
{
}

/*******************************************************************************
* Function Name  : EP3_OUT_Callback
* Description    :
* Input          : None.
* Output         : None.
* Return         : None.
*******************************************************************************/
void EP3_OUT_Callback(void)
{
    uint8_t buf[VIRTUAL_COM_PORT_DATA_SIZE];
    uint8_t *pbuf = buf;
    static BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    uint32_t len = GetEPRxCount(ENDP3);
    len = len > VIRTUAL_COM_PORT_DATA_SIZE ?
        VIRTUAL_COM_PORT_DATA_SIZE : len;

    if (len) {
        PMAToUserBufferCopy((unsigned char *)buf, ENDP3_RXADDR, len);
        while (len-- && rbuf_put(&vcp_rx_buf, pbuf++)) {
            ;
        }
        xSemaphoreGiveFromISR(vcp_rx_sem, &xHigherPriorityTaskWoken);
    }
}

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/