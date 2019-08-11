#ifndef WORLD_H_INCLUDED
#define WORLD_H_INCLUDED

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>

#include "CommandQueue.h"
#include "SceneNode.h"
#include "SquareNode.h"
#include "Fleet.h"

#include <array>

const unsigned int FIELD_SIZE = 10;

class World : public sf::Drawable, private sf::NonCopyable
{
private:

	SceneNode							mSceneGraph;
	CommandQueue						mCommandQueue;

	sf::Sprite							mBackgroundSprite;
	sf::Texture							mBackgroundTexture;
	sf::Texture							mExplosionTexture;
	sf::Font							mFont;
	unsigned							mActiveShipSourceCategory;

	Fleet								mPlayer1Fleet;
	Fleet								mPlayer2Fleet;

	bool								mWeaponsActive;

public:
										World();
	void								handleMouse(sf::Vector2f mousePosition);
	void								update();

	CommandQueue&						getCommandQueue();

	unsigned							getSquareNodeCategory(const sf::Vector2f& mousePosition) const;
	unsigned							getSquareNodeCategory(const sf::Vector2i& position, const unsigned& playerCategory) const;
	void								setShipSourceActive(const unsigned& shipSourceCategory);
	unsigned							getActiveShipSourceCategory() const;
	sf::Vector2i						getMouseGridPosition(const sf::Vector2f& mousePosition) const;
	bool								spaceForShipAvailable(const sf::Vector2i& gridPosition, bool rotated, unsigned playerCategory, unsigned shipCategory);
	int									getFleetSize(const unsigned& playerCategory) const;
	void								resetFleetSize(const unsigned& playerCategory);
	void								increaseFleetSize(const unsigned& category);
	int 								getMaxFleetHitpoints() const;

	const Fleet&						readFleet(const unsigned& playerCategory) const;
	void								decreaseFleetHitpoints(const unsigned& playerCategory);
	const sf::Font&						getFont() const;

	void 								activateWeapons();
	void								deactivateWeapons();
	bool								weaponsActive() const;

private:
	void								draw(sf::RenderTarget& target, sf::RenderStates states) const;
	void								addShipSource(float x, int n, const unsigned& category);
	void								addGameField(const sf::Vector2f& rootPosition, const unsigned& category);
	Fleet*								getFleet(const unsigned& playerCategory);
};

#endif // WORLD_H_INCLUDED
