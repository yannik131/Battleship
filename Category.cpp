#include "Category.h"

namespace Category
{

	/*std::string toString(const unsigned& category)
	{
		std::string result;
		if(category & Player1)
	}*/
}

unsigned getShipTypeFromSource(const unsigned& shipSource)
{
	if(shipSource & Category::ShipSourceGiant)
		return Category::ShipGiant;
	else if(shipSource & Category::ShipSourceBig)
		return Category::ShipBig;
	else if(shipSource & Category::ShipSourceMedium)
		return Category::ShipMedium;
	else if(shipSource & Category::ShipSourceSmall)
		return Category::ShipSmall;
	return Category::None;
}
unsigned getShipTypeFromField(const unsigned& category)
{
	if(category & Category::ShipGiant)
		return Category::ShipGiant;
	else if(category & Category::ShipBig)
		return Category::ShipBig;
	else if(category & Category::ShipMedium)
		return Category::ShipMedium;
	else if(category & Category::ShipSmall)
		return Category::ShipSmall;
	return Category::None;
}

std::string shipCategoryToString(unsigned shipCategory)
{
	if(shipCategory & Category::ShipSmall)
		return "small ship";
	else if(shipCategory & Category::ShipMedium)
		return "medium ship";
	else if(shipCategory & Category::ShipBig)
		return "big ship";
	else
		return "giant ship";
}
