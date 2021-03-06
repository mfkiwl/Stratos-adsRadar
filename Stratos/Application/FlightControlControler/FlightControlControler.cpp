/*
 * FlightControlControler.cpp
 *
 *  Created on: 30.10.2017
 *      Author: Karol
 */

#include "FlightControlControler.h"
#include "stm32f7xx_hal.h"

FlightControlControler::FlightControlControler(FlightControl& model,FlightCotrolView& view) : model(model), view(view)
{
	modelChanged = false;
}

void FlightControlControler::NotifyDisconnected()
{
	view.ShowWarningMsg();
}

void FlightControlControler::NotifyConnected()
{
	view.HideWarningMsg();
}

void FlightControlControler::PassNewMessage(const ADS_BMessage& msg)
{
	std::string ICAO_Address = GetICAO_AddresAsString(msg);
	bool recordExist = false;
	std::list<AircraftRecord>::iterator recordIt;

	if(!msg.crcok)
	{
		//view.UpdateStats(msg);
		return;
	}
	recordExist = model.FindAircraftByICAO_Address(ICAO_Address,recordIt);

	if(recordExist == false)
	{
		AircraftRecord newRecord = AircraftRecord(ICAO_Address);
		UpdateRecord(msg,newRecord);
		model.AddRecord(newRecord);

	}
	else
	{
		UpdateRecord(msg,*recordIt);
	}
	//view.UpdateStats(msg);
	modelChanged = true;
}

void FlightControlControler::UpdateRecord(const ADS_BMessage& msg, AircraftRecord& record)
{
	 switch(msg.msgtype)
	 {
	 case DF17:

		 if (msg.metype >= 1 && msg.metype <= 4)
		 {
			 record.SetFlightName(msg.flight);
		 }
		 else if(msg.metype >= 9 && msg.metype <= 18)
		 {
			 record.SetAltitude(msg.altitude);
			 record.decodeCPR(msg.fflag,msg.raw_latitude,msg.raw_longitude);

		 }
		 else if(msg.metype == 19 && msg.mesub >= 1 && msg.mesub <= 4)
		 {
			 record.SetVelocityAndHeading(msg.velocity,msg.heading);
		 }
		 break;
	 }
}

std::string FlightControlControler::GetICAO_AddresAsString(const ADS_BMessage& msg)
{
	char buff[9];
	std::sprintf(buff,"%.2X%.2X%.2X",msg.aa1,msg.aa2,msg.aa3);
	return std::string(buff);
}

void FlightControlControler::UpdateTicksCount(uint32_t ticks)
{
	if(model.TickAllRecords(ticks) == true)
	{
		modelChanged = true;
	}
}

void FlightControlControler::UpdateView()
{
		view.Update(model.GetAllRecords());
		modelChanged = false;
}
