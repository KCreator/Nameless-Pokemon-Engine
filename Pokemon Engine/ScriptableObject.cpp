#include "stdafx.h"
#include "utils.h"
#include "ScriptableObject.h"
#include "text.h"
#include <vector>

extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;
extern OverworldController *m_World;
extern std::vector<ScriptableObject*> MapObjects;

ObjectFlags obj;

void ScriptableObject::SetUp( int index, int x, int y )
{
	m_bRenderable = false;

	m_iIndex = index;

	LoadData();
	PlaceAt( x, y );
}

void ScriptableObject::PlaceAt( int x, int y )
{
	m_iX = x;
	m_iY = y;
}

void ScriptableObject::LoadData()
{
	//Load a .png file with my index number:
	std::string str = "DATA/GFX/Overworlds/NPC/";

	str += std::to_string( (_ULonglong)m_iIndex );
	str += ".png";

	//Load image at specified path
	SDL_Surface* loadedSurface = IMG_Load( str.c_str() );
	if( loadedSurface == NULL )
	{
		printf( "Unable to load image %s! SDL_image Error: %s\n", str.c_str(), IMG_GetError() );
		m_bRenderable = false;
	}
	else
	{
		texture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		m_bRenderable = true;

		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	iDirection = 0;

	//Read the first line:
	//Load:
	str = "DATA/Overworld/Events/";

	str += std::to_string( (_ULonglong)m_iIndex );
	str += ".txt";

	FILE *file = NULL;
	file = fopen( str.c_str(), "r" );

	//This is tricky :/
	if( file == NULL )
		return;

	long lSize;
	// obtain file size:
	fseek (file , 0 , SEEK_END);
	lSize = ftell(file);
	rewind(file);

	char * buffer;

	buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL)
	{
		return;
	}

	int buffEOF = fread( buffer, 1, lSize, file);
	int i = 0;

	std::string command;
	bool ready = false;
	while( buffer[i] )
	{
		if( !ready && buffer[i] == '/' )
			ready = true;
		else if( ready && buffer[i] == '/' )
		{
			ready = false;
			break;
		}
		else if( ready )
		{
			command+=buffer[i];
		}
		i++;
	}

	if( command == "NPC" )
	{
		Type = 1;
	}
	if( command == "TRIGGER" )
	{
		Type = 2;
	}

	fclose( file );
}

void ScriptableObject::Render( int xofs, int yofs )
{
	if( m_bRenderable )
	{
		//18/22

		SDL_RenderCopyEx( gRenderer, texture, &GetRect( 18*iDirection, 0, 18, 22 ), &GetRect( m_iX*40 - xofs, (m_iY*40 - 10 ) - yofs, 40, 50 ), 0, NULL, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE );
	}
}

void ScriptableObject::Interact()
{
	if( obj.Objflags[ m_iIndex ] == true )
		return;

	//Some stuff:
	bool isHeader = false;
	//Load and parse my script:
	std::string str = "DATA/Overworld/Events/";

	str += std::to_string( (_ULonglong)m_iIndex );
	str += ".txt";

	FILE *file = NULL;
	file = fopen( str.c_str(), "r" );

	if( file == NULL )
		return;

	long lSize;
	// obtain file size:
	fseek (file , 0 , SEEK_END);
	lSize = ftell(file);
	rewind(file);

	char * buffer;

	buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL)
	{
		return;
	}

	int buffEOF = fread( buffer, 1, lSize, file);
	int i = 0;

	std::string command;

	while( buffer[i] )
	{
		//Run script:
		//Semicolon is a terminator

		command = "";

		while( (buffer[i] != ';' || buffer[i] != '\n' ) && buffer[i] != NULL )
		{
			if( !isHeader && buffer[i] == '/' )
				isHeader = true;
			else if( isHeader && buffer[i] == '/' )
			{
				isHeader = false;
				break;
			}
			else if( isHeader )
			{
				i++;
				continue;
			}

			if( (buffer[i] == ';') || (buffer[i] == '\n') )
				break;
			command += buffer[i];
			i++;
		}

		if( command != "" )
		{
			//Parse command:
			char seps[] = " ";
			char *token;

			token = strtok( &command[0], seps );

			int argNum = 0;

			//First args stuff
			if( !strcmp(token, "facePlayer") )
			{
				switch( m_World->GetPlayerFacing() )
				{
					case 1: iDirection = 1;	flip = false; break;
					case 2: iDirection = 0;	flip = false; break;
					case 3: iDirection = 2; flip = true; break;
					case 4: iDirection = 2;	flip = false; break;
				}
			}
			if( !strcmp(token, "msgBox") )
			{
				OWText( command.replace( 0, 7, "" ), gRenderer, m_World, gFont );
			}

			if( !strcmp(token, "toggleFlag") )
			{
				token = strtok( NULL, seps );
				int flag = atoi( token );
				obj.Objflags[ flag ] = !obj.Objflags[ flag ];

				obj.Objflags[ flag ];
			}

			argNum = 0;
			if( !strcmp(token, "playerMoveTo") )
			{
				int X, Y;
				X = Y = 0;
				while( token != NULL )
				{
					token = strtok( NULL, seps );
					argNum++;

					switch( argNum )
					{
					case 1: X = atoi( token );
					case 2: Y = atoi( token );
					}

					if( argNum == 2 )
						break;
				}

				//Move the player to the X and Y co-ordinates:
				m_World->MovePlayer( X, Y );
			}
			else if( !strcmp(token, "turnEvent") )
			{
				int index = 0;

				token = strtok( NULL, seps );
				index = atoi( token );
				token = strtok( NULL, seps );

				for( int i = 0; i < MapObjects.size(); i++ )
				{
					if( MapObjects.at(i)->GetIndex() == index )
					{
						MapObjects.at(i)->iDirection = atoi( token );
						break;
					}
				}
			}
			else if( !strcmp(token, "silentHealPokemon") )
			{
				for( int i = 0; i < 5; i ++ )
				{
					if( m_World->thePlayer->m_pkmParty[i] != NULL )
					{
						m_World->thePlayer->m_pkmParty[i]->Heal();
					}
				}
			}
			else if( !strcmp(token, "setMapPos") )
			{
				//Get map:
				std::string mapName = "DATA/Maps/";
				token = strtok( NULL, seps );
				mapName += token;

				int X; int Y;
				token = strtok( NULL, seps );
				X = atoi( token );
				token = strtok( NULL, seps );
				Y = atoi( token );

				//Set map:
				m_World->SetMapPos( mapName, X, Y );
			}
		}
		i++;
	}

	fclose( file );
}