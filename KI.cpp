#include "KI.h"
#include "World.h"

#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cmath>

#include <algorithm>
#include <cassert>

KI::KI(Player& player) :
	mPlayer(player),
	mPlayerCategory(Category::Player2)
{
	std::srand(std::time(0));
	reset();
}


void KI::placeShip()
{
	if(!mShipStack.empty())
	{
		createShip(mShipStack.top());
		mShipStack.pop();
	}
}

void KI::createShip(unsigned shipCategory)
{
	static bool rotated;
	rotated = !rotated;
	std::random_shuffle(mPossiblePositions.begin(), mPossiblePositions.end());
	for(const auto& position : mPossiblePositions)
	{
		if (mPlayer.addShip(mPlayerCategory, rotated, shipCategory, position) ||
			mPlayer.addShip(mPlayerCategory, !rotated, shipCategory, position))
			return;
	}
	throw std::logic_error("Ship could not be placed!");
}

bool KI::ready() const
{
	return mShipStack.empty();
}

void KI::reset()
{
	fillStack();
	fillPossiblePositions();
	mHitGrid.clear();
	mPriorityGrid.clear();
	mHitGrid.resize(FIELD_SIZE);
	mPriorityGrid.resize(FIELD_SIZE);
	for(int i = 0; i < FIELD_SIZE; ++i)
	{
		mHitGrid[i].resize(FIELD_SIZE);
		for(int j = 0; j < FIELD_SIZE; ++j)
			mPriorityGrid[i].push_back(0);
	}
	mPlayer.resetField(mPlayerCategory);
}

void KI::fillStack()
{
	mShipStack.push(Category::ShipSmall);
	mShipStack.push(Category::ShipSmall);
	mShipStack.push(Category::ShipMedium);
	mShipStack.push(Category::ShipMedium);
	mShipStack.push(Category::ShipMedium);
	mShipStack.push(Category::ShipBig);
	mShipStack.push(Category::ShipBig);
	mShipStack.push(Category::ShipGiant);
}

void KI::makeMove()
{
	calculateNextMove();
	sf::Vector2i attackPos(mPossiblePositions[0].x - 1, mPossiblePositions[0].y - 1);
	unsigned squareCategory = mPlayer.attackSquare(mPossiblePositions[0], Category::Player1, &mHitGrid[attackPos.y][attackPos.x].shipNumber);
	mPriorityGrid[attackPos.y][attackPos.x] = -10000;
	if(getShipTypeFromField(squareCategory) != Category::None)
	{
		mHitGrid[attackPos.y][attackPos.x].category = HitCategory::Success;
		mHitGrid[attackPos.y][attackPos.x].shipCategory = getShipTypeFromField(squareCategory);
	}
	else
		mHitGrid[attackPos.y][attackPos.x].category = HitCategory::Failure;
	mPossiblePositions.erase(mPossiblePositions.begin());
}

void KI::findDestroyedShips()
{
	std::vector<int> occurrences;
    occurrences.resize(Fleet::DEFAULTSIZE+1, 0);
	for(int i = 0; i < FIELD_SIZE; ++i)
	{
		for(int j = 0; j < FIELD_SIZE; ++j)
		{
			if(mHitGrid[i][j].shipNumber != 0)
				++occurrences[mHitGrid[i][j].shipNumber];
		}
	}

	for(int i = 0; i < FIELD_SIZE; ++i)
	{
		for(int j = 0; j < FIELD_SIZE; ++j)
		{
			if(mHitGrid[i][j].shipNumber != 0)
			{
				if(mHitGrid[i][j].shipCategory & Category::ShipSmall && occurrences[mHitGrid[i][j].shipNumber] == 2)
					mHitGrid[i][j].partOfDestroyedShip = true;
				else if(mHitGrid[i][j].shipCategory & Category::ShipMedium && occurrences[mHitGrid[i][j].shipNumber] == 3)
					mHitGrid[i][j].partOfDestroyedShip = true;
				else if(mHitGrid[i][j].shipCategory & Category::ShipBig && occurrences[mHitGrid[i][j].shipNumber] == 4)
					mHitGrid[i][j].partOfDestroyedShip = true;
				else if(mHitGrid[i][j].shipCategory & Category::ShipGiant && occurrences[mHitGrid[i][j].shipNumber] == 5)
					mHitGrid[i][j].partOfDestroyedShip = true;
			}
		}
	}
}

void KI::addFieldToAttack(const sf::Vector2i& fieldToAttack)
{
    #ifdef DEBUG
    if(std::find(mPossiblePositions.begin(), mPossiblePositions.end(), fieldToAttack) == mPossiblePositions.end())
        throw std::logic_error("Field (" + std::to_string(fieldToAttack.x) + "," + std::to_string(fieldToAttack.y) + ") not in mPossiblePositions!");
    #endif // DEBUG
	std::swap(mPossiblePositions[0], *std::find(mPossiblePositions.begin(), mPossiblePositions.end(), fieldToAttack));
}

void KI::fillPossiblePositions()
{
	mPossiblePositions.clear();
	for(int i = 1; i <= FIELD_SIZE; ++i)
	{
		for(int j = 1; j <= FIELD_SIZE; ++j)
			mPossiblePositions.push_back(sf::Vector2i(i, j));
	}
}

bool KI::validField(int x, int y) const
{
	if(x < 0 || x >= FIELD_SIZE || y < 0 || y >= FIELD_SIZE)
		return false;
	return true;
}

void KI::calculateNextMove()
{
	for(int i = 0; i < FIELD_SIZE; ++i) //reset all priorities (except negative ones, i.e. hit squares)
	{
		for(int j = 0; j < FIELD_SIZE; ++j)
		{
			if(mPriorityGrid[i][j] > 0)
				mPriorityGrid[i][j] = 0;
		}
	}

	findDestroyedShips();

	for(int i = 0; i < FIELD_SIZE; ++i)
	{
		for(int j = 0; j < FIELD_SIZE; ++j)
		{
			if(validField(j - 1, i) && mHitGrid[i][j - 1].category == HitCategory::Pending) //every surrounding square that has not been attacked yet increases priority by 1
				mPriorityGrid[i][j] += EMPTY_FIELD_BONUS;
			if(validField(j + 1, i) && mHitGrid[i][j + 1].category == HitCategory::Pending)
				mPriorityGrid[i][j] += EMPTY_FIELD_BONUS;
			if(validField(j, i - 1) && mHitGrid[i - 1][j].category == HitCategory::Pending)
				mPriorityGrid[i][j] += EMPTY_FIELD_BONUS;
			if(validField(j, i + 1) && mHitGrid[i + 1][j].category == HitCategory::Pending)
				mPriorityGrid[i][j] += EMPTY_FIELD_BONUS;

			if(mHitGrid[i][j].category == HitCategory::Success && !mHitGrid[i][j].partOfDestroyedShip) //if square has been hit and is not part of a destroyed ship, priority
			{																						   //of surrounding squares increases by 4
				if(validField(j - 1, i) && mHitGrid[i][j - 1].category == HitCategory::Pending)
					mPriorityGrid[i][j - 1] += NEAR_SUCCESS_BONUS;
				if(validField(j + 1, i) && mHitGrid[i][j + 1].category == HitCategory::Pending)
					mPriorityGrid[i][j + 1] += NEAR_SUCCESS_BONUS;
				if(validField(j, i - 1) && mHitGrid[i - 1][j].category == HitCategory::Pending)
					mPriorityGrid[i - 1][j] += NEAR_SUCCESS_BONUS;
                if(validField(j, i + 1) && mHitGrid[i + 1][j].category == HitCategory::Pending)
					mPriorityGrid[i + 1][j] += NEAR_SUCCESS_BONUS;

                //if there are two connected hits, priority of the assumed ends of the ship are increased by 4
                if(validField(j, i-1) && mHitGrid[i-1][j].category == HitCategory::Success &&
                   mHitGrid[i][j].shipNumber == mHitGrid[i-1][j].shipNumber) //two vertically connected hits
                {
                    unsigned startX = j, startY = i-1, endY = i-1;
                    while(validField(startY, startX) && mHitGrid[startY][startX].category == HitCategory::Success)
                        --startY;
                    while(validField(endY, startX) && mHitGrid[endY][startX].category == HitCategory::Success)
                        ++endY;
                    //ship extends probably beyond startY and endY
                    if(mHitGrid[startY][startX].category == HitCategory::Pending)
                        mPriorityGrid[startY][startX] += CONNECTED_BONUS;
                    if(mHitGrid[endY][startX].category == HitCategory::Pending)
                        mPriorityGrid[endY][startX] += CONNECTED_BONUS;
                }
                if(validField(j-1, i) && mHitGrid[i][j-1].category == HitCategory::Success &&
                   mHitGrid[i][j].shipNumber == mHitGrid[i][j-1].shipNumber) //two horizontally connected hits
                {
                    unsigned startX = j-1, startY = i, endX = j-1;
                    while(validField(startY, startX) && mHitGrid[startY][startX].category == HitCategory::Success)
                        --startX;
                    while(validField(startY, endX) && mHitGrid[startY][endX].category == HitCategory::Success)
                        ++endX;
                    if(mHitGrid[startY][startX].category == HitCategory::Pending)
                        mPriorityGrid[startY][startX] += CONNECTED_BONUS;
                    if(mHitGrid[startY][endX].category == HitCategory::Pending)
                        mPriorityGrid[startY][endX] += CONNECTED_BONUS;
                }
			}
		}
	}

	std::vector<sf::Vector2i> suitableSquares;
	int maxI = 0, maxJ = 0;
	for(int i = 0; i < FIELD_SIZE; ++i)
	{
		for(int j = 0; j < FIELD_SIZE; ++j)
		{
			if(mPriorityGrid[i][j] > mPriorityGrid[maxI][maxJ])
			{
				maxI = i;
				maxJ = j;
				suitableSquares.clear();
				suitableSquares.push_back(sf::Vector2i(j + 1, i + 1));
			}
			else if(mPriorityGrid[i][j] == mPriorityGrid[maxI][maxJ])
				suitableSquares.push_back(sf::Vector2i(j + 1, i + 1));
		}
	}
	std::random_shuffle(suitableSquares.begin(), suitableSquares.end());
	if(suitableSquares.empty())
		addFieldToAttack(mPossiblePositions[0]);
	else
		addFieldToAttack(suitableSquares[0]);
	#ifdef DEBUG
	//std::cout << "Low priority attack: " << std::boolalpha << (mMovesWithoutHit >= 3) << " (mMovesWithoutHit: " << mMovesWithoutHit << ")\n";
	std::cout << "   ";
	for(int i = 0; i < FIELD_SIZE; ++i)
		std::cout << i << "  ";
	std::cout << "\n\n";
	for(int i = 0; i < FIELD_SIZE; ++i)
	{
		std::cout << static_cast<char>('A' + i) << "  ";
		for(int j = 0; j < FIELD_SIZE; ++j)
		{
			if(mHitGrid[i][j].category == HitCategory::Failure)
				std::cout << "O  ";
			else if(mHitGrid[i][j].category == HitCategory::Success)
				std::cout << "X  ";
            else
                std::cout << mPriorityGrid[i][j] << (mPriorityGrid[i][j] < 10? "  " : " ");
		}
		std::cout << "\n";
	}
	std::cout << "\n";
	std::cout << "Attacking: " << suitableSquares[0].x-1 << static_cast<char>('A' + suitableSquares[0].y - 1) << "\n\n";
	#endif
}
