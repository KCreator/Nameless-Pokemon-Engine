#include "stdafx.h"
#include "bag.h"
#include "text.h"
#include "Overworld.h"

//Todo: Add the remaining needed externs:
extern bool pressingEnter;
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
	}

	const Uint8 *keystate = SDL_GetKeyboardState(NULL);

	if( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
	{
		SDL_Delay( 100 );
		selection--;
		if( selection < 0 )
			selection = 0;
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
	{
		SDL_Delay( 100 );
		selection++;
		if( selection > items.size() )
		{
			selection = items.size();
		}
	}

	if( pressingEnter )
	{
		pressingEnter = false; //Force debounce!

		//Temp:
		//USE item:
		if( selection < items.size() )
		{
			if( items[selection]->CanUse( IsBattle ) )
			{
				items[selection]->Use();
				DeleteUsedItems();
			}
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
	SDL_RenderCopy( gRenderer, BagUI, &GetRect( 504 + (8 * isMovingItem ? 1 : 0), 172, 8, 10 ), &GetRect( 285, 55 + ( 30 * selection ) - 2, 20, 30 ) );

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
				txt->Render( &GetRect( 10, 330 + (30 * newLines ), 0, 0 ));
				delete txt;

				token = strtok( NULL, split );
				newLines++;
			}

			delete token;
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

		txt = new CText( "the field.", gRenderer, gFont, 1 );
		txt->Render( &GetRect( 10, 360, 0, 0 ));
		delete txt;
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