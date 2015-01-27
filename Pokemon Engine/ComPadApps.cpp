#include "stdafx.h"
#include "ComPadApps.h"
#include "text.h"
#include "ComuniPad.h"
#include "Overworld.h"

extern TTF_Font *gFont;
extern SDL_Renderer *gRenderer;
extern bool pressingEnter;

extern OverworldController *m_World;


//APP STORE:
void CPadAppStore::RenderBtn()
{
	//I am number '5'...
	SDL_RenderCopy( gRenderer, cPad->GetTexture(), &GetRect(20*5, 340, 20, 20 ), &GetRect( 90, 200, 80, 80 ) );
}

void CPadAppStore::RenderApp()
{
	//Render BG:
	m_World->Render();
	cPad->RenderBG();
	
	//Create the "app store" front!
	SDL_SetRenderDrawColor(gRenderer,100,100,100,255);

	SDL_RenderFillRect( gRenderer, &GetRect( 80, 80, 440, 355 ) );
	SDL_SetRenderDrawColor(gRenderer,255,255,255,255);
	SDL_RenderFillRect( gRenderer, &GetRect( 85, 85, 430, 345 ) );
	SDL_SetRenderDrawColor(gRenderer,240,240,240,240);
	SDL_RenderFillRect( gRenderer, &GetRect( 85, 85, 430, 30 ) );

	CText *txt = new CText( "APP Store", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 90, 90, 0, 0 ) );
	delete txt;

	SDL_SetRenderDrawColor(gRenderer,0,0,0,0);
	SDL_RenderDrawRect( gRenderer, &GetRect( 90, 120, 80, 240 ));

	SDL_RenderDrawRect( gRenderer, &GetRect( 90, 200, 80, 80 ));

	//stuff
	SDL_RenderCopy( gRenderer, cPad->GetTexture(), &GetRect(20*6, 340, 20, 20 ), &GetRect( 90, 200, 80, 80 ) );
	SDL_RenderCopy( gRenderer, cPad->GetTexture(), &GetRect(20*7, 340, 20, 20 ), &GetRect( 90, 280, 80, 80 ) );

	//Temp
	txt = new CText( "TIME APP:", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 150, 0, 0 ) );
	delete txt;
	txt = new CText( "A usefull app that will display", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 180, 0, 0 ) );
	delete txt;
	txt = new CText( "the time!", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 210, 0, 0 ) );
	delete txt;
	txt = new CText( "Cost: 100P", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 240, 0, 0 ) );
	delete txt;

	SDL_SetRenderDrawColor(gRenderer,0,0,0,0);
}

bool CPadAppStore::Think()
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if( keystate[SDL_GetScancodeFromKey(SDLK_ESCAPE)] && !pressingEnter )
	{
		return false;
	}

	SDL_RenderClear( gRenderer );
	RenderApp();
	SDL_RenderPresent( gRenderer );

	return true;
}



//PHONE:
void CPadPhone::RenderBtn()
{
	//I am number '0'...
	SDL_RenderCopy( gRenderer, cPad->GetTexture(), &GetRect(20*0, 340, 20, 20 ), &GetRect( 90, 200, 80, 80 ) );
}

void CPadPhone::RenderApp()
{
	//Render BG:
	m_World->Render();
	cPad->RenderBG();
	
	//Create the "Phone" front!
	SDL_SetRenderDrawColor(gRenderer,100,100,100,255);

	SDL_RenderFillRect( gRenderer, &GetRect( 80, 80, 440, 355 ) );
	SDL_SetRenderDrawColor(gRenderer,255,255,255,255);
	SDL_RenderFillRect( gRenderer, &GetRect( 85, 85, 430, 345 ) );
	SDL_SetRenderDrawColor(gRenderer,240,240,240,240);
	SDL_RenderFillRect( gRenderer, &GetRect( 85, 85, 430, 30 ) );

	CText *txt = new CText( "Phone", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 90, 90, 0, 0 ) );
	delete txt;

	txt = new CText( "Contacts:", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 90, 123, 0, 0 ) );
	delete txt;

	SDL_SetRenderDrawColor(gRenderer,0,0,0,0);
	SDL_RenderDrawRect( gRenderer, &GetRect( 85, 120, 130, 240 ));
	SDL_RenderDrawRect( gRenderer, &GetRect( 85, 120, 130, 30 ));

	//Temp
	txt = new CText( "Prof. Oak", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 90, 153, 0, 0 ) );
	delete txt;
	txt = new CText( "Game Dev", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 90, 183, 0, 0 ) );
	delete txt;

	SDL_SetRenderDrawColor(gRenderer,0,0,0,0);
}

bool CPadPhone::Think()
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if( keystate[SDL_GetScancodeFromKey(SDLK_ESCAPE)] && !pressingEnter )
	{
		return false;
	}

	SDL_RenderClear( gRenderer );
	RenderApp();
	SDL_RenderPresent( gRenderer );

	return true;
}
