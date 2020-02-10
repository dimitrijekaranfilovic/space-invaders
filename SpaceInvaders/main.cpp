#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include "Objects.h"
#include <vector>
#include <unordered_map> 
#define SCREEN_HEIGHT 400
using namespace std;

bool squareSquareCollision(float x1, float y1, float x2, float y2, int w1, int w2)
{
	if (x1 + w1 > x2 && x2 + w2 > x1 && y1 + w1 > y2 && y2 + w2 > y1)
		return true;
	return false;
}


class SpaceInvaders : public olc::PixelGameEngine
{

public:
	olc::Sprite shipSprite;
	olc::Sprite bulletSprite;
	olc::Sprite meteorSprite;
	olc::Sprite indestructibleSprite;
	olc::Sprite speedSprite;
	olc::Sprite doublePointSprite;
	olc::Sprite bossSprite;
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
	std::vector<Star> stars;
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
		speedSprite.LoadFromFile("../resources/speed10.png");
		doublePointSprite.LoadFromFile("../resources/two5.png"); 
		indestructibleSprite.LoadFromFile("../resources/strength9.png"); 
		bossSprite.LoadFromFile("../resources/boss3.png");
		SetPixelMode(olc::Pixel::MASK);
		prizeDurationMap[Prize::SPEED] = 0.0f;
		prizeDurationMap[Prize::INDESTRUCTIBLE] = 0.0f;
		prizeDurationMap[Prize::DOUBLE_POINT] = 0.0f;

		//add stars to random positions
		for (unsigned int i = 0; i < 25; ++i)
		{
			int r = rand() % 4 + 1;
			int x = r + (std::rand() % (ScreenWidth() - 2 * r + 1));
			int y = r + (std::rand() % (SCREEN_HEIGHT - 2 * r + 1));
			Star s(x * 1.0f, y * 1.0f, r * 1.0f);
			stars.push_back(s);
		}
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
					int x = meteorSprite.width + (std::rand() % (ScreenWidth() -  2 * meteorSprite.width + 1));
					Obstacle o(x * 1.0f, quotient * i * 1.0f);
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
					if (score % 5 == 0 || (pointCount == 2 && score % 5 == 1))
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

		//add boss



		//add prizes
		int n = rand() % 5500;
		if (n < 3 && !gameOver) //n < 3
		{
			Prize p;
			p.px = (speedSprite.width + (std::rand() % (ScreenWidth() - 2 * speedSprite.width + 1))) * 1.0f;
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

		//update stars positions
		for (unsigned int i = 0; i < stars.size(); ++i)
		{
			if (!gameOver)
			{
				stars[i].py += obstacleSpeed;
				if (stars[i].py > SCREEN_HEIGHT)
					stars[i].py = 0;

			}
		}
			
		//remove bullets that went out of bounds or have destroyed an obstacle
		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) {return b.used || b.py < 0; }), bullets.end());

		//remove obstacles that went out of bounds or have been destroyed
		obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(), [](const Obstacle& o) {return o.destroyed || o.py > SCREEN_HEIGHT; }), obstacles.end());

		//remove prizes that were collected or went out ouf bounds
		prizes.erase(std::remove_if(prizes.begin(), prizes.end(), [](const Prize& p) {return p.collected || p.py > SCREEN_HEIGHT; }), prizes.end());

		//clear screen	
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		//draw stars
		for (unsigned int i = 0; i < stars.size(); ++i)
		{
			DrawCircle(stars[i].px, stars[i].py, stars[i].radius, olc::WHITE);
			FillCircle(stars[i].px, stars[i].py, stars[i].radius, olc::WHITE);
		}

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

		//prize time remaining
		int y = 0;
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
		
		//DrawSprite(200, 200, &bossSprite);

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