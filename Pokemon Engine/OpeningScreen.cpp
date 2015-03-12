#include "stdafx.h"
#include "OpeningScreen.h"
#include "utils.h"
#include "text.h"
#include "DialogFrame.h"

extern bool pressingEnter;
extern bool pressingEsc;
extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;
extern int battleScene;

void OpeningScreen::Init()
{
	//Set up preliminary variables:
	m_bIsTitleMenu = false;
	m_iAnimationState = 0;
	m_iMenuSelection = 0;

	m_iTimer1 = 0;
}

bool OpeningScreen::Think()
{
	//Allow engine to exit if requested:
	SDL_Event events;
	if (SDL_PollEvent( &events ))
	{
		if (events.type == SDL_QUIT)
		{
			return false;
		}
	}

	//Branch off here:
	if( !m_bIsTitleMenu )
		OpeningAnimationThink();
	else
		MenuThink();

	//Render:
	SDL_RenderClear(gRenderer);
	Render();
	SDL_RenderPresent(gRenderer);

	return true;
}

void OpeningScreen::AnimationRender()
{
	//Render our opening animations:
	if( m_iAnimationState == 0 )
	{
		Particles.push_back( new CBaseEmitter( "DATA/GFX/Particles/Glow.png", 640/2, 480,0,3,4,0.2,2,0,0.3) );
		Particles.push_back( new CBaseEmitter( "DATA/GFX/Particles/YellowGlow.png", 640/3, 480,0,3,1,0,2,0,1) );
		Particles.push_back( new CBaseEmitter( "DATA/GFX/Particles/YellowGlow.png", 640 - 640/3, 480,0,3,1,0,2,0,1) );
		m_iAnimationState = 1;
	}
	if( m_iAnimationState == 1 )
	{
		SDL_SetRenderDrawColor(gRenderer,0,0,0,255);
		SDL_RenderFillRect(gRenderer,&GetRect( 0, 0, 640, 480 ) );
		Particles[0]->SetY( 480 - m_iTimer1 );
		Particles[0]->Emit( 5 );

		Particles[1]->Emit( 2 );
		Particles[2]->Emit( 2 );

		Particles[1]->SetY( 480 - m_iTimer1/2 );
		Particles[2]->SetY( 480 - m_iTimer1/2 );

		m_iTimer1++;

		if( m_iTimer1 > 480 )
		{
			m_iAnimationState = 2;
		}
	}
	if( m_iAnimationState == 2 )
	{
		Particles[1]->Emit( 2 );
		Particles[2]->Emit( 2 );

		Particles[1]->SetY( 480 - m_iTimer1/2 );
		Particles[2]->SetY( 480 - m_iTimer1/2 );

		m_iTimer1++;

		if( m_iTimer1 > 480*2 )
		{
			m_iAnimationState = 3;
		}
	}
	RenderAllParticles();
}

void OpeningScreen::MenuRender()
{
}

void OpeningScreen::Render()
{
	//Branch off here:
	if( !m_bIsTitleMenu )
		AnimationRender();
	else
		MenuRender();
}

void OpeningScreen::OpeningAnimationThink()
{
}

void OpeningScreen::MenuThink()
{

}

void OpeningScreen::RenderAllParticles()
{
	if( Particles.size() > 0 )
	{
		for( int i = 0; i < Particles.size(); i++ )
		{
			Particles[i]->SimulateAndRender();
		}
	}
}