#ifndef PLAYER_H_INCLUDED
#define PLAYER_H_INCLUDED

#include <SFML/Window/Event.hpp>
#include <functional>

#include "SquareNode.h"

class CommandQueue;
class World;
class KI;

class Player
{
private:
	bool 					mRotationSelected;
	bool 					mWantsToStartGame;
	bool					mIsActive;
	sf::Vector2i 	mMouseGridPosition;

	CommandQueue&			mCommandQueue;
	World&					mWorld;

private:
	friend class KI;

private:
	void					createCommand(	const std::function<void(SceneNode&)>& action,
											unsigned category);
	void 					toggleShipSources(const unsigned& shipSource);
	bool 					addShip(unsigned playerCategory, bool rotated = false, unsigned shipCategory = Category::None, sf::Vector2i gridPosition = sf::Vector2i());
	void 					blockAdding();
	unsigned				attackSquare(sf::Vector2i gridPosition, const unsigned& attackedPlayer, int* shipNumber = nullptr);

public:
	Player(World& world);
	void	handleEvent(const sf::Event& event);
	void	handleRealtimeInput(CommandQueue& commands);
	bool	rotateSelected() const;
	bool	wantsToStartGame() const;
	bool	isAlive(const unsigned& category) const;
	void	setActive();
	bool	isActive() const;
	void 	resetField(const unsigned& playerCategory);
	int		getHitPoints(const unsigned& playerCategory) const;
};

#endif // PLAYER_H_INCLUDED
