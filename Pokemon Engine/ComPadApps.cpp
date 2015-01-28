#include "stdafx.h"
#include "ComPadApps.h"
#include "text.h"
#include "ComuniPad.h"
#include "Overworld.h"

extern TTF_Font *gFont;
extern SDL_Renderer *gRenderer;
extern bool pressingEnter;

extern OverworldController *m_World;
extern bool pressingEsc;

//APP STORE:

CPadAppStore::CPadAppStore()
{
	Apps.push_back( new CPadCalc() );
	Apps.push_back( new CPadClock() );

	Selector = 0;
}

void CPadAppStore::RenderBtn(int x, int y)
{
	//I am number '5'...
	SDL_RenderCopy( gRenderer, cPad->GetTexture(), &GetRect(20*4, 340, 20, 20 ), &GetRect( x, y, 80, 80 ) );
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
	SDL_SetRenderDrawColor(gRenderer,255,0,0,0);
	SDL_RenderDrawRect( gRenderer, &GetRect( 90, 200, 80, 80 ));

	//Draw text and icons:
	if( Selector-1 < Apps.size() && Selector > 0 && Apps.at(Selector-1) != NULL )
	{
		Apps[Selector-1]->cPad = cPad;
		Apps[Selector-1]->RenderBtn( 90, 120 );
	}
	if( Selector < Apps.size() && Apps.at(Selector) != NULL )
	{
		Apps[Selector]->cPad = cPad;
		Apps[Selector]->RenderBtn( 90, 200 );
		Apps[Selector]->RenderAppStoreText();
	}
	if( Selector+1 < Apps.size() && Apps.at(Selector+1) != NULL )
	{
		Apps[Selector+1]->cPad = cPad;
		Apps[Selector+1]->RenderBtn( 90, 280 );
	}

	SDL_SetRenderDrawColor(gRenderer,0,0,0,0);
}

bool CPadAppStore::Think()
{
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	if( pressingEsc )
	{
		pressingEsc = false;
		return false;
	}

	if( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
	{
		if( debouncer )
		{
			Selector--;
			if( Selector < 0 )
				Selector = 0;
		}
		debouncer = false;
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
	{
		if( debouncer )
		{
			Selector++;
		}
		debouncer = false;
	}
	else
		debouncer = true;

	if( pressingEnter )
	{
		pressingEnter = false;

		if( Selector < Apps.size() && Apps.at(Selector) != NULL )
		{
			cPad->InstallApp( Apps.at( Selector ) );
			Apps.erase( Apps.begin() + Selector );
			Apps.shrink_to_fit();
		}
	}

	SDL_RenderClear( gRenderer );
	RenderApp();
	SDL_RenderPresent( gRenderer );

	return true;
}

//PHONE:
void CPadPhone::RenderBtn(int x, int y)
{
	//I am number '0'...
	SDL_RenderCopy( gRenderer, cPad->GetTexture(), &GetRect(20*0, 340, 20, 20 ), &GetRect( x, y, 80, 80 ) );
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
	if( pressingEsc )
	{
		pressingEsc = false;
		return false;
	}

	SDL_RenderClear( gRenderer );
	RenderApp();
	SDL_RenderPresent( gRenderer );

	return true;
}

//CALCULATOR:
void CPadCalc::RenderBtn(int x, int y)
{
	SDL_RenderCopy( gRenderer, cPad->GetTexture(), &GetRect(20*5, 340, 20, 20 ), &GetRect( x, y, 80, 80 ) );
}

bool CPadCalc::Think()
{
	return false;
}

void CPadCalc::RenderApp()
{

}

void CPadCalc::RenderAppStoreText()
{
	CText *txt = new CText( "CALCULATOR APP:", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 150, 0, 0 ) );
	delete txt;
	txt = new CText( "A simple calculator! Perfect", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 180, 0, 0 ) );
	delete txt;
	txt = new CText( "for those tricky questions!", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 210, 0, 0 ) );
	delete txt;
	txt = new CText( "Cost: 1000P", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 240, 0, 0 ) );
	delete txt;
};

//CLOCK:
void CPadClock::RenderBtn(int x, int y)
{
	SDL_RenderCopy( gRenderer, cPad->GetTexture(), &GetRect(20*6, 340, 20, 20 ), &GetRect( x, y, 80, 80 ) );
}

bool CPadClock::Think()
{
	if( pressingEsc )
	{
		pressingEsc = false;
		return false;
	}

	SDL_RenderClear( gRenderer );
	RenderApp();
	SDL_RenderPresent( gRenderer );

	return true;
}

//Need the time:
#include <ctime>

void CPadClock::RenderApp()
{
	//Render the CLOCK app:
	//Render BG:
	m_World->Render();
	cPad->RenderBG();
	
	//Create the "CLOCK" front!
	SDL_SetRenderDrawColor(gRenderer,100,100,100,255);
	SDL_RenderFillRect( gRenderer, &GetRect( 80, 80, 440, 355 ) );

	SDL_SetRenderDrawColor(gRenderer,0,0,0,255);
	SDL_RenderFillRect( gRenderer, &GetRect( 85, 85, 430, 345 ) );

	SDL_SetRenderDrawColor(gRenderer,240,240,240,240);
	SDL_RenderFillRect( gRenderer, &GetRect( 85, 85, 430, 30 ) );

	CText *txt = new CText( "CLOCK:", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 90, 90, 0, 0 ) );
	delete txt;

	// get time now
	time_t t = time(0);   
    struct tm * now = localtime( &t );

	//Draw the time:
	std::string Time;
	Time += std::to_string( (_ULonglong)now->tm_hour );
	//Hacky:
	int Blinker = now->tm_sec%2;
	if( Blinker == 1 )
	{
		Time += " : ";
	}
	else
	{
		Time += "   ";
	}
	Time += std::to_string( (_ULonglong)now->tm_min );

	txt = new CText( Time, gRenderer, gFont, 0, 0, 255, 0 );
	txt->Render(&GetRect( 100, 125, 400, 50 ) );
	delete txt;

	SDL_SetRenderDrawColor(gRenderer,0,0,0,255);
}

void CPadClock::RenderAppStoreText()
{
	CText *txt = new CText( "CLOCK APP:", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 150, 0, 0 ) );
	delete txt;
	txt = new CText( "Tell the time with this", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 180, 0, 0 ) );
	delete txt;
	txt = new CText( "handy CLOCK APP!", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 210, 0, 0 ) );
	delete txt;
	txt = new CText( "Cost: 100P", gRenderer, gFont, 1 );
	txt->Render(&GetRect( 190, 240, 0, 0 ) );
	delete txt;
};