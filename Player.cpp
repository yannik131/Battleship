#include "Player.h"
#include "Command.h"
#include "CommandQueue.h"
#include "SquareNode.h"
#include "World.h"

#include <cmath>
#include <iostream>

Player::Player(World& world) :
	mRotationSelected(false),
	mWantsToStartGame(false),
	mIsActive(true),
	mWorld(world),
	mCommandQueue(world.getCommandQueue())
{

}

void Player::handleRealtimeInput(CommandQueue& commands)
{

}

void Player::handleEvent(const sf::Event& event)
{
	if(event.type == sf::Event::MouseButtonPressed)
	{
		if(event.mouseButton.button == sf::Mouse::Left)
		{
			unsigned category = mWorld.getSquareNodeCategory(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
			if((category & Category::ShipSourceGiant || category & Category::ShipSourceBig ||
				category & Category::ShipSourceMedium || category & Category::ShipSourceSmall) && !mWantsToStartGame)
				toggleShipSources(category);
			else if(category & Category::Player1)
			{
				mMouseGridPosition = mWorld.getMouseGridPosition(sf::Vector2f(event.mouseButton.x, event.mouseButton.y));
				addShip(Category::Player1);
			}
			else if(category & Category::Player2)
			{
				if(mWorld.weaponsActive())
					attackSquare(mWorld.getMouseGridPosition(sf::Vector2f(event.mouseButton.x, event.mouseButton.y)), Category::Player2);
			}
		}
	}
	else if(event.type == sf::Event::KeyPressed)
	{
		if(event.key.code == sf::Keyboard::BackSpace && !mWantsToStartGame)
			resetField(Category::Player1);
		else if(event.key.code == sf::Keyboard::R)
			mRotationSelected = !mRotationSelected;
		else if(event.key.code == sf::Keyboard::Return)
		{
			if(mWorld.getFleetSize(Category::Player1) == Fleet::DEFAULTSIZE)
			{
				blockAdding();
				mWorld.activateWeapons();
			}
		}
	}
}

bool Player::rotateSelected() const
{
	return mRotationSelected;
}

bool Player::wantsToStartGame() const
{
	return mWantsToStartGame;
}

void Player::createCommand(const std::function<void(SceneNode&)>& action, unsigned category)
{
	Command command;
	command.category = category;
	command.action = action;
	mCommandQueue.push(command);
}

void Player::toggleShipSources(const unsigned& category)
{
	mWorld.setShipSourceActive(category);
	createCommand(
	[] (SceneNode& s)
	{
		if(dynamic_cast<SquareNode*>(&s)->isSelected())
			dynamic_cast<SquareNode*>(&s)->toggleSelect();
	},
	Category::ShipSourceGiant | Category::ShipSourceBig | Category::ShipSourceMedium | Category::ShipSourceSmall);

	createCommand(
	[] (SceneNode& s)
	{
		dynamic_cast<SquareNode*>(&s)->toggleSelect();
	},
	category);
}

bool Player::addShip(unsigned playerCategory, bool rotated, unsigned shipCategory, sf::Vector2i gridPosition)
{
	if(playerCategory & Category::Player1)
	{
		shipCategory = getShipTypeFromSource(mWorld.getActiveShipSourceCategory());
		gridPosition = mMouseGridPosition;
		rotated = mRotationSelected;
	}
	int length = std::log2(static_cast<float>(shipCategory));

	if(shipCategory == Category::None ||
		rotated && (gridPosition.y + length) > FIELD_SIZE+1 || !rotated && (gridPosition.x + length) > FIELD_SIZE+1 ||
		!mWorld.spaceForShipAvailable(gridPosition, rotated, playerCategory, shipCategory) ||
		mWorld.getFleetSize(playerCategory) == Fleet::DEFAULTSIZE)
		return false;
	//ship can be added
	mWorld.increaseFleetSize(playerCategory | shipCategory);
	int shipNumber = mWorld.getFleetSize(playerCategory);
	std::function<void(SceneNode&)> lambda = [=] (SceneNode& s)
	{
		sf::Vector2i nodePos = dynamic_cast<SquareNode*>(&s)->getGridPosition();
		if(	rotated && nodePos.x == gridPosition.x && nodePos.y >= gridPosition.y && nodePos.y < gridPosition.y + length ||
		   !rotated && nodePos.y == gridPosition.y && nodePos.x >= gridPosition.x && nodePos.x < gridPosition.x + length)
		{
			dynamic_cast<SquareNode*>(&s)->setCategory(shipCategory | playerCategory);
			dynamic_cast<SquareNode*>(&s)->setShipNumber(shipNumber);

		}
	};
	createCommand(lambda, playerCategory);
	return true;
}

void Player::resetField(const unsigned& playerCategory)
{
	Command command;
	command.category = playerCategory;
	command.action = [=] (SceneNode& s)
	{
		dynamic_cast<SquareNode*>(&s)->setCategory(playerCategory);
		dynamic_cast<SquareNode*>(&s)->setShipNumber(0);
	};
	mCommandQueue.push(command);
	mWorld.resetFleetSize(playerCategory);
	mWantsToStartGame = false;
}

void Player::blockAdding()
{
	mWantsToStartGame = true;
	mWorld.setShipSourceActive(Category::None);
	Command command;
	command.category = Category::ShipSourceGiant | Category::ShipSourceBig | Category::ShipSourceMedium | Category::ShipSourceSmall;
	command.action = [] (SceneNode& s)
	{
		if(dynamic_cast<SquareNode*>(&s)->isSelected())
			dynamic_cast<SquareNode*>(&s)->toggleSelect();
	};
	mCommandQueue.push(command);
}

bool Player::isAlive(const unsigned& playerCategory) const
{
	return mWorld.readFleet(playerCategory).hitpoints > 0;
}

int Player::getHitPoints(const unsigned& playerCategory) const
{
	return mWorld.readFleet(playerCategory).hitpoints;
}

unsigned Player::attackSquare(sf::Vector2i position, const unsigned& attackedPlayer, int* shipNumber)
{
	unsigned attackedSquareCategory = mWorld.getSquareNodeCategory(position, attackedPlayer);
	if(attackedSquareCategory & Category::Destroyed)
		return Category::None;
	mIsActive = false;
	bool hasToExplode = false;
	if(attackedSquareCategory & Category::ShipGiant || attackedSquareCategory & Category::ShipBig ||
	   attackedSquareCategory & Category::ShipMedium || attackedSquareCategory & Category::ShipSmall)
	{
		mWorld.decreaseFleetHitpoints(attackedPlayer);
	}
	Command command;
	command.category = attackedPlayer;
	command.action = [=] (SceneNode& s)
	{
		if(dynamic_cast<SquareNode*>(&s)->getGridPosition().x == position.x && dynamic_cast<SquareNode*>(&s)->getGridPosition().y == position.y)
		{
			if(shipNumber != nullptr)
				*shipNumber = dynamic_cast<SquareNode*>(&s)->getShipNumber();
			dynamic_cast<SquareNode*>(&s)->setCategory(dynamic_cast<SquareNode*>(&s)->getCategory() | Category::Destroyed);
		}
	};
	mCommandQueue.push(command);
	return attackedSquareCategory;
}

bool Player::isActive() const
{
	return mIsActive;
}

void Player::setActive()
{
	mIsActive = true;
}
