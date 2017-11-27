/*********************************************************************
*                                                                    *
*                SEGGER Microcontroller GmbH & Co. KG                *
*        Solutions for real time microcontroller applications        *
*                                                                    *
**********************************************************************
*                                                                    *
* C-file generated by:                                               *
*                                                                    *
*        GUI_Builder for emWin version 5.40                          *
*        Compiled Jun 22 2017, 10:13:26                              *
*        (c) 2017 Segger Microcontroller GmbH & Co. KG               *
*                                                                    *
**********************************************************************
*                                                                    *
*        Internet: www.segger.com  Support: support@segger.com       *
*                                                                    *
**********************************************************************
*/

// USER START (Optionally insert additional includes)
// USER END



/*********************************************************************
*
*       Defines
*
**********************************************************************
*/
#define ID_WINDOW_0 (GUI_ID_USER + 0x00)
#define ID_LISTVIEW_0 (GUI_ID_USER + 0x01)
#define ID_IMAGE_0 (GUI_ID_USER + 0x02)
#define ID_LISTVIEW_1 (GUI_ID_USER + 0x03)

#define ID_IMAGE_0_IMAGE_0  0x00
#include "DIALOG.h"
#include "string"
#include "FlightCotrolView.h"
// USER START (Optionally insert additional defines)
// USER END

/*********************************************************************
*
*       Static data
*
**********************************************************************
*/
/*********************************************************************
*
*       _acImage_0, "C:\Users\Karol\Desktop\radar2test.jpg", ID_IMAGE_0_IMAGE_0
*/

#define xCenter 307
#define yCenter 301
#define yOffset 300
WM_HWIN aircraftsLitView;
WM_HWIN radarImage;
WM_HWIN radar;
std::list<AircraftRecord>* pAircrafts = NULL;
//extern const U8 _acImage_0[76390];

/*
GUI_CONST_STORAGE GUI_BITMAP bmbig_airplain = {
  120, // xSize
  120, // ySize
  480, // BytesPerLine
  32, // BitsPerPixel
  (unsigned char *)_acbig_airplain,  // Pointer to picture data
  NULL,  // Pointer to palette
  GUI_DRAW_BMP8888
};
*/
char b[64];
int count = 0;

void DisplayAircraft(const AircraftRecord& record)
{
	const float NmPerPixel = 180.0F/520.0;
	const float latRef = 51.253777F;
	const float lonRef = 15.395414F;
	float latDiff = (latRef - record.GetLat()) * 60.0F;
	float lonDiff = (lonRef - record.GetLon()) * -60.0F;

	int x = lonDiff/NmPerPixel + xCenter;
	int y = latDiff/NmPerPixel + yCenter;

	GUI_DrawCircle(x,y ,3);
	GUI_DispStringAt(record.GetICAO_Address().c_str(), x, y - 10 );

}
void cbImage(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId){

	  case WM_PAINT:
	  {

		//IMAGE_Callback(pMsg);

		GUI_SetColor(0x3ae635);
		GUI_SetBkColor(GUI_BLACK);

		GUI_DrawCircle(307,301,1);
		GUI_DrawCircle(307,301,88);
		GUI_DrawCircle(307,301,88*2);
		GUI_DrawCircle(307,301,88*3);

		sprintf(b,"15 Nm");
		GUI_DispStringAt(b, 365, 297);
		sprintf(b,"30 Nm");
		GUI_DispStringAt(b, 457, 297);
		sprintf(b,"45 Nm");
		GUI_DispStringAt(b, 548, 297);
		if(pAircrafts != NULL)
			{
				for(auto it = pAircrafts->begin(); it != pAircrafts->end(); it++)
				{
					if(it->altitudeKnown == true)
					{
						DisplayAircraft(*it);
					}
				}
			}
	  }
	    break;
	  default:
	    //
	    // Any messages not handled in this callback are passed to the default callback function
	    //
	    WM_DefaultProc(pMsg);
	    break;
	  }
}


void cbWin(WM_MESSAGE * pMsg)
{
	switch (pMsg->MsgId){

	  case WM_PAINT:
	  {
	    //
	    // Handle the default drawing by the default callback
		GUI_SetColor(GUI_TRANSPARENT);
		GUI_FillRect(0,0,620,600);
		GUI_SetColor(0x3ae635);
		GUI_SetBkColor(GUI_BLACK);

		GUI_DrawCircle(307,301,1);
		if(pAircrafts != NULL)
		{
			for(auto it = pAircrafts->begin(); it != pAircrafts->end(); it++)
			{
				if(it->altitudeKnown == true)
				{
					DisplayAircraft(*it);
				}
			}
		}
	  }
	    break;
	  default:
	    //
	    // Any messages not handled in this callback are passed to the default callback function
	    //
	    WM_DefaultProc(pMsg);
	    break;
	  }
}

// USER START (Optionally insert additional static data)
// USER END

/*********************************************************************
*
*       _aDialogCreate
*/
static const GUI_WIDGET_CREATE_INFO _aDialogCreate[] = {
  { WINDOW_CreateIndirect, "Window", ID_WINDOW_0, 0, 0, 1024, 600, 0, 0x0, 0 },
  { LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_0, 620, 0, 404, 900, 0, 0x0, 0 },
  //{ LISTVIEW_CreateIndirect, "Listview", ID_LISTVIEW_1, 620, 290, 404, 600, 0, 0x0, 0 },
  { IMAGE_CreateIndirect, "Image", ID_IMAGE_0, 0, 0, 620, 600, 0, 0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

static const GUI_WIDGET_CREATE_INFO _aDialogCreate2[] = {
  //{ WINDOW_CreateIndirect, "Window", ID_WINDOW_1, 0, 0, 1024, 600, WM_CF_HASTRANS, 0x0, 0 },
 // { IMAGE_CreateIndirect, "Image", ID_IMAGE_1, 0, 0, 620, 600, 0, 0, 0 },
  // USER START (Optionally insert additional widgets)
  // USER END
};

/*********************************************************************
*
*       Static code
*
**********************************************************************
*/
/*********************************************************************
*
*       _GetImageById
*/
static const void * _GetImageById(U32 Id, U32 * pSize) {
  switch (Id) {
  case ID_IMAGE_0_IMAGE_0:;
  //  *pSize = sizeof(_acImage_0);
   // return (const void *)_acImage_0;
  }
  return NULL;
}

// USER START (Optionally insert additional static code)
// USER END

/*********************************************************************
*
*       _cbDialog
*/

#include "MESSAGEBOX.h"
WM_HWIN hWin0;
void DialogCallback(WM_MESSAGE * pMsg)  {
  const void * pData;
  WM_HWIN      hItem;
  U32          FileSize;
  int          NCode;
  int          Id;
  // USER START (Optionally insert additional variables)
  // USER END

  switch (pMsg->MsgId) {
  case WM_INIT_DIALOG:
  {
    //
    // Initialization of 'Listview'
    //

	hItem = pMsg->hWin;
	WINDOW_SetBkColor(hItem, GUI_MAKE_COLOR(0x00000000));


    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_0);
    aircraftsLitView = hItem;
    LISTVIEW_SetBkColor(aircraftsLitView,LISTVIEW_CI_UNSEL,0x000000);
    LISTVIEW_SetTextColor(aircraftsLitView,LISTVIEW_CI_UNSEL,0x3ae635);
    HEADER_SetTextColor(LISTVIEW_GetHeader(aircraftsLitView),0x3ae635);
    HEADER_SetBkColor(LISTVIEW_GetHeader(aircraftsLitView),0x000000);


    LISTVIEW_AddColumn(hItem, 80, "ICAO Address", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_SetGridVis(hItem, 1);
    LISTVIEW_AddColumn(hItem, 60, "Altitide", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 104, "Position", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 50, "Velocity", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 50, "Heading", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 60, "Flight name", GUI_TA_HCENTER | GUI_TA_VCENTER);
/*
    hItem = WM_GetDialogItem(pMsg->hWin, ID_LISTVIEW_1);
    LISTVIEW_AddColumn(hItem, 80, "ICAO Address", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_SetGridVis(hItem, 1);
    LISTVIEW_AddColumn(hItem, 60, "Altitide", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 104, "Position", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 50, "Velocity", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 50, "Heading", GUI_TA_HCENTER | GUI_TA_VCENTER);
    LISTVIEW_AddColumn(hItem, 60, "Flight name", GUI_TA_HCENTER | GUI_TA_VCENTER);
*/



    hItem = WM_GetDialogItem(pMsg->hWin, ID_IMAGE_0);
    radarImage = hItem;
    WM_SetCallback(radarImage,cbImage);





    /*WM_HWIN hBox = MESSAGEBOX_Create("Closes in a\n"
                           	"few seconds...",
                           	"Message", GUI_MESSAGEBOX_CF_MODAL);*/

    // USER END
  }
    break;
  case WM_NOTIFY_PARENT:
    Id    = WM_GetId(pMsg->hWinSrc);
    NCode = pMsg->Data.v;
    switch(Id) {
    case ID_LISTVIEW_0: // Notifications sent by 'Listview'
      switch(NCode) {
      case WM_NOTIFICATION_CLICKED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_RELEASED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      case WM_NOTIFICATION_SEL_CHANGED:
        // USER START (Optionally insert code for reacting on notification message)
        // USER END
        break;
      // USER START (Optionally insert additional code for further notification handling)
      // USER END
      }
      break;
    // USER START (Optionally insert additional code for further Ids)
    // USER END
    }
    break;
  // USER START (Optionally insert additional message handling)
  // USER END
  default:
    WM_DefaultProc(pMsg);
    break;
  }
}





WM_HWIN CreateWindow(void) {
	  FRAMEWIN_SetDefaultSkinClassic();
	  HEADER_SetDefaultSkinClassic();

  WM_HWIN hWin;
 // WM_MULTIBUF_Enable(1);
  hWin = GUI_CreateDialogBox(_aDialogCreate, GUI_COUNTOF(_aDialogCreate), DialogCallback, WM_GetDesktopWindowEx(0), 0, 0);
  //hWin0 = GUI_CreateDialogBox(_aDialogCreate2, GUI_COUNTOF(_aDialogCreate2), DialogCallback2, WM_GetDesktopWindowEx(1), 0, 0);

  //hWin0 = WM_CreateWindowAsChild( 0, 300, 1024, 900,WM_GetDesktopWindowEx(0), WM_CF_SHOW | WM_CF_HASTRANS, cbWin, 0);
  return hWin;
}

// USER START (Optionally insert additional public code)
// USER END

/*************************** End of file ****************************/


FlightCotrolView::FlightCotrolView()
{
	rows = 0;

}

void FlightCotrolView::Init()
{
	GUI_Init();
	CreateWindow();


}


void FlightCotrolView::Update(const std::list<AircraftRecord>& aircrafts)
{
	LISTVIEW_DeleteAllRows(aircraftsLitView);
	uint16_t cur_row = 0;
	for(auto it = aircrafts.begin(); it != aircrafts.end(); it++)
	{
		const AircraftRecord& currentRecord = *it;
		GUI_ConstString ICAO_AsCString = currentRecord.GetICAO_Address().c_str();
		LISTVIEW_AddRow(aircraftsLitView,&ICAO_AsCString);
		for(uint8_t i = 1; i < 5; i++)
		{
			LISTVIEW_SetItemText(aircraftsLitView,i,cur_row,"");
		}

		if(currentRecord.altitudeKnown == true)
		{
			LISTVIEW_SetItemText(aircraftsLitView,1,cur_row,currentRecord.altStr.c_str());
			LISTVIEW_SetItemText(aircraftsLitView,2,cur_row,currentRecord.positionStr.c_str());
		}

		if(currentRecord.velocityAndHeadingKnown == true)
		{
			LISTVIEW_SetItemText(aircraftsLitView,3,cur_row,currentRecord.velocityStr.c_str());
			LISTVIEW_SetItemText(aircraftsLitView,4,cur_row,currentRecord.headStr.c_str());

		}
		if(currentRecord.flightNameKnown == true)
		{
			LISTVIEW_SetItemText(aircraftsLitView,5,cur_row,currentRecord.GetFlightName().c_str());
		}
		cur_row++;
	}

	pAircrafts = &(const_cast<std::list<AircraftRecord>&>(aircrafts));

}

void FlightCotrolView::UpdateRadar()
{
	GUI_RECT rec = { 0,0 , 620, 600};
	WM_InvalidateRect(radarImage,&rec);
	/*
	WM_MESSAGE msg;
	msg.MsgId = WM_PAINT;
	//pAircrafts = &const_cast<std::list<AircraftRecord>&>(aircrafts);
	WM_SendMessage(radarImage,&msg);
	*/
}