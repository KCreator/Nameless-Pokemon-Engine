#pragma once

#include "stdafx.h"
#include "graphics.h"

//Emitter types:
#define EMITTER_OTHER_BURST 1
#define EMITTER_SELF_BURST 2
#define EMITTER_OTHER_CONSTANT 3
#define EMITTER_SELF_CONSTANT 4
#define EMITTER_STREAM_TO_OTHER 5
#define EMITTER_STREAM_TO_SELF 6

#define MAX_PARTICLES 5000

//Base particle:
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

	bool Dead;

protected:
	float X,Y,XVel,YVel,lifeTime, StartSize, EndSize;
	SDL_Texture *particleTex;

	float Size;
};

//Base emitter:
class CBaseEmitter
{
public:
	CBaseEmitter::CBaseEmitter( const char* path, int x, int y, float xvel, float yvel, float xvelVar, float yvelVar, float startSize, float endSize, float Life );
	CBaseEmitter::~CBaseEmitter();

	void Emit( int count );
	void SimulateAndRender(  );

	void SetX( int i ){ X = i;};
	void SetY( int i ){ Y = i;};

	float GetLife(){return Life;};

protected:
	CBaseParticle *Particle[ MAX_PARTICLES ];

	std::string PartPath;
	int X,Y;
	int activeParticles;
	float XVel, YVel, XVelVar, YVelVar, StartSize, EndSize, Life;
};

//class CStreamEmitter : public CBaseEmitter
//{
//public:
//	CStreamEmitter::CStreamEmitter( const char* path, int x, int y, float xtarg, float ytarg, float xvar, float yvar, float startSize, float endSize );
//	void Emit( int count );
//};