/*
 * Airplaine.cpp
 *
 *  Created on: 29.10.2017
 *      Author: Karol
 */

#include <AircraftRecord.h>
#include <cmath>

AircraftRecord::AircraftRecord(const std::string& ICAO_Address) : ICAO_Address(ICAO_Address)
{
	ticksToExpire = DEFAULT_LIVE_SPAN;
	recordExpiered = false;
	altitudeKnown = false;
	flightNameKnown = false;
	velocityAndHeadingKnown = false;

}

void AircraftRecord::Tick(uint32_t ticks)
{
	if(ticksToExpire > 0U)
	{
		if(ticksToExpire > ticks)
		{
			ticksToExpire -= ticks;
		}
		else
		{
			ticksToExpire = 0U;
		}
		if(ticksToExpire == 0U)
		{
			recordExpiered = true;
		}

	}
}

void AircraftRecord::SetAltitude(const uint32_t& newAltitude)
{
	char buff[64];
	sprintf(buff,"%lu ft",newAltitude);
	altStr = std::string(buff);
	altitude = newAltitude;
	altitudeKnown = true;
	ticksToExpire = DEFAULT_LIVE_SPAN;
}

void AircraftRecord::SetFlightName(const char* newFlightName)
{
	flightName = std::string(newFlightName);
	flightNameKnown = true;
	ticksToExpire = DEFAULT_LIVE_SPAN;
}

void AircraftRecord::SetVelocityAndHeading(const int& velocity, const float& heading)
{
	char buff[64];
	sprintf(buff,"%d kts",velocity);
	velocityStr = std::string(buff);
	float degHead = heading * 180.0F / M_PI;
	if(degHead < 0.0F)
	{
		degHead += 360.0F;
	}
	sprintf(buff,"%d�",(int)degHead);
	headStr = std::string(buff);
	velocityAndHeadingKnown = true;
	ticksToExpire = DEFAULT_LIVE_SPAN;

	this->velocity = velocity;
	this->heading = heading;
}
void AircraftRecord::decodeCPR(const int& fflag, const int& rawLat, const int& rawLon)
{
	float cprLat = float(rawLat)/131072.0F;
	float cprLon = float(rawLon)/131072.0F;

	double dLat;
	if(fflag)
	{
		dLat = 360.0F / 59.0F;
	}
	else
	{
		dLat = 360.0F / 60.0F;
	}
	uint32_t j = floor(latRef/dLat) + floor( CprMod(latRef,dLat)/dLat - cprLat + 0.5F );

	latitude = dLat * (float(j) + cprLat);

	float dLon;
	float NlLat = cprNLFunction(latitude);
	if( NlLat > 0)
	{
		dLon = 360.0F/ NlLat;
	}
	else
	{
		dLon = 360.0;
	}

	uint32_t m = floor(lonRef/dLon) + floor( CprMod(lonRef,dLon)/dLon - cprLon + 0.5F );
	lognitude = dLon * (float(m) + cprLon);

	char buff[256];
	sprintf(buff,"%.4fN %.4fE",latitude,lognitude);
	positionStr = std::string(buff);
	positionStr.replace(2,1,"�");
	positionStr.insert(5,"'");
	positionStr.insert(8,"\"");

	positionStr.replace(13,1,"�");
	positionStr.insert(16,"'");
	positionStr.insert(19,"\"");
	ticksToExpire = DEFAULT_LIVE_SPAN;

}


void AircraftRecord::CalcNewPosition(int time)
{
	if((velocityAndHeadingKnown == false) || ( altitudeKnown == false) )
		return;
    const float velInMs = velocity * 0.514444444;
	const float dist = velInMs * (float)time;
	const float earthR = 6371000.0F;

	float distRatio = dist / earthR;
	float distRatioSin = std::sin(distRatio);
	float distRatioCos = std::cos(distRatio);

	float startLatRad = latitude * M_PI/180.0F;
	float startLonRad = lognitude * M_PI/180.0F;

	float startLatCos = std::cos(startLatRad);
	float startLatSin = std::sin(startLatRad);

	float endLatRads = std::asin((startLatSin * distRatioCos) + (startLatCos * distRatioSin * std::cos(heading)));
	float endLonRads = startLonRad + std::atan2(std::sin(heading) * distRatioSin * startLatCos,  distRatioCos - startLatSin * std::sin(endLatRads));

	latitude = endLatRads * 180.0F / M_PI;
	lognitude = endLonRads * 180.0F / M_PI;

	char buff[256];
	sprintf(buff,"%.4fN %.4fE",latitude,lognitude);
	positionStr = std::string(buff);
	positionStr.replace(2,1,"�");
	positionStr.insert(5,"'");
	positionStr.insert(8,"\"");

	positionStr.replace(13,1,"�");
	positionStr.insert(16,"'");
	positionStr.insert(19,"\"");

}
float AircraftRecord::CprMod(const float& x, const float& y)
{
	return (x - y * floor(x/y));
}

int AircraftRecord::cprNLFunction(double lat) {
    if (lat < 0) lat = -lat; /* Table is simmetric about the equator. */
    if (lat < 10.47047130) return 59;
    if (lat < 14.82817437) return 58;
    if (lat < 18.18626357) return 57;
    if (lat < 21.02939493) return 56;
    if (lat < 23.54504487) return 55;
    if (lat < 25.82924707) return 54;
    if (lat < 27.93898710) return 53;
    if (lat < 29.91135686) return 52;
    if (lat < 31.77209708) return 51;
    if (lat < 33.53993436) return 50;
    if (lat < 35.22899598) return 49;
    if (lat < 36.85025108) return 48;
    if (lat < 38.41241892) return 47;
    if (lat < 39.92256684) return 46;
    if (lat < 41.38651832) return 45;
    if (lat < 42.80914012) return 44;
    if (lat < 44.19454951) return 43;
    if (lat < 45.54626723) return 42;
    if (lat < 46.86733252) return 41;
    if (lat < 48.16039128) return 40;
    if (lat < 49.42776439) return 39;
    if (lat < 50.67150166) return 38;
    if (lat < 51.89342469) return 37;
    if (lat < 53.09516153) return 36;
    if (lat < 54.27817472) return 35;
    if (lat < 55.44378444) return 34;
    if (lat < 56.59318756) return 33;
    if (lat < 57.72747354) return 32;
    if (lat < 58.84763776) return 31;
    if (lat < 59.95459277) return 30;
    if (lat < 61.04917774) return 29;
    if (lat < 62.13216659) return 28;
    if (lat < 63.20427479) return 27;
    if (lat < 64.26616523) return 26;
    if (lat < 65.31845310) return 25;
    if (lat < 66.36171008) return 24;
    if (lat < 67.39646774) return 23;
    if (lat < 68.42322022) return 22;
    if (lat < 69.44242631) return 21;
    if (lat < 70.45451075) return 20;
    if (lat < 71.45986473) return 19;
    if (lat < 72.45884545) return 18;
    if (lat < 73.45177442) return 17;
    if (lat < 74.43893416) return 16;
    if (lat < 75.42056257) return 15;
    if (lat < 76.39684391) return 14;
    if (lat < 77.36789461) return 13;
    if (lat < 78.33374083) return 12;
    if (lat < 79.29428225) return 11;
    if (lat < 80.24923213) return 10;
    if (lat < 81.19801349) return 9;
    if (lat < 82.13956981) return 8;
    if (lat < 83.07199445) return 7;
    if (lat < 83.99173563) return 6;
    if (lat < 84.89166191) return 5;
    if (lat < 85.75541621) return 4;
    if (lat < 86.53536998) return 3;
    if (lat < 87.00000000) return 2;
    else return 1;
}

int AircraftRecord::cprNFunction(double lat, int isodd) {
    int nl = cprNLFunction(lat) - isodd;
    if (nl < 1) nl = 1;
    return nl;
}

double AircraftRecord::cprDlonFunction(double lat, int isodd) {
    return 360.0 / cprNFunction(lat, isodd);
}
