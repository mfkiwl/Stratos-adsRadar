/*********************************************************************
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
**********************************************************************
*                                                                    *
*        (c) 1996 - 2017  SEGGER Microcontroller GmbH & Co. KG       *
*                                                                    *
*        Internet: www.segger.com    Support:  support@segger.com    *
*                                                                    *
**********************************************************************

** emWin V5.40 - Graphical user interface for embedded applications **
All  Intellectual Property rights  in the Software belongs to  SEGGER.
emWin is protected by  international copyright laws.  Knowledge of the
source code may not be used to write a similar product.  This file may
only be used in accordance with the following terms:

The  software has  been licensed  to STMicroelectronics International
N.V. a Dutch company with a Swiss branch and its headquarters in Plan-
les-Ouates, Geneva, 39 Chemin du Champ des Filles, Switzerland for the
purposes of creating libraries for ARM Cortex-M-based 32-bit microcon_
troller products commercialized by Licensee only, sublicensed and dis_
tributed under the terms and conditions of the End User License Agree_
ment supplied by STMicroelectronics International N.V.
Full source code is available at: www.segger.com

We appreciate your understanding and fairness.
----------------------------------------------------------------------
File        : LCDConf_Lin_Template.c
Purpose     : Display controller configuration (single layer)
---------------------------END-OF-HEADER------------------------------
*/

/**
  ******************************************************************************
  * @attention
  *
  * Licensed under MCD-ST Liberty SW License Agreement V2, (the "License");
  * You may not use this file except in compliance with the License.
  * You may obtain a copy of the License at:
  *
  *        http://www.st.com/software_license_agreement_liberty_v2
  *
  * Unless required by applicable law or agreed to in writing, software 
  * distributed under the License is distributed on an "AS IS" BASIS, 
  * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
  * See the License for the specific language governing permissions and
  * limitations under the License.
  *
  ******************************************************************************
  */

#include "GUI.h"
#include "GUIDRV_Lin.h"
#include "stm32f767xx.h"
#include "stm32f7xx_hal.h"
#include "string.h"
/*********************************************************************
*
*       Layer configuration (to be modified)
*
**********************************************************************
*/
//
// Physical display size
//
#define XSIZE_PHYS 1024
#define YSIZE_PHYS 600

static uint32_t pending_buffer_index = 0;
static uint8_t bufferChanged = 0;
static int32_t pending_buffer = 0;
void * pppData;
//
// Color conversion
//

//
// Display driver
//

//
// Buffers / VScreens
//
#define NUM_BUFFERS  2 // Number of multiple buffers to be used
#define NUM_VSCREENS 1 // Number of virtual screens to be used

/*********************************************************************
*
*       Configuration checking
*
**********************************************************************
*/
#ifndef VRAM_ADDR
  #define VRAM_ADDR 0xD0000000 // TBD by customer: This has to be the frame buffer start address
#endif
#ifndef   XSIZE_PHYS
  #error Physical X size of display is not defined!
#endif
#ifndef   YSIZE_PHYS
  #error Physical Y size of display is not defined!
#endif

#ifndef   NUM_VSCREENS
  #define NUM_VSCREENS 1
#else
  #if (NUM_VSCREENS <= 0)
    #error At least one screeen needs to be defined!
  #endif
#endif
#if (NUM_VSCREENS > 1) && (NUM_BUFFERS > 1)
  #error Virtual screens and multiple buffers are not allowed!
#endif

/*********************************************************************
*
*       Public code
*
**********************************************************************
*/
/*********************************************************************
*
*       LCD_X_Config
*
* Purpose:
*   Called during the initialization process in order to set up the
*   display driver configuration.
*   
*/
//#define FRAME_BUFFER_ADDRESS_LAYER_1 0xD0000000
//#define FRAME_BUFFER_ADDRESS_LAYER_0 0xD0000000 + (1024*600/2)

static void _CopyBuffer(int LayerIndex, int IndexSrc, int IndexDst)
{
	unsigned long BufferSize, AddrSrc, AddrDst;
	//
	// Calculate the size of one frame buffer
	//
	BufferSize = (XSIZE_PHYS * YSIZE_PHYS * 16) / 8;
	//
	// Calculate source- and destination address
	//
	AddrSrc = VRAM_ADDR + BufferSize * IndexSrc;
	AddrDst = VRAM_ADDR + BufferSize * IndexDst;
	memcpy((void *)AddrDst, (void *)AddrSrc, BufferSize);
}

void LCD_X_Config(void) {

 // At first initialize use of multiple buffers on demand
 //
 #if (NUM_BUFFERS > 1)
   GUI_MULTIBUF_Config(NUM_BUFFERS);
 #endif
 //
 // Set display driver and color conversion for 1st layer
 //
 GUI_DEVICE_CreateAndLink(GUIDRV_LIN_16, GUICC_M1555I, 0, 0);
 LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER, (void (*)())_CopyBuffer);
 //
 // Display driver configuration, required for Lin-driver
 //
 if (LCD_GetSwapXY()) {
   LCD_SetSizeEx (0, YSIZE_PHYS, XSIZE_PHYS);
   LCD_SetVSizeEx(0, YSIZE_PHYS * NUM_VSCREENS, XSIZE_PHYS);
 } else {
   LCD_SetSizeEx (0, XSIZE_PHYS, YSIZE_PHYS);
   LCD_SetVSizeEx(0, XSIZE_PHYS, YSIZE_PHYS * NUM_VSCREENS);
 }
 LCD_SetVRAMAddrEx(0, (void *)VRAM_ADDR);
 //
 // Set user palette data (only required if no fixed palette is used)
 //
 #if defined(PALETTE)
   LCD_SetLUTEx(0, PALETTE);
 #endif
	//LCD_SetDevFunc(0, LCD_DEVFUNC_COPYBUFFER,(void (*)(void)) CUSTOM_CopyBuffer);
	//LCD_SetDevFunc(0, LCD_DEVFUNC_COPYRECT,(void (*)(void)) CUSTOM_CopyRect);
//	LCD_SetDevFunc(0, LCD_DEVFUNC_DRAWBMP_32BPP,(void (*)(void)) CUSTOM_DrawBitmap32bpp);
 //
 // Set custom functions for several operations to optimize native processes
 //
}

/*********************************************************************
*
*       LCD_X_DisplayDriver
*
* Purpose:
*   This function is called by the display driver for several purposes.
*   To support the according task the routine needs to be adapted to
*   the display controller. Please note that the commands marked with
*   'optional' are not cogently required and should only be adapted if 
*   the display controller supports these features.
*
* Parameter:
*   LayerIndex - Index of layer to be configured
*   Cmd        - Please refer to the details in the switch statement below
*   pData      - Pointer to a LCD_X_DATA structure
*
* Return Value:
*   < -1 - Error
*     -1 - Command not handled
*      0 - Ok
*/
int LCD_X_DisplayDriver(unsigned LayerIndex, unsigned Cmd, void * pData) {
  int r;

  switch (Cmd) {
  case LCD_X_INITCONTROLLER: {
    //
    // Called during the initialization process in order to set up the
    // display controller and put it into operation. If the display
    // controller is not initialized by any external routine this needs
    // to be adapted by the customer...
    //
    // ...
    return 0;
  }
  case LCD_X_SETVRAMADDR: {
    //
    // Required for setting the address of the video RAM for drivers
    // with memory mapped video RAM which is passed in the 'pVRAM' element of p
    //
    LCD_X_SETVRAMADDR_INFO * p;
    p = (LCD_X_SETVRAMADDR_INFO *)pData;
    //...
    return 0;
  }
  case LCD_X_SETORG: {
    //
    // Required for setting the display origin which is passed in the 'xPos' and 'yPos' element of p
    //
    LCD_X_SETORG_INFO * p;
    p = (LCD_X_SETORG_INFO *)pData;
    //...
    return 0;
  }
  case LCD_X_SHOWBUFFER: {
    //
    // Required if multiple buffers are used. The 'Index' element of p contains the buffer index.

    LCD_X_SHOWBUFFER_INFO * p;
    p = (LCD_X_SHOWBUFFER_INFO *)pData;
    //
    pending_buffer = p->Index;
    pppData = pData;
    return 0;
  }
  case LCD_X_SETLUTENTRY: {
    //
    // Required for setting a lookup table entry which is passed in the 'Pos' and 'Color' element of p
    //
    LCD_X_SETLUTENTRY_INFO * p;
    p = (LCD_X_SETLUTENTRY_INFO *)pData;
    //...
    return 0;
  }
  case LCD_X_ON: {
    //
    // Required if the display controller should support switching on and off
    //
    return 0;
  }
  case LCD_X_OFF: {
    //
    // Required if the display controller should support switching on and off
    //
    // ...
    return 0;
  }
  default:
    r = -1;
  }
  return r;
}

/*************************** End of file ****************************/



void HAL_LTDC_LineEvenCallback(LTDC_HandleTypeDef *hltdc) {

	if(pending_buffer >= 0)
	{
		unsigned long BufferSize, Addr;
		BufferSize = (XSIZE_PHYS * YSIZE_PHYS * 16) / 8;
		Addr = VRAM_ADDR + BufferSize * pending_buffer;
		__HAL_LTDC_LAYER(hltdc, 1)->CFBAR = Addr;
		__HAL_LTDC_RELOAD_CONFIG(hltdc);
		GUI_MULTIBUF_Confirm(pending_buffer);
		pending_buffer = -1;
	}
	/*
	if(bufferChanged == 1)
	{
		bufferChanged = 0;
		if(pending_buffer_index == 0)
		{
			U32 Addr = FRAME_BUFFER_ADDRESS_LAYER_0;
			__HAL_LTDC_LAYER(hltdc, 0)->CFBAR = Addr;
			__HAL_LTDC_RELOAD_CONFIG(hltdc);

		   GUI_MULTIBUF_ConfirmEx(0, pending_buffer_index);
		}
		else if(pending_buffer_index == 1)
		{
			U32 Addr = FRAME_BUFFER_ADDRESS_LAYER_1;
			__HAL_LTDC_LAYER(hltdc, 0)->CFBAR = Addr;
			__HAL_LTDC_RELOAD_CONFIG(hltdc);

		   GUI_MULTIBUF_ConfirmEx(0, pending_buffer_index);

		}
	}
	*/
	HAL_LTDC_ProgramLineEvent(hltdc, 0);
}
