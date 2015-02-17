#include "stdafx.h"
#include "save.h"
#include "bag.h"

extern OverworldController *m_World;
extern ObjectFlags obj;
extern Player *gPlayer;
extern BagScene *m_Bag;

#define MAX_PARTY 6 //Only 6 party slots for now!
#define MAX_POKEMON MAX_PARTY //Only 6 storage slots for now!

//Gah
#define INT_TO_STRING( x ) std::to_string( (_ULonglong)x )

void SaveHandler::Save()
{
	//Grab the save file:
	FILE *fp;   
    fp = fopen("DATA/Save/save.txt", "w");

	//Write map, mapx, mapy

	std::string saveFile = "";

	saveFile += m_World->mapPrefix;
	saveFile += "\n  \n";
	saveFile += std::to_string( (_ULonglong)m_World->mapX ) + " ";
	saveFile += std::to_string( (_ULonglong)m_World->mapY ) + " ";
	saveFile += "\n";

	//Write player X, Y:

	saveFile += std::to_string( (_ULonglong)m_World->GetPlayerX() ) + " ";
	saveFile += std::to_string( (_ULonglong)m_World->GetPlayerY() ) + " ";
	saveFile += "\n";

	//Write OW flags:
	for( int i = 0; i < MAX_OBJECT_TYPES; i++ )
	{
		if( obj.Objflags[i] )
			saveFile += "1";
		else
			saveFile += "0";

		saveFile += " ";
	}
	saveFile += "\n";

	//Write Pokemon data:
	for( int i = 0; i < MAX_POKEMON; i++ )
	{
		if( i < MAX_PARTY )
		{
			if( gPlayer->m_pkmParty[i] != NULL )
			{
				//Species, Level and EXP:
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->m_iSpecies) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetEXP()) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->m_iLevel) + " ";

				//EVs and IVs

				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetEvs().hp) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetEvs().atk) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetEvs().def) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetEvs().spatk) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetEvs().spdef) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetEvs().speed) + " ";

				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetIvs().hp) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetIvs().atk) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetIvs().def) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetIvs().spatk) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetIvs().spdef) + " ";
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetIvs().speed) + " ";

				//HP and PP
				saveFile += INT_TO_STRING(gPlayer->m_pkmParty[i]->GetHealth()) + " ";
				saveFile += "\n";
			}
			else
			{
				saveFile += "0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0\n";
			}
		}
	}

	//Write OT and player name

	//And finaly, write items!
	for( int i = 1; i < MAX_ITEMS; i++ )
	{
		saveFile += INT_TO_STRING( m_Bag->GetItemCount( i ) ) + " ";
	}

	fwrite( saveFile.c_str(), sizeof( char ), saveFile.size(), fp );

	fclose( fp );
}

void SaveHandler::Load()
{
	//Read mapX, mapY, mapPath:
	FILE *fp;   
    fp = fopen("DATA/Save/save.txt", "rb");

	int mapX, mapY;
	std::string mapName;

	fscanf( fp, "%s", &mapName );

	fscanf( fp, "%d", &mapX );
	fscanf( fp, "%d", &mapY );

	m_World->mapX = mapX;
	m_World->mapY = mapY;
	m_World->mapPrefix = mapName;

	int X, Y;

	fscanf( fp, "%d", &X );
	fscanf( fp, "%d", &Y );

	m_World->SetMapPos( mapName.c_str(), X, Y, mapX, mapY );

	//Read objFlags:
	for( int i = 0; i < MAX_OBJECT_TYPES; i++ )
	{
		int State;
		fscanf( fp, "%d", &State );
		if( State == 1 )
			obj.Objflags[i] = true;
		else
			obj.Objflags[i] = false;
	}

	//Read pokemonData:
	for( int i = 0; i < MAX_POKEMON; i++ )
	{
		if( i < MAX_PARTY )
		{
			//Read data:
			//Define vars:
			int species, exp, level, evhp, evatk, evdef, evspatk, evspdef, evspeed, ivhp, ivatk, ivdef, ivspatk, ivspdef, ivspeed, curhp;
			//Species:
			fscanf( fp, "%d", &species );
			fscanf( fp, "%d", &exp );
			fscanf( fp, "%d", &level );

			fscanf( fp, "%d", &evhp );
			fscanf( fp, "%d", &evatk );
			fscanf( fp, "%d", &evdef );
			fscanf( fp, "%d", &evspatk );
			fscanf( fp, "%d", &evspdef );
			fscanf( fp, "%d", &evspeed );

			fscanf( fp, "%d", &ivhp );
			fscanf( fp, "%d", &ivatk );
			fscanf( fp, "%d", &ivdef );
			fscanf( fp, "%d", &ivspatk );
			fscanf( fp, "%d", &ivspdef );
			fscanf( fp, "%d", &ivspeed );

			fscanf( fp, "%d", &curhp );

			//Generate a pokemon if its valid:
			if( species > 0 )
			{
				Pokemon *poke = new Pokemon();
				ivs iv;
				iv.hp = ivhp;
				iv.atk = ivatk;
				iv.def = ivdef;
				iv.spatk = ivspatk;
				iv.spdef = ivspdef;
				iv.speed = ivspeed;

				evs ev;
				ev.hp = evhp;
				ev.atk = evatk;
				ev.def = evdef;
				ev.spatk = evspatk;
				ev.spdef = evspdef;
				ev.speed = evspeed;
				poke->side = 0;
				poke->Init( species, iv, ev, level );
				poke->SetExp( exp );
				poke->IncrementHealth( curhp - poke->GetHealth() );

				m_World->thePlayer->AddToParty( poke );
			}
		}
	}

	//And finaly, Read items!
	for( int i = 1; i < MAX_ITEMS; i ++ )
	{
		int count;
		fscanf( fp, "%d", &count );

		if( count > 0 )
		{
			m_Bag->AddItem( GetFromID( i ), count );
		}
	}


	//Close the file.
	fclose( fp );
}