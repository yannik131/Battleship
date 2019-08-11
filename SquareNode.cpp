#include "SquareNode.h"

#include <cmath>
#include <cassert>


SquareNode::SquareNode(const sf::Font& font, sf::Vector2f position, const unsigned& category, const sf::Texture& explosionTexture, const std::string& s) :
	SquareNode(position, category, explosionTexture)
{
	mText.setFont(font);
	mText.setString(s);
	mText.setCharacterSize(26);
	mText.setFillColor(sf::Color::Black);
	mText.setPosition(sf::Vector2f(position.x + 16, position.y + 8));
	align(position);
	/*mExplosion.setFrameSize(sf::Vector2i(256, 256));
	mExplosion.setNumFrames(16);
	mExplosion.setDuration(sf::seconds(1));*/
}

SquareNode::SquareNode(sf::Vector2f position, const unsigned& category, const sf::Texture& explosionTexture) :
	mIsCrossedOut(false),
	mIsSelected(false),
	mCategory(category),
	mShipNumber(0),
	mHasToExplode(false)
{
	align(position);
	/*mExplosion.setTexture(explosionTexture);
	mExplosion.setPosition(mRectangle.getPosition());*/
}

void SquareNode::crossOut()
{
	mIsCrossedOut = true;
}

void SquareNode::setText(const std::string& s)
{
	mText.setString(s);
}

void SquareNode::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mRectangle);
	target.draw(mText);

	unsigned edgeLength = 0;
	unsigned thisCategory = getCategory();

	if(thisCategory & Category::ShipGiant && (thisCategory & Category::Player1 || thisCategory & Category::Player2 && thisCategory & Category::Destroyed)
		|| thisCategory & Category::ShipSourceGiant)
		edgeLength = 36;
	else if(thisCategory & Category::ShipBig && (thisCategory & Category::Player1 || thisCategory & Category::Player2 && thisCategory & Category::Destroyed)
			|| thisCategory & Category::ShipSourceBig)
		edgeLength = 28;
	else if(thisCategory & Category::ShipMedium && (thisCategory & Category::Player1 || thisCategory & Category::Player2 && thisCategory & Category::Destroyed)
			|| thisCategory & Category::ShipSourceMedium)
		edgeLength = 20;
	else if(thisCategory & Category::ShipSmall && (thisCategory & Category::Player1 || thisCategory & Category::Player2 && thisCategory & Category::Destroyed)
			|| thisCategory & Category::ShipSourceSmall)
		edgeLength = 12;
	if(edgeLength > 0)
	{
		sf::RectangleShape smallRectangle(sf::Vector2f(edgeLength, edgeLength));
		smallRectangle.setPosition(mRectangle.getPosition() + sf::Vector2f((mEDGELENGTH - edgeLength) / 2, (mEDGELENGTH - edgeLength) / 2));
		sf::Color fillColor = sf::Color::Black;
		if(mCategory & Category::ShipMedium || mCategory & Category::ShipSmall)
			 fillColor -= sf::Color(0, 0, 0, (mShipNumber - 1) * 40);
		smallRectangle.setFillColor(fillColor);
		target.draw(smallRectangle);
	}
	if(mIsCrossedOut || getCategory() & Category::Destroyed)
	{
		sf::RectangleShape line(sf::Vector2f(std::sqrt(mEDGELENGTH * mEDGELENGTH + mEDGELENGTH * mEDGELENGTH), 2));
		line.setPosition(mRectangle.getPosition());
		line.setOrigin(sf::Vector2f(0, 2));
		if(mShipNumber != 0)
			line.setFillColor(sf::Color::Red);
		else
			line.setFillColor(sf::Color::Green);
		line.rotate(45);
		target.draw(line);
		line.setPosition(mRectangle.getPosition() + sf::Vector2f(0, mEDGELENGTH));
		line.rotate(270);
		target.draw(line);
	}
	/*if(mHasToExplode)
		target.draw(mExplosion, states);*/
}

bool SquareNode::contains(sf::Vector2f position) const
{
	return mRectangle.getGlobalBounds().contains(position);
}

void SquareNode::drawCurrent(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(mRectangle, states);
}

void SquareNode::updateCurrent()
{
	/*if(mHasToExplode)
	{
		mExplosion.update();
		if(mExplosion.isFinished())
			mHasToExplode = false;
	}*/
}

void SquareNode::align(sf::Vector2f position)
{
	mRectangle.setPosition(position);
	mRectangle.setSize(sf::Vector2f(mEDGELENGTH, mEDGELENGTH));
	mRectangle.setOutlineColor(sf::Color::Black);
	mRectangle.setOutlineThickness(1.3f);
}

unsigned SquareNode::getCategory() const
{
	return mCategory;
}

void SquareNode::toggleSelect()
{
	mIsSelected = !mIsSelected;
	if(mIsSelected)
	{
		mRectangle.setOutlineColor(sf::Color::Green);
		mRectangle.setOutlineThickness(2.f);
	}
	else
	{
		mRectangle.setOutlineColor(sf::Color::Black);
		mRectangle.setOutlineThickness(1.3f);
	}
}

void SquareNode::setCategory(const unsigned& category)
{
	mCategory = category;
}

bool SquareNode::isSelected() const
{
	return mIsSelected;
}

void SquareNode::setBackgroundColor(const sf::Color& color)
{
	//mRectangle.setFillColor(color);
}

void SquareNode::setGridPosition(int x, int y)
{
	//assert(x >= 1 && x <= FIELD_SIZE && y >= 1 && y <= FIELD_SIZE);
	mGridPosition.x = x;
	mGridPosition.y = y;
}

sf::Vector2i SquareNode::getGridPosition() const
{
	return mGridPosition;
}

void SquareNode::setShipNumber(const unsigned& n)
{
	mShipNumber = n;
}

unsigned SquareNode::getShipNumber() const
{
	return mShipNumber;
}

void SquareNode::explode()
{
	mHasToExplode = true;
}
