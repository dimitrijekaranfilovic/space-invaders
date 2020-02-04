#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <vector>
#include <string>
#include <iostream>
// Override base class with your custom functionality

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
	Bullet(float x, float y, int width, int height, int s) : px(x), py(y), w(width), h(height), speed(s) {}
};

struct Obstacle
{
	float px, py;
	int w, h;
	int speed;
	Obstacle(float x, float y, int width, int height, int s) : px(x), py(y), w(width), h(height), speed(s){}
};

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
		// Called once at the start, so create things here
		ship.px = 150;
		ship.py = 350;
		ship.w = 10;
		ship.h = 10;
		timePassed = 0;

		return true;
	}
	bool OnUserUpdate(float fElapsedTime) override
	{
		if (GetKey(olc::Key::LEFT).bHeld)
			ship.px -= 2;
		if (GetKey(olc::Key::RIGHT).bHeld)
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
			Obstacle o(rand() % ScreenWidth(), 0, 8, 8, 1);
			obstacles.push_back(o);
			timePassed = 0.0f;
		}
		else if (score >= 5 && score < 10 && timePassed > 4)
		{
			Obstacle o(rand() % ScreenWidth(), 0, 8, 8, 2);
			obstacles.push_back(o);
			timePassed = 0.0f;
		}
		else if (score >= 10 && score < 15 && timePassed > 3)
		{
			Obstacle o(rand() % ScreenWidth(), 0, 8, 8, 3);
			obstacles.push_back(o);
			timePassed = 0.0f;
		}

		else if (score >= 15 && score < 20 && timePassed > 2)
		{
			Obstacle o(rand() % ScreenWidth(), 0, 8, 8, 4);
			obstacles.push_back(o);
			timePassed = 0.0f;
		}

		//update obstacles' positions
		for (int i = 0; i < obstacles.size(); ++i)
		{
			obstacles[i].py += obstacles[i].speed;
		}
			


		//clear screen	
		FillRect(0, 0, ScreenWidth(), ScreenHeight(), olc::BLACK);
		
		//draw ship
		DrawRect(ship.px, ship.py, ship.w, ship.h, olc::DARK_CYAN);
		FillRect(ship.px, ship.py, ship.w, ship.h, olc::DARK_CYAN);
		
		//draw bullets
		for (int i = 0; i < bullets.size(); i++)
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
	if (demo.Construct(400, 400, 2, 2))
		demo.Start();
	return 0;
}