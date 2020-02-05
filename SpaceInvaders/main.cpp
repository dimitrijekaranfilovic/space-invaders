#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#define SCREEN_HEIGHT 400
/*TODO: stavi nagrade:
1.dva metka istovremeno
2.brze kretanje
3.neunistivost
*/
bool squareSquareCollision(float x1, float y1, float x2, float y2, int w1, int w2)
{
	if (x1 + w1 > x2 && x2 + w2 > x1 && y1 + w1 > y2 && y2 + w2 > y1)
		return true;
	return false;
}


struct Prize
{
	enum Kind { SPEED = 0, DOUBLE_POINT = 1, INDESTRUCTIBLE = 2};
	int kind;
	float px;
	float py;
	float speed;
	bool collected = false;
};

struct Ship
{
	float px = 150.0f;
	float py = 350.0f;
	float speed = 2.0f;
	int w = 10;
	int h = 10;
	bool indestructible = false;
	float speedQuotient = 1.0f;

};

struct Bullet
{
	float px, py;
	int w, h;
	int speed;
	bool used = false;
	Bullet(float x, float y, int width, int height, int s) : px(x), py(y), w(width), h(height), speed(s) {}
};

struct Obstacle
{
	float px, py;
	int w, h;
	float speed;
	bool destroyed = false;
	Obstacle(float x, float y, int width, int height, float s) : px(x), py(y), w(width), h(height), speed(s) {}
};



class Example : public olc::PixelGameEngine
{

public:
	olc::Sprite shipSprite;
	olc::Sprite bulletSprite;
	olc::Sprite meteorSprite;
	olc::Sprite indestructibleSprite;
	olc::Sprite speedSprite;
	olc::Sprite doublePointSprite;
	Ship ship;
	int pointCount = 1;
	int score = 0;
	int timeBound = 5;
	int numObstacles = 1;
	int scoreLowerBound = 0;
	int scoreUpperBound = 5;
	int quotient = -100;
	float obstacleSpeed = 0.5f;
	float timePassed = 0;
	int bulletSpeed = 3;
	float prizeSpeed = 0.8f;
	std::vector<Bullet> bullets;
	std::vector<Obstacle> obstacles;
	std::vector<Prize> prizes;
	
	Example()
	{
		sAppName = "Space invaders";
	}
public:
	bool OnUserCreate() override
	{
		shipSprite.LoadFromFile("../resources/spaceship2.png");
		bulletSprite.LoadFromFile("../resources/bullet1.png");
		meteorSprite.LoadFromFile("../resources/meteor4.png");
		speedSprite.LoadFromFile("../resources/speed1.png");
		doublePointSprite.LoadFromFile("../resources/two1.png");
		indestructibleSprite.LoadFromFile("../resources/strength1.png");
		SetPixelMode(olc::Pixel::MASK);
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		if ((GetKey(olc::Key::LEFT).bHeld || GetKey(olc::Key::A).bHeld) && ship.px > 0)
			ship.px -= ship.speed * ship.speedQuotient;

		if ((GetKey(olc::Key::RIGHT).bHeld || GetKey(olc::Key::D).bHeld) && ship.px < ScreenWidth() - shipSprite.width)
			ship.px += ship.speed * ship.speedQuotient;


		if (GetKey(olc::Key::SPACE).bPressed)
		{
			Bullet b((ship.px + bulletSprite.width / 2), ship.py, bulletSprite.width, bulletSprite.height, bulletSpeed);
			bullets.push_back(b);
		}

		//update bullets' positions
		for (int i = 0; i < bullets.size(); i++)
			bullets[i].py -= bullets[i].speed;

		//add obstacles
		timePassed += fElapsedTime;
		//std::cout << score << ", " << scoreLowerBound << ", " << scoreUpperBound << ", " << timeBound << std::endl;
		if ((score >= scoreLowerBound) && (score < scoreUpperBound) && (timePassed > timeBound))
		{
			for (int i = 0; i < numObstacles; ++i)
			{
				Obstacle o(rand() % ScreenWidth(), quotient * i, meteorSprite.width, meteorSprite.height, obstacleSpeed);
				obstacles.push_back(o);
			}
			timePassed = 0.0f;
		}

		//correct distance between obstacles
		if (obstacles.size() > 3)
			quotient *= 2.0;
		if (obstacles.size() <= 3)
			quotient = -100;


		//update obstacles' positions
		for (int i = 0; i < obstacles.size(); ++i)
		{
			if (squareSquareCollision(ship.px, ship.py, obstacles[i].px, obstacles[i].py, ship.w, obstacles[i].w))
			{
				std::cout << "GAME OVER!\n" << "Your score was: " << score << std::endl;
				std::this_thread::sleep_for(std::chrono::milliseconds(2000));
				exit(0);
			}
			else
			{
				obstacles[i].py += obstacles[i].speed;
			}
		}

		


		//check if any of the obstacles was destroyed and update parameters
		for (int i = 0; i < obstacles.size(); ++i)
		{
			for (int j = 0; j < bullets.size(); ++j)
			{
				if (squareSquareCollision(obstacles[i].px, obstacles[i].py, bullets[j].px, bullets[j].py, obstacles[i].w, bullets[j].w))
				{
					obstacles[i].destroyed = true;
					bullets[j].used = true;
					score += pointCount;
					if (score % 5 == 0)
					{
						numObstacles += 1;
						if (timeBound > 1)
							timeBound -= 1;
						if (timeBound < 0)
							timeBound = 1;
						scoreLowerBound += 5;
						scoreUpperBound += 5;
						obstacleSpeed *= 1.15f;
					}
				}
			}
		}

		//add prizes
		int n = rand() % 5500;
		if (n < 3)
		{
			Prize p;
			p.px = rand() % ScreenWidth();
			p.py = 0;
			p.kind = rand() % 3;
			p.speed = prizeSpeed;
			prizes.push_back(p);
		}

		//update prizes' position
		for (int i = 0; i < prizes.size(); ++i)
			prizes[i].py += prizes[i].speed;

		//remove bullets that went out of bounds or have destroyed an obstacle
		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) {return b.used || b.py < 0; }), bullets.end());

		//remove obstacles that went out of bounds or have been destroyed
		obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(), [](const Obstacle& o) {return o.destroyed || o.py > SCREEN_HEIGHT; }), obstacles.end());

		//remove prizes that were collected or went out ouf bounds
		prizes.erase(std::remove_if(prizes.begin(), prizes.end(), [](const Prize& p) {return p.collected || p.py > SCREEN_HEIGHT; }), prizes.end());

		//clear screen	
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		//draw ship
		DrawSprite(ship.px, ship.py, &shipSprite);


		//draw bullets
		for (int i = 0; i < bullets.size(); ++i)
			DrawSprite(bullets[i].px, bullets[i].py, &bulletSprite);

		//draw obstacles
		for (int i = 0; i < obstacles.size(); ++i)
			DrawSprite(obstacles[i].px, obstacles[i].py, &meteorSprite);
		

		//draw prizes
		for (int i = 0; i < prizes.size(); ++i)
		{
			olc::Sprite* pointer = nullptr;
			if (prizes[i].kind == Prize::SPEED)
				pointer = &speedSprite;
			else if (prizes[i].kind == Prize::DOUBLE_POINT)
				pointer = &doublePointSprite;
			else if (prizes[i].kind == Prize::INDESTRUCTIBLE)
				pointer = &indestructibleSprite;
			DrawSprite(prizes[i].px, prizes[i].py, pointer);
			//std::cout << "Crtam nagradu na poziciji: " << prizes[i].px << ", " << prizes[i].py << std::endl;
		}

		//display score
		DrawString(0, 0, "Score: " + std::to_string(score), olc::DARK_YELLOW);

		return true;
	}
};
int main()
{
	Example demo;
	if (demo.Construct(400, SCREEN_HEIGHT, 2, 2))
		demo.Start();
		
	

	return 0;
}