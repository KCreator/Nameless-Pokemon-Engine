#include "stdafx.h"
#include "bag.h"
#include "text.h"
#include "Overworld.h"
#include "DialogFrame.h"

//Todo: Add the remaining needed externs:
extern bool pressingEnter;
extern bool pressingEsc;
extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;
extern int battleScene;
extern OverworldController *m_World;

void BagScene::Init()
{
	//Load bag textures:
	SDL_Surface* loadedSurface = IMG_Load( "DATA/GFX/UI/Bag.png" );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", "DATA/GFX/UI/GBASummary.png", IMG_GetError() );
	}

	BagUI = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	//Set needed values:
	m_iCurPocket = 0; //Generic Items
	numItems = 0;

	selection = 0;

	SelectedItem = false;
	m_iSubSelection = 0;
}

bool BagScene::Tick()
{
	//Do the render thingy:
	SDL_Event events;
	if (SDL_PollEvent(&events))
	{
		if (events.type == SDL_QUIT)
		{
			return false;
		}
		if (events.type == SDL_KEYDOWN)
		{
			//if( events.key.repeat == 0 )
			//{
			if( !SelectedItem )
			{
				if( events.key.keysym.sym == SDLK_s || events.key.keysym.sym == SDLK_DOWN )
				{
					selection++;
					if( selection > items.size() )
					{
						selection = items.size();
					}
				}
				if( events.key.keysym.sym == SDLK_w | events.key.keysym.sym == SDLK_UP )
				{
					selection--;
					if( selection < 0 )
						selection = 0;
				}
			}
			//}
		}
	}

	//Submenu handler:
	if( SelectedItem )
	{
		const Uint8 *keystate = SDL_GetKeyboardState(NULL);
		if ( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
		{
			if( m_iSubSelection == 3 )
			{
				m_iSubSelection = 1;
			}
			if( m_iSubSelection == 4 )
			{
				m_iSubSelection = 2;
			}
		}
		else if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
		{
			if( m_iSubSelection == 1 )
			{
				m_iSubSelection = 3;
			}
			if( m_iSubSelection == 2 )
			{
				m_iSubSelection = 4;
			}
		}
		else if( keystate[SDL_GetScancodeFromKey(SDLK_d)] || keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)] )
		{
			if( !IsBattle )
			{
				if( m_iSubSelection == 1 )
				{
					m_iSubSelection = 2;
				}
				if( m_iSubSelection == 3 )
				{
					m_iSubSelection = 4;
				}
			}
		}
		else if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)])
		{
			if( !IsBattle )
			{
				if( m_iSubSelection == 2 )
				{
					m_iSubSelection = 1;
				}
				if( m_iSubSelection == 4 )
				{
					m_iSubSelection = 3;
				}
			}
		}
	}

	if( pressingEnter )
	{
		pressingEnter = false; //Force debounce!

		//Handle item usage:
		if( !SelectedItem )
		{
			if( selection < items.size() )
			{
				SelectedItem = true;
				m_iSubSelection = 1;
			}

			if( selection == items.size() )
			{
				battleScene = PreviousScene;
				FadeToBlack();
				//Switch previes scenes:
				switch( PreviousScene )
				{
				case SCENE_OVERWORLD: m_World->FadeIn(); break;
				}
				return true;
			}
		}
		else
		{
			if( m_iSubSelection == 1 )
			{
				SelectedItem = false;
				m_iSubSelection = 1;
				if( items[selection]->CanUse( IsBattle ) )
				{
					items[selection]->Use();
					DeleteUsedItems();
				}
			}
			else if( m_iSubSelection == 3 )
			{
				//Battle dialog has less options.
				if( IsBattle )
				{
					SelectedItem = false;
					m_iSubSelection = 1;
				}
			}
			else if( m_iSubSelection == 4 )
			{
				SelectedItem = false;
				m_iSubSelection = 1;
			}
		}
	}

	if( pressingEsc )
	{
		pressingEsc = false;

		if( !SelectedItem )
		{
			battleScene = PreviousScene;
			FadeToBlack();
			//Switch previes scenes:
			switch( PreviousScene )
			{
				case SCENE_OVERWORLD: m_World->FadeIn(); break;
			}
			return true;
		}
		else
		{
			SelectedItem = false;
			m_iSubSelection = 1;
		}
	}

	SDL_RenderClear( gRenderer );
	Render();
	SDL_RenderPresent( gRenderer );

	return true;
}

void BagScene::AddItem( CBaseItem *item, int count )
{
	//Null pointer check
	if( item == NULL )
		return;
	if( !HasItem( item->ID ) )
	{
		item->Add( count );
		items.push_back( item );
		numItems++;
	}
	else
	{
		for( int i = 0; i < items.size(); i ++ )
		{
			if( items[i]->ID == item->ID )
			{
				items[i]->Add( count );
				delete item;
			}
		}
	}
}

bool BagScene::HasItem( int ID )
{
	for( int i = 0; i < items.size(); i ++ )
	{
		if( items[i]->ID == ID )
			return true;
	}
	return false;
}

int BagScene::GetItemCount( int ID )
{
	for( int i = 0; i < items.size(); i ++ )
	{
		if( items[i]->ID == ID )
		{
			return items[i]->GetAmount();
		}
	}
	return 0;
}

//Render it!
void BagScene::Render()
{
	//Copy main screen to render buffer:
	SDL_RenderCopy( gRenderer, BagUI, &GetRect( 8, 24, 240, 160 ), &GetRect( 0, 0, 600, 480 ) );

	//Render the "bag" sprite:
	SDL_RenderCopy( gRenderer, BagUI, &GetRect( 12, 194, 51, 60 ), &GetRect( 100, 120, 150, 160 ) );

	//Render "Current pocket" sprite:
	SDL_RenderCopy( gRenderer, BagUI, &GetRect( 504, 24 + ( 16 * m_iCurPocket ), 80, 16 ), &GetRect( 61, 24, 210, 48 ) );

	//Render cursor:
	if( !SelectedItem )
		SDL_RenderCopy( gRenderer, BagUI, &GetRect( 504 + (8 * isMovingItem ? 1 : 0), 172, 8, 10 ), &GetRect( 285, 55 + ( 30 * selection ) - 2, 20, 30 ) );
	else 
		SDL_RenderCopy( gRenderer, BagUI, &GetRect( 504 + (8 * 1 ), 172, 8, 10 ), &GetRect( 285, 55 + ( 30 * selection ) - 2, 20, 30 ) );

	CText *txt;
	//Render Item names:

	for( int i = 0; i < items.size(); i++ )
	{
		txt = new CText( items[i]->GetName(), gRenderer, gFont, 1 );
		txt->Render( &GetRect( 305, 55 + ( i * 30 ), 0 , 0 ));
		delete txt;

		txt = new CText( "x" + std::to_string((_ULonglong)items[i]->GetAmount() ), gRenderer, gFont, 1 );
		txt->Render( &GetRect( 505, 55 + ( i * 30 ), 0 , 0 ));
		delete txt;

		if( selection == i )
		{
			if( !SelectedItem )
			{
				//Render item description:

				//Hacky split string!
				char split[] = "\n";
				char *token;
				std::string str = "";
				str += items[i]->GetDesc();
				token = strtok( &str[0], split );
				int newLines = 0;
				while( token )
				{
					txt = new CText( token, gRenderer, gFont, 1 );
					txt->Render( &GetRect( 10, 310 + (30 * newLines ), 0, 0 ));
					delete txt;

					token = strtok( NULL, split );
					newLines++;
				}

				delete token;
			}
			else
			{
				//render: ITEM is selected
				std::string str = "";
				str += items[i]->GetName() + " is";

				txt = new CText( str, gRenderer, gFont, 1 );
				txt->Render( &GetRect( 10, 310 + (30 * 0 ), 0, 0 ));
				delete txt;
				txt = new CText( "selected.", gRenderer, gFont, 1 );
				txt->Render( &GetRect( 10, 310 + (30 * 1 ), 0, 0 ));
				delete txt;
			}
		}
	}

	//Render "Close bag"
	txt = new CText( "Close bag", gRenderer, gFont, 1 );
	txt->Render( &GetRect( 305, 55 + (numItems * 30 ), 0 , 0 ));
	delete txt;

	if( selection == numItems )
	{
		//Render big return arrow, and text:
		SDL_RenderCopy( gRenderer, BagUI, &GetRect( 600 , 160, 24, 24 ), &GetRect( 10, 215, 80, 80 ) );

		txt = new CText( "Return to", gRenderer, gFont, 1 );
		txt->Render( &GetRect( 10, 330, 0, 0 ));
		delete txt;

		if( !IsBattle )
		{
			txt = new CText( "the field.", gRenderer, gFont, 1 );
			txt->Render( &GetRect( 10, 360, 0, 0 ));
			delete txt;
		}
		else
		{
			txt = new CText( "the battle.", gRenderer, gFont, 1 );
			txt->Render( &GetRect( 10, 360, 0, 0 ));
			delete txt;
		}
	}

	//Render submenu when applicable:
	if( SelectedItem )
	{
		if( !IsBattle )
		{
			DialogFrame *txtbox = new DialogFrame();
			txtbox->Render( 285, 380, 310, 90 );
			delete txtbox;

			//Cursor:
			switch( m_iSubSelection )
			{
			case 1: SDL_RenderCopy( gRenderer, BagUI, &GetRect( 504, 172, 8, 10 ), &GetRect( 285 + 10, 400, 20, 30 ) ); break;
			case 2: SDL_RenderCopy( gRenderer, BagUI, &GetRect( 504, 172, 8, 10 ), &GetRect( 285 + 10 + 130, 400, 20, 30 ) ); break;
			case 3: SDL_RenderCopy( gRenderer, BagUI, &GetRect( 504, 172, 8, 10 ), &GetRect( 285 + 10, 380 + 20 + 30, 20, 30 ) ); break;
			case 4: SDL_RenderCopy( gRenderer, BagUI, &GetRect( 504, 172, 8, 10 ), &GetRect( 285 + 10 + 130, 380 + 20 + 30, 20, 30 ) ); break;
			}

			//Options:
			txt = new CText( "Use", gRenderer, gFont, 1 );
			txt->Render( &GetRect( 285 + 30, 380 + 20, 0 , 0 ));
			delete txt;

			txt = new CText( "Give", gRenderer, gFont, 1 );
			txt->Render( &GetRect( 285 + 30 + 130, 380 + 20, 0 , 0 ));
			delete txt;

			txt = new CText( "Toss", gRenderer, gFont, 1 );
			txt->Render( &GetRect( 285 + 30, 380 + 20 + 30, 0 , 0 ));
			delete txt;

			txt = new CText( "Cancel", gRenderer, gFont, 1 );
			txt->Render( &GetRect( 285 + 30 + 130, 380 + 20 + 30, 0 , 0 ));
			delete txt;
		}
		else
		{
			DialogFrame *txtbox = new DialogFrame();
			txtbox->Render( 280 + 130, 380, 180, 90 );
			delete txtbox;

			//Cursor:
			switch( m_iSubSelection )
			{
			case 1: SDL_RenderCopy( gRenderer, BagUI, &GetRect( 504, 172, 8, 10 ), &GetRect( 285 + 10 + 130, 400, 20, 30 ) ); break;
			case 3: SDL_RenderCopy( gRenderer, BagUI, &GetRect( 504, 172, 8, 10 ), &GetRect( 285 + 10 + 130, 380 + 20 + 30, 20, 30 ) ); break;
			}

			//Options:
			txt = new CText( "Use", gRenderer, gFont, 1 );
			txt->Render( &GetRect( 285 + 30 + 130, 380 + 20, 0 , 0 ));
			delete txt;

			/*txt = new CText( "Give", gRenderer, gFont, 1 );
			txt->Render( &GetRect( 285 + 30 + 130, 380 + 20, 0 , 0 ));
			delete txt;

			txt = new CText( "Toss", gRenderer, gFont, 1 );
			txt->Render( &GetRect( 285 + 30, 380 + 20 + 30, 0 , 0 ));
			delete txt;*/

			txt = new CText( "Cancel", gRenderer, gFont, 1 );
			txt->Render( &GetRect( 285 + 30 + 130, 380 + 20 + 30, 0 , 0 ));
			delete txt;
		}
	}
}

void BagScene::DeleteUsedItems()
{
	for( int i = 0; i < items.size(); i ++ )
	{
		if( items[i]->GetAmount() == 0 )
		{
			delete items[i];
			items.erase( items.begin() + i );
			items.shrink_to_fit();
			numItems--;
		}
	}
}