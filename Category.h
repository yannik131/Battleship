#ifndef CATEGORY_H_INCLUDED
#define CATEGORY_H_INCLUDED

#include <string>

namespace Category
{
	enum
	{
		None				= 0,
		Player1 			= 1 << 0,
		Player2				= 1 << 1,
		ShipSmall 			= 1 << 2,
		ShipMedium 			= 1 << 3,
		ShipBig				= 1 << 4,
		ShipGiant			= 1 << 5,
		ShipSourceSmall 	= 1 << 6,
		ShipSourceMedium	= 1 << 7,
		ShipSourceBig		= 1 << 8,
		ShipSourceGiant		= 1 << 9,
		Border				= 1 << 10,
		Destroyed			= 1 << 11
	};
	//std::string toString(const unsigned& category);
}

unsigned getShipTypeFromSource(const unsigned& shipSource);
unsigned getShipTypeFromField(const unsigned& category);

std::string shipCategoryToString(unsigned shipCategory);

#endif // CATEGORY_H_INCLUDED
