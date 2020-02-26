#include "Objects.h"

bool squareSquareCollision(float x1, float y1, float x2, float y2, int w1, int w2, int h1, int h2)
{
	return x1 + w1 >= x2 && x2 + w2 >= x1 && y1 + h1 >= y2 && y2 + h2 >= y1;
}

class SpaceInvaders : public olc::PixelGameEngine
{

public:
#if ANIMATED
	olc::Sprite bulletSprite;
	olc::Sprite meteorSprite;
	olc::Sprite indestructibleSprite;
	olc::Sprite speedSprite;
	olc::Sprite doublePointSprite;
	olc::Sprite bossBulletSprite;
	olc::Sprite bossSprite;
	olc::Sprite shipSprite;
#endif
	Boss boss;
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
	bool paused = false;

	SpaceInvaders()
	{
		sAppName = "Space invaders";
	}
public:
	bool OnUserCreate() override
	{
#if ANIMATED
		shipSprite.LoadFromFile("../resources/spaceship21.png");
		bossSprite.LoadFromFile("../resources/boss4.png");
		bulletSprite.LoadFromFile("../resources/bullet14.png");
		meteorSprite.LoadFromFile("../resources/meteor12.png");
		speedSprite.LoadFromFile("../resources/speed10.png");
		doublePointSprite.LoadFromFile("../resources/two5.png");
		indestructibleSprite.LoadFromFile("../resources/strength9.png");
		bossBulletSprite.LoadFromFile("../resources/circle2.png");
		SetPixelMode(olc::Pixel::MASK);
#endif
		prizeDurationMap[Prize::SPEED] = 0.0f;
		prizeDurationMap[Prize::INDESTRUCTIBLE] = 0.0f;
		prizeDurationMap[Prize::DOUBLE_POINT] = 0.0f;

		boss.setHealth(10);

		//add stars to random positions
		for (unsigned int i = 0; i < 25; ++i)
		{
			int r = rand() % 4 + 1;
			int x = r + (std::rand() % (ScreenWidth() - 2 * r + 1));
			int y = r + (std::rand() % (SCREEN_HEIGHT - 2 * r + 1));
			Star s(x * 1.0f, y * 1.0f, r * 1.0f);
			stars.push_back(s);
		}
		srand((unsigned)time(0));
		return true;
	}

private:
	void GetUserInput()
	{
		if ((GetKey(olc::Key::LEFT).bHeld || GetKey(olc::Key::A).bHeld) && ship.px > 0 && !gameOver && !paused)
			ship.px -= ship.speed;

		if ((GetKey(olc::Key::RIGHT).bHeld || GetKey(olc::Key::D).bHeld) && (ship.px < ScreenWidth() - SHIP_WIDTH) && !gameOver && !paused)
			ship.px += ship.speed;

		if (GetKey(olc::Key::SPACE).bPressed && !gameOver && !paused)
		{
			float x;
#if ANIMATED
			x = ship.px + SHIP_WIDTH / 2 + 1.99f;
#else
			x = ship.px + SHIP_WIDTH / 2 - BULLET_WIDTH / 2;
#endif
			Bullet b(x, ship.py);
			bullets.push_back(b);
		}
		//start a new game
		if (gameOver && GetKey(olc::Key::ENTER).bPressed && !paused)
		{
			obstacles.clear();
			bullets.clear();
			prizes.clear();
			boss.projectiles.clear();
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
			boss.setHealth(10);
			boss.parts = 60;
			boss.reset();
			prizeDurationMap[Prize::INDESTRUCTIBLE] = 0.0f;
			prizeDurationMap[Prize::DOUBLE_POINT] = 0.0f;
			prizeDurationMap[Prize::SPEED] = 0.0f;
		}

		if (GetKey(olc::Key::ESCAPE).bPressed && !gameOver)
			paused = !paused;
	}


	void DrawObjects()
	{
		//clear screen	
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);

		//draw stars
		for (unsigned int i = 0; i < stars.size(); ++i)
		{
			DrawCircle(stars[i].px, stars[i].py, stars[i].radius, olc::WHITE);
			FillCircle(stars[i].px, stars[i].py, stars[i].radius, olc::WHITE);
		}
#if ANIMATED //draw sprites which represent in-game objects
		//draw ship
		DrawSprite(ship.px, ship.py, &shipSprite);


		//draw boss and its health
		if (boss.active)
		{
			DrawSprite(boss.px, boss.py, &bossSprite);
			DrawString(80, 0, "Health ", olc::DARK_RED);
			float q = 1.0f * boss.currentHealth / boss.maxHealth;
			DrawLine(130, 5, 130 + q * 170, 5, olc::DARK_RED);
		}


		//draw bullets
		for (unsigned int i = 0; i < bullets.size(); ++i)
		{
			DrawRect(bullets[i].px, bullets[i].py, BULLET_WIDTH, BULLET_HEIGHT, olc::YELLOW);
			FillRect(bullets[i].px, bullets[i].py, BULLET_WIDTH, BULLET_HEIGHT, olc::YELLOW);
		}


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

		for (unsigned int i = 0; i < boss.projectiles.size(); ++i)
		{
			DrawCircle(boss.projectiles[i].px, boss.projectiles[i].py, PROJECTILE_WIDTH, olc::DARK_BLUE);
			FillCircle(boss.projectiles[i].px, boss.projectiles[i].py, PROJECTILE_WIDTH, olc::DARK_BLUE);
		}

		

#else //draw only rectangles(internal representation of in-game objects)
		//draw ship
		DrawRect(ship.px, ship.py, SHIP_WIDTH, SHIP_HEIGHT, olc::ORANGE);

		//draw boss and its health
		if (boss.active)
		{
			DrawRect(boss.px, boss.py, BOSS_SIZE, BOSS_SIZE, olc::DARK_RED);
			DrawString(80, 0, "Health ", olc::DARK_RED);
			float q = 1.0f * boss.currentHealth / boss.maxHealth;
			DrawLine(130, 5, 130 + q * 170, 5, olc::DARK_RED);
		}


		//draw bullets
		for (unsigned int i = 0; i < bullets.size(); ++i)
			DrawRect(bullets[i].px, bullets[i].py, BULLET_WIDTH, BULLET_HEIGHT, olc::YELLOW);

		//draw obstacles
		for (unsigned int i = 0; i < obstacles.size(); ++i)
			DrawRect(obstacles[i].px, obstacles[i].py, METEOR_SIZE, METEOR_SIZE, olc::MAGENTA);


		//draw prizes
		for (unsigned int i = 0; i < prizes.size(); ++i)
		{
			if (prizes[i].kind == Prize::SPEED)
				DrawRect(prizes[i].px, prizes[i].py, SPEED_WIDTH, SPEED_HEIGHT, olc::GREEN);
			else if (prizes[i].kind == Prize::DOUBLE_POINT)
				DrawRect(prizes[i].px, prizes[i].py, DOUBLE_WIDTH, SPEED_HEIGHT, olc::BLUE);
			else if (prizes[i].kind == Prize::INDESTRUCTIBLE)
				DrawRect(prizes[i].px, prizes[i].py, STRENGTH_SIZE, STRENGTH_SIZE, olc::CRIMSON);
		}

		//draw boss projectiles
		for (unsigned int i = 0; i < boss.projectiles.size(); ++i)
			DrawCircle(boss.projectiles[i].px, boss.projectiles[i].py, PROJECTILE_WIDTH, olc::DARK_BLUE);
		
			
#endif


		//display score
		DrawString(0, 0, "Score: " + std::to_string(score), olc::DARK_YELLOW);

		//prize time remaining
		int y = 0;
		if (ship.indestructible && !boss.active && !gameOver)
		{
			DrawString(100, y, "Indestructible time remaining: " + std::to_string(prizeDurationMap[Prize::INDESTRUCTIBLE]), olc::DARK_YELLOW);
			y += 15;
		}

		if (pointCount > 1 && !boss.active && !gameOver)
		{
			DrawString(100, y, "Double point time remaining: " + std::to_string(prizeDurationMap[Prize::DOUBLE_POINT]), olc::DARK_YELLOW);
			y += 15;
		}

		if (ship.speed > 2.0f && !boss.active && !gameOver)
			DrawString(100, y, "Speed boost time remaining: " + std::to_string(prizeDurationMap[Prize::SPEED]), olc::DARK_YELLOW);


		if (gameOver)
			DrawString(ScreenWidth() / 2 - 50, ScreenHeight() / 2, "GAME OVER!", olc::DARK_RED, 3);
		
		if(paused)
			DrawString(ScreenWidth() / 2 - 50, ScreenHeight() / 2, "PAUSED", olc::WHITE, 3);

	}



	void UpdatePositions()
	{
		//update bullets' positions
		for (unsigned int i = 0; i < bullets.size(); i++)
		{
			if (!gameOver)
				bullets[i].py -= bulletSpeed;
		}

		//update obstacles' positions
		for (unsigned int i = 0; i < obstacles.size(); ++i)
		{
			if (squareSquareCollision(ship.px, ship.py, obstacles[i].px, obstacles[i].py, SHIP_WIDTH, METEOR_SIZE, SHIP_HEIGHT, METEOR_SIZE) && !ship.indestructible)
				gameOver = true;
			else
			{
				if (!gameOver)
					obstacles[i].py += obstacleSpeed;
			}
		}

		//update prizes' position
		for (unsigned int i = 0; i < prizes.size(); ++i)
		{
			if (!gameOver)
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

		//update projectile position
		if (boss.active && !gameOver)
		{
			for (unsigned int i = 0; i < boss.projectiles.size(); ++i)
			{
				float s = boss.projectiles[i].speed * 1.41f / 2;
				if (boss.projectiles[i].dir == Projectile::LEFT)
				{
					boss.projectiles[i].px -= s * 0.5f;
					boss.projectiles[i].py += s;
				}
				else if (boss.projectiles[i].dir == Projectile::RIGHT)
				{
					boss.projectiles[i].px += s * 0.5f;
					boss.projectiles[i].py += s;
				}
				else if(boss.projectiles[i].dir == Projectile::MIDDLE)
					boss.projectiles[i].py += boss.projectiles[i].speed;
			}
		}
	}

	void ClearVectors()
	{
		//remove bullets that went out of bounds or have destroyed an obstacle
		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), [](const Bullet& b) {return b.used || b.py < 0; }), bullets.end());

		//remove obstacles that went out of bounds or have been destroyed
		obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(), [](const Obstacle& o) {return o.destroyed || o.py > SCREEN_HEIGHT; }), obstacles.end());
		if (boss.active)
			obstacles.clear();

		//remove prizes that were collected or went out ouf bounds
		prizes.erase(std::remove_if(prizes.begin(), prizes.end(), [](const Prize& p) {return p.collected || p.py > SCREEN_HEIGHT; }), prizes.end());
		if (boss.active)
			prizes.clear();

		//remove projectiles that went out of bounds
		boss.projectiles.erase(std::remove_if(boss.projectiles.begin(), boss.projectiles.end(), [](const Projectile& p) {return p.py > SCREEN_HEIGHT; }), boss.projectiles.end());
	}

public:
	bool OnUserUpdate(float fElapsedTime) override
	{
		GetUserInput();

		//add obstacles
		timePassed += fElapsedTime;
		if ((score >= scoreLowerBound) && (score < scoreUpperBound) && (timePassed > timeBound) && !boss.active && !paused)
		{
			for (int i = 0; i < numObstacles; ++i)
			{
				if (!gameOver)
				{
					int x = METEOR_SIZE + (std::rand() % (ScreenWidth() - 2 * METEOR_SIZE + 1));
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

		//see if prizes' effects have worn off
		if (prizeDurationMap[Prize::SPEED] <= 0.0f)
		{
			prizeDurationMap[Prize::SPEED] = 0.0f;
			ship.speed = 2.0f;
		}
		else if(!paused)
			prizeDurationMap[Prize::SPEED] = prizeDurationMap[Prize::SPEED] - fElapsedTime;

		if (prizeDurationMap[Prize::DOUBLE_POINT] <= 0.0f)
		{
			prizeDurationMap[Prize::DOUBLE_POINT] = 0.0f;
			pointCount = 1;
		}
		else if(!paused)
			prizeDurationMap[Prize::DOUBLE_POINT] = prizeDurationMap[Prize::DOUBLE_POINT] - fElapsedTime;

		if (prizeDurationMap[Prize::INDESTRUCTIBLE] <= 0.0f)
		{
			prizeDurationMap[Prize::INDESTRUCTIBLE] = 0.0f;
			ship.indestructible = false;
		}
		else if(!paused)
			prizeDurationMap[Prize::INDESTRUCTIBLE] = prizeDurationMap[Prize::INDESTRUCTIBLE] - fElapsedTime;

		//check if any of the prizes was collected and apply its effects
		for (unsigned int i = 0; i < prizes.size(); ++i)
		{			
			if (prizes[i].kind == Prize::DOUBLE_POINT)
			{
				if (squareSquareCollision(ship.px, ship.py, prizes[i].px, prizes[i].py, SHIP_WIDTH, DOUBLE_WIDTH, SHIP_HEIGHT, DOUBLE_HEIGHT))
				{
					prizes[i].collected = true;
					pointCount = 2;
					prizeDurationMap[Prize::DOUBLE_POINT] = prizeDurationMap[Prize::DOUBLE_POINT] + prizeDurationLimit;
				}
			}		
			else if (prizes[i].kind == Prize::INDESTRUCTIBLE)
			{
				if (squareSquareCollision(ship.px, ship.py, prizes[i].px, prizes[i].py, SHIP_WIDTH, STRENGTH_SIZE, SHIP_HEIGHT, STRENGTH_SIZE))
				{
					prizes[i].collected = true;
					ship.indestructible = true;
					prizeDurationMap[Prize::INDESTRUCTIBLE] = prizeDurationMap[Prize::INDESTRUCTIBLE] + prizeDurationLimit;
				}
			}
			else if (prizes[i].kind == Prize::SPEED)
			{
				if (squareSquareCollision(ship.px, ship.py, prizes[i].px, prizes[i].py, SHIP_WIDTH, SPEED_WIDTH, SHIP_HEIGHT, SPEED_HEIGHT))
				{
					prizes[i].collected = true;
					ship.speed = 6.0f;
					prizeDurationMap[Prize::SPEED] = prizeDurationMap[Prize::SPEED] + prizeDurationLimit;
				}
			}	
		}


		//check if any of the obstacles were destroyed and update parameters
		for (unsigned int i = 0; i < obstacles.size(); ++i)
		{
			for (unsigned int j = 0; j < bullets.size(); ++j)
			{
				if (squareSquareCollision(obstacles[i].px, obstacles[i].py, bullets[j].px, bullets[j].py, METEOR_SIZE, BULLET_WIDTH, METEOR_SIZE, BULLET_HEIGHT))
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
					if (score % 10 == 0 || (pointCount == 2 && score % 10 == 1))
					{
						boss.active = true;
						boss.setHealth(boss.maxHealth + 10);
						boss.parts *= 0.85f;
						prizeDurationMap[Prize::INDESTRUCTIBLE] = 0.0f;
						prizeDurationMap[Prize::DOUBLE_POINT] = 0.0f;
						prizeDurationMap[Prize::SPEED] = 0.0f;
						
					}
				}
			}
		}

		//check if any of the bullets have hit the boss
		if (boss.active)
		{
			if (boss.currentHealth == 0)
				boss.reset();	
			else
			{
				for (unsigned int i = 0; i < bullets.size(); ++i)
				{
					if (squareSquareCollision(bullets[i].px, bullets[i].py, boss.px, boss.py, BULLET_WIDTH, BOSS_SIZE, BULLET_HEIGHT, BOSS_SIZE))
					{
						bullets[i].used = true;
						boss.currentHealth -= 1;
					}
				}
			}
		}

		//check if any of the boss' projectiles have hit the ship
		for (unsigned int i = 0; i < boss.projectiles.size(); ++i)
		{ 
			if (squareSquareCollision(ship.px, ship.py, boss.projectiles[i].px, boss.projectiles[i].py, SHIP_WIDTH, PROJECTILE_WIDTH, SHIP_HEIGHT, PROJECTILE_WIDTH))
				gameOver = true;
		}

		//check if any of the projectiles have collided with any of the bullets
		for (unsigned int i = 0; i < boss.projectiles.size(); ++i)
		{
			for (unsigned int j = 0; j < bullets.size(); ++j)
			{
				if (squareSquareCollision(boss.projectiles[i].px, boss.projectiles[i].py, bullets[j].px, bullets[j].py, PROJECTILE_WIDTH, BULLET_WIDTH, PROJECTILE_WIDTH, BULLET_HEIGHT))
					bullets[j].used = true;
			}
		}

		//add prizes
		int n = rand() % 5500;
		if (n < 3 && !gameOver && !boss.active && !paused) //n < 3
		{
			Prize p;
			p.px = (SPEED_WIDTH + (std::rand() % (ScreenWidth() - 2 * SPEED_WIDTH + 1))) * 1.0f;
			p.py = 0;
			p.kind = rand() % 3;
			prizes.push_back(p);
		}


		if (!gameOver && !paused)
		{
			boss.attack(ship.px, ship.py);
			boss.implementDive();
		}

		if (boss.active && !paused)
			boss.appeared += fElapsedTime;

		//see if ship has collided with the boss
		if (boss.active && squareSquareCollision(ship.px, ship.py, boss.px, boss.py, SHIP_WIDTH, BOSS_SIZE, SHIP_HEIGHT, BOSS_SIZE))
			gameOver = true;

		
		ClearVectors();
		if(!paused)
			UpdatePositions();
		DrawObjects();
		
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