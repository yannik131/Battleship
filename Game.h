#ifndef GAME_H_INCLUDED
#define GAME_H_INCLUDED

#include <SFML/System/NonCopyable.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Window/Event.hpp>

#include "World.h"
#include "Player.h"
#include "KI.h"

#include <stack>

class Game : private sf::NonCopyable
{
private:
	sf::RenderWindow			mWindow;
	World						mWorld;
	Player						mPlayer;
	KI							mKI;
	bool						mGameStarted;
	bool						mGameOver;
	std::stack<sf::Text>		mTextStack;

private:
	void						update();
	void						processInput();
	void						render();
	void						printStatistics();
	void						addText(float x, float y, const std::string& text, const sf::Color& color = sf::Color::Black, int characterSize = 15);
	void						stop();

public:
								Game();
	void						run();

	const unsigned int STATISTICS_X_POSITION = 1000;
};

#endif // GAME_H_INCLUDED
