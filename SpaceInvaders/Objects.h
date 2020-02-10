#pragma once
#include "olcPixelGameEngine.h"
#include <string>
#include <vector>
#include <unordered_map> 
#define SCREEN_HEIGHT 400
struct Star
{
	float px, py;
	float radius;
	Star(float x, float y, float r) : px(x), py(y), radius(r) {}
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
	float px = 170;
	float py = 60;
	bool active = false;
	int currentHealth;
	int maxHealth;
	olc::Sprite sprite;
	std::string imagePath = "../resources/boss4.png";
	Boss()
	{
		sprite.LoadFromFile(imagePath);
	}
	
	void setHealth(int health)
	{
		maxHealth = health;
		currentHealth = health;
	}

};