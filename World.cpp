#include "World.h"
#include "Category.h"
#include "Fleet.h"

#include <cmath>
#include <cassert>


World::World() :
	mActiveShipSourceCategory(Category::None),
	mWeaponsActive(false)
{
	mFont.loadFromFile("arial.ttf");
    mBackgroundTexture.loadFromFile("GameField.png");
    //mExplosionTexture.loadFromFile("Explosion.png");
    mBackgroundSprite.setTexture(mBackgroundTexture);
    addGameField(sf::Vector2f(5, 95), Category::Player1);
    addGameField(sf::Vector2f(600, 95), Category::Player2);
	addShipSource(25, 5, Category::ShipSourceGiant);
	addShipSource(338, 4, Category::ShipSourceBig);
	addShipSource(603, 3, Category::ShipSourceMedium);
	addShipSource(839, 2, Category::ShipSourceSmall);
}

void World::addShipSource(float x, int n, const unsigned& category)
{
	sf::Vector2f rootPosition(x, 847);
	for(int i = 0; i < n; ++i)
	{
		std::unique_ptr<SquareNode> square(new SquareNode(rootPosition, category, mExplosionTexture));
		mSceneGraph.attachChild(std::move(square));
		rootPosition.x += 48;
	}
}

void World::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mBackgroundSprite, states);
	target.draw(mSceneGraph);
}

void World::update()
{
	mSceneGraph.update();
	while(!mCommandQueue.isEmpty())
		mSceneGraph.onCommand(mCommandQueue.pop());
}

void World::addGameField(const sf::Vector2f& rootPosition, const unsigned& playerCategory)
{
	sf::Vector2f currentPosition = rootPosition;
	for(int i = 0; i < FIELD_SIZE+1; ++i)
	{
		for(int j = 0; j < FIELD_SIZE+1; ++j)
		{
			std::unique_ptr<SquareNode> square(new SquareNode(mFont, currentPosition, Category::Border, mExplosionTexture));
			if(currentPosition.x != rootPosition.x && currentPosition.y == rootPosition.y)
				square->setText(std::string() + static_cast<char>('0' + j - 1));
			else if(currentPosition.x == rootPosition.x && currentPosition.y != rootPosition.y)
				square->setText(std::string() + static_cast<char>('A' + i - 1));
			else if(currentPosition.x != rootPosition.x && currentPosition.y != rootPosition.y)
			{
				square->setCategory(playerCategory);
				square->setBackgroundColor(sf::Color(0, 0, 255, 150));
				square->setGridPosition(j, i);
			}

			mSceneGraph.attachChild(std::move(std::unique_ptr<SquareNode>(std::move(square))));
			currentPosition.x += 51;
		}
		currentPosition.y += 51;
		currentPosition.x = rootPosition.x;
	}
}

CommandQueue& World::getCommandQueue()
{
	return mCommandQueue;
}

unsigned World::getSquareNodeCategory(const sf::Vector2f& mousePosition) const
{
	for(const auto& i : mSceneGraph.getTree())
		if(dynamic_cast<SquareNode*>(i.get())->contains(mousePosition))
			return i->getCategory();
	return Category::None;
}

unsigned World::getSquareNodeCategory(const sf::Vector2i& position, const unsigned& playerCategory) const
{
	for(const auto& i : mSceneGraph.getTree())
		if(dynamic_cast<SquareNode*>(i.get())->getCategory() & playerCategory &&
			dynamic_cast<SquareNode*>(i.get())->getGridPosition().x == position.x && dynamic_cast<SquareNode*>(i.get())->getGridPosition().y == position.y)
			return i->getCategory();
	return Category::None;
}

void World::setShipSourceActive(const unsigned& shipSourceCategory)
{
	mActiveShipSourceCategory = shipSourceCategory;
}

unsigned World::getActiveShipSourceCategory() const
{
	return mActiveShipSourceCategory;
}

sf::Vector2i World::getMouseGridPosition(const sf::Vector2f& mousePosition) const
{
	for(const auto& i : mSceneGraph.getTree())
	{
		if(dynamic_cast<SquareNode*>(i.get())->contains(mousePosition))
			return dynamic_cast<SquareNode*>(i.get())->getGridPosition();
	}
}

void World::increaseFleetSize(const unsigned& category)
{
	Fleet* fleetToChange = getFleet(category);

	if(category & Category::ShipGiant)
		++fleetToChange->giantCount;
	else if(category & Category::ShipBig)
		++fleetToChange->bigCount;
	else if(category & Category::ShipMedium)
		++fleetToChange->mediumCount;
	else if(category & Category::ShipSmall)
		++fleetToChange->smallCount;
	++fleetToChange->size;
}

int World::getFleetSize(const unsigned& category) const
{
	if(category == Category::Player1)
		return mPlayer1Fleet.size;
	return mPlayer2Fleet.size;
}

bool World::spaceForShipAvailable(const sf::Vector2i& gridPosition, bool rotated, unsigned playerCategory, unsigned shipCategory)
{
	if(gridPosition.x < 0 || gridPosition.x > FIELD_SIZE || gridPosition.y < 0 || gridPosition.y > FIELD_SIZE)
		return false;
	int length = std::log2(static_cast<float>(shipCategory));
	if(rotated && gridPosition.y + length - 1 > FIELD_SIZE || !rotated && gridPosition.x + length - 1 > FIELD_SIZE)
		return false;
	Fleet* fleetToCheck = getFleet(playerCategory);

	if(	shipCategory & Category::ShipGiant && fleetToCheck->giantCount >= Fleet::MAX_GIANT_COUNT ||
		shipCategory & Category::ShipBig && fleetToCheck->bigCount >= Fleet::MAX_BIG_COUNT ||
		shipCategory & Category::ShipMedium && fleetToCheck->mediumCount >= Fleet::MAX_MEDIUM_COUNT ||
		shipCategory & Category::ShipSmall && fleetToCheck->smallCount >= Fleet::MAX_SMALL_COUNT)
		return false;

	for(const auto& i : mSceneGraph.getTree())
	{
		sf::Vector2i nodePos = dynamic_cast<SquareNode*>(i.get())->getGridPosition();
		if(	rotated && nodePos.x == gridPosition.x && nodePos.y >= gridPosition.y && nodePos.y < gridPosition.y + length ||
		   !rotated && nodePos.y == gridPosition.y && nodePos.x >= gridPosition.x && nodePos.x < gridPosition.x + length)
		{
			if(dynamic_cast<SquareNode*>(i.get())->getShipNumber() != 0)
				return false;
		}
	}
	return true;
}


void World::resetFleetSize(const unsigned& playerCategory)
{
	Fleet* fleetToChange = getFleet(playerCategory);
	fleetToChange->reset();
}

Fleet* World::getFleet(const unsigned& category)
{
	if(category & Category::Player1)
		return &mPlayer1Fleet;
	return &mPlayer2Fleet;
}

const Fleet& World::readFleet(const unsigned& playerCategory) const
{
	if(playerCategory & Category::Player1)
		return mPlayer1Fleet;
	return mPlayer2Fleet;
}

const sf::Font& World::getFont() const
{
	return mFont;
}

void World::activateWeapons()
{
	mWeaponsActive = true;
}

void World::deactivateWeapons()
{
	mWeaponsActive = false;
}

bool World::weaponsActive() const
{
	return mWeaponsActive;
}

void World::decreaseFleetHitpoints(const unsigned& playerCategory)
{
	--getFleet(playerCategory)->hitpoints;
}
