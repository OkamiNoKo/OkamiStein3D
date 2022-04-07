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
		8 - armor
*/

#include <cmath>

#include <windows.h>
#include <d2d1.h>
#pragma comment(lib,"d2d1.lib")

#include <ctime>
#include <InitGuid.h>

//#include <dinput8.h>
#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dwrite.lib")
#pragma comment(lib, "dinput8.lib")


#include <dinput.h>
#include <dsound.h>
#include <dwrite.h>

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
		this->x = 1;
		this->y = 1;
	}

	friend point operator-(point a, point b)
	{
		return point(a.x - b.x, a.y - b.y);
	}

	friend point operator+(point a, point b)
	{
		return point(a.x + b.x, a.y + b.y);
	}
};


struct box
{
	int width;
	int hight;
};

class Player
{
public:
	point position;
	point rotation;
	int health;
	int ammo;
	int inGun;
	int GunCapasity;
	int actionCooldown;
	int MoveCD;
	int ReloadCD;
	int FireCD;
	int RotationCD;
	int damage;
	int armor;
	int meleeDamage;
	int idealRange;
	int last;

	Player(point pos)
	{
		rotation = point(1, 0);
		position = pos;
		health = 100;
		ammo = 16;
		inGun = 8;
		GunCapasity = 8;
		actionCooldown = 0;
		MoveCD = 37;
		ReloadCD = 37;
		FireCD = 33;
		RotationCD = 34;
		damage = 31;
		armor = 0;
		meleeDamage = 47;
		idealRange = 4;
		last = 1;
	}
	Player()
	{
		rotation = point(1, 0);
		position = point();
		health = 100;
		ammo = 16;
		inGun = 8;
		actionCooldown = 0;
		MoveCD = 17;
		ReloadCD = 37;
		FireCD = 13;
		RotationCD = 14;
		damage = 21;
		armor = 0;
		meleeDamage = 33;
		idealRange = 4;
		last = 1;
	}
};

class Enemy
{
public:
	int HP;
	int actionCooldown;
	int actionCooldownVal;
	int range;
	int SmellRange;
	int damage;
	int location;
	point position;
	Enemy(point pos, int range, int smell, int damage, int health, int actVal)
	{
		position = pos;
		this->range = range;
		SmellRange = smell;
		this->damage = damage;
		HP = health;
		actionCooldown = 0;
		actionCooldownVal = actVal;
		location = 0;
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
		location = 0;
	}
	Enemy Enemy1(point pos)
	{
		return Enemy(pos, 1, 7, 11, 42, 13);
	}

	Enemy Enemy2(point pos)
	{
		return Enemy(pos, 5, 5, 9, 27, 17);
	}
};


class level
{
public:
	int width;
	int hight;
	int ** map;
	int enemy1count = 0;
	int enemy2count = 0;
	Player player = Player();
	Enemy * enemy1 = new Enemy[41];
	Enemy * enemy2 = new Enemy[41];
	level(string path)
	{
		std::string line;
		std::ifstream in(path); // окрываем файл для чтения
		int c = 0;
		int S = 0;
		if (in.is_open())
		{
			while (getline(in, line))
			{
				if (c < 2)
				{
					S = 0;
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
					c++;
				}
				else
				{
					S = 0;
					for (int i = 0; i < line.length(); i++)
					{
						if (line[i] == ',' || line[i] == '.')
						{
							point pos;
							pos.x = (c - 2) % width;
							pos.y = (c - 2) / width;
							map[pos.y][pos.x] = S;
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
							S = 0;
						}
						else
						{
							S = S * 10 + (int)line[i] - (int)'0';
						}
					}
				}
				
			}
		}
		in.close();
	}

	void playerFire()
	{
		int x = player.position.x;
		int y = player.position.y;

		int X = player.rotation.x;
		int Y = player.rotation.y;

		if (map[y + Y][x + X] == 3)
		{
			for (int i = 0; i < enemy1count; i++)
			{
				if (enemy1[i].position.x == x + X && enemy1[i].position.y == y + Y)
				{
					enemy1[i].HP -= player.meleeDamage;
					if (enemy1[i].HP <= 0)
					{
						map[enemy1[i].position.y][enemy1[i].position.x] = enemy1[i].location;
						for (int j = i; j < enemy1count - 1; j++)
						{
							enemy1[j] = enemy1[j + 1];
						}
						enemy1count--;
					}
					return;
				}
			}
		}
		else if (map[y + Y][x + X] == 4)
		{
			for (int i = 0; i < enemy2count; i++)
			{
				if (enemy2[i].position.x == x + X && enemy2[i].position.y == y + Y)
				{
					enemy2[i].HP -= player.meleeDamage;
					if (enemy2[i].HP <= 0)
					{
						map[enemy2[i].position.y][enemy2[i].position.x] = enemy2[i].location;
						for (int j = i; j < enemy2count - 1; j++)
						{
							enemy2[j] = enemy2[j + 1];
						}
						enemy2count--;
					}
					return;
				}
			}
		}
		else
		{
			if (player.inGun > 0)
			{
				player.inGun--;
				x += X;
				y += Y;
				int k = 2;
				while (map[y][x] != 1)
				{
					if (map[y + Y][x + X] == 3)
					{
						for (int i = 0; i < enemy1count; i++)
						{
							if (enemy1[i].position.x == x + X && enemy1[i].position.y == y + Y)
							{
								enemy1[i].HP -= floor(player.damage * pow(0.9, abs(k - player.idealRange)));
								if (enemy1[i].HP <= 0)
								{
									map[enemy1[i].position.y][enemy1[i].position.x] = enemy1[i].location;
									for (int j = i; j < enemy1count - 1; j++)
									{
										enemy1[j] = enemy1[j + 1];
									}
									enemy1count--;
								}
								return;
							}
						}
					}
					else if (map[y + Y][x + X] == 4)
					{
						for (int i = 0; i < enemy2count; i++)
						{
							if (enemy2[i].position.x == x + X && enemy2[i].position.y == y + Y)
							{
								enemy2[i].HP -= floor(player.damage * pow(0.9, abs(k - player.idealRange)));
								if (enemy2[i].HP <= 0)
								{
									map[enemy2[i].position.y][enemy2[i].position.x] = enemy2[i].location;
									for (int j = i; j < enemy2count - 1; j++)
									{
										enemy2[j] = enemy2[j + 1];
									}
									enemy2count--;
								}
								return;
							}
						}
					}
					x += X;
					y += Y;
					k++;
				}
			}
		}
	}

	void getTurn()
	{
		if (map[player.position.y][player.position.x] == 6)
		{
			if (player.health >= 75)
			{
				player.health = 100;
			}
			else
			{
				player.health += 25;
			}
		}

		if (map[player.position.y][player.position.x] == 7)
		{
			player.ammo += 24;
		}

		if (map[player.position.y][player.position.x] == 8)
		{
			if (player.armor >= 35)
			{
				player.armor = 50;
			}
			else
			{
				player.armor += 15;
			}
		}
		if (map[player.position.y][player.position.x] != 2)
		{
			map[player.position.y][player.position.x] = 2;
			map[player.position.y - player.rotation.y * player.last][player.position.x - player.rotation.x * player.last] = 0;
		}

		for (int i = 0; i < enemy1count; i++)
		{
			int y = enemy1[i].position.y;
			int x = enemy1[i].position.x;
			if (enemy1[i].actionCooldown <= 0 && abs(y - player.position.y) + abs(x - player.position.x) <= enemy1[i].SmellRange)
			{
				if (map[y][x + 1] != 1 && map[y][x + 1] != 2 && map[y][x + 1] != 3 && map[y][x + 1] != 4)
				{
					if (abs(player.position.x - x) + abs(player.position.y - y) > abs(player.position.x - x - 1) + abs(player.position.y - y))
					{
						enemy1[i].position.x += 1;
					}
				}
				else
				{

					if (map[y][x - 1] != 1 && map[y][x - 1] != 2 && map[y][x - 1] != 3 && map[y][x - 1] != 4)
					{
						if (abs(player.position.x - x) + abs(player.position.y - y) > abs(player.position.x - x + 1) + abs(player.position.y - y))
						{
							enemy1[i].position.x -= 1;
						}
					}
					else
					{

						if (map[y + 1][x] != 1 && map[y + 1][x] != 2 && map[y + 1][x] != 3 && map[y + 1][x] != 4)
						{
							if (abs(player.position.x - x) + abs(player.position.y - y) > abs(player.position.x - x) + abs(player.position.y - y - 1))
							{
								enemy1[i].position.y += 1;
							}
						}
						else
						{

							if (map[y - 1][x] != 1 && map[y - 1][x] != 2 && map[y - 1][x] != 3 && map[y - 1][x] != 4)
							{
								if (abs(player.position.x - x) + abs(player.position.y - y) > abs(player.position.x - x) + abs(player.position.y - y + 1))
								{
									enemy1[i].position.y -= 1;
								}
							}
						}
					}
				}

				map[y][x] = enemy1[i].location;
				enemy1[i].location = map[enemy1[i].position.y][enemy1[i].position.x];
				map[enemy1[i].position.y][enemy1[i].position.x] = 3;
				if (abs(player.position.x - enemy1[i].position.x) + abs(player.position.y - enemy1[i].position.y) <= enemy1[i].range)
				{
					if (player.armor > 0)
					{
						if (player.armor > enemy1[i].damage / 2)
						{
							player.armor -= enemy1[i].damage / 2;
							player.health -= enemy1[i].damage / 2;
						}
						else
						{
							player.health -= enemy1[i].damage - player.armor;
							player.armor = 0;
						}
					}
					else
					{
						player.health -= enemy1[i].damage;
					}
				}
				enemy1[i].actionCooldown = enemy1[i].actionCooldownVal;
			}
			enemy1[i].actionCooldown--;
		}

		for (int i = 0; i < enemy2count; i++)
		{
			int y = enemy2[i].position.y;
			int x = enemy2[i].position.x;
			if (enemy2[i].actionCooldown <= 0 && abs(y - player.position.y) + abs(x - player.position.x) <= enemy2[i].SmellRange)
			{
				if (map[y][x + 1] != 1 && map[y][x + 1] != 2 && map[y][x + 1] != 3 && map[y][x + 1] != 4)
				{
					if (abs(player.position.x - x) + abs(player.position.y - y) > abs(player.position.x - x - 1) + abs(player.position.y - y))
					{
						enemy2[i].position.x += 1;
					}
				}
				else
				{

					if (map[y][x - 1] != 1 && map[y][x - 1] != 2 && map[y][x - 1] != 3 && map[y][x - 1] != 4)
					{
						if (abs(player.position.x - x) + abs(player.position.y - y) > abs(player.position.x - x + 1) + abs(player.position.y - y))
						{
							enemy2[i].position.x -= 1;
						}
					}
					else
					{

						if (map[y + 1][x] != 1 && map[y + 1][x] != 2 && map[y + 1][x] != 3 && map[y + 1][x] != 4)
						{
							if (abs(player.position.x - x) + abs(player.position.y - y) > abs(player.position.x - x) + abs(player.position.y - y - 1))
							{
								enemy2[i].position.y += 1;
							}
						}
						else
						{

							if (map[y - 1][x] != 1 && map[y - 1][x] != 2 && map[y - 1][x] != 3 && map[y - 1][x] != 4)
							{
								if (abs(player.position.x - x) + abs(player.position.y - y) > abs(player.position.x - x) + abs(player.position.y - y + 1))
								{
									enemy2[i].position.y -= 1;
								}
							}
						}
					}
				}

				map[y][x] = enemy2[i].location;
				enemy2[i].location = map[enemy2[i].position.y][enemy2[i].position.x];
				map[enemy2[i].position.y][enemy2[i].position.x] = 3;
				if (player.position.x == enemy2[i].position.x || player.position.y == enemy2[i].position.y)
				{
					if (abs(player.position.x - enemy2[i].position.x) <= enemy2[i].range && abs(player.position.x - enemy2[i].position.x) <= enemy2[i].range)
					{
						bool clear = true;
						int X = (player.position.x - enemy2[i].position.x);
						int Y = (player.position.y - enemy2[i].position.y);
						int rot = 0;
						if (X != 0)
						{
							rot = X / abs(X);
							for (int i = 1; i < abs(X); i++)
							{
								if (map[enemy2[i].position.y][enemy2[i].position.x + i * rot] == 1 || map[enemy2[i].position.y][enemy2[i].position.x + i * rot] == 3 || map[enemy2[i].position.y][enemy2[i].position.x + i * rot] == 4)
								{
									clear = false;
								}
							}
						}
						if (Y != 0)
						{
							rot = Y / abs(Y);
							for (int i = 1; i < abs(Y); i++)
							{
								if (map[enemy2[i].position.y + i * rot][enemy2[i].position.x] == 1 || map[enemy2[i].position.y + i * rot][enemy2[i].position.x] == 3 || map[enemy2[i].position.y + i * rot][enemy2[i].position.x] == 4)
								{
									clear = false;
								}
							}
						}
						
						if (clear)
						{
							//player.health -= floor(enemy2[i].damage * (((double)(X + enemy2[i].range / 2) / enemy2[i].range) + ((double)(Y) / enemy2[i].range)));
							if (X != 0)
							{
								int dam = player.health -= floor(enemy2[i].damage * (1 - (double)(abs(enemy2[i].range / 2 + 1 - abs(X))) / enemy2[i].range));

								if (player.armor > 0)
								{
									if (player.armor > (dam / 4) * 3)
									{
										player.armor -= (dam / 4) * 3;
										player.health -= dam / 4;
									}
									else
									{
										player.health -= dam - player.armor;
										player.armor = 0;
									}
								}
								else
								{
									player.health -= dam;
								}
							}
							else if (Y != 0)
							{
								int dam = floor(enemy2[i].damage * (1 - (double)(abs(enemy2[i].range / 2 + 1 - abs(Y))) / enemy2[i].range));

								if (player.armor > 0)
								{
									if (player.armor > (dam / 4) * 3)
									{
										player.armor -= (dam / 4) * 3;
										player.health -= dam / 4;
									}
									else
									{
										player.health -= dam - player.armor;
										player.armor = 0;
									}
								}
								else
								{
									player.health -= dam;
								}
							}
						}
					}
				}
				enemy2[i].actionCooldown = enemy2[i].actionCooldownVal;
			}
			enemy2[i].actionCooldown--;
		}
	}

	void boxVIS(ID2D1HwndRenderTarget* rt, int mod, int act, box box, ID2D1SolidColorBrush ** brushes)
	{
		if (mod == 0)
		{

		}
		else
		{
			int boxSize = box.width / 16;
			point Center = point(box.width / 2, box.hight / 2);
			point leftTop;
			point rightDown;
			int edge = box.hight / 360;
			if (player.position.x >= 8)
			{
				leftTop.x = player.position.x - 8;
			}
			else
			{
				leftTop.x = 0;
			}

			if (player.position.x + 8 < width)
			{
				rightDown.x = player.position.x + 8;
			}
			else
			{
				rightDown.x = width;
			}

			if (player.position.y >= 4)
			{
				leftTop.y = player.position.y - 4;
			}
			else
			{
				leftTop.y = 0;
			}

			if (player.position.y + 4 < hight)
			{
				rightDown.y = player.position.y + 4;
			}
			else
			{
				rightDown.y = hight;
			}

			int ple, pup;
			for (int j = leftTop.x; j < rightDown.x; j++)
			{
				for (int i = leftTop.y; i < rightDown.y; i++)
				{
					ple = player.position.x - leftTop.x;
					pup = player.position.y - leftTop.y;
					int bruh = 0;
					if (map[i][j] == 1) //wall
					{
						bruh = 1;
					}

					else if (map[i][j] == 2) //player
					{
						bruh = 21;
					}

					else if (map[i][j] == 3) //enemy 1
					{
						bruh = 20;
					}

					else if (map[i][j] == 4) //enemy 2
					{
						bruh = 14;
					}

					else if (map[i][j] == 5) //exit
					{
						bruh = 17;
					}

					else if (map[i][j] == 6) //health
					{
						bruh = 18;
					}
					else if (map[i][j] == 7) //ammo
					{
						bruh = 12;
					}
					else if (map[i][j] == 8) //armor
					{
						bruh = 3;
					}
					else
					{
						bruh = 26;
					}

					rt->FillRectangle(D2D1::RectF(Center.x - boxSize / 2  + edge - (ple - j + leftTop.x) * boxSize, Center.y - boxSize / 2 + edge - (pup - i + leftTop.y) * boxSize, Center.x + boxSize / 2 - edge - (ple - j + leftTop.x) * boxSize, Center.y + boxSize / 2 - edge - (pup - i + leftTop.y) * boxSize), brushes[bruh]);
				}
			}


			rt->FillEllipse(D2D1::Ellipse( D2D1::Point2F(Center.x + player.rotation.x * boxSize / 4, Center.y + player.rotation.y * boxSize / 4), boxSize / 8, boxSize / 8), brushes[18]);
		}
	}

	void HUD(ID2D1HwndRenderTarget* rt, box box, ID2D1SolidColorBrush ** brushes, IDWriteTextFormat * pTextFormat)
	{
		//HP
		string hp = std::to_string(player.health);
		for (int i = 0; i < hp.length(); i++)
		{
			DrawNum(rt, D2D1::RectF(box.width - (float)box.width / 16 - (float)box.width * i / 9 / hp.length(), 0, box.width, (float)box.hight / 8), pTextFormat, brushes[18], hp[hp.length() - 1 - i]);
		}

		//HP
		string arm = std::to_string(player.armor);
		for (int i = 0; i < arm.length(); i++)
		{
			DrawNum(rt, D2D1::RectF(box.width - (float)box.width / 16 - (float)box.width * i / 9 / hp.length(), (float)box.hight / 7, box.width, (float)box.hight / 8), pTextFormat, brushes[6], arm[arm.length() - 1 - i]);
		}

		string enem = std::to_string(enemy1count + enemy2count);
		for (int i = 0; i < enem.length(); i++)
		{
			DrawNum(rt, D2D1::RectF(box.width / 18 - -(float)box.width * i / 9 / hp.length(), 0, box.width/7, (float)box.hight / 8), pTextFormat, brushes[11], enem[enem.length() - 1 - i]);
		}
		
		string ammo = std::to_string(player.inGun);
		string amm2 = std::to_string(player.ammo);
		ammo = ammo + "/" + amm2;
		for (int i = 0; i < ammo.length(); i++)
		{
			DrawNum(rt, D2D1::RectF(box.width - (float)box.width / 16 - (float)box.width * i / 9 / hp.length(), box.hight - (float)box.hight / 4, box.width, box.hight), pTextFormat, brushes[6], ammo[ammo.length() - 1 - i]);
		}
		// добавить отображение HP, armor and AMMO 
		// Также можно добавить полупрозрачных красных кравратов или капель через картинки при низком уровне здоровья.
		// Добавлять ли эффект получения урона???????? может пойзже.
	}

	void DrawNum(ID2D1HwndRenderTarget* rt, D2D1_RECT_F pos, IDWriteTextFormat * pTextFormat, ID2D1SolidColorBrush * bruh, char num)
	{
		switch (num)
		{
		case('0'):
			rt->DrawText(L"0", 1, pTextFormat, pos, bruh);
			break;
		case('1'):
			rt->DrawText(L"1", 1, pTextFormat, pos, bruh);
			break;
		case('2'):
			rt->DrawText(L"2", 1, pTextFormat, pos, bruh);
			break;
		case('3'):
			rt->DrawText(L"3", 1, pTextFormat, pos, bruh);
			break;
		case('4'):
			rt->DrawText(L"4", 1, pTextFormat, pos, bruh);
			break;
		case('5'):
			rt->DrawText(L"5", 1, pTextFormat, pos, bruh);
			break;
		case('6'):
			rt->DrawText(L"6", 1, pTextFormat, pos, bruh);
			break;
		case('7'):
			rt->DrawText(L"7", 1, pTextFormat, pos, bruh);
			break;
		case('8'):
			rt->DrawText(L"8", 1, pTextFormat, pos, bruh);
			break;
		case('9'):
			rt->DrawText(L"9", 1, pTextFormat, pos, bruh);
			break;
		case('/'):
			rt->DrawText(L"/", 1, pTextFormat, pos, bruh);
			break;
		}

	}
};