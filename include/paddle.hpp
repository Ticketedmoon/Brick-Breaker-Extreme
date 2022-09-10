#pragma once

#include <SFML/Graphics/RectangleShape.hpp>

class Paddle {

private:
    sf::RectangleShape rectangle;
    float velocity = 15.0;
    float x;
    float y;
    float width;
    float height;

public:
    Paddle();
    Paddle(float width, float height, float x, float y);
    sf::RectangleShape getRectangleShapeForPaddle();
    void moveLeft();
    void moveRight();
    float getX();
    float getWidth();
};
