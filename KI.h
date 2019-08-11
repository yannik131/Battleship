#ifndef KI_H_INCLUDED
#define KI_H_INCLUDED

#include "Player.h"

#include <stack>
#include <vector>

class KI
{
private:
	enum HitCategory
	{
		Success,
		Failure,
		Pending
	};
	struct Hit
	{
		Hit() :
			category(HitCategory::Pending), shipCategory(Category::None), partOfDestroyedShip(false), shipNumber(0)
		{ }
		HitCategory category;
		unsigned	shipCategory;
		bool	 	partOfDestroyedShip;
		int			shipNumber;
	};

private:
	Player&										mPlayer;
	std::stack<unsigned> 						mShipStack;
	std::vector<sf::Vector2i> 					mPossiblePositions;
	std::vector<std::vector<Hit>>				mHitGrid;
	std::vector<std::vector<int>>				mPriorityGrid;
	unsigned									mPlayerCategory;
	const unsigned int                          EMPTY_FIELD_BONUS = 1;
	const unsigned int                          NEAR_SUCCESS_BONUS = 4;
	const unsigned int                          CONNECTED_BONUS = 6;

public:
							KI(Player& player);
	void					placeShip();
	bool					ready() const;
	void					reset();
	void					makeMove();

private:
	void					createShip(unsigned shipCategory);
	void					fillStack();
	void					fillPossiblePositions();
	bool					validField(int x, int y) const;
	void					addFieldToAttack(const sf::Vector2i& field);
	void					calculateNextMove();
	void 					findDestroyedShips();
};

#endif // KI_H_INCLUDED
