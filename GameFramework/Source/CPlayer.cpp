//-----------------------------------------------------------------------------
// File: CPlayer.cpp
//
// Desc: This file stores the player object class. This class performs tasks
//       such as player movement, some minor physics as well as rendering.
//
// Original design by Adam Hoult & Gary Simmons. Modified by Mihai Popescu.
//-----------------------------------------------------------------------------

//-----------------------------------------------------------------------------
// CPlayer Specific Includes
//-----------------------------------------------------------------------------
#include "CPlayer.h"
#include <vector>
#include <vector>

//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
CPlayer::CPlayer(const BackBuffer *pBackBuffer)
{
	//m_pSprite = new Sprite("data/planeimg.bmp", "data/planemask.bmp");
	m_pSprite = new Sprite("data/PlaneImgAndMask.bmp", RGB(0xff,0x00, 0xff));
	m_pSprite->setBackBuffer( pBackBuffer );
	m_eSpeedState = SPEED_STOP;
	m_fTimer = 0;

	// Animation frame crop rectangle
	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;

	m_pExplosionSprite	= new AnimatedSprite("data/explosion.bmp", "data/explosionmask.bmp", r, 16);
	m_pExplosionSprite->setBackBuffer( pBackBuffer );
	m_bExplosion		= false;
	m_iExplosionFrame	= 0;

	this->pBackBuffer = pBackBuffer;
	
}

//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
CPlayer::~CPlayer()
{
	delete m_pSprite;
	delete m_pExplosionSprite;
	
}

void CPlayer::Update(float dt)
{
	// Update sprite
	m_pSprite->update(dt);
	int it = 0;
	RECT Desktop;
	GetWindowRect(GetDesktopWindow(), &Desktop);
	for (Sprite* Bullet : Bullets) {
		Bullet->update(dt);
		if (Bullet->mPosition.y > Desktop.bottom) {
			Bullets.erase(Bullets.begin() + it);
		}
		it++;
	}


	// Get velocity
	double v = m_pSprite->mVelocity.Magnitude();

	// NOTE: for each async sound played Windows creates a thread for you
	// but only one, so you cannot play multiple sounds at once.
	// This creation/destruction of threads also leads to bad performance
	// so this method is not recommanded to be used in complex projects.

	// update internal time counter used in sound handling (not to overlap sounds)
	m_fTimer += dt;

	// A FSM is used for sound manager 
	switch(m_eSpeedState)
	{
	case SPEED_STOP:
		if(v > 35.0f)
		{
			m_eSpeedState = SPEED_START;
			PlaySound("data/jet-start.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		break;
	case SPEED_START:
		if(v < 25.0f)
		{
			m_eSpeedState = SPEED_STOP;
			PlaySound("data/jet-stop.wav", NULL, SND_FILENAME | SND_ASYNC);
			m_fTimer = 0;
		}
		else
			if(m_fTimer > 1.f)
			{
				PlaySound("data/jet-cabin.wav", NULL, SND_FILENAME | SND_ASYNC);
				m_fTimer = 0;
			}
		break;
	}

	// NOTE: For sound you also can use MIDI but it's Win32 API it is a bit hard
	// see msdn reference: http://msdn.microsoft.com/en-us/library/ms711640.aspx
	// In this case you can use a C++ wrapper for it. See the following article:
	// http://www.codeproject.com/KB/audio-video/midiwrapper.aspx (with code also)
}

void CPlayer::Draw()
{
	if(!m_bExplosion)
		m_pSprite->draw();
	else
		m_pExplosionSprite->draw();

	for (Sprite* Bullet : Bullets) {
		Bullet->setBackBuffer(pBackBuffer);
		Bullet->draw();
		
	}
	
		
}

void CPlayer::Move(ULONG ulDirection)
{
	if (ulDirection & CPlayer::DIR_LEFT)
		m_pSprite->mVelocity.x -= 3;
	if (m_pSprite->mPosition.x < m_pSprite->width()/2)
		m_pSprite->mVelocity.x = 0;

	if( ulDirection & CPlayer::DIR_RIGHT )
		m_pSprite->mVelocity.x += 3;
	if (m_pSprite->mPosition.x > 750)
		m_pSprite->mVelocity.x = -1;


	if( ulDirection & CPlayer::DIR_FORWARD )
		m_pSprite->mVelocity.y -= 3;
	if (m_pSprite->mPosition.y < m_pSprite->height()/2)
		m_pSprite->mVelocity.y = 0;

	if( ulDirection & CPlayer::DIR_BACKWARD )
		m_pSprite->mVelocity.y += 3;
	if (m_pSprite->mPosition.y > 490)
		m_pSprite->mVelocity.y = -1;
	
}


Vec2& CPlayer::Position()
{
	return m_pSprite->mPosition;
}

Vec2& CPlayer::Velocity()
{
	return m_pSprite->mVelocity;
}

void CPlayer::Explode()
{
	m_pExplosionSprite->mPosition = m_pSprite->mPosition;
	m_pExplosionSprite->SetFrame(0);
	PlaySound("data/explosion.wav", NULL, SND_FILENAME | SND_ASYNC);
	m_bExplosion = true;
}

bool CPlayer::AdvanceExplosion()
{
	if(m_bExplosion)
	{
		m_pExplosionSprite->SetFrame(m_iExplosionFrame++);
		if(m_iExplosionFrame==m_pExplosionSprite->GetFrameCount())
		{
			m_bExplosion = false;
			m_iExplosionFrame = 0;
			m_pSprite->mVelocity = Vec2(0,0);
			m_eSpeedState = SPEED_STOP;
			return false;
		}
	}

	return true;
}

void CPlayer::BulletExplosion() {
	Bullets.push_back(new Sprite("data/bullet.bmp", RGB(0xff, 0x00, 0xff)));
	Bullets.back()->mPosition = m_pSprite->mPosition;
	Bullets.back()->mVelocity.y = -1000;

}

void CPlayer::RotateSprite(int i)
{
	std::vector<char*> Rotate;

	Rotate.push_back("data/PlaneImgAndMask.bmp");
	Rotate.push_back("data/PlaneImgAndMaskRight.bmp");
	Rotate.push_back("data/PlaneImgAndMaskDown.bmp");
	Rotate.push_back("data/PlaneImgAndMaskLeft.bmp");

	m_pSprite->setSprite(Rotate.at(i), RGB(0xff, 0x00, 0xff));
}
double CPlayer::getPlayerWidth() {
	return m_pSprite->width();
 }
bool CPlayer::CheckExplosion() {
	if (m_bExplosion)
		return true;
	else return false;

}
std::vector<Sprite*> CPlayer::getVectorCplayer()
{
	return Bullets;
}

void CPlayer::updatebullets(std::vector <Sprite*> bulletsP1)
{
	this->Bullets = bulletsP1;
}
void CPlayer::lvl1() {


	m_pSprite->setSprite("data/peste.bmp", RGB(0xff, 0x00, 0xff));
}
void CPlayer::lvl2() {


	m_pSprite->setSprite("data/rechin.bmp", RGB(0xff, 0x00, 0xff));
}

void CPlayer::SpawnHeart() {
	heart.push_back(new Sprite("data/inimaa.bmp", RGB(0xff, 0x00, 0xff)));
	heart.back()->mPosition.x = rand() % 700 + 100;
	heart.back()->mPosition.y = rand() % 300 + 100;
}
void CPlayer::DrawHeart() {
	for (Sprite* inima : heart) {
		inima->setBackBuffer(pBackBuffer);
		inima->draw();

	}
}
void CPlayer::UpdateHeart(float dt) {
	for (Sprite* inima : heart) {
		inima->update(dt);

	}
}
std::vector<Sprite*> CPlayer::GetVectorHeart() {
	return heart;

}
void CPlayer::UpdateVectorHeart(std::vector<Sprite*> heart) {
	this->heart = heart;
}