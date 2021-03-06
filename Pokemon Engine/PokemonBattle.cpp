#include "stdafx.h"
#include "PokemonBattle.h"
#include "partyMenu.h"
#include "text.h"
#include "particle.h"
#include "bag.h"
#include "networkhandler.h"

extern BattleEngineGraphics *BattleUIGFX;
extern TTF_Font *gFont;
extern SDL_Renderer *gRenderer;
extern int battleScene;
extern bool pressingEnter;

extern PokemonPartyScene *m_Party;
extern BagScene *m_Bag;
extern Player *gPlayer;
extern PokemonBattle *m_Battle;
extern OverworldController *m_World;

extern CMultiplayerHandler *MPhandler;

void MoveCursorMenu0( const Uint8 *keystate, BattleMenu *menu, SDL_Event events );
int MoveCursorMenu1( const Uint8 *keystate, BattleMenu *menu, SDL_Event events );

void PokemonBattle::Initialise( Pokemon *battler1, Pokemon *battler2, bool isWild, Trainer *m_Trainer )
{
	m_pkmBattler1 = battler1;
	m_pkmBattler2 = battler2;

	m_pkmBattler1->side = 1;
	m_pkmBattler2->side = 0;

	BattleUIGFX->hpDisp->SetPokes( m_pkmBattler1, m_pkmBattler2 );

	BattleUIGFX->menu->SetCurrentPokemon( m_pkmBattler2 );

	BattleUIGFX->AddPoke(m_pkmBattler1, 0);
	BattleUIGFX->AddPoke(m_pkmBattler2, 1);

	m_bWild = isWild;
	m_trTrainer = m_Trainer;
};

bool PokemonBattle::Tick()
{
	if (SDL_PollEvent(&events))
	{
		if (events.type == SDL_QUIT)
		{
			return false;
		}
	}

	//Keyboard:
	const Uint8 *keystate = SDL_GetKeyboardState(NULL);
	//Uint8 *mousestate = SDL_GetMouseState(NULL, NULL);

	if( BattleUIGFX->menu->subMenu == 0 )
	{
		MoveCursorMenu0( keystate, BattleUIGFX->menu, events );
		//Slightly hacky:
		if( battleScene == SCENE_PARTY )
		{
			FadeToBlack();
			m_Party->FadeIn();
			return true;
		}
	}
	else if( BattleUIGFX->menu->subMenu == 1 )
	{
		int attack = MoveCursorMenu1( keystate, BattleUIGFX->menu, events );
		attack--;

		if( attack >= 0 && attack <= 3 )
		{
			if( m_pkmBattler2->pAttacks[attack]->GetID() == 0 )
				goto renderFuncs;

			if( m_bIsMultiplayer == false )
			{
				//Todo: Move priority
				if( m_pkmBattler2->GetStat( "speed" ) > m_pkmBattler1->GetStat( "speed" ) ) //If defender outspeeds attacker:
				{
					m_pkmBattler2->Attack( m_pkmBattler1, attack );
					m_pkmBattler1->Attack( m_pkmBattler2, rand()%4, true ); //Todo: add better AI!
				}
				else if( m_pkmBattler2->GetStat( "speed" ) < m_pkmBattler1->GetStat( "speed" ) ) //If attacker outspeeds defender:
				{
					m_pkmBattler1->Attack( m_pkmBattler2, rand()%4, true ); //Todo: add better AI!
					m_pkmBattler2->Attack( m_pkmBattler1, attack );
				}
				else if( rand()%2 == 0 )
				{
					m_pkmBattler2->Attack( m_pkmBattler1, attack );
					m_pkmBattler1->Attack( m_pkmBattler2, rand()%4, true ); //Todo: add better AI!
				}
				else
				{
					m_pkmBattler1->Attack( m_pkmBattler2, rand()%4, true ); //Todo: add better AI!
					m_pkmBattler2->Attack( m_pkmBattler1, attack );
				}
				//Check if pokemon1 is fainted, if it is, and we are running a trainer battle, check if the trainer can swap out...
				if( !m_pkmBattler1->GetActive() && m_bWild == false )
				{
					if( m_trTrainer->GetNumActivePkm() > 0 )
					{
						//Try swapping out:
						for( int i = 0; i < m_trTrainer->m_iNumPoke; i++ )
						{
							if( m_trTrainer->m_pkmParty[i]->GetActive() )
							{
								SwapOut( m_trTrainer->m_pkmParty[i], 0 );
								break;
							}
						}
					}
					else
					{
						//End the battle scene.
						//Clear trainer memory:
						if( !m_bWild )
							delete m_trTrainer;
						//Todo: Add victory text!
						BattleUIGFX->menu->cursorPos = 1;
						BattleUIGFX->menu->subMenu = 0;

						FadeToBlack(  );
						battleScene = SCENE_OVERWORLD;

						m_World->ReplayLastMusic(); //Since I will leave now, call the needed deactivation code.
						return true; //Leaving now.
					}
				}
			}
			else //I'm in a multiplayer battle!
			{
				//Transmit my attack:
				MPhandler->Transmit( std::to_string((_ULonglong)attack ) );
				std::string response;

				BattleUIGFX->menu->subMenu = -1;
				

				//Directly render text, a nessisary evil here:
				SDL_RenderClear( gRenderer );
				BattleUIGFX->bg->Render();
				m_pkmBattler1->Render( gRenderer );
				m_pkmBattler2->Render( gRenderer );
				BattleUIGFX->menu->Render();
				BattleUIGFX->hpDisp->UpdateHP( m_pkmBattler2->GetHealth(), m_pkmBattler1->GetHealth(), m_pkmBattler2->GetStat( "hp" ), m_pkmBattler1->GetStat( "hp" ) );
				BattleUIGFX->hpDisp->Render();
				
				CText *txt = new CText( "Waiting for other trainer...", gRenderer, gFont, 1, 255, 255, 255 );
				txt->Render( &GetRect( 50, 400, 20, 20 ) );
				delete txt;

				SDL_RenderPresent( gRenderer );

				while( !MPhandler->Recieve( response ) ) //Await response:
				{
					SDL_PumpEvents();
					Sleep( 10 );
				}

				int othertrainerselection = atoi(response.c_str());

				if( othertrainerselection < 4 )
				{
					//Todo: Move priority
					if( m_pkmBattler2->GetStat( "speed" ) > m_pkmBattler1->GetStat( "speed" ) ) //If defender outspeeds attacker:
					{
						m_pkmBattler2->Attack( m_pkmBattler1, attack );
						m_pkmBattler1->Attack( m_pkmBattler2, othertrainerselection );
					}
					else if( m_pkmBattler2->GetStat( "speed" ) < m_pkmBattler1->GetStat( "speed" ) ) //If attacker outspeeds defender:
					{
						m_pkmBattler1->Attack( m_pkmBattler2, othertrainerselection );
						m_pkmBattler2->Attack( m_pkmBattler1, attack );
					}
					else
					{
						std::string randomVal;
						if( MPhandler->m_bIsServer )
						{
							randomVal = ( rand()%2 == 0 ) ? "0" : "1";
							MPhandler->Transmit( randomVal );

							m_pkmBattler1->Attack( m_pkmBattler2, othertrainerselection );
							m_pkmBattler2->Attack( m_pkmBattler1, attack );
						}
						else
						{
							while( !MPhandler->Recieve( randomVal ) ) //Await response:
							{
								SDL_PumpEvents();
								Sleep( 10 );
							}
						}
						if( atoi( randomVal.c_str() ) == 0 )
						{
							m_pkmBattler1->Attack( m_pkmBattler2, othertrainerselection );
							m_pkmBattler2->Attack( m_pkmBattler1, attack );
						}
						else
						{
							m_pkmBattler2->Attack( m_pkmBattler1, attack );
							m_pkmBattler1->Attack( m_pkmBattler2, othertrainerselection );
						}
					}
				}
				else
				{
					SwapOut( m_trTrainer->m_pkmParty[ othertrainerselection - 3 ], 0 );

					//Swap party around, becuase the other player will have a different order now
					Pokemon *pokeBuffer;
					pokeBuffer = m_trTrainer->m_pkmParty[othertrainerselection - 3];

					m_trTrainer->m_pkmParty[othertrainerselection - 3] = m_trTrainer->m_pkmParty[0];
					m_trTrainer->m_pkmParty[0] = pokeBuffer;

					m_pkmBattler2->Attack( m_pkmBattler1, attack );
				}
			}

			//Check if player pokemon fainted, and then test if we should black out, or something...
			if( !m_pkmBattler2->GetActive() )
			{
				if( gPlayer->GetNumActivePkm() > 0 )
				{
					//Open the party screen:
					m_Party->IsBattle = true;
					m_Party->PreventExit = true;
					m_Party->m_iSelection = 0;
					battleScene = SCENE_PARTY;
					pressingEnter = false;
					FadeToBlack();
					return true;
				}
			}

			BattleUIGFX->menu->cursorPos = 1;
			BattleUIGFX->menu->subMenu = 0;
		}
	}

	renderFuncs:

	if( battleScene == SCENE_BATTLE ) //Only render when the scene is the battle scene. Believe it or not, this is needed...
	{
		SDL_RenderClear( gRenderer );

		BattleUIGFX->bg->Render();
		m_pkmBattler1->Render( gRenderer );
		m_pkmBattler2->Render( gRenderer );

		BattleUIGFX->menu->Render();

		BattleUIGFX->hpDisp->UpdateHP( m_pkmBattler2->GetHealth(), m_pkmBattler1->GetHealth(), m_pkmBattler2->GetStat( "hp" ), m_pkmBattler1->GetStat( "hp" ) );
		BattleUIGFX->hpDisp->Render();

		SDL_RenderPresent( gRenderer );
	}

	//Deactivate battle when I should..
	if( battleScene == SCENE_OVERWORLD )
	{
		m_World->ReplayLastMusic(); //Replay last music track!
	}

	return true;
}

void PokemonBattle::Capture()
{
	if( !m_bWild )
		return;

	BattleUIGFX->menu->cursorPos = 1;
	BattleUIGFX->menu->subMenu = 0;

	FadeToBlack(  );
	battleScene = SCENE_OVERWORLD;
	m_pkmBattler1->side = 0;
	m_pkmBattler1->LoadSprite();
	gPlayer->AddToParty( m_pkmBattler1 );
}

void MoveCursorMenu0( const Uint8 *keystate, BattleMenu *menu, SDL_Event events )
{
	if ( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)] )
	{
		if( menu->cursorPos == 3 )
		{
			menu->cursorPos = 1;
		}
		if( menu->cursorPos == 4 )
		{
			menu->cursorPos = 2;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)] )
	{
		if( menu->cursorPos == 1 )
		{
			menu->cursorPos = 3;
		}
		if( menu->cursorPos == 2 )
		{
			menu->cursorPos = 4;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_d)] || keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)] )
	{
		if( menu->cursorPos == 1 )
		{
			menu->cursorPos = 2;
		}
		if( menu->cursorPos == 3 )
		{
			menu->cursorPos = 4;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)])
	{
		if( menu->cursorPos == 2 )
		{
			menu->cursorPos = 1;
		}
		if( menu->cursorPos == 4 )
		{
			menu->cursorPos = 3;
		}
	}

	if( pressingEnter )
	{
		if( menu->cursorPos == 1 )
		{
			menu->subMenu = 1;
		}
		if( menu->cursorPos == 2 )
		{
			if( m_Battle->IsMP() == false )
			{
				m_Bag->IsBattle = true;
				m_Bag->PreviousScene = battleScene;
				battleScene = SCENE_BAG;
				FadeToBlack();
				//menu->subMenu = -1;
				//BattleText( "BAG is not implemented!", gRenderer, BattleUIGFX, gFont );
				//BattleText( "PLAYER_NAME used a Pokeball!", gRenderer, BattleUIGFX, gFont );

				//m_Battle->Capture();
			}
			else
			{
				BattleText( "Cannot use items in multiplayer!", gRenderer, BattleUIGFX, gFont );
			}
		}
		if( menu->cursorPos == 3 )
		{
			m_Party->m_iLastScene = battleScene;

			battleScene = SCENE_PARTY;
			m_Party->m_iSelection = 0;
			m_Party->IsBattle = true;
			pressingEnter = false;
		}
		if( menu->cursorPos == 4 )
		{
			if( m_Battle->IsWild() )
			{
				menu->subMenu = -1;
				BattleText( "Got away safely!", gRenderer, BattleUIGFX, gFont );
				menu->cursorPos = 1;
				menu->subMenu = 0;

				FadeToBlack(  );
				battleScene = SCENE_OVERWORLD;

				m_Battle->Clear();
			}
			else
			{
				menu->subMenu = -1;
				BattleText( "You can't run from a trainer battle!", gRenderer, BattleUIGFX, gFont );
				menu->subMenu = 0;
			}
		}

		pressingEnter = false;
	}
}

int MoveCursorMenu1( const Uint8 *keystate, BattleMenu *menu, SDL_Event events )
{
	if ( keystate[SDL_GetScancodeFromKey(SDLK_w)] || keystate[SDL_GetScancodeFromKey(SDLK_UP)])
	{
		if( menu->cursorPos == 3 )
		{
			menu->cursorPos = 1;
		}
		if( menu->cursorPos == 4 )
		{
			menu->cursorPos = 2;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_s)] || keystate[SDL_GetScancodeFromKey(SDLK_DOWN)])
	{
		if( menu->cursorPos == 1 )
		{
			menu->cursorPos = 3;
		}
		if( menu->cursorPos == 2 )
		{
			menu->cursorPos = 4;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_d)] || keystate[SDL_GetScancodeFromKey(SDLK_RIGHT)])
	{
		if( menu->cursorPos == 1 )
		{
			menu->cursorPos = 2;
		}
		if( menu->cursorPos == 3 )
		{
			menu->cursorPos = 4;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_a)] || keystate[SDL_GetScancodeFromKey(SDLK_LEFT)] )
	{
		if( menu->cursorPos == 2 )
		{
			menu->cursorPos = 1;
		}
		if( menu->cursorPos == 4 )
		{
			menu->cursorPos = 3;
		}
	}
	else if( keystate[SDL_GetScancodeFromKey(SDLK_ESCAPE)] )
	{
		menu->subMenu = 0;
	}
	if( pressingEnter )
	{
		pressingEnter = false;
		return menu->cursorPos;
	}

	return 0;
}

void EmitPokeballParticle( Pokemon *targ, SDL_Renderer *gRenderer )
{
	//Particle emitter:
	CBaseEmitter *emitter = new CBaseEmitter( "DATA/GFX/Particles/YellowGlow.png", targ->m_iPositionX + 80, targ->m_iPositionY + 80, 0, 0, 20, 20, 2, 0, 1 );
	emitter->Emit( 100 );

	for( int timer = 0; timer <= 100; timer++ )
	{
		SDL_RenderClear( gRenderer );

		BattleUIGFX->bg->Render();
		BattleUIGFX->RenderPokes();

		emitter->SimulateAndRender();

		BattleUIGFX->menu->Render();
		BattleUIGFX->hpDisp->Render();

		SDL_RenderPresent( gRenderer );
		SDL_Delay( 10 );
	}
}

void PokemonBattle::SwapOut( Pokemon *NewBattler, int side, bool isFaintedSwapout )
{
	//Multiplayer stuff:
	int othertrainerselection = 0;
	if( m_bIsMultiplayer && side == 1 )
	{
		for( int i = 0; i < MAX_PARTY; i++ )
		{
			if( m_World->thePlayer->m_pkmParty[i] == NewBattler )
			{
				MPhandler->Transmit( std::to_string((_ULonglong)(i+4) ) );
				break;
			}
		}

		std::string response;

		BattleUIGFX->menu->subMenu = -1;

		//Directly render text, a nessisary evil here:
		SDL_RenderClear( gRenderer );
		BattleUIGFX->bg->Render();
		m_pkmBattler1->Render( gRenderer );
		m_pkmBattler2->Render( gRenderer );
		BattleUIGFX->menu->Render();
		BattleUIGFX->hpDisp->UpdateHP( m_pkmBattler2->GetHealth(), m_pkmBattler1->GetHealth(), m_pkmBattler2->GetStat( "hp" ), m_pkmBattler1->GetStat( "hp" ) );
		BattleUIGFX->hpDisp->Render();
				
		CText *txt = new CText( "Waiting for other trainer...", gRenderer, gFont, 1, 255, 255, 255 );
		txt->Render( &GetRect( 50, 400, 20, 20 ) );
		delete txt;

		SDL_RenderPresent( gRenderer );

		while( !MPhandler->Recieve( response ) ) //Await response:
		{
			SDL_PumpEvents();
			Sleep( 10 );
		}

		othertrainerselection = atoi( response.c_str() );
	}


	//Todo: Add animation!
	std::string WithdrawText;

	switch( side )
	{
		//This should only be called by the trainer func, so its safe to assume stuff:
		case 0: 
			BattleUIGFX->menu->subMenu = -1;
			m_pkmBattler1->m_bShouldRender = false; //Hide pokemon now!
			WithdrawText += m_trTrainer->m_strName;
			WithdrawText += " withdrew ";
			WithdrawText += m_pkmBattler1->m_sPkmName;
			BattleText( WithdrawText, gRenderer, BattleUIGFX, gFont );

			WithdrawText = "";
			WithdrawText += m_trTrainer->m_strName;
			WithdrawText += " sent out ";
			WithdrawText += NewBattler->m_sPkmName;
			BattleText( WithdrawText, gRenderer, BattleUIGFX, gFont );

			m_pkmBattler1 = NewBattler;
			m_pkmBattler1->m_bShouldRender = true; //Show pokemon now!
			BattleUIGFX->AddPoke(m_pkmBattler1, 0);
			m_pkmBattler1->Render(gRenderer);
			EmitPokeballParticle( m_pkmBattler1, gRenderer ); //Emit a particle!
			break;
		case 1: 
			BattleUIGFX->menu->subMenu = -1;
			m_pkmBattler2->m_bShouldRender = false; //Hide pokemon now!
			WithdrawText = "PLAYER_NAME withdrew ";
			WithdrawText += m_pkmBattler2->m_sPkmName;
			BattleText( WithdrawText, gRenderer, BattleUIGFX, gFont );

			WithdrawText = "PLAYER_NAME sent out ";
			WithdrawText += NewBattler->m_sPkmName;
			BattleText( WithdrawText, gRenderer, BattleUIGFX, gFont );

			m_pkmBattler2 = NewBattler;
			m_pkmBattler2->m_bShouldRender = true; //Show pokemon now!
			BattleUIGFX->AddPoke(m_pkmBattler2, 1);
			m_pkmBattler2->Render(gRenderer);
			EmitPokeballParticle( m_pkmBattler2, gRenderer ); //Emit a particle!
			break;
	}

	//Refresh UI elements!
	BattleUIGFX->hpDisp->SetPokes( m_pkmBattler1, m_pkmBattler2 );

	BattleUIGFX->menu->SetCurrentPokemon( m_pkmBattler2 );

	BattleUIGFX->hpDisp->UpdateHP( m_pkmBattler2->GetHealth(), m_pkmBattler1->GetHealth(), m_pkmBattler2->GetStat( "hp" ), m_pkmBattler1->GetStat( "hp" ) );

	if( side == 1 && !isFaintedSwapout )
	{
		if( !m_bIsMultiplayer )
		{
			m_pkmBattler1->Attack( m_pkmBattler2, rand()%4, true );
		}
		else
		{
			if( othertrainerselection < 4 )
			{
				m_pkmBattler1->Attack( m_pkmBattler2, othertrainerselection );
			}
			else
			{
				SwapOut( m_trTrainer->m_pkmParty[ othertrainerselection-3 ], 0 );

				//Swap party around, becuase the other player will have a differnt order now
				Pokemon *pokeBuffer;
				pokeBuffer = m_trTrainer->m_pkmParty[othertrainerselection-3];

				m_trTrainer->m_pkmParty[othertrainerselection-3] = m_trTrainer->m_pkmParty[0];
				m_trTrainer->m_pkmParty[0] = pokeBuffer;
			}
		}
	}

	BattleUIGFX->menu->cursorPos = 1;
	BattleUIGFX->menu->subMenu = 0;

	pressingEnter = false;
}

void PokemonBattle::WildBattleStartAnim()
{
	//Fade in:
	BattleUIGFX->menu->subMenu = -1;

	int progress = 255;
	while( true )
	{
		SDL_Surface *surf = SDL_CreateRGBSurface( SDL_SWSURFACE, 600, 480, 1, 0,0,0, progress );
		SDL_Texture *texture = SDL_CreateTextureFromSurface( gRenderer, surf );

		//Get rid of old loaded surface
		SDL_FreeSurface( surf );

		SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
		SDL_SetTextureAlphaMod( texture, progress );
		SDL_SetTextureColorMod( texture, 0, 0, 0 );

		SDL_RenderClear(gRenderer);
		BattleUIGFX->bg->Render();
		BattleUIGFX->menu->Render();
		m_pkmBattler1->Render( gRenderer );

		SDL_RenderCopy( gRenderer, texture, NULL, NULL );

		SDL_RenderPresent( gRenderer );

		progress-=5;

		//Free up RAM!
		SDL_DestroyTexture( texture );

		if( progress <= 0 )
		{
			break;
		}

		SDL_Delay( 1 );
	}

	BattleUIGFX->bg->Render();
	m_pkmBattler1->Render( gRenderer );
	BattleUIGFX->menu->Render();

	m_pkmBattler2->m_bShouldRender = false;

	std::string pokeText = "";
	pokeText = "A wild " + m_pkmBattler1->GetName() + " Appeared!";
	BattleText( pokeText, gRenderer, BattleUIGFX, gFont, false );

	//Move trainter sprite:
	pokeText = "Go " + m_pkmBattler2->GetName() + "!";
	BattleText( pokeText, gRenderer, BattleUIGFX, gFont, false );

	//Spawn player pokemon:
	m_pkmBattler2->m_bShouldRender = true;
	m_pkmBattler2->Render(gRenderer); //Force the pokemon to render, to init co-ordinates!

	//Create a quick particle...
	CBaseEmitter *emitter = new CBaseEmitter( "DATA/GFX/Particles/YellowGlow.png", m_pkmBattler2->m_iPositionX + 80, m_pkmBattler2->m_iPositionY + 80, 0, 0, 20, 20, 2, 0, 1 );
	emitter->Emit( 100 );

	for( int timer = 0; timer <= 100; timer++ )
	{
		SDL_RenderClear( gRenderer );

		BattleUIGFX->bg->Render();
		BattleUIGFX->RenderPokes();

		emitter->SimulateAndRender();

		BattleUIGFX->menu->Render();

		SDL_RenderPresent( gRenderer );
		SDL_Delay( 10 );
	}

	delete emitter;

	//Done!
	BattleUIGFX->menu->subMenu = 0;

	pressingEnter = false;
}