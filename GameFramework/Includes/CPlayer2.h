#pragma once
//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//	   such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

#ifndef _CPLAYER2_H_
#define _CPLAYER2_H_

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "Main.h"
#include "Sprite.h"
#include <vector>

//-----------------------------------------------------------------------------
// Main Class Definitions
//-----------------------------------------------------------------------------
//-----------------------------------------------------------------------------
// Name : CPlayer (Class)
// Desc : Player class handles all player manipulation, update and management.
//-----------------------------------------------------------------------------
class CPlayer2
{
public:
	//-------------------------------------------------------------------------
	// Enumerators
	//-------------------------------------------------------------------------
	enum DIRECTION2
	{
		DIR_FORWARD = 1,
		DIR_BACKWARD = 2,
		DIR_LEFT = 4,
		DIR_RIGHT = 8,
	};

	enum ESpeedStates
	{
		SPEED_START,
		SPEED_STOP
	};

	//-------------------------------------------------------------------------
	// Constructors & Destructors for This Class.
	//-------------------------------------------------------------------------
	CPlayer2(const BackBuffer* pBackBuffer);
	virtual ~CPlayer2();

	//-------------------------------------------------------------------------
	// Public Functions for This Class.
	//-------------------------------------------------------------------------
	void					Update(float dt);
	void					Draw();
	void					Move(ULONG ulDirection2);
	Vec2& Position();
	Vec2& Velocity();

	void					Explode();
	bool					AdvanceExplosion();
	void                    BulletExplosion();

	double getPlayerWidth();

	bool CheckExplosion();

	void RotateSprite(int i);

	std::vector<Sprite*> getVectorCplayer2();

	void lvl21();

	void lvl22();

	void updatebullets2(std::vector<Sprite*> bulletsP1);

	

private:
	//-------------------------------------------------------------------------
	// Private Variables for This Class.
	//-------------------------------------------------------------------------
	Sprite* m_pSprite;
	std::vector<Sprite*> Bullets;
	const BackBuffer* pBackBuffer;
	Sprite* Bullet;
	ESpeedStates			m_eSpeedState;
	float					m_fTimer;

	bool					m_bExplosion;
	bool                    if_Bullet;
	AnimatedSprite* m_pExplosionSprite;
	int						m_iExplosionFrame;
};

#endif // _CPLAYER_H_