#include "Game.h"
#include <cassert>
#include <chrono>

Game::Game() :
	mWindow(sf::VideoMode(1320, 954), "Battleship 1.2", sf::Style::Close),
	mGameStarted(false),
	mGameOver(false),
	mPlayer(mWorld),
	mKI(mPlayer)
{
	mWindow.setFramerateLimit(30);
}

void Game::update()
{
	mWorld.update();
	if(!mPlayer.isAlive(Category::Player1) || !mPlayer.isAlive(Category::Player2) && mGameStarted)
		stop();
	if(!mPlayer.isActive() && mGameStarted)
	{
		mKI.makeMove();
		mPlayer.setActive();
	}
}

void Game::processInput()
{
    /*
	static auto t1 = std::chrono::high_resolution_clock::now();
	static int count = 1;
	if(std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::high_resolution_clock::now() - t1) > std::chrono::milliseconds(500))
	{
		t1 = std::chrono::high_resolution_clock::now();
		mKI.reset();
		mGameStarted = false;
		sf::Vector2u windowSize = mWindow.getSize();
		sf::Texture texture;
		texture.create(windowSize.x, windowSize.y);
		texture.update(mWindow);
		sf::IntRect textureRect(600, 95, 556, 556);
		sf::Image screenshot = texture.copyToImage();
		texture.loadFromImage(screenshot, textureRect);
		screenshot = texture.copyToImage();
		screenshot.saveToFile(std::to_string(count++) + ".png");
	}
	if(!mGameStarted)
	{
		mKI.placeShip();
		if(mKI.ready())
			mGameStarted = true;
	}*/
	if(mPlayer.wantsToStartGame() && !mGameStarted)
	{
		mKI.placeShip();
		if(mKI.ready())
			mGameStarted = true;
	}
	CommandQueue& commands = mWorld.getCommandQueue();
	sf::Event event;
	while(mWindow.pollEvent(event))
	{
		mPlayer.handleEvent(event);
		if(event.type == sf::Event::Closed ||
			(event.type == sf::Event::KeyPressed && event.key.code == sf::Keyboard::Escape))
			mWindow.close();
		if(event.type == sf::Event::KeyPressed && mGameOver)
		{
			if(event.key.code == sf::Keyboard::Y)
			{
				mKI.reset();
				mPlayer.resetField(Category::Player1);
				mPlayer.setActive();
				mGameOver = false;
				mGameStarted = false;
			}
			else if(event.key.code == sf::Keyboard::N)
				mWindow.close();
		}
	}
	mPlayer.handleRealtimeInput(commands);
}

void Game::render()
{
	mWindow.clear();
	mWindow.draw(mWorld);
	printStatistics();
	while(!mTextStack.empty())
	{
		mWindow.draw(mTextStack.top());
		mTextStack.pop();
	}
	mWindow.display();
}

void Game::run()
{
	while(mWindow.isOpen())
    {
        processInput();
        update();
        render();
    }
}

void Game::printStatistics()
{
	if(!mGameStarted && !mGameOver)
	{
		addText(130, 910, std::to_string(Fleet::MAX_GIANT_COUNT - mWorld.readFleet(Category::Player1).giantCount) + " left");
		addText(415, 910, std::to_string(Fleet::MAX_BIG_COUNT - mWorld.readFleet(Category::Player1).bigCount) + " left");
		addText(655, 910, std::to_string(Fleet::MAX_MEDIUM_COUNT - mWorld.readFleet(Category::Player1).mediumCount) + " left");
		addText(868, 910, std::to_string(Fleet::MAX_SMALL_COUNT - mWorld.readFleet(Category::Player1).smallCount) + " left");
		addText(STATISTICS_X_POSITION + 20, 665, "Orientation:");
		addText(STATISTICS_X_POSITION, 685, "(Press R to change)");
		sf::RectangleShape line(sf::Vector2f(35, 4));
		line.setPosition(STATISTICS_X_POSITION+40, 735);
		line.setFillColor(sf::Color::Black);
		if(mPlayer.rotateSelected())
		{
			line.rotate(90);
			line.setPosition(line.getPosition() + sf::Vector2f(35 / 2 + 2, -35 / 2 + 2));
		}
		mWindow.draw(line);
		addText(STATISTICS_X_POSITION, 770, "Press backspace to");
		addText(STATISTICS_X_POSITION, 790, "remove all ships");
		if(mWorld.getActiveShipSourceCategory() == Category::None && !mPlayer.wantsToStartGame())
			addText(STATISTICS_X_POSITION, 830, "Select a shipsource!", sf::Color(200, 0, 0, 255));
		else if(mWorld.getFleetSize(Category::Player1) == Fleet::DEFAULTSIZE && !mPlayer.wantsToStartGame())
		{
			addText(STATISTICS_X_POSITION, 830, "Press Enter to", sf::Color(200, 0, 0, 255));
			addText(STATISTICS_X_POSITION, 850, "start the game", sf::Color(200, 0, 0, 255));
		}
		else if(mPlayer.wantsToStartGame())
			addText(STATISTICS_X_POSITION, 830, "Placing ships...", sf::Color(200, 0, 0, 255));
		else
			addText(STATISTICS_X_POSITION, 830, "Click on a square!", sf::Color(200, 0, 0, 255));
	}
	else if(mGameOver || mGameStarted)
	{
		if(mGameOver)
		{
			if(mPlayer.isAlive(Category::Player1))
				addText(STATISTICS_X_POSITION, 700, "You won", sf::Color::Green, 45);
			else
				addText(STATISTICS_X_POSITION, 700, "You lost", sf::Color::Red, 45);
			addText(STATISTICS_X_POSITION, 800, "Again? Y/N", sf::Color::Red, 40);
		}
		addText(85, 30, std::to_string(static_cast<int>(100.f / Fleet::MAX_HITPOINTS * mPlayer.getHitPoints(Category::Player1))) + "%", sf::Color::Black, 35);
		addText(775, 30, std::to_string(static_cast<int>(100.f / Fleet::MAX_HITPOINTS * mPlayer.getHitPoints(Category::Player2))) + "%", sf::Color::Black, 35);
	}
}

void Game::addText(float x, float y, const std::string& string, const sf::Color& color, int characterSize)
{
	static sf::Text text;
	text.setFont(mWorld.getFont());
	text.setCharacterSize(characterSize);
	text.setFillColor(color);
	text.setPosition(x, y);
	text.setString(string);
	mTextStack.push(text);
}

void Game::stop()
{
	mGameOver = true;
	mGameStarted = false;
	mWorld.deactivateWeapons();
}
