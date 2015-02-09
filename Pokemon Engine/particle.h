#include "stdafx.h"
#include "graphics.h"

#define MAX_PARTICLES 5000

class CBaseParticle
{
public:
	CBaseParticle::CBaseParticle( const char* path, float x, float y, float xvel, float yvel, float Life, float startSize, float endSize );
	CBaseParticle::~CBaseParticle()
	{
		//Free up RAM:
		SDL_DestroyTexture( particleTex );
	};

	void SimulateAndRender();
private:
	float X,Y,XVel,YVel,lifeTime, StartSize, EndSize;
	SDL_Texture *particleTex;

	float Size;

	bool Dead;
};

class CBaseEmitter
{
public:
	CBaseEmitter::CBaseEmitter( const char* path, int x, int y, float xvel, float yvel, float xvelVar, float yvelVar, float startSize, float endSize );
	CBaseEmitter::~CBaseEmitter();

	void Emit( int count );
	void SimulateAndRender(  );
private:
	CBaseParticle *Particle[ MAX_PARTICLES ];

	const char *PartPath;
	int X,Y;
	int activeParticles;
	float XVel, YVel, XVelVar, YVelVar, StartSize, EndSize;
};

//class CStreamEmitter : public CBaseEmitter
//{
//public:
//	CStreamEmitter::CStreamEmitter( const char* path, int x, int y, float xtarg, float ytarg, float xvar, float yvar, float startSize, float endSize );
//	void Emit( int count );
//};