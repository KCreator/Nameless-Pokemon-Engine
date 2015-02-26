#include "stdafx.h"
#include "utils.h"
#include "particle.h"

extern SDL_Renderer *gRenderer;
extern BattleEngineGraphics *BattleUIGFX;

CBaseParticle::CBaseParticle( const char* path, float x, float y, float xvel, float yvel, float Life, float startSize, float endSize )
{
	particleTex = NULL;

	X=x;
	Y=y;
	XVel=xvel;
	YVel=yvel;
	lifeTime = SDL_GetTicks() + (Life*1000);
	int temp = SDL_GetTicks();
	temp;
	StartSize = startSize;
	EndSize = endSize;

	Dead = false;

	Size = StartSize;

	SDL_Surface* loadedSurface = IMG_Load( path );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
	}
	particleTex = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	//Get rid of old loaded surface
	SDL_FreeSurface( loadedSurface );
};

void CBaseParticle::SimulateAndRender()
{
	if( Dead )
		return;

	Size = lerp( Size, EndSize, 0.05 );

	//Handle lifetime first:
	int curLife = SDL_GetTicks();
	if( lifeTime <= curLife )
	{
		Dead = true;
	}

	//Move:
	X += XVel;
	Y += YVel;

	if( X > 599 )
	{
		Dead = true;
	}
	if( Y > 479 )
	{
		Dead = true;
	}

	//Draw to the renderer
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture( particleTex, NULL, NULL, &texW, &texH);

	float texScaleX = texW;
	float texScaleY = texH;

	texScaleX *=Size;
	texScaleY *=Size;

	SDL_RenderCopy( gRenderer, particleTex, NULL, &GetRect( X - (texScaleX/2), Y - (texScaleY/2), texScaleX, texScaleY) );
}

CBaseEmitter::CBaseEmitter( const char* path, int x, int y, float xvel, float yvel, float xvelVar, float yvelVar, float startSize, float endSize, float life )
{
	PartPath = path;
	X = x;
	Y = y;
	XVel = xvel;
	YVel = yvel;
	XVelVar = xvelVar;
	YVelVar = yvelVar;

	StartSize = startSize;
	EndSize = endSize;

	Life = life;

	activeParticles = 0;
}

CBaseEmitter::~CBaseEmitter()
{
	for( int i = 0; i < activeParticles; ++i )
    {
        delete Particle[ i ];
    }
}

void CBaseEmitter::Emit( int count )
{
	for( int i = 0; i < count; ++i )
    {
		float varianceX;
		varianceX = (float)rand()/RAND_MAX;

		rand()%2 ? varianceX *= -1: varianceX *= 1;

		varianceX *= XVelVar;

		float varianceY;
		varianceY = (float)rand()/RAND_MAX;

		varianceY *= YVelVar;

		rand()%2 ? varianceY *= -1: varianceY *= 1;

		Particle[ activeParticles ] = new CBaseParticle( PartPath, X, Y, XVel + varianceX, YVel + varianceY, Life, StartSize, EndSize );

		activeParticles++;
	}
}

void CBaseEmitter::SimulateAndRender()
{
	for( int i = 0; i < activeParticles; ++i )
    {
		if( Particle[ i ] != NULL )
		{
			Particle[ i ]->SimulateAndRender();
		}
    }
}

//void CStreamEmitter::Emit( int count )
//{
//	for( int i = 0; i < count; ++i )
//    {
//		float varianceX;
//		varianceX = rand()/rand()%RAND_MAX;
//
//		rand()%2 ? varianceX *= -1: varianceX *= 1;
//
//		varianceX *= XVelVar;
//
//		float varianceY;
//		varianceY = rand()/rand()%RAND_MAX;
//
//		varianceY *= YVelVar;
//
//		rand()%2 ? varianceY *= -1: varianceY *= 1;
//
//		Particle[ activeParticles ] = new CBaseParticle( PartPath, X, Y, XVel + varianceX, YVel + varianceY, 1, StartSize, EndSize );
//
//		activeParticles++;
//	}
//}