/*
 * LTDCDriver.cpp
 *
 *  Created on: 24.10.2017
 *      Author: Karol
 */

#include "LTDCDriver.h"
#define FMC_START_ADDR (0xD0000000)
LTDCDriver::LTDCDriver(	uint16_t screenWidth,
						uint16_t screenHeight,
						uint16_t vsync,
						uint16_t hsync,
						uint16_t horizontalFP,
						uint16_t horizontalBP,
						uint16_t verticalFP,
						uint16_t verticalBP	) :

						screenWidth(screenWidth),
						screenHeight(screenHeight),
						vsync(vsync),
						hsync(hsync),
						horizontalFP(horizontalFP),
						horizontalBP(horizontalBP),
						verticalFP(verticalFP),
						verticalBP(verticalBP)


{

	MX_LTDC_Init(screenWidth,
				 screenHeight,
				 vsync,
				 hsync,
				 horizontalFP,
				 horizontalBP,
				 verticalFP,
				 verticalBP);

	LTDC_LayerCfgTypeDef pLayerCfg;
	  pLayerCfg.WindowX0 = 0;
	  pLayerCfg.WindowX1 = 1024;
	  pLayerCfg.WindowY0 = 0;
	  pLayerCfg.WindowY1 = 600;
	  pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB1555;
	  pLayerCfg.Alpha = 255;
	  pLayerCfg.Alpha0 = 0;
	  pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	  pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR1_CA;
	  pLayerCfg.FBStartAdress = FMC_START_ADDR;
	  pLayerCfg.ImageWidth = 1024;
	  pLayerCfg.ImageHeight = 600;
	  pLayerCfg.Backcolor.Blue = 0;
	  pLayerCfg.Backcolor.Green = 0;
	  pLayerCfg.Backcolor.Red = 0;
	  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, 1) != HAL_OK)
	  {
	    _Error_Handler(__FILE__, __LINE__);
	  }
/*
	  LTDC_LayerCfgTypeDef pLayerCfg1;
	  pLayerCfg1.WindowX0 = 0;
	  pLayerCfg1.WindowX1 = 620;
	  pLayerCfg1.WindowY0 = 0;
	  pLayerCfg1.WindowY1 = 600;
	  pLayerCfg1.PixelFormat = LTDC_PIXEL_FORMAT_ARGB1555;
	  pLayerCfg1.Alpha = 255;
	  pLayerCfg1.Alpha0 = 0;
	  pLayerCfg1.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	  pLayerCfg1.BlendingFactor2 = LTDC_BLENDING_FACTOR1_CA;
	  pLayerCfg1.FBStartAdress = FMC_START_ADDR + 1024*600;
	  pLayerCfg1.ImageWidth = 620;
	  pLayerCfg1.ImageHeight = 600;
	  pLayerCfg1.Backcolor.Blue = 0;
	  pLayerCfg1.Backcolor.Green = 0;
	  pLayerCfg1.Backcolor.Red = 0;
		  if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg1, 0) != HAL_OK)
		  {
		    _Error_Handler(__FILE__, __LINE__);
		  }
		  */
}


uint16_t LTDCDriver::GetScreenWidth() const
{
	return screenWidth;
}

uint16_t LTDCDriver::GetScreenHeight() const
{
	return screenHeight;
}

void LTDCDriver::ConfigLayer(uint8_t layerIndex, uint32_t bufferAdress)
{
/*
	LTDC_LayerCfgTypeDef pLayerCfg;
	pLayerCfg.WindowX0 = 0;
	pLayerCfg.WindowX1 = screenWidth;
	pLayerCfg.WindowY0 = 0;
	pLayerCfg.WindowY1 = screenHeight;
	pLayerCfg.PixelFormat = LTDC_PIXEL_FORMAT_ARGB8888;
	pLayerCfg.Alpha = 255;
	pLayerCfg.Alpha0 = 0;
	pLayerCfg.BlendingFactor1 = LTDC_BLENDING_FACTOR1_CA;
	pLayerCfg.BlendingFactor2 = LTDC_BLENDING_FACTOR2_CA;
	pLayerCfg.FBStartAdress = bufferAdress;
	pLayerCfg.ImageWidth = screenWidth;
	pLayerCfg.ImageHeight = screenHeight;
	pLayerCfg.Backcolor.Blue = 0;
	pLayerCfg.Backcolor.Green = 0;
	pLayerCfg.Backcolor.Red = 0;
	if (HAL_LTDC_ConfigLayer(&hltdc, &pLayerCfg, layerIndex) != HAL_OK)
	{
	  _Error_Handler(__FILE__, __LINE__);
	}

*/
}

