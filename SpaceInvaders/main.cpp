#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#include <unordered_map> 
#define SCREEN_HEIGHT 400

bool squareSquareCollision(float x1, float y1, float x2, float y2, int w1, int w2)
{
	if (x1 + w1 > x2 && x2 + w2 > x1 && y1 + w1 > y2 && y2 + w2 > y1)
		return true;
	return false;
}

struct Prize
{
	enum Kind { SPEED = 0, DOUBLE_POINT = 1, INDESTRUCTIBLE = 2 };
	int kind;
	float px;
	float py;
	bool collected = false;
};

struct Ship
{
	float px = 150.0f;
	float py = 350.0f;
	float speed = 2.0f;
	bool indestructible = false;
};

struct Bullet
{
	float px, py;
	bool used = false;
	Bullet(float x, float y) : px(x), py(y){}
};

struct Obstacle
{
	float px, py;
	bool destroyed = false;
	Obstacle(float x, float y) : px(x), py(y){}
};



class SpaceInvaders : public olc::PixelGameEngine
{

public:
	olc::Sprite shipSprite;
	olc::Sprite bulletSprite;
	olc::Sprite meteorSprite;
	olc::Sprite indestructibleSprite;
	olc::Sprite speedSprite;
	olc::Sprite doublePointSprite;
	Ship ship;
	int prizeDurationLimit = 5;
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
	std::unordered_map<int, float> prizeDurationMap;
	bool gameOver = false;

	SpaceInvaders()
	{
		sAppName = "Space invaders";
	}
public:
	bool OnUserCreate() override
	{
		shipSprite.LoadFromFile("../resources/spaceship21.png"); 
		bulletSprite.LoadFromFile("../resources/bullet14.png"); 
		meteorSprite.LoadFromFile("../resources/meteor12.png");  
		speedSprite.LoadFromFile("../resources/speed9.png");
		doublePointSprite.LoadFromFile("../resources/two5.png"); 
		indestructibleSprite.LoadFromFile("../resources/strength9.png"); 
		SetPixelMode(olc::Pixel::MASK);
		prizeDurationMap[Prize::SPEED] = 0.0f;
		prizeDurationMap[Prize::INDESTRUCTIBLE] = 0.0f;
		prizeDurationMap[Prize::DOUBLE_POINT] = 0.0f;
		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		if ((GetKey(olc::Key::LEFT).bHeld || GetKey(olc::Key::A).bHeld) && ship.px > 0 && !gameOver)
			ship.px -= ship.speed;

		if ((GetKey(olc::Key::RIGHT).bHeld || GetKey(olc::Key::D).bHeld) && (ship.px < ScreenWidth() - shipSprite.width) && !gameOver)
			ship.px += ship.speed;


		if (GetKey(olc::Key::SPACE).bPressed && !gameOver)
		{
			Bullet b(ship.px, ship.py);
			bullets.push_back(b);
		}
		//start a new game
		if (gameOver && GetKey(olc::Key::ENTER).bPressed)
		{
			obstacles.clear();
			bullets.clear();
			prizes.clear();
			scoreLowerBound = 0;
			scoreUpperBound = 5;
			numObstacles = 1;
			score = 0;
			timeBound = 5;
			gameOver = false;
			pointCount = 1;
			ship.speed = 2.0f;
			obstacleSpeed = 0.5f;
			ship.indestructible = false;
			ship.px = 150.0f;
			ship.py = 350.0f;
		}

		//update bullets' positions
		for (unsigned int i = 0; i < bullets.size(); i++)
		{
			if(!gameOver)
				bullets[i].py -= bulletSpeed;
		}
			
		//add obstacles
		timePassed += fElapsedTime;
		if ((score >= scoreLowerBound) && (score < scoreUpperBound) && (timePassed > timeBound))
		{
			for (int i = 0; i < numObstacles; ++i)
			{
				if (!gameOver) 
				{
					Obstacle o(rand() % ScreenWidth(), quotient * i * 1.0f);
					obstacles.push_back(o);
				}
			}
			timePassed = 0.0f;
		}

		//correct distance between obstacles
		if (obstacles.size() > 3)
			quotient *= 2;
		if (obstacles.size() <= 3)
			quotient = -100;

		//update obstacles' positions
		for (unsigned int i = 0; i < obstacles.size(); ++i)
		{
			if (squareSquareCollision(ship.px, ship.py, obstacles[i].px, obstacles[i].py, shipSprite.width - 5, meteorSprite.width - 5) && !ship.indestructible)
			{
				gameOver = true;
			}
			else
			{
				if(!gameOver)
					obstacles[i].py += obstacleSpeed;
			}
		}

		
		//see if prizes' effects have worn off
		if (prizeDurationMap[Prize::SPEED] <= 0.0f)
		{
			prizeDurationMap[Prize::SPEED] = 0.0f;
			ship.speed = 2.0f;
		}
		else
			prizeDurationMap[Prize::SPEED] = prizeDurationMap[Prize::SPEED] - fElapsedTime;


		if (prizeDurationMap[Prize::DOUBLE_POINT] <= 0.0f)
		{
			prizeDurationMap[Prize::DOUBLE_POINT] = 0.0f;
			pointCount = 1;
		}
		else
			prizeDurationMap[Prize::DOUBLE_POINT] = prizeDurationMap[Prize::DOUBLE_POINT] - fElapsedTime;


		if (prizeDurationMap[Prize::INDESTRUCTIBLE] <= 0.0f)
		{
			prizeDurationMap[Prize::INDESTRUCTIBLE] = 0.0f;
			ship.indestructible = false;
		}
			
		else
			prizeDurationMap[Prize::INDESTRUCTIBLE] = prizeDurationMap[Prize::INDESTRUCTIBLE] - fElapsedTime;
		


		//check if any of the prizes was collected and apply its effects
		for (unsigned int i = 0; i < prizes.size(); ++i)
		{
			if (squareSquareCollision(ship.px, ship.py, prizes[i].px, prizes[i].py, shipSprite.width, speedSprite.width))
			{
				prizes[i].collected = true;
				switch (prizes[i].kind)
				{
				case Prize::DOUBLE_POINT:
					pointCount = 2;
					prizeDurationMap[Prize::DOUBLE_POINT] = prizeDurationMap[Prize::DOUBLE_POINT] + prizeDurationLimit;
					break;
				case Prize::INDESTRUCTIBLE:
					ship.indestructible = true;
					prizeDurationMap[Prize::INDESTRUCTIBLE] = prizeDurationMap[Prize::INDESTRUCTIBLE] + prizeDurationLimit;
					break;
				case Prize::SPEED:
					ship.speed = 6.0f;
					prizeDurationMap[Prize::SPEED] = prizeDurationMap[Prize::SPEED] + prizeDurationLimit;
					break;
				default:
					break;
				}
			}
		}
		//check if any of the obstacles were destroyed and update parameters
		for (unsigned int i = 0; i < obstacles.size(); ++i)
		{
			for (unsigned int j = 0; j < bullets.size(); ++j)
			{
				if (squareSquareCollision(obstacles[i].px, obstacles[i].py, bullets[j].px, bullets[j].py, meteorSprite.width-5, bulletSprite.width-5))
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
		if (n < 3) //n < 3
		{
			Prize p;
			p.px = rand() % ScreenWidth();
			p.py = 0;
			p.kind = rand() % 3;
			prizes.push_back(p);
		}

		//update prizes' position
		for (unsigned int i = 0; i < prizes.size(); ++i)
		{
			if(!gameOver)
				prizes[i].py += prizeSpeed;
		}
			
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
		for (unsigned int i = 0; i < bullets.size(); ++i)
			DrawSprite(bullets[i].px, bullets[i].py, &bulletSprite);

		//draw obstacles
		for (unsigned int i = 0; i < obstacles.size(); ++i)
			DrawSprite(obstacles[i].px, obstacles[i].py, &meteorSprite);

		//draw prizes
		for (unsigned int i = 0; i < prizes.size(); ++i)
		{
			olc::Sprite* pointer = nullptr;
			if (prizes[i].kind == Prize::SPEED)
				pointer = &speedSprite;
			else if (prizes[i].kind == Prize::DOUBLE_POINT)
				pointer = &doublePointSprite;
			else if (prizes[i].kind == Prize::INDESTRUCTIBLE)
				pointer = &indestructibleSprite;
			DrawSprite(prizes[i].px, prizes[i].py, pointer);
		}

		//display score
		DrawString(0, 0, "Score: " + std::to_string(score), olc::DARK_YELLOW);

		int y = 0;
		
		//prize time remaining
		if (ship.indestructible)
		{
			DrawString(100, y, "Indestructible time remaining: " + std::to_string(prizeDurationMap[Prize::INDESTRUCTIBLE]), olc::DARK_YELLOW);
			y += 15;
		}
			
		if (pointCount > 1)
		{
			DrawString(100, y, "Double point time remaining: " + std::to_string(prizeDurationMap[Prize::DOUBLE_POINT]), olc::DARK_YELLOW);
			y += 15;
		}
			
		if (ship.speed > 2.0f)
			DrawString(100, y, "Speed boost time remaining: " + std::to_string(prizeDurationMap[Prize::SPEED]), olc::DARK_YELLOW);
		

		if (gameOver)
			DrawString(ScreenWidth() / 2 - 50, ScreenHeight() / 2, "GAME OVER!", olc::DARK_RED, 3);
		

		return true;
	}
};
int main()
{
	SpaceInvaders demo;
	if (demo.Construct(400, SCREEN_HEIGHT, 2, 2))
		demo.Start();
	return 0;
}