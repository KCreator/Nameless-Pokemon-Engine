#include "stdafx.h"
#include "save.h"

extern OverworldController *m_World;

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

	//Write Pokemon data:

	//Write OT and player name

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

	m_World->SetMapPos( mapName.c_str(), X, Y );


}