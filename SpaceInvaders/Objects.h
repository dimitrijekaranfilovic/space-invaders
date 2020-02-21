#pragma once
#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"
#include <string>
#include <vector>
#include <unordered_map> 
#include <iostream>
#include <iomanip>
#define SCREEN_HEIGHT 400
#define SHIP_WIDTH 14
#define SHIP_HEIGHT 18
#define METEOR_SIZE 21
#define DOUBLE_HEIGHT 16
#define DOUBLE_WIDTH 18
#define STRENGTH_SIZE 22
#define SPEED_HEIGHT 20
#define SPEED_WIDTH 13
#define BULLET_HEIGHT 14
#define BULLET_WIDTH 3
#define BOSS_SIZE 40
#define BOSS_X 170
#define BOSS_Y 60


struct Star
{
	float px, py;
	float radius;
	Star(float x, float y, float r) : px(x), py(y), radius(r) {}
};

struct Projectile
{
	float px, py;
	float kx, ky;
	float speed = 2.0f;
	int w=2, h=4;
};

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
	olc::Sprite sprite;
	std::string imagePath = "../resources/spaceship21.png";
	Ship()
	{
		sprite.LoadFromFile(imagePath);
	}
	bool indestructible = false;
};

struct Bullet
{
	float px, py;
	bool used = false;
	Bullet(float x, float y) : px(x), py(y) {}
};

struct Obstacle
{
	float px, py;
	bool destroyed = false;
	Obstacle(float x, float y) : px(x), py(y) {}
};

struct Boss
{
	float px = BOSS_X;
	float py = BOSS_Y;
	float parts = 60;
	float appeared = 0.0f;
	
	bool active = false;
	bool doTheDive = false;

	float y0, x0, y1, x1, speed;
	float q;

	int currentHealth;
	int maxHealth;

	olc::Sprite sprite;
	std::string imagePath = "../resources/boss4.png";
	std::vector<Projectile> projectiles;
	Boss()
	{
		sprite.LoadFromFile(imagePath);
	}
	
	void setHealth(int health)
	{
		maxHealth = health;
		currentHealth = health;
	}

	void dive(float ship_x, float ship_y)
	{
		speed = abs(ship_x - px) / parts * 1.0f;
		x0 = px;
		y0 = py;
		x1 = ship_x;
		y1 = ship_y;
		doTheDive = true;
		if (px > ship_x)
			q = -1;
		else 
			q = 1;

	}

	float interpolate(float x)
	{
		return y0 + (x - x0) * (y1 - y0) / (x1 - x0);
	}

	void shoot()
	{

	}


};