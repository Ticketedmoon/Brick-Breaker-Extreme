#include <iostream>
#include <stdlib.h>
#include "engine.hpp"

GameState gameState = GameState::PLAYING;
Ball ball;
Paddle paddle;

std::vector<Brick> bricks;

const int BRICK_WIDTH = 160;
const int BRICK_HEIGHT = 40;
const int TOTAL_BRICKS = 32;
const int WINDOW_STANDARD_OFFSET_FOR_BALL_COLLISION = 20; 
const int TOTAL_BRICKS_PER_ROW = constants::WINDOW_WIDTH / BRICK_WIDTH;
const int TOTAL_BRICK_ROWS = TOTAL_BRICKS < TOTAL_BRICKS_PER_ROW ? TOTAL_BRICKS : TOTAL_BRICKS / TOTAL_BRICKS_PER_ROW;

Engine::Engine() 
{
    // Window
    resolution = sf::Vector2i(constants::WINDOW_WIDTH, constants::WINDOW_HEIGHT);
    window.create(sf::VideoMode(resolution.x, resolution.y), "Brick Breaker Extreme", sf::Style::Titlebar);

	auto desktop = sf::VideoMode::getDesktopMode();
    
	window.setPosition(sf::Vector2i(desktop.width/2 - constants::X_SCREEN_LOCATION_OFFSET, desktop.height/2 - constants::Y_SCREEN_LOCATION_OFFSET));
    window.setVerticalSyncEnabled(true);
    window.setFramerateLimit(FPS);

    // Fonts
    bool wasSuccessfullyLoaded = font.loadFromFile("./assets/fonts/totally_different.ttf");
    // Must be loaded relative to where the executable is.
    if (!wasSuccessfullyLoaded) 
    {
        throw std::runtime_error("Font failed to load");
    }

    // Create paddle
    paddle = createPaddle();

    // Create ball
    ball = createBall();

    // Create bricks
    createBricks();
}

// Note: Run until window is closed
void Engine::run() 
{
    while (window.isOpen()) 
	{
        // Check for any event on the window
        sf::Event event{};
        while (window.pollEvent(event))
        {
            switch (event.type) 
			{
                case sf::Event::Closed:
                    window.close();
                    break;
                case sf::Event::LostFocus:
                        std::cout << "Pause game now since focus to window lost" << std::endl;
                        break;
                case sf::Event::GainedFocus:
                    std::cout << "Resume game now since focus to window Regained" << std::endl;
                    break;
            }
        }
        this->draw();
    }
}

void Engine::draw() 
{
    onUpdate();
    onRender();
    onKeyboardEvent();
}

void Engine::onUpdate() {
    if (gameState == GameState::PLAYING) 
    {
        if (bricks.size() == 0) 
        {
            gameState = GameState::VICTORY;
        }
        else 
        {
            ball.startMovement();
            checkForPaddleTouch();
            checkForBrickTouch();
            checkForWindowBorderCollision();
        }
    }
}

void Engine::onRender() {
    if (gameState == GameState::VICTORY)
    {
        showViewOnGameStateChange("Victorious!\nPress [SPACE] to restart", sf::Color::Green, sf::Color::White);
    }
    else if (gameState == GameState::GAMEOVER)
    {
        showViewOnGameStateChange("Game Over!\nPress [SPACE] to restart", sf::Color::Red, sf::Color::White);
    }
    else 
    {
        window.clear(sf::Color::Blue);
        window.draw(paddle.getRectangleShapeForPaddle());
        window.draw(ball.getCircleShapeForBall());
        for (int i = 0; i < bricks.size(); i++)
        {
            window.draw(bricks[i].getRectangleShapeForBrick());
        }
    }
    window.display();
}

void Engine::onKeyboardEvent() 
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)) 
    {
        paddle.moveLeft();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)) 
    {
        paddle.moveRight();
    }

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) 
    {
        if (gameState != GameState::PLAYING)
        {
            gameState = GameState::PLAYING;
            ball = createBall();
            paddle = createPaddle();
            createBricks();
        }
    }
}

Paddle Engine::createPaddle() 
{
    const float paddleWidth = 200.f;
    const float paddleHeight = 10.f;
    float paddleXPosition = (window.getSize().x / 2.f) - (paddleWidth / 2.f);
    float paddleYPosition = window.getSize().y - 100.f;
    return Paddle(paddleWidth, paddleHeight, paddleXPosition, paddleYPosition);
}

Ball Engine::createBall() 
{
    const float ballRadius = 20.f;
    float ballStartX = (window.getSize().x / 2.f) - ballRadius;
    float ballStartY = (window.getSize().y / 2.f) - ballRadius;
    return Ball(ballRadius, ballStartX, ballStartY);
}

void Engine::createBricks() 
{
    bricks.reserve(TOTAL_BRICKS);
	for (int i = 0; i < TOTAL_BRICK_ROWS; i++) 
	{
		for (int j = 0; j < TOTAL_BRICKS_PER_ROW; j++)
		{
			int xPos = j * BRICK_WIDTH;
			int yPos = 20 + (i * BRICK_HEIGHT);
			int r = Utils::randomNumber(0, 255);
			int g = Utils::randomNumber(0, 255);
			int b = Utils::randomNumber(0, 255);
 			sf::Color color = sf::Color(r, g, b);
			bricks.emplace_back(BRICK_WIDTH, BRICK_HEIGHT, xPos, yPos, color);
		}
    }
}

void Engine::checkForWindowBorderCollision()
{
    
    bool isTouchingDeathZoneBottomBorder = ball.getY() >= constants::WINDOW_HEIGHT - WINDOW_STANDARD_OFFSET_FOR_BALL_COLLISION;
    if (isTouchingDeathZoneBottomBorder) 
    {
        gameState = GameState::GAMEOVER;
    }

    bool isTouchingLeftOrRightBorder = ball.getX() <= -WINDOW_STANDARD_OFFSET_FOR_BALL_COLLISION || ball.getX() >= constants::WINDOW_WIDTH;
    bool isTouchTopBorder = (ball.getY() <= 0);
    if (isTouchingLeftOrRightBorder) 
    {
        ball.setVelocityX(-ball.getVelocityX());
        int newDirection = ball.getDirection() * -1;
        ball.setDirection(newDirection);
    } 
    if (isTouchTopBorder)
    {
        ball.setVelocityY(-ball.getVelocityY());
    }
}

void Engine::checkForBrickTouch() {
    for (int i = 0; i < bricks.size(); i++)
    {
        Brick* brick = &bricks.at(i);
        float tolerance = 10.0;
		bool isBallAfterBrickStartX = brick->getRectangleShapeForBrick().getPosition().x <= (ball.getX()+ball.getRadius());
		bool isBallBeforeBrickEndX = (brick->getRectangleShapeForBrick().getPosition().x + brick->getWidth()) >= ball.getX();
        bool isTouchingBrickX = isBallAfterBrickStartX && isBallBeforeBrickEndX;
        bool isTouchingBrickY = (ball.getY() - brick->getRectangleShapeForBrick().getPosition().y) <= tolerance;
        if (isTouchingBrickY && isTouchingBrickX)
        {
        	ball.setVelocityY(-ball.getVelocityY());
            ball.playBrickDestroySound();
            destroyBrick(i);
            std::cout << "brick ID: " << i << std::endl;
        }
    }
}

void Engine::checkForPaddleTouch() 
{
    bool isTouchingPaddleX = (
        (paddle.getRectangleShapeForPaddle().getPosition().x <= ball.getX()) &&
        (paddle.getRectangleShapeForPaddle().getPosition().x + paddle.getWidth()) >= ball.getX());

    int paddleY = paddle.getRectangleShapeForPaddle().getPosition().y;

    // Offset is to account for the y position pointing to the top-left of the shape.
    // We need this to be the bottom so we hit the paddle correctly.
    // So add the radius * 2 for the diameter.
    int ballYPlusOffset = (ball.getY() + ball.getRadius() * 2);

    bool isTouchingPaddleY = (paddleY - ballYPlusOffset) <= 0;
    if (isTouchingPaddleY && isTouchingPaddleX)
    {
        bool touchRight = (ball.getX() >= paddle.getRectangleShapeForPaddle().getPosition().x + paddle.getWidth() / 2)
            && (ball.getX() <= paddle.getRectangleShapeForPaddle().getPosition().x + paddle.getWidth());
        bool isMovingRight = ball.getDirection() == 1;

        if (touchRight) 
        {
            if (isMovingRight) 
            {
                ball.setVelocityX(ball.getVelocityX() - 3);
            } else 
            {
                ball.setVelocityX(ball.getVelocityX() + 3);
            }
        } else 
        {
            if (isMovingRight) 
            {
                ball.setVelocityX(ball.getVelocityX() + 3);
            } 
            else 
            {
                ball.setVelocityX(ball.getVelocityX() - 3);
            }
        }
        ball.setVelocityY(ball.getVelocityY() * -1);
    }
}

void Engine::destroyBrick(int brickIndex)
{
	std::vector<Brick>::iterator brickIndexToRemove = bricks.begin() + brickIndex;
	bricks.erase(brickIndexToRemove);
	std::cout << "bricks size: " << bricks.size() << '\n';
}

void Engine::showViewOnGameStateChange(std::string text, sf::Color backgroundColor, sf::Color textColour) 
{
    sf::Text sf_text = sf::Text(text, font);
    window.clear(backgroundColor);
    sf_text.setFillColor(textColour);
    sf_text.setCharacterSize(128); // in pixels, not points!
    sf_text.setStyle(sf::Text::Bold | sf::Text::Underlined);
    sf::FloatRect bounds = sf_text.getLocalBounds();
    sf_text.setPosition(constants::WINDOW_WIDTH/2 - bounds.width/2, 225);
    window.draw(sf_text);
}