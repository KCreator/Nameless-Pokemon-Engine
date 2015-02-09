#include "stdafx.h"
#include "item.h"
#include "overworld.h"
#include "PokemonBattle.h"

//Generic Includes:
extern SDL_Renderer *gRenderer;
extern BattleEngineGraphics *BattleUIGFX;
extern TTF_Font *gFont;
extern PokemonBattle *m_Battle;
extern int battleScene;

//Base Item:
CBaseItem::CBaseItem()
{
	//Do stuff here:
	count = 0;
	m_itemSprite = NULL;
}

CBaseItem::~CBaseItem()
{
	//Clear memory
	if( m_itemSprite != NULL )
	{
		SDL_DestroyTexture( m_itemSprite );
	}
}

bool CBaseItem::CanUse( bool IsBattle )
{
	return false;
}

void CBaseItem::Use()
{
	//Todo: Generic message
}

//Base Pokeball:
void CBasePokeBall::Use()
{
	//Remove one item:
	Add( -1 );

	//Hook into m_Battle and capture a pokemon if formula is successful:
	//-1 = master ball or other 100% capture balls (Park ball, dream ball, ect)
	if( CatchMod == -1 )
	{
		//Todo: Animation
		m_Battle->Capture();
	}
	
	//Hopefully this doesnt kill the RAM:
	Pokemon *Poke = m_Battle->GetPoke( 2 );
	int M, H, TileType, C, Status;

	//Todo: Add these.
	TileType = Status = 1;

	//M = Max HP, H = Hp:
	M = Poke->GetStat( "hp" );
	H = Poke->GetHealth();

	//C = pokemon catch rate:
	C = Poke->m_iCatchRate;

	float Capture;
	Capture = ((((3*M - 2*H) * TileType * C * CatchMod )/3*M) * Status) * (1/100);
}

//Tempory:
extern OverworldController *m_World;
void RareCandy::Use()
{
	//Raise the level of a pokemon by one:
	Add( -1 );
	m_World->thePlayer->m_pkmParty[0]->ForceLevelUp();
	m_World->thePlayer->m_pkmParty[0]->CheckEvolution();
}
