#ifndef SquareNode_H_INCLUDED
#define SquareNode_H_INCLUDED

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>

#include "Category.h"
#include "SceneNode.h"

#include <string>

class SquareNode : public SceneNode
{
private:
	sf::RectangleShape	mRectangle;
    sf::Text 			mText;
    sf::Texture			mExplosionTexture;
    bool 				mIsCrossedOut;
    bool				mIsSelected;
    unsigned			mCategory;
    const unsigned		mEDGELENGTH = 48;
    sf::Vector2i		mGridPosition;
	unsigned			mShipNumber;
	//Animation			mExplosion;
	bool				mHasToExplode;

public:
						SquareNode(const sf::Font& font, sf::Vector2f position, const unsigned& category, const sf::Texture& explosionTexture, const std::string& s = "");
						SquareNode(sf::Vector2f position, const unsigned& category, const sf::Texture& explosionTexture);

	virtual void		drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const;
	virtual void		updateCurrent();

    void 				crossOut();
    void 				setText(const std::string& s);
    void				toggleSelect();
    bool				isSelected() const;
    bool				contains(sf::Vector2f position) const;
    virtual unsigned	getCategory() const;
    void				setCategory(const unsigned& category);
    void				setBackgroundColor(const sf::Color& color);
    void				setGridPosition(int x, int y);
    sf::Vector2i		getGridPosition() const;
    void				setShipNumber(const unsigned& n);
    unsigned			getShipNumber() const;
    void				explode();

private:
    void 				draw(sf::RenderTarget& target, sf::RenderStates states) const;
    void				align(sf::Vector2f position);
};

#endif // SquareNode_H_INCLUDED
