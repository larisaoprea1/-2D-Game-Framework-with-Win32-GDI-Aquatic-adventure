#pragma once
#ifndef _ENEMY_H_
#define _ENEMY_H_

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"
#include <list>
#include <vector>
#include <windows.h>
#include <cstdlib>
#include <iostream>
#include <chrono>
#include <thread>
#include <functional>
#include <fstream>
#include <io.h>
#include <time.h>


//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CPlayer (Class)
// Desc : Player class handles all player manipulation, update and management.
//-----------------------------------------------------------------------------
class Enemy
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	Enemy(const BackBuffer* pBackBuffer);
	virtual ~Enemy();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					Update(float dt);
	void					Draw();
	void					SpawnCrate();
	Vec2& Position();
	Vec2& Velocity();
	std::vector<Sprite*>    getVectorCrate();
	void GetCratePos();
	double					getShipWidth();
	bool					ifExplosion();
	void updatecrate(std::vector <Sprite*> crates);

	void spawncoins();

	void updatecoins(float dt);

	void drawcoins();

	std::vector<Sprite*> getVectorCoin();

	void updatevectorcoin(std::vector<Sprite*> crates);


private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	std::vector<Sprite*>	crates;
	const BackBuffer* pBackBuffer;
	std::vector<Sprite*>	coins;

};

#endif // _ENEMY_H_
;