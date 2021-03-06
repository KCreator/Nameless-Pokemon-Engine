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
	//Cleanup.
	for(int i = 0; i < MemoryX; i++)
		delete [] Tiles[i];
	delete [] Tiles;

	for(int i = 0; i < MemoryX; i++)
		delete [] LayeredTiles[i];
	delete [] LayeredTiles;

	//Clear textures?
	SDL_DestroyTexture( m_Texture_1 );
	SDL_DestroyTexture( m_PriorityTexture );
	m_Texture_1 = NULL;
	m_PriorityTexture = NULL;
}

void TileMap::LoadMap( const char *Path )
{
	FILE *fp;   
    fp = fopen(Path, "rb");

	ResolutionX = 16;
	ResolutionY = 16;

	//Map Size:
	fscanf( fp, "%d", &MemoryX );
	fscanf( fp, "%d", &MemoryY );

	//Music File
	fscanf( fp, "%s", &MusicPath );

	//Tileset
	fscanf( fp, "%s", &TilesetPath );

	tileMap *TM = new tileMap;
	TM->Load( TilesetPath.c_str() );

	//Load border tiles:
	fscanf( fp, "%d", &BorderTiles[0] );
	fscanf( fp, "%d", &BorderTiles[1] );
	fscanf( fp, "%d", &BorderTiles[2] );
	fscanf( fp, "%d", &BorderTiles[3] );

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
			Tiles[x][y].CollisionData = 0;
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

	m_Texture_1 = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	SDL_FreeSurface( loadedSurface );

	loadedSurface = IMG_Load( fileName2 );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", fileName2, IMG_GetError() );
	}

	m_PriorityTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );

	//Get rid of old loaded surface
	SDL_FreeSurface( loadedSurface );
}

void TileMap::RenderMap()
{
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture( m_Texture_1, NULL, NULL, &texW, &texH);

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
			SDL_RenderCopy( gRenderer, m_Texture_1, &GetRect( ResolutionX * tilex, ResolutionY * tiley, ResolutionX, ResolutionY ), &GetRect( (40*x) - camX, (40*y) - camY, 40, 40 ) );

			if( debug ) //Render debug "blocks"
			{
				if( Tiles[x][y].CollisionData == TILE_SOLID_BLOCK )
				{
					SDL_RenderDrawRect( gRenderer, &GetRect( (40*x) - camX, (40*y) - camY, 40, 40 ) );
				}
				if( Tiles[x][y].CollisionData == TILE_SOLID_GRASS )
				{
					SDL_SetRenderDrawColor( gRenderer, 0, 200, 0, 100);
					SDL_RenderDrawRect( gRenderer, &GetRect( (40*x) - camX, (40*y) - camY, 40, 40 ) );
					SDL_RenderDrawRect( gRenderer, &GetRect( (40*x) - camX + 1, (40*y) - camY+1, 39, 39 ) );
					SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 255);
				}
				if( Tiles[x][y].CollisionData == TILE_SOLID_CLIFF_UP )
				{
					SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 255);
					SDL_RenderFillRect( gRenderer, &GetRect( (40*x) - camX, (40*y) - camY, 40, 20 ) );
					SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 255);
				}
				if( Tiles[x][y].CollisionData == TILE_SOLID_CLIFF_DOWN )
				{
					SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 255);
					SDL_RenderFillRect( gRenderer, &GetRect( (40*x) - camX, (40*y) - camY + 20, 40, 20 ) );
					SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 255);
				}
				if( Tiles[x][y].CollisionData == TILE_SOLID_CLIFF_LEFT )
				{
					SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 255);
					SDL_RenderFillRect( gRenderer, &GetRect( (40*x) - camX, (40*y) - camY, 20, 40 ) );
					SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 255);
				}
				if( Tiles[x][y].CollisionData == TILE_SOLID_CLIFF_RIGHT )
				{
					SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 255);
					SDL_RenderFillRect( gRenderer, &GetRect( (40*x) - camX + 20, (40*y) - camY, 20, 40 ) );
					SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 255);
				}
			}
		}
	}
}

void TileMap::RenderBG()
{
	//Texture info:
	int texW = 0;
	int texH = 0;
	SDL_QueryTexture( m_Texture_1 , NULL, NULL, &texW, &texH);

	//Background tiles:
	for( int x = -16; x < MemoryX + 16 ; x++ )
	{
		for( int y = -16; y < MemoryY + 16 ; y++ )
		{
			for( int i = 0; i < 4; i++ )
			{
				int tile = BorderTiles[i];
				int tilex = 0, tiley = 0;
				tilex = tile;
				while( tilex >= texW/16 )
				{
					tilex -= texW/16;
					tiley++;
				}

				switch( i )
				{
				case 0: SDL_RenderCopy( gRenderer, m_Texture_1, &GetRect( 16 * tilex, 16 * tiley, 16, 16 ), &GetRect( (80*x) - camX, (80*y) - camY, 40, 40 ) ); break;
				case 1: SDL_RenderCopy( gRenderer, m_Texture_1, &GetRect( 16 * tilex, 16 * tiley, 16, 16 ), &GetRect( (80*x) + 40 - camX, (80*y) - camY, 40, 40 ) ); break;
				case 2: SDL_RenderCopy( gRenderer, m_Texture_1, &GetRect( 16 * tilex, 16 * tiley, 16, 16 ), &GetRect( (80*x) - camX, (80*y) + 40 - camY, 40, 40 ) ); break;
				case 3: SDL_RenderCopy( gRenderer, m_Texture_1, &GetRect( 16 * tilex, 16 * tiley, 16, 16 ), &GetRect( (80*x) + 40 - camX, (80*y) + 40 - camY, 40, 40 ) ); break;
				}
			}
			//SDL_RenderCopy( gRenderer, m_Texture, &GetRect( 64, 928, 31, 32 ), &GetRect( (80*x) - camX, (80*y) - camY, 80, 80 ) );
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
			if( LayeredTiles[x][y].ID > 0 )
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
}

void TileMap::SaveMap()
{
	FILE *fp;   
    fp = fopen(TilePath.c_str(), "w");
	
	std::string level;
	level += std::to_string( (_ULonglong)MemoryX ) + " " + std::to_string((_ULonglong)MemoryY );
	level += '\n';

	level += "NULL";
	level += '\n';
	level += '\n';

	//Save border tiles:
	level += std::to_string( (_ULonglong)BorderTiles[0] ) + " ";
	level += std::to_string( (_ULonglong)BorderTiles[1] ) + " ";
	level += '\n';
	level += std::to_string( (_ULonglong)BorderTiles[2] ) + " ";
	level += std::to_string( (_ULonglong)BorderTiles[3] ) + " ";

	level += '\n';
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

	ResolutionX = 16;
	ResolutionY = 16;

	//Music: TEMP!
	fscanf( fp, "%s", &MusicPath );

	//Load border tiles:
	fscanf( fp, "%d", &BorderTiles[0] );
	fscanf( fp, "%d", &BorderTiles[1] );
	fscanf( fp, "%d", &BorderTiles[2] );
	fscanf( fp, "%d", &BorderTiles[3] );

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

void TileMap::PlayMusic( AudioController *audio )
{
	std::string filePath = "DATA/Sounds/Music/";
	filePath += MusicPath.c_str();
	filePath += ".ogg";

	if( FileExists( filePath.c_str() ) )
		audio->PlayMusic( filePath.c_str() );
}

//Animated tile:

void AnimatedTile::Render()
{
	//Todo: Add code related to rendering!

};

#include "IniParser.h"
void tileMap::Load( const char *path )
{
	std::string appenedPath = "DATA/Tilesets/";
	appenedPath += path;
	appenedPath += ".ini";

	INI *ini = new INI( appenedPath );
	
	std::map< std::string, std::string >::iterator it = ini->sections["tiledata"].begin();

	int size = ini->sections["tiledata"].size();
	OverrideIDs = new int[size];
	int number = 0;
	for( it; it != ini->sections["tiledata"].end(); ++it )
	{
		std::string val = it->second;
		//Strip {} from value:
		for( int i = 0; i < val.size(); i++ )
		{
			if( val.at(i) == '{' || val.at(i) == '}' )
			{
				val.erase( i, 1 );
			}
		}
		//Tokenise the value:
		char tok[] = { '|' };
		char *tokenised;
		tokenised = strtok( &val[0], tok );

		OverrideIDs[number] = atoi( tokenised );
		number++;

		tokenised = strtok( NULL, tok );
	
		//Load the animated tile info.
	}
}