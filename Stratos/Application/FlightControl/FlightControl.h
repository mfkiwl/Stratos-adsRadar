/*
 * FlightControlModel.h
 *
 *  Created on: 28.10.2017
 *      Author: Karol
 */

#ifndef FLIGHTCONTROL_FLIGHTCONTROL_H_
#define FLIGHTCONTROL_FLIGHTCONTROL_H_

#include <AircraftRecord.h>
#include <string>
#include <list>

#include "ADSBMessage.h"

class FlightControl
{
public:
	FlightControl();

	bool FindAircraftByICAO_Address(const std::string& ICAO_Address,std::list<AircraftRecord>::iterator& retIt);
	void AddRecord(const AircraftRecord& record);

	bool TickAllRecords(uint32_t ticks);
	const std::list<AircraftRecord>& GetAllRecords() const { return aircrafts;}
private:
	std::list<AircraftRecord> aircrafts;


};

#endif /* FLIGHTCONTROL_FLIGHTCONTROL_H_ */
