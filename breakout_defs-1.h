/* --------------------------------------------------------
 *    File: breakout_defs.h
 *  Author: Kevin Kostage
 *   Class: COP 2001 Programming Methodology Spring 2022 10410
 * Purpose: definitions
 * -------------------------------------------------------- */
#include <SFML/Graphics.hpp>
#ifndef COLLISION_BREAKOUT_DEFS_H
#define COLLISION_BREAKOUT_DEFS_H

// Global Constants
// --------------------------------------------------------

// window constants
const int WINDOW_WIDTH = 640;
const int WINDOW_HEIGHT = 480;
const sf::Color WINDOW_COLOR = sf::Color::White;

// drawing constants
const float FRAME_RATE = (1.0/30.0) * 1000.0;       // FPS in ms
const sf::Color BALL_COLOR = sf::Color::Blue;

// ball constants
const float BALL_RADIUS = 10.0;

// wall constants
const float WALL_THICKNESS = 10.0;
const sf::Color WALL_COLOR = sf::Color::Black;

// paddle constants
const float PADDLE_WIDTH = 80.0;
const float PADDLE_THICKNESS = 10.0;
const sf::Color PADDLE_COLOR = sf::Color::Red;
const float PADDLE_SPEED = PADDLE_WIDTH/10.0/1000.0;

//brick constants
const int BRICK_ROWS = 8;                                                    //# of rows of bricks
const int BRICK_COLUMNS = 14;                                                //# of columns of bricks
const float BRICK_WIDTH = WINDOW_WIDTH / BRICK_COLUMNS;                     //width of 1 brick
const float BRICK_HEIGHT = PADDLE_THICKNESS * 2;                            // height of 1 brick
const float BRICKS_HEIGHT = BRICK_ROWS * BRICKS_HEIGHT;                     // height of rack
const float BRICKS_TOP = WINDOW_HEIGHT / 2.0 - BRICKS_HEIGHT * 0.75;         // top of rack
const float BRICKS_LEFT = WALL_THICKNESS;                                    // left of first brick in each row
const float FIRST_BRICK = BRICKS_TOP + (BRICK_ROWS - 1) * BRICKS_HEIGHT;     // top of lowest brick in rack




// These are just for fun
// speed that the can accelerate at to span window in
// n-seconds (in ms) broken up for each frame
const float SPEED_TIME = (3.0 * 1000.0) * 30.0;     //
const float BALL_SPEED_X = BALL_RADIUS * 10 / 1000.0;    // speed horizontally
const float BALL_SPEED_Y = BALL_RADIUS * 8.5 / 1000.0;   // span  vertically

// --------------------------------------------------------

enum Direction{
    Exit = -1,
    None = 0,
    Left = 1,
    Up = 2,
    Right = 3,
    Down = 4,
    Start = 5
};

// ball properties
struct Ball {
    float radius;
    float coordinateX;
    float coordinateY;
    float velocityX;
    float velocityY;
    sf::Color color;
};

//block structure
struct Block {
    float left;
    float top;
    float width;
    float height;
    sf::Color color;
    sf::RectangleShape rectangle;
};

//moving block properties
struct MovingBlock {
    Block block;
    float velocityX;
    float velocityY;
};

//borders properties
struct Borders {
    Block leftBlock;
    Block topBlock;
    Block rightBlock;
    Block bottomBlock;
};

struct Brick {
    Block block;
    bool hit;
    int points;
    float speedAdjust;
};
#endif //COLLISION_BREAKOUT_DEFS_H
