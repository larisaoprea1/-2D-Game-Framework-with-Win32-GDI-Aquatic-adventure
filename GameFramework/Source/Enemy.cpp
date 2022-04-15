#include "Enemy.h"


//-----------------------------------------------------------------------------
// Name : CPlayer () (Constructor)
// Desc : CPlayer Class Constructor
//-----------------------------------------------------------------------------
Enemy::Enemy(const BackBuffer* pBackBuffer)
{
	this->pBackBuffer = pBackBuffer;


	RECT r;
	r.left = 0;
	r.top = 0;
	r.right = 128;
	r.bottom = 128;
	srand(time(NULL));
}
//-----------------------------------------------------------------------------
// Name : ~CPlayer () (Destructor)
// Desc : CPlayer Class Destructor
//-----------------------------------------------------------------------------
Enemy::~Enemy()
{

}

void Enemy::Update(float dt)
{
	// Update sprite

	int i = 0;
	RECT desktop;
	GetWindowRect(GetDesktopWindow(), &desktop);
	for (Sprite* crate : crates) {
		crate->update(dt);

		if (crate->mPosition.y > desktop.bottom)
		{
			crates.erase(crates.begin() + i);
		}
		i++;
	}


}

void Enemy::Draw()
{

	for (Sprite* bullet : crates)
	{
		bullet->setBackBuffer(pBackBuffer);
		bullet->draw();
	}


}

void Enemy::SpawnCrate()
{
	crates.push_back(new Sprite("data/crate.bmp", RGB(0xff, 0x00, 0xff)));
	crates.back()->mPosition.x = 0;
	crates.back()->mPosition.x = rand() % 600 + 100;
	crates.back()->mVelocity.y = 300 ;
}


std::vector<Sprite*> Enemy::getVectorCrate()
{
	return crates;
}
void Enemy::GetCratePos()

{
	for (Sprite* crate : crates)
	{

  	}
}

void Enemy::updatecrate(std::vector <Sprite*> crates)
{
	this->crates = crates;
}
void Enemy::spawncoins(){
	coins.push_back(new Sprite("data/coin.bmp", RGB(0xff, 0x00, 0xff)));
	coins.back()->mPosition.x = rand() % 750 + 50;
	coins.back()->mPosition.y = rand() % 500 + 50;
}
void Enemy::updatecoins(float dt){
	for (Sprite* crate :coins) {
		crate->update(dt);

		
	}
}
void Enemy::drawcoins(){
	for (Sprite* crate : coins) {
		crate->setBackBuffer(pBackBuffer);
		crate->draw();

	}

}
std::vector<Sprite*> Enemy::getVectorCoin()
{
	return coins;
}
void Enemy::updatevectorcoin(std::vector <Sprite*> crates)
{
	this->coins = crates;
}
