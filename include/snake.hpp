#pragma once
#include <vector>
#include "raylib.h"

const int cellCountX = 11;
const int cellCountY = 10;
const int tableyoffset =1;

const float moveTimeDuration = 0.2;
const float moveTimeDurationlvl2 = 0.16;
const float moveTimeDurationlvl3 = 0.12;


struct Coordinate{
	float x;
	float y;
	float rotation;
	float prev_rotation;
	bool corner;
	Coordinate(): x(-1), y(-1), rotation(0),prev_rotation(-1),corner(false){} 
	Coordinate(float x1,float y1): x(x1), y(y1),rotation(0),prev_rotation(-1),corner(false){} 
	Coordinate(float x1,float y1, float r): x(x1), y(y1),rotation(r),prev_rotation(-1),corner(false){} 
	bool operator==(const Coordinate& other){
		return (x == other.x && y == other.y);
	}
};

extern float cooldownDuration;
extern float cellSize;
extern float timer;
extern float cooldown;
extern float cooldownpause;
extern std::vector<Coordinate> SnakePos;
extern Coordinate fruitpos;
extern Coordinate Dir; 
