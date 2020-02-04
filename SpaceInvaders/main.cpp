#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <string>
#include <iostream>
#include <chrono>
#include <thread>
#define SCREEN_HEIGHT 400


bool squareSquareCollision(float x1, float y1, float x2, float y2, int w1, int w2)
{
	if (x1 + w1 > x2 && x2 + w2 > x1 && y1 + w1 > y2 && y2 + w2 > y1)
		return true;
	return false;
}

	
struct Ship
{
	float px, py;
	int w, h;
};

struct Bullet
{

	float px, py;
	int w,  h;
	int speed;
	bool used = false;
	Bullet(float x, float y, int width, int height, int s) : px(x), py(y), w(width), h(height), speed(s) {}
};

struct Obstacle
{
	float px, py;
	int w, h;
	int speed;
	bool destroyed = false;
	Obstacle(float x, float y, int width, int height, int s) : px(x), py(y), w(width), h(height), speed(s){}
};

 
bool obstacleRemovable(const Obstacle& o)
{
	return o.py > SCREEN_HEIGHT || o.destroyed;
}


bool bulletRemovable(const Bullet& b)
{
	return b.py < 0 || b.used;
}

class Example : public olc::PixelGameEngine
{

public:
	Ship ship;
	int score;
	float timePassed;
	std::vector<Bullet> bullets;
	std::vector<Obstacle> obstacles;
	Example()
	{
		sAppName = "Space invaders";
	}
public:
	bool OnUserCreate() override
	{
		ship.px = 150;
		ship.py = 350;
		ship.w = 10;
		ship.h = 10;
		timePassed = 0;

		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::Key::LEFT).bHeld || GetKey(olc::Key::A).bHeld)
			ship.px -= 2;
		if (GetKey(olc::Key::RIGHT).bHeld || GetKey(olc::Key::D).bHeld)
			ship.px += 2;
		if (GetKey(olc::Key::SPACE).bPressed)
		{
			Bullet b(ship.px, ship.py, 5, 5, 3);
			bullets.push_back(b);
		}

		//update bullets' positions
		for (int i = 0; i < bullets.size(); i++)
			bullets[i].py -= bullets[i].speed;
			
		
		//add obstacles
		timePassed += fElapsedTime;
		if (score < 5 && timePassed > 5)
		{
			//std::cout << "Proslo 5 sekundi." << std::endl;
			Obstacle o(rand() % ScreenWidth(), 0, 8, 8, 1);
			obstacles.push_back(o);
			timePassed = 0.0f;
		}
		else if (score >= 5 && score < 10 && timePassed > 4)
		{
			//std::cout << "Proslo 4 sekunde." << std::endl;
			for (int i = 0; i < 2; ++i)
			{
				Obstacle o(rand() % ScreenWidth(), -40 * i, 8, 8, 2);
				obstacles.push_back(o);
			}
			timePassed = 0.0f;
		}
		else if (score >= 10 && score < 15 && timePassed > 3)
		{
			for (int i = 0; i < 3; ++i)
			{
				Obstacle o(rand() % ScreenWidth(), -40 * i, 8, 8, 3);
				obstacles.push_back(o);
			}
			
			timePassed = 0.0f;
		}

		else if (score >= 15 && score < 20 && timePassed > 2)
		{
			for (int i = 0; i < 4; ++i)
			{
				Obstacle o(rand() % ScreenWidth(), -40 * i, 8, 8, 4);
				obstacles.push_back(o);
			}
			timePassed = 0.0f;
		}



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

		for (int i = 0; i < obstacles.size(); ++i)
		{
			for (int j = 0; j < bullets.size(); ++j)
			{
				if (squareSquareCollision(obstacles[i].px, obstacles[i].py, bullets[j].px, bullets[j].py, obstacles[i].w, bullets[j].w))
				{
					obstacles[i].destroyed = true;
					bullets[j].used = true;
					score += 1;
				}
			}
		}

		//remove bullets that went out of bounds
		bullets.erase(std::remove_if(bullets.begin(), bullets.end(), bulletRemovable), bullets.end());

		//remove obstacles that went out of bounds
		obstacles.erase(std::remove_if(obstacles.begin(), obstacles.end(), obstacleRemovable), obstacles.end());
			
		//clear screen	
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);
		
		//draw ship
		DrawRect(ship.px, ship.py, ship.w, ship.h, olc::DARK_CYAN);
		FillRect(ship.px, ship.py, ship.w, ship.h, olc::DARK_CYAN);
		

		//draw bullets
		for (int i = 0; i < bullets.size(); ++i)
		{
			DrawRect(bullets[i].px, bullets[i].py, bullets[i].w, bullets[i].h, olc::DARK_GREY);
			FillRect(bullets[i].px, bullets[i].py, bullets[i].w, bullets[i].h, olc::DARK_GREY);
		}

		//draw obstacles
		for (int i = 0; i < obstacles.size(); ++i)
		{
			DrawRect(obstacles[i].px, obstacles[i].py, obstacles[i].w, obstacles[i].h, olc::RED);
			FillRect(obstacles[i].px, obstacles[i].py, obstacles[i].w, obstacles[i].h, olc::RED);
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