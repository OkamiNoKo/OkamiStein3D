/*
Description:
    In file level:
	    0 - blank space
		1 - wall
		2 - Player
		3 - Wolf(melee)
		4 - Wolf - Licantrop(range)
		5 - Exit (only when all enemies died)
		6 - health
		7 - ammo
*/

#include <iostream>
#include <fstream>
#include <string>

using namespace std;

struct point
{
	int x;
	int y;
	point(int x, int y)
	{
		this->x = x;
		this->y = y;
	}
	point()
	{
		this->x = 0;
		this->y = 0;
	}
};

class Player
{
	point position;
	int health;
	int ammo;
	int inGun;
	int actionCooldown;
	int actionCooldownVal;
	int damage;
public:
	Player(point pos)
	{
		position = pos;
		health = 100;
		ammo = 8;
		inGun = 8;
		actionCooldown = 0;
		actionCooldownVal = 7;
		damage = 31;
	}
	Player()
	{
		position = point();
		health = 100;
		ammo = 8;
		inGun = 8;
		actionCooldown = 0;
		actionCooldownVal = 7;
		damage = 31;
	}
};

class Enemy
{
	int HP;
	int actionCooldown;
	int actionCooldownVal;
	int range;
	int SmellRange;
	int damage;
	point position;
public:
	Enemy(point pos, int range, int smell, int damage, int health)
	{
		position = pos;
		this->range = range;
		SmellRange = smell;
		this->damage = damage;
		HP = health;
		actionCooldown = 0;
		actionCooldownVal = 17;
	}
	Enemy()
	{
		position = point();
		this->range = 0;
		SmellRange = 0;
		this->damage = 0;
		HP = 0;
		actionCooldown = 0;
		actionCooldownVal = 17;
	}
	Enemy Enemy1(point pos)
	{
		return Enemy(pos, 1, 7, 11, 42);
	}

	Enemy Enemy2(point pos)
	{
		return Enemy(pos, 7, 8, 9, 27);
	}
};


class level
{
	int width;
	int hight;
	int ** map;
	int enemy1count = 0;
	int enemy2count = 0;
public:
	Player player = Player();
	Enemy * enemy1 = new Enemy[41];
	Enemy * enemy2 = new Enemy[41];
	level(string path)
	{
		std::string line;
		std::ifstream in(path); // окрываем файл для чтения
		int c = 0;
		if (in.is_open())
		{
			while (getline(in, line))
			{
				if (c < 2)
				{
					int S = 0;
					for (int i = 0; i < line.length(); i++)
					{
						S = S * 10 + (int)line[i] - (int)'0';
					}
					if (c == 0)
					{
						width = S;
					}
					else
					{
					    hight = S;
						map = new int*[width];
						for (int i = 0; i < width; i++)
						{
							map[i] = new int[hight];
						}
					}
				}
				else
				{
					int S = 0;
					for (int i = 0; i < line.length(); i++)
					{
						if (line[i] == ',')
						{
							point pos;
							pos.x = (c - 2) % hight;
							pos.y = (c - 2) / hight;
							map[pos.y][pos.x] = S;
							S = 0;
							switch (S) {
							case 2:
								player = Player(pos);
								break;
							case 3:
								enemy1[enemy1count] = enemy1[enemy1count].Enemy1(pos);
								enemy1count++;
								break;
							case 4:
								enemy2[enemy2count] = enemy2[enemy2count].Enemy2(pos);
								enemy2count++;
								break;
								
							}
							c++;
						}
						else
						{
							S = S * 10 + (int)line[i] - (int)'0';
						}
					}
				}
				c++;
			}
		}
		in.close();
	}
};