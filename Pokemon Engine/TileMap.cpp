#include "stdafx.h"
#include "TileMap.h"
#include "utils.h"
#include "ScriptableObject.h"
#include <vector>

//A bit hacky...
extern std::vector<ScriptableObject*> MapObjects;

extern SDL_Renderer *gRenderer;

TileMap::~TileMap()
{
	delete Tiles;

	delete LayeredTiles;
}

void TileMap::LoadMap( const char *Path )
{
	FILE *fp;   
    fp = fopen(Path, "rb");

	fscanf( fp, "%d", &MemoryX );
	fscanf( fp, "%d", &MemoryY );

	Tiles = new Tile*[MemoryX];
	for(int i = 0; i < MemoryX; i++)
		Tiles[i] = new Tile[MemoryY];

	LayeredTiles = new Tile*[MemoryX];
	for(int i = 0; i < MemoryX; i++)
		LayeredTiles[i] = new Tile[MemoryY];

	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			Tiles[x][y].ID = 0;
			LayeredTiles[x][y].ID = 0; //Temp:
			LayeredTiles[x][y].CollisionData = 0;
		}
	}

	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			fscanf( fp, "%d", &Tiles[x][y].ID );
		}
	}

	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			fscanf( fp, "%d", &Tiles[x][y].CollisionData );
		}
	}

	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			int mapObj;
			fscanf( fp, "%d", &mapObj );

			if( mapObj > 0 )
			{
				ScriptableObject *obj = new ScriptableObject();
				obj->SetUp( mapObj, x, y );
				MapObjects.push_back( obj );
			}
		}
	}

	//Priority tiles:
	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			fscanf( fp, "%d", &LayeredTiles[x][y].ID );
		}
	}

	fclose(fp);

	TilePath = Path;

	//Load encounters:
	std::string str = Path;
	strReplace( str, "Maps/", "Maps/encounterData/" );
	if( FileExists( str.c_str() ) )
	{
		fp = fopen( str.c_str(), "rb" );
		int encounterNumbers = 0;
		fscanf( fp, "%d", &encounterNumbers );
		encounters.size = encounterNumbers;
		encounters.encounters = new int[encounterNumbers];
		encounters.minLevel = new int[encounterNumbers];
		encounters.maxLevel = new int[encounterNumbers];

		for( int i = 0; i < encounterNumbers; i++ )
		{
			fscanf( fp, "%d", &encounters.encounters[i]);
			fscanf( fp, "%d", &encounters.minLevel[i]);
			fscanf( fp, "%d", &encounters.maxLevel[i]);
		}

		fclose(fp);
	}
}
void TileMap::LoadTileImage( const char *path, const char *fileName2 )
{
	//Textures:
	SDL_Surface* loadedSurface = IMG_Load( path );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", path, IMG_GetError() );
	}

	m_Texture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	loadedSurface = IMG_Load( fileName2 );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", fileName2, IMG_GetError() );
	}

	m_PriorityTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
}

void TileMap::RenderMap()
{
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(m_Texture, NULL, NULL, &texW, &texH);

	//Tiles
	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			int tile = Tiles[x][y].ID;
			int tilex = 0, tiley = 0;
			tilex = tile;
			while( tilex >= texW/16 )
			{
				tilex -= texW/16;
				tiley++;
			}
			SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 16 * tilex, 16 * tiley, 16, 16 ), &GetRect( (40*x) - camX, (40*y) - camY, 40, 40 ) );

			if( debug )
			{
				if( Tiles[x][y].CollisionData == 1 )
				{
					SDL_RenderDrawRect( gRenderer, &GetRect( (40*x) - camX, (40*y) - camY, 40, 40 ) );
				}

				if( Tiles[x][y].CollisionData == 2 )
				{
					SDL_SetRenderDrawColor( gRenderer, 0, 100, 0, 255);
					SDL_RenderDrawRect( gRenderer, &GetRect( (40*x) - camX, (40*y) - camY, 40, 40 ) );
					SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 255);
				}
			}
		}
	}
}

void TileMap::RenderBG()
{
	//Background tiles:
	for( int x = -16; x < MemoryX + 16 ; x++ )
	{
		for( int y = -16; y < MemoryY + 16 ; y++ )
		{
			SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 64, 928, 31, 32 ), &GetRect( (80*x) - camX, (80*y) - camY, 80, 80 ) );
		}
	}
}

void TileMap::RenderPriorityTiles()
{
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture(m_PriorityTexture, NULL, NULL, &texW, &texH);

	//Tiles
	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			int tile = LayeredTiles[x][y].ID;
			int tilex = 0, tiley = 0;
			tilex = tile;
			while( tilex >= texW/16 )
			{
				tilex -= texW/16;
				tiley++;
			}
			SDL_RenderCopy( gRenderer, m_PriorityTexture, &GetRect( 16 * tilex, 16 * tiley, 16, 16 ), &GetRect( (40*x) - camX, (40*y) - camY, 40, 40 ) );
		}
	}
}

void TileMap::SaveMap()
{
	FILE *fp;   
    fp = fopen(TilePath, "w");
	
	std::string level;
	level += std::to_string( (_ULonglong)MemoryX ) + " " + std::to_string((_ULonglong)MemoryY );
	level += '\n';

	//Write tiles:
	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			int tile = Tiles[x][y].ID;
			level += std::to_string( (_ULonglong)Tiles[x][y].ID ) + " ";
		}
		level += '\n';
	}

	level += '\n';

	//Write collision info:
	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			int tile = Tiles[x][y].CollisionData;
			level += std::to_string( (_ULonglong)Tiles[x][y].CollisionData ) + " ";
		}
		level += '\n';
	}

	level += '\n';

	//Write overworld events:
	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			std::string sav;
			sav = "0 ";
			for( int i = 0; i < MapObjects.size(); i++ )
			{
				if( MapObjects.at(i)->GetX() == x && MapObjects.at(i)->GetY() == y )
				{
					int tile = MapObjects.at(i)->GetIndex();
					sav = std::to_string( (_ULonglong)tile ) + " ";
				}
			}
			level += sav;
		}
		level += '\n';
	}

	level += '\n';

	//Write tiles:
	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			int tile = Tiles[x][y].ID;
			level += std::to_string( (_ULonglong)LayeredTiles[x][y].ID ) + " ";
		}
		level += '\n';
	}

	level += '\n';

	fwrite( level.c_str(), sizeof( char ), level.size(), fp );

	fclose(fp);
}

void TileMap::LoadMapAdjacent( const char *Path )
{
	FILE *fp;   
    fp = fopen(Path, "rb");

	fscanf( fp, "%d", &MemoryX );
	fscanf( fp, "%d", &MemoryY );

	Tiles = new Tile*[MemoryX];
	for(int i = 0; i < MemoryX; i++)
		Tiles[i] = new Tile[MemoryY];

	LayeredTiles = new Tile*[MemoryX];
	for(int i = 0; i < MemoryX; i++)
		LayeredTiles[i] = new Tile[MemoryY];

	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			Tiles[x][y].ID = 0;
			LayeredTiles[x][y].ID = 0; //Temp:
			LayeredTiles[x][y].CollisionData = 0;
		}
	}

	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			fscanf( fp, "%d", &Tiles[x][y].ID );
		}
	}

	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			fscanf( fp, "%d", &Tiles[x][y].CollisionData );
		}
	}

	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			int mapObj;
			fscanf( fp, "%d", &mapObj );
		}
	}

	//Priority tiles:
	for( int y = 0; y < MemoryY; y++ )
	{
		for( int x = 0; x < MemoryX; x++ )
		{
			fscanf( fp, "%d", &LayeredTiles[x][y].ID );
		}
	}

	fclose(fp);

	TilePath = Path;
}