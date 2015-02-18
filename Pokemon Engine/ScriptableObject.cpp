#include "stdafx.h"
#include "utils.h"
#include "ScriptableObject.h"
#include "text.h"
#include "bag.h"
#include <vector>
#include <map>

//Custom scripting engine, credit to Po0ka/Salthar for some variable formating!

extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;
extern OverworldController *m_World;
extern std::vector<ScriptableObject*> MapObjects;
extern BagScene *m_Bag;

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
	if( command == "MAPEVENT" )
	{
		Type = 3;
	}

	fclose( file );
}

extern bool EditorEnabled;

void ScriptableObject::Render( int xofs, int yofs )
{
	if( m_bRenderable )
	{
		//18/22

		SDL_RenderCopyEx( gRenderer, texture, &GetRect( 18*iDirection, 0, 18, 22 ), &GetRect( m_iX*40 - xofs, (m_iY*40 - 10 ) - yofs, 40, 50 ), 0, NULL, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE );
	}

	if( EditorEnabled )
	{
		//Render a square with my ID on it:
		SDL_RenderDrawRect( gRenderer, &GetRect( m_iX*40 - xofs, (m_iY*40 ) - yofs, 40, 40 ) );

		CText *id = new CText( std::to_string( (_ULonglong)m_iIndex ), gRenderer, gFont, 0 );
		id->Render( &GetRect( m_iX*40 - xofs, (m_iY*40 ) - yofs, 40, 40 ) );
		delete id;
	}
}

void ScriptableObject::AnimatedRender( int xofs, int yofs )
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
	bool controlChar = false;

	//Conditionals and loops:
	bool isFalse = false;
	bool isLooping = false;
	bool alreadyDefined = false;
	int numLoops = 0;

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
			//Control char:
			if( !controlChar && buffer[i] == '\\' )
				controlChar = true;
			else if( buffer[i] == '\\' || buffer[i] == '/' )
			{
				command += buffer[i];
				i++;
			}

			//"Header" char:
			if( !isHeader && !controlChar && buffer[i] == '/' )
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

			//Newline/new command controller
			if( (buffer[i] == ';') || (buffer[i] == '\n') )
				break;
			command += buffer[i];
			i++;
		}

		if( command != "" )
		{
			alreadyDefined = false; //quick hack to prevent redefinition!

			if( command != "end" && isFalse )
				continue;
			else
				isFalse = false;

			//Parse command ( pre variables ):
			char seps[] = " ";
			char *token;

			std::string commandBackup = command;

			token = strtok( &commandBackup[0], seps );

			if( !strcmp(token, "math") )
			{
				std::string str = "";
				token = strtok( NULL, seps );
				str = token;

				int i = Variables[str];
				token = strtok( NULL, seps );
				char op = token[0];
				token = strtok( NULL, seps );

				str = token;

				int value;
				if( Variables.find( str ) == Variables.end() )
					value = atoi( token );
				else
					value = Variables[str];
				
				if( op == '+' )
				{
					i += value;
				}
				if( op == '-' )
				{
					i -= value;
				}
				if( op == '*' )
				{
					i *= value;
				}
				if( op == '/' )
				{
					i /= value;
				}

				Variables[str] = i;
				continue;
			}
			else if( !strcmp(token, "set") )
			{
			}
			else if( !strcmp(token, "multiChoiceTextbox") )
			{
				std::string str = "";
				std::string varName = "";
				char seps2[] = "\"";
				token = strtok( NULL, seps2 );
				if( token != NULL )
				{
					str += token;
				}

				token = strtok( NULL, seps );
				varName = token;

				int numArgs = 0;
				std::vector< std::string > choices;
				token = strtok( NULL, seps );
				while( token != NULL )
				{
					choices.push_back( token );
					numArgs ++;

					token = strtok( NULL, seps );
				}

				int output = OWMultichoice( str, &choices[0], choices.size(), gRenderer, m_World, gFont );
				Variables[ varName ] = output;
				
				continue;
			}
			//Make sure to not redefine stuff all the time!
			else if( !strcmp(token, "define") )
			{
				token = strtok( NULL, seps );
				if( !strcmp(token, "int") )
				{
					token = strtok( NULL, seps );
					std::string str = token;
					if( Variables.find( str ) != Variables.end() )
					{
						token = strtok( NULL, seps );
						Variables[ str ] = atoi( token );
						alreadyDefined = true;
					}
				}
			}

			//Replace all varable strings:

			std::map<std::string, int>::iterator localVariables_int_it;

			for (localVariables_int_it = Variables.begin(); localVariables_int_it != Variables.end(); ++localVariables_int_it)
				strReplace(command, (*localVariables_int_it).first, std::to_string((_ULonglong)(*localVariables_int_it).second));

			////Parse command:
			token = NULL;
			token = strtok( &command[0], seps );

			int argNum = 0;

			//First args stuff

			//Misc commands:
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
			else if( !strcmp(token, "turnPlayer") )
			{
				int direction = 0;

				token = strtok( NULL, seps );
				direction = atoi( token );

				m_World->SetPlayerFacing( direction );
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
				std::string mapName = "";
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
			else if( !strcmp(token, "fadeOut") )
			{
				FadeToBlack();
			}
			else if( !strcmp(token, "fadeIn") )
			{
				m_World->FadeIn();
			}
			else if( !strcmp(token, "givePokemon") )
			{
				//Generate random IV's and blank EV's
				evs ev;
				ev.hp = 0;
				ev.atk = 0;
				ev.def = 0;
				ev.spatk = 0;
				ev.spdef = 0;
				ev.speed = 0;

				ivs iv;
				iv.hp = rand()%32;
				iv.atk = rand()%32;
				iv.def = rand()%32;
				iv.spatk = rand()%32;
				iv.spdef = rand()%32;
				iv.speed = rand()%32;

				int Species, Level;

				token = strtok( NULL, seps );
				Species = atoi( token );

				token = strtok( NULL, seps );
				Level = atoi( token );

				//Create the Pokemon:
				Pokemon *poke = new Pokemon();
				poke->side = 0;
				poke->Init( Species, iv, ev, Level );
				
				//"Give" it to the player!
				m_World->thePlayer->AddToParty( poke );
			}
			else if( !strcmp(token, "giveItem") )
			{
				//Give an item to the player!
				int item, amount;

				token = strtok( NULL, seps );
				item = atoi( token );

				token = strtok( NULL, seps );
				amount = atoi( token );

				m_Bag->AddItem( GetFromID( item ), amount );
			}

			//Conditionals and logic:
			else if( !strcmp(token, "define") )
			{
				if( alreadyDefined )
					continue;
				token = strtok( NULL, seps );
				if( !strcmp(token, "int") )
				{
					token = strtok( NULL, seps );
					std::string str = token;
					token = strtok( NULL, seps );
					Variables[ str ] = atoi( token );
				}
			}
			else if( !strcmp(token, "if") )
			{
				token = strtok( NULL, seps );
				int var1 = atoi( token );
				
				token = strtok( NULL, seps );
				std::string op = "";
				op += token;

				token = strtok( NULL, seps );
				int var2 = atoi( token );

				if( op == "==" && var1 == var2 )
					continue;
				if( op == "!=" && var1 != var2 )
					continue;
				if( op == ">=" && var1 >= var2 )
					continue;
				if( op == "<=" && var1 <= var2 )
					continue;
				if( op == "<" && var1 < var2 )
					continue;
				if( op == ">" && var1 > var2 )
					continue;

				isFalse = true;
			}
		}
		i++;
	}

	fclose( file );
}