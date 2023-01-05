/* --------------------------------------------------------
 *    File: collision.cpp
 *  Author: Kevin Kostage
 *   Class: COP 2001 Programming Methodology Spring 2022 10410
 * Purpose: final product of breakout game
 * -------------------------------------------------------- */


#include <cmath>
#include "breakout_defs.h"

// Function declarations (prototypes)
// --------------------------------------------------------
void setup(Ball &ball, Borders &walls, MovingBlock &paddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS]);
Direction processInput() ;
bool update(Direction &input, bool &started, Ball &ball, MovingBlock &paddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], Borders walls , float delta);
void render(sf::RenderWindow &window, Ball ball, MovingBlock paddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], Borders walls, float delta);
int collisionPoint(Ball *pBall, Block *pBlock);
bool collisionCheck(Ball *pBall, Block *pBlock);
bool checkBlockCollision (Block moving, Block stationary);
bool doCollisionChecks(Ball &ball, MovingBlock& paddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], Borders walls);

/**
 * The main application
 * @return OS status message (0=Success)
 */

int main() {

    // create a 2d graphics window
    sf::RenderWindow window(sf::VideoMode(WINDOW_WIDTH, WINDOW_HEIGHT), "Hello SFML");
    window.clear(WINDOW_COLOR);

    //ball variable
    Ball theBall;

    //wall variable
    Borders theWalls;

    //paddle variable
    MovingBlock thePaddle;

    //brick variable
    Brick theBricks[BRICK_ROWS][BRICK_COLUMNS];

    //initialization
    setup(theBall,theWalls, thePaddle, theBricks);

    // timing variables for the main game loop
    sf::Clock clock;
    sf::Time startTime = clock.getElapsedTime();
    sf::Time stopTime = startTime;
    float delta = 0.0;

    //start the game
    bool started = false;

    //main game loop
    bool gameOver = false;
    while (!gameOver)
    {
        // calculate frame time
        stopTime = clock.getElapsedTime();
        delta += (stopTime.asMilliseconds() - startTime.asMilliseconds());
        startTime = stopTime;

        // process events
        sf::Event event;
        while (!gameOver && window.pollEvent(event)) {
            // "close requested" event: we close the window
            if (event.type == sf::Event::Closed)
                gameOver = true;
        }

        // Process user input
        // ------------------------------------------------
        Direction userInput = processInput();
        if (userInput == Exit)
            gameOver = true;

        // Process Updates
        // ------------------------------------------------
        if (delta >= FRAME_RATE)
        {    // if we have made it at least a full frame time

            gameOver = update (userInput, started, theBall, thePaddle, theBricks,theWalls, delta);

            // subtract the frame-rate from the current frame-time
            // for each full frame covered by this update
            while (delta >= FRAME_RATE)
                delta -= FRAME_RATE;
        }
        // Render the window
        // ------------------------------------------------
        render(window, theBall, thePaddle, theBricks, theWalls, delta);

    } // end main game loop

    // game ending, close the graphics window
    window.close();

    return 0;   // return success to the OS
} // end main


/**
 * here to initialize all objects
 * @param ball initializes the ball
 * @param walls initializes the wall
 * @param paddle initializes the paddle
 * @param bricks initializes the bricks
 */

void setup(Ball &ball, Borders &walls, MovingBlock &paddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS])
{
    //initialize left wall
    walls.leftBlock.left = 0.0;
    walls.leftBlock.top = 0.0;
    walls.leftBlock.width = WALL_THICKNESS;
    walls.leftBlock.height = WINDOW_HEIGHT;
    walls.leftBlock.color = WALL_COLOR;
    walls.leftBlock.rectangle.setSize(sf::Vector2(walls.leftBlock.width, walls.leftBlock.height));
    walls.leftBlock.rectangle.setPosition(walls.leftBlock.left, walls.leftBlock.top);
    walls.leftBlock.rectangle.setFillColor(WALL_COLOR);

    //initialize top wall
    walls.topBlock.left = 0.0;
    walls.topBlock.top = 0.0;
    walls.topBlock.width = WINDOW_WIDTH;
    walls.topBlock.height = WALL_THICKNESS;
    walls.topBlock.color = WALL_COLOR;
    walls.topBlock.rectangle.setSize(sf::Vector2(walls.topBlock.width, walls.topBlock.height));
    walls.topBlock.rectangle.setPosition(walls.topBlock.left, walls.leftBlock.top);
    walls.topBlock.rectangle.setFillColor(WALL_COLOR);

    //initialize right wall
    walls.rightBlock.left = WINDOW_WIDTH - WALL_THICKNESS;
    walls.rightBlock.top = 0.0;
    walls.rightBlock.width = WALL_THICKNESS;
    walls.rightBlock.height = WINDOW_HEIGHT;
    walls.rightBlock.color = WALL_COLOR;
    walls.rightBlock.rectangle.setSize(sf::Vector2(walls.rightBlock.width, walls.rightBlock.height));
    walls.rightBlock.rectangle.setPosition(walls.rightBlock.left, walls.rightBlock.top);
    walls.rightBlock.rectangle.setFillColor(WALL_COLOR);

    //initialize bottom wall
    walls.bottomBlock.left = 0.0;
    walls.bottomBlock.top = WINDOW_HEIGHT - WALL_THICKNESS;
    walls.bottomBlock.width = WINDOW_WIDTH;
    walls.bottomBlock.height = WALL_THICKNESS;
    walls.bottomBlock.color = WALL_COLOR;
    walls.bottomBlock.rectangle.setSize(sf::Vector2(walls.bottomBlock.width, walls.bottomBlock.height));
    walls.bottomBlock.rectangle.setPosition(walls.bottomBlock.left, walls.bottomBlock.top);
    walls.bottomBlock.rectangle.setFillColor(WALL_COLOR);

    //initialize paddle
    paddle.block.left = (WINDOW_WIDTH - PADDLE_WIDTH) / 2.0;
    paddle.block.top = WINDOW_HEIGHT - (2.0 * PADDLE_THICKNESS);
    paddle.block.width = PADDLE_WIDTH;
    paddle.block.height = PADDLE_THICKNESS;
    paddle.block.color = PADDLE_COLOR;
    paddle.block.rectangle.setSize(sf::Vector2(paddle.block.width, paddle.block.height));
    paddle.block.rectangle.setFillColor(PADDLE_COLOR);
    paddle.velocityX = 0.0;
    paddle.velocityY = 0.0;



    //initialize ball
    ball = { BALL_RADIUS,
             paddle.block.left + (paddle.block.width / 2),
             paddle.block.top - BALL_RADIUS - 1 ,
             0.0 ,
             0.0 ,
             BALL_COLOR};



    //initialize bricks
    float bricksTop = FIRST_BRICK; // start at lowest brick row

    Brick *pNext = &bricks[0][0]; // get pointer to 1st brick
    for(int row = 0; row < BRICK_ROWS; row++)
    {
        float bricksLeft = BRICKS_LEFT;// start back at far left

        for(int column = 0; column < BRICK_COLUMNS; column++)
        {

            pNext->block.left = bricksLeft +1; // 1 pixel right to leave blank pixel on left

            // offset left/top by 1 pixel to give each brick a border
            pNext->block.left = bricksLeft + 1;
            pNext->block.top = bricksTop + 1;

            // subtract 2 from width/height to make room for the 1 pixel
            //border on each side

            pNext->block.width = BRICK_WIDTH - 2;
            pNext->block.height = BRICK_HEIGHT - 2;


            // set row specific properties
            if (row < 2)
            {
                pNext->block.color = sf::Color::Yellow;
                pNext->points = 1;
                pNext->speedAdjust = 0;
            }

            else if (row < 4)
            {
                pNext->block.color = sf::Color::Green;
                pNext->points = 3;
                pNext->speedAdjust = 0;
            }

            else if (row < 6)
            {
                pNext->block.color = sf::Color(255,165,0);
                pNext->points = 5;
                pNext->speedAdjust = 1;
            }

            else
            {
                pNext->block.color = sf::Color::Red;
                pNext->points = 7;
                pNext->speedAdjust = 2;
            }

            // set the drawing rectangle
            pNext->block.rectangle.setSize(sf::Vector2f(pNext->block.width, pNext->block.height));
            pNext->block.rectangle.setFillColor(pNext->block.color);
            pNext->block.rectangle.setPosition(pNext->block.left, pNext->block.top);

            // initialize hit flag off so brick is displayed
            pNext->hit = false;

            pNext++; // get next brick

            bricksLeft += BRICK_WIDTH; //move next brick to the right
        }//brick column

        bricksTop -= BRICK_HEIGHT; // move up to next row
    }//brick row
}
//end setup

/**
 *Checks the collision points when the ball touches the block
 * @param pBall: represents the ball's collision points
 * @param pBlock: represent each block's collision points
 * @return if the ball collided
 */

bool collisionCheck(Ball *pBall, Block *pBlock){
    bool collision = false;

    //grabbing return variable
    int getCollisionPoint = collisionPoint(pBall, pBlock);

    //if it is touching wall
    if(getCollisionPoint != 0)
    {
        collision = true;

        //the bounce for x coordinate
        if((getCollisionPoint > 255) && (getCollisionPoint < 315))
        {
            pBall->velocityX *= -1;
            pBall->coordinateX = pBlock->left + pBlock->width + pBall->radius + 1;
        }
        else if((getCollisionPoint > 45) && (getCollisionPoint < 135))
        {
            pBall->velocityX *= -1;
            pBall->coordinateX = pBlock->left - pBall->radius - 1;
        }

        //the bounce for y coordinate
        if((getCollisionPoint >= 315) || (getCollisionPoint <= 45))
        {
            pBall->velocityY *= -1;
            pBall->coordinateY = pBlock->top + pBlock->height + pBall->radius + 1;
        }
        else if((getCollisionPoint >= 135) && (getCollisionPoint <= 225))
        {
            pBall->velocityY *= -1;
            pBall->coordinateY = pBlock->top - pBall->radius - 1;
        }
    }
    return collision;
}//end of collision check


/**
 *  checking collision between paddle and walls
 * @param moving
 * @param stationary
 * @return
 */
bool checkBlockCollision (Block moving, Block stationary)
{
    bool collision;
    collision = false;

    if (moving.left < stationary.left + stationary.width &&
        moving.left + moving.width > stationary.left &&
        moving.top < stationary.top + stationary.height &&
        moving.top + moving.height > stationary.top)
    {
        collision = true;
    }
    return collision;
}


/**
 * Compares position of ball and blocks
 * @param pBall - represents the ball's direction
 * @param pBlock - represent each block's direction
 * @return the direction of the ball
 */

int collisionPoint(Ball *pBall, Block *pBlock) {
    int heading = 0;

    float checkPointX;
    float checkPointY;

    //--- ball is left of wall---
    if(pBall->coordinateX < pBlock->left)
    {
        checkPointX = pBlock->left;
    }

        //---ball is right of  wall---
    else if (pBall->coordinateX > pBlock->left + pBlock->width)
    {
        checkPointX = pBlock->left + pBlock->width;
    }

        //---center of ball coord x---
    else
    {
        checkPointX = pBall->coordinateX;
    }

    //---ball is above wall---
    if (pBall->coordinateY < pBlock->top)
    {
        checkPointY = pBlock->top;
    }
        //---ball is below wall---
    else if (pBall->coordinateY > pBlock->top + pBlock->height)
    {
        checkPointY = pBlock->top + pBlock->height;
    }

        // center of ball coord y ---
    else
    {
        checkPointY = pBall->coordinateY;
    }

    float diffX;
    float diffY;

    diffX = checkPointX - pBall->coordinateX;
    diffY = (WINDOW_HEIGHT - checkPointY) - (WINDOW_HEIGHT - pBall->coordinateY);


    double distance = std::sqrt(pow(diffX, 2.0) + pow(diffY,2.0));

    //if ball radius is less than the distance of the wall, to keep the ball from phasing through wall
    if(distance <= pBall->radius)
    {
        double theta = std::atan2(diffY, diffX);

        double degrees = 90.0 - theta * 180 / M_PI;

        if(degrees <= 0)
        {
            degrees += 360;
        }
        heading = int(degrees);
    }

    return heading;
}


/**
 * convert user keyboard input into recognized integer values
 * for left=1/up=2/right=3/down=4
 * @return int - user input (default no-input=0, quit=-1)
 */

Direction processInput() {
    Direction input = None;  // no input

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
        input = Left;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
        input = Up;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
        input = Right;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
        input = Down;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::X)) {
        input = Exit;
    }
    else if (sf::Keyboard::isKeyPressed(sf::Keyboard::Space)) {
        input = Start;
    }

    return input;
} // end getUserInput

/**
 *
 * @param ball - check collision of ball
 * @param paddle - check collision of paddle
 * @param bricks - check collision of bricks
 * @param walls - check collision of walls
 * @return
 */

bool doCollisionChecks(Ball &ball, MovingBlock& paddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], Borders walls)
{
    bool gameOver = false;

    // --- check vertical collisions ---
    if (!collisionCheck(&ball, &paddle.block))
    {
        if (!collisionCheck(&ball, &walls.topBlock))
        {
            gameOver = collisionCheck(&ball, &walls.bottomBlock);
        }
    }
    // --- check horizontal collisions ---
    if (!collisionCheck(&ball, &walls.leftBlock))
    {
        collisionCheck(&ball, &walls.rightBlock);
    }

    //collision between paddle and walls
    if(checkBlockCollision(paddle.block , walls.leftBlock))
    {
        paddle.velocityX = 0.0;
        paddle.block.left =  walls.leftBlock.left + walls.leftBlock.width + 1;
    }

    else if(checkBlockCollision(paddle.block , walls.rightBlock))
    {
        paddle.velocityX = 0.0;
        paddle.block.left =  walls.rightBlock.left - paddle.block.width - 1;
    }

    Brick *pBrick = &bricks[0][0];
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int column = 0; column < BRICK_COLUMNS; column++)
        {
            if (!pBrick->hit)
            {
               pBrick->hit = collisionCheck(&ball, &pBrick->block);
            }

            pBrick++;
        }//columns
    }//rows

    return gameOver;
}

/**
 * update the state of game objects
 * @param input - user keyboard input
 * @param ball  - update ball position and speed
 * @param delta - current frame time
 * @param walls - update walls collision
 * @param bricks - brick set up and hiding when hit
 * @param started - to start the game
 */
bool update(Direction &input, bool &started, Ball &ball, MovingBlock &paddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], Borders walls , float delta) {

    //start the game
    bool gameOver = false;

    // adjust velocity directions for user input
    if (input) {
        switch (input) {
            case Left:
                paddle.velocityX -= PADDLE_SPEED;
                break;

            case Right:
                paddle.velocityX += PADDLE_SPEED;
                break;

            case Start:
                if(!started)
                {
                    ball.velocityX = BALL_SPEED_X;
                    ball.velocityY = BALL_SPEED_Y * -1;

                    /* randomly set horizontal velocity to positive or negative
                 by seeing if the tenths place of current delta
                 (i.e. 1st digit after the decimal) is odd or even.  */
                    if ((int(delta * 10) & 1) % 2)
                    {
                        ball.velocityX *= -1;  // ball goes left if odd
                    }

                    started = true;
                }
                break;
        }
        // clear input
        input = None;
    }

    //adjust the location of the x coordinate for paddle
    paddle.block.left += paddle.velocityX * delta;

    if(started)
    {
        // adjust the location of the ball for speed * time
        ball.coordinateX += ball.velocityX * delta;
        ball.coordinateY += ball.velocityY * delta;
    }
    else
    {
        ball.coordinateX = paddle.block.left + (paddle.block.width / 2);
        ball.coordinateY = paddle.block.top - ball.radius - 1;
    }


    //collisions between ball and paddle

    gameOver = doCollisionChecks(ball, paddle, bricks, walls);

    return gameOver;
} // end update


/**
 * draw the ball on the graphics window
 * @param window - handle to open graphics window
 * @param ball   - structure variable with properties for the ball
 * @param delta  - amount of frame time plus lag (in ms)
 * @param bricks - structure of bricks
 * @param theWalls - structure variable with properties for the walls
 * @param paddle - structure variable with properties for the paddle
 */
void render(sf::RenderWindow &window, Ball ball, MovingBlock paddle, Brick bricks[BRICK_ROWS][BRICK_COLUMNS], Borders walls, float delta) {
    // Render drawing objects
    // ------------------------------------------------
    // clear the window with the background color
    window.clear(WINDOW_COLOR);

    // draw the ball
    // ------------------------------------------------
    sf::CircleShape circle;
    circle.setFillColor(ball.color);
    circle.setRadius(ball.radius);
    // set screen coordinates relative to the center of the circle
    circle.setOrigin(ball.radius / 2.0 , ball.radius / 2.0);
    // calculate current drawing location relative to speed and frame-time
    float xPosition = ball.coordinateX + ball.velocityX * delta;
    float yPosition = ball.coordinateY + ball.velocityY * delta;
    circle.setPosition(xPosition, yPosition);
    window.draw(circle);

    //draw paddle
    //---------------------------------------------------

    paddle.block.left += paddle.velocityX * delta;
    paddle.block.top += paddle.velocityY * delta;

    paddle.block.rectangle.setPosition(paddle.block.left, paddle.block.top);

    window.draw(paddle.block.rectangle);

    //draw bricks
    //---------------------------------------------------------------
    Brick *pBrick = &bricks[0][0];
    for (int row = 0; row < BRICK_ROWS; row++)
    {
        for (int column = 0; column < BRICK_COLUMNS; column++)
        {
            if (!pBrick->hit)
            {
                window.draw(pBrick->block.rectangle);
            }
            pBrick++;
        }//columns
    }//rows


    //draw the borders
    //------------------------------------------------
    window.draw(walls.leftBlock.rectangle);
    window.draw(walls.bottomBlock.rectangle);
    window.draw(walls.rightBlock.rectangle);
    window.draw(walls.topBlock.rectangle);
    // show the new window
    // ------------------------------------------------
    window.display();
} // end render