#include "brick.hpp"

Brick::Brick(float width, float height, float x, float y, sf::Color colour) {
    this->x = x;
    this->y = y;
    this->width=width;
    this->height=height;
    this->colour = colour;
    this->rectangle = sf::RectangleShape(sf::Vector2f(width, height));
    this->rectangle.setFillColor(colour);
    this->rectangle.setPosition(x, y);
}

sf::RectangleShape Brick::getRectangleShapeForBrick() {
    return this->rectangle;
}

float Brick::getWidth() {
    return this->width;
}

Brick::Brick() = default;
