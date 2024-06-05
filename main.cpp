#include <SFML/Graphics.hpp>
#include <cmath>
#include <vector>
#include <string>

using namespace sf;

const int numCheckpoints = 9; // Number of checkpoints
int points[numCheckpoints][2] = {
        300, 610, 1270, 430, 1380, 2380, 1900, 2460, 1970, 1700,
        2550, 1680, 2560, 3150, 500, 3300, 300, 1800
};

struct Car
{
    float x, y, speed, angle;
    int n;
    int lap;
    int checkpointsCollected;
    std::vector<bool> collectedCheckpoints;

    Car() : lap(0), checkpointsCollected(0)
    {
        speed = 2;
        angle = 0;
        n = 0;
        collectedCheckpoints.resize(numCheckpoints, false);
    }

    void move(float bgWidth, float bgHeight, float carWidth, float carHeight)
    {
        x += sin(angle) * speed;
        y -= cos(angle) * speed;

        // Boundary checking
        if (x < carWidth / 2) x = carWidth / 2;
        if (x > bgWidth - carWidth / 2) x = bgWidth - carWidth / 2;
        if (y < carHeight / 2) y = carHeight / 2;
        if (y > bgHeight - carHeight / 2) y = bgHeight - carHeight / 2;
    }

    void findTarget()
    {
        float tx = points[n][0];
        float ty = points[n][1];
        float beta = angle - atan2(tx - x, -ty + y);
        if (sin(beta) < 0) angle += 0.005 * speed; else angle -= 0.005 * speed;
        if ((x - tx) * (x - tx) + (y - ty) * (y - ty) < 25 * 25) {
            if (!collectedCheckpoints[n]) {
                collectedCheckpoints[n] = true;
                checkpointsCollected++;
            }
            n = (n + 1) % numCheckpoints;
            if (n == 0) lap++;
        }
    }
};

int main()
{
    Texture t1, t2;
    t1.loadFromFile("C:\\projects\\c++\\labs\\SFML-games\\images\\background.png");
    t2.loadFromFile("C:\\projects\\c++\\labs\\SFML-games\\images\\car.png");
    t1.setSmooth(true);
    t2.setSmooth(true);

    Sprite sBackground(t1), sCar(t2);
    sBackground.scale(2, 2);

    sCar.setOrigin(22, 22);
    float R = 22;

    // Background dimensions (scaled)
    float bgWidth = t1.getSize().x * sBackground.getScale().x;
    float bgHeight = t1.getSize().y * sBackground.getScale().y;
    float carWidth = sCar.getGlobalBounds().width;
    float carHeight = sCar.getGlobalBounds().height;

    RenderWindow app(VideoMode(1500, 850), "Car Racing Game!");
    app.setFramerateLimit(60);

    const int N = 5;
    Car car[N];
    for (int i = 0; i < N; i++)
    {
        car[i].x = 300 + i * 50;
        car[i].y = 1700 + i * 80;
        car[i].speed = 7 + i;
    }

    float speed = 0, angle = 0;
    float maxSpeed = 12.0;
    float acc = 0.2, dec = 0.3;
    float turnSpeed = 0.08;

    View view(FloatRect(0, 0, 1200, 800));
    app.setView(view);

    // Create checkpoints
    std::vector<CircleShape> checkpoints;
    for (int i = 0; i < numCheckpoints; i++)
    {
        CircleShape checkpoint(12);
        checkpoint.setFillColor(Color::Yellow);
        checkpoint.setOrigin(12, 12);
        checkpoint.setPosition(points[i][0], points[i][1]);
        checkpoints.push_back(checkpoint);
    }

    bool gameOver = false;

    while (app.isOpen())
    {
        Event e;
        while (app.pollEvent(e))
        {
            if (e.type == Event::Closed)
                app.close();
        }

        bool Up = 0, Right = 0, Down = 0, Left = 0;
        if (Keyboard::isKeyPressed(Keyboard::Up)) Up = 1;
        if (Keyboard::isKeyPressed(Keyboard::Right)) Right = 1;
        if (Keyboard::isKeyPressed(Keyboard::Down)) Down = 1;
        if (Keyboard::isKeyPressed(Keyboard::Left)) Left = 1;

        // car movement
        if (!gameOver)
        {
            if (Up && speed < maxSpeed)
                if (speed < 0)  speed += dec;
                else  speed += acc;

            if (Down && speed > -maxSpeed)
                if (speed > 0) speed -= dec;
                else  speed -= acc;

            if (!Up && !Down)
                if (speed - dec > 0) speed -= dec;
                else if (speed + dec < 0) speed += dec;
                else speed = 0;

            if (Right && speed != 0)  angle += turnSpeed * speed / maxSpeed;
            if (Left && speed != 0)   angle -= turnSpeed * speed / maxSpeed;

            car[0].speed = speed;
            car[0].angle = angle;
        }

        for (int i = 0; i < N; i++) car[i].move(bgWidth, bgHeight, carWidth, carHeight);
        for (int i = 1; i < N; i++) car[i].findTarget();

        // Check if player reaches a checkpoint
        float px = car[0].x;
        float py = car[0].y;
        for (int i = 0; i < numCheckpoints; i++)
        {
            if (!car[0].collectedCheckpoints[i])
            {
                float cx = points[i][0];
                float cy = points[i][1];
                if ((px - cx) * (px - cx) + (py - cy) * (py - cy) < 25 * 25)
                {
                    if (i == numCheckpoints - 1) // Last checkpoint
                    {
                        // Check if all previous checkpoints are collected
                        bool previousCollected = true;
                        for (int j = 0; j < i; j++)
                        {
                            if (!car[0].collectedCheckpoints[j])
                            {
                                previousCollected = false;
                                break;
                            }
                        }
                        if (previousCollected)
                        {
                            car[0].collectedCheckpoints[i] = true;
                            car[0].checkpointsCollected++;
                            if (car[0].checkpointsCollected == numCheckpoints)
                            {
                                gameOver = true;
                            }
                        }
                    }
                    else
                    {
                        car[0].collectedCheckpoints[i] = true;
                        car[0].checkpointsCollected++;
                    }
                }
            }
        }

        if (gameOver)
        {
            app.close();
        }

        app.clear(Color::Black); // Set the background to black instead of white

        int offsetX = car[0].x - view.getSize().x / 2;
        int offsetY = car[0].y - view.getSize().y / 2;

        // Constrain view offset to the background boundaries
        if (offsetX < 0) offsetX = 0;
        if (offsetY < 0) offsetY = 0;
        if (offsetX > bgWidth - view.getSize().x) offsetX = bgWidth - view.getSize().x;
        if (offsetY > bgHeight - view.getSize().y) offsetY = bgHeight - view.getSize().y;

        view.setCenter(offsetX + view.getSize().x / 2, offsetY + view.getSize().y / 2);
        app.setView(view);

        sBackground.setPosition(0, 0);
        app.draw(sBackground);

        // Draw checkpoints
        for (int i = 0; i < numCheckpoints; i++)
        {
            if (!car[0].collectedCheckpoints[i])
                app.draw(checkpoints[i]);
        }

        Color colors[10] = { Color::Red, Color::Green, Color::Magenta, Color::Blue, Color::White };

        for (int i = 0; i < N; i++)
        {
            sCar.setPosition(car[i].x, car[i].y);
            sCar.setRotation(car[i].angle * 180 / 3.141593);
            sCar.setColor(colors[i]);
            app.draw(sCar);
        }

        app.display();
    }

    return 0;
}
