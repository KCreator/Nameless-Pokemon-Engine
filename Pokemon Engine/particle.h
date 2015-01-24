#include "stdafx.h"
#include "graphics.h"

#define MAX_PARTICLES 100

class CBaseParticle
{
public:
	CBaseParticle::CBaseParticle( const char* path, float x, float y, float xvel, float yvel, float Life, float startSize, float endSize );

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