#include "stdafx.h"
#include "utils.h"
#include "ScriptableObject.h"
#include "text.h"
#include "bag.h"
#include "trainer.h"
#include "PokemonBattle.h"
#include "partyMenu.h"
#include "PokemonSummaryScreen.h"
#include <vector>
#include <map>

#include "networkhandler.h"

//Custom scripting engine, credit to Po0ka/Salthar for some variable formating!

extern SDL_Renderer *gRenderer;
extern TTF_Font *gFont;
extern OverworldController *m_World;
extern PokemonBattle *m_Battle;
extern std::vector<ScriptableObject*> MapObjects;
extern BagScene *m_Bag;
extern PokemonPartyScene *m_Party;
extern PokemonSummaryScene *m_summary;
extern CMultiplayerHandler *MPhandler;
extern int battleScene;

ObjectFlags obj;

void ScriptableObject::SetUp( int index, int x, int y )
{
	m_bRenderable = false;

	m_iIndex = index;

	AnimStepX = AnimStepY = 0; //This is only non-zero when im moving!

	m_bIsMoving = false;

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
	std::string arguements;
	bool ready = false;
	bool arguementGet,arguementGot;
	arguementGet = arguementGot = false;
	while( buffer[i] )
	{
		if( !ready && buffer[i] == '/' )
			ready = true;
		else if( ready && arguementGot && buffer[i] == '/' )
		{
			ready = false;
			break;
		}
		else if( ready && !arguementGot && buffer[i] == '/' )
		{
			arguementGet = true;
		}
		else if( ready && !arguementGet )
		{
			command+=buffer[i];
		}
		else if( arguementGet )
		{
			arguements += buffer[i];
		}
		i++;
	}

	if( command == "NPC" )
	{
		Type = 1;

		//tokenise arguement:
		char tok[] = { ',' };
		char *tokenised;
		tokenised = strtok( &arguements[0], tok );

		m_xSize = atoi( tokenised );
		tokenised = strtok( NULL, tok );
		m_ySize = atoi( tokenised );
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
extern bool pressingEnter;

void ScriptableObject::Render( int xofs, int yofs )
{
	if( m_bRenderable )
	{
		//18/22

		SDL_RenderCopyEx( gRenderer, texture, &GetRect( m_xSize*iDirection, 0, m_xSize, m_ySize ), &GetRect( (m_iX*40 - xofs) + AnimStepX, ((m_iY*40 - 10 ) - yofs) + AnimStepY, 40, 50 ), 0, NULL, flip ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE );
	}

	if( EditorEnabled )
	{
		//Render a square with my ID on it:
		SDL_RenderDrawRect( gRenderer, &GetRect( m_iX*40 - xofs, (m_iY*40 ) - yofs, 40, 40 ) );

		CText *id = new CText( std::to_string( (_ULonglong)m_iIndex ), gRenderer, gFont, 0 );
		id->Render( &GetRect( (m_iX*40 - xofs), (m_iY*40 - yofs), 40, 40 ) );
		delete id;
	}

	if( m_bIsMoving )
	{
		HandleMotion( m_GoalX, m_GoalY, false );
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

void PseudoRunEngine();

void ScriptableObject::Interact()
{
	if( obj.Objflags[ m_iIndex ] == true )
		return;

	//This is somehow nessisary...
	if( Type == 1 || Type == 0 )
	{
		pressingEnter = false; 
	}

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
			else if( !strcmp(token, "eventMoveTo") )
			{
				token = strtok( NULL, seps );
				int index = 0;
				index = atoi( token );

				token = strtok( NULL, seps );
				int DestX = atoi( token );
				token = strtok( NULL, seps );
				int DestY = atoi( token );
				
				for( int i = 0; i < MapObjects.size(); i++ )
				{
					if( MapObjects.at(i)->GetIndex() == index )
					{
						MapObjects.at(i)->MoveSelfTo( DestX, DestY, false, false );
						break;
					}
				}
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
			//Pokemon related commands:
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
			//Misc pokemon trainer stuff:
			else if( !strcmp(token, "trainerBattle" ) )
			{
				//Load trainer from file:
				token = strtok( NULL, seps );

				std::string strn = token;

				if( strn[ strn.size() - 4 ] != '.' )
				{
					strn = "";
					while( token != NULL )
					{
						strn += token;
						strn += " ";
						token = strtok( NULL, seps );
					}
				}

				Trainer *trainer = new Trainer();
				trainer->LoadFromFile(strn);
				
				//Open battle scene with the trainer:
				m_Battle->Initialise( trainer->m_pkmParty[0], m_World->thePlayer->m_pkmParty[0], false, trainer );

				if( FileExists( trainer->m_strMusicPath.c_str() ) )
					m_World->PlayMusic( trainer->m_strMusicPath.c_str() );

				battleScene = SCENE_BATTLE;

				FadeToBlack();
				//Dirty, dirty hack:
				PseudoRunEngine();
			}

			//Create a wild battle
			else if( !strcmp(token, "WildPokemon" ) || !strcmp(token, "WildBattle" ) )
			{
				//Todo: Get result of wild battle.

				//Generate a wild pokemon:
				Pokemon *WildPoke;
				WildPoke = new Pokemon();

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

				std::string m_strMusicPath;
				token = strtok( NULL, seps );
				m_strMusicPath = "DATA/Sounds/Music/";
				m_strMusicPath += token;

				WildPoke->side = 1;
				WildPoke->Init( Species, iv, ev, Level );
				

				//Open battle scene:
				m_Battle->Initialise( WildPoke, m_World->thePlayer->m_pkmParty[0], true, NULL );

				if( FileExists( m_strMusicPath.c_str() ) )
					m_World->PlayMusic( m_strMusicPath.c_str() );

				battleScene = SCENE_BATTLE;

				FadeToBlack();

				m_Battle->WildBattleStartAnim();

				//Dirty, dirty hack:
				PseudoRunEngine();
			}

			//Start a multiplayer battle:
			else if( !strcmp(token, "multiplayerBattle" ) )
			{
				//Ok, here we go:
				std::string options[2];
				options[0] = "Host";
				options[1] = "Join";

				int output = OWMultichoice( "MP OPTIONS", &options[0], 2, gRenderer, m_World, gFont );
				
				bool handshake = false;
				if( output == 1 )
				{
					MPhandler->StartServer();
					while( !MPhandler->IsClientConnected() )
					{
						SDL_PumpEvents();
						Sleep( 10 );
					}
					handshake = true;
				}
				else if( output == 2 )
				{
					//Get desired IP:
					//To be totaly honest, I dont like this entire block. I need to relocate it. This is tempory:
					//________________________________________________________________________________
					FILE *ipFile;   
					ipFile = fopen( "DATA/MP/IPs.txt", "rb");
					
					fseek (ipFile , 0 , SEEK_END);
					long ipSize = ftell(ipFile);
					rewind(ipFile);

					char * ipBuff;

					ipBuff = (char*)malloc(sizeof(char)*ipSize);
					if (ipBuff == NULL)
					{
						break;
					}

					int ipBuffEOF = fread( ipBuff, 1, lSize, ipFile);

					char newLine[] = "\n";
					char *iptoken = strtok( ipBuff, newLine );
					//_________________________________________________________________________________

					while( !MPhandler->Connect( iptoken ) )
					{
						SDL_PumpEvents();
						Sleep( 10 );
					}
					handshake = true;
				}
				if( handshake )
				{
					Trainer *trainer = new Trainer();
					//Generate a trainer based on the others party.
					if( output == 1 ) //Server goes first.
					{
						for( int i = 0; i < MAX_PARTY; i++ )
						{
							std::string serialisedPokemon;
							if( m_World->thePlayer->m_pkmParty[i] != NULL )
							{
								MPhandler->Transmit( m_World->thePlayer->m_pkmParty[i]->SerialisePokemon() );
							}
							else
							{
								MPhandler->Transmit( "ERR" );
							}

							while( !MPhandler->Recieve( serialisedPokemon ) )
							{
								SDL_PumpEvents();
								Sleep( 10 );
							}
							if( serialisedPokemon != "ERR" )
							{
								Pokemon *poke = new Pokemon();
								poke->side = 1;
								poke->DeSerialisePokemon( serialisedPokemon );
								//Failsafe:
								poke->LoadSprite();
								trainer->AddToParty( poke );
							}
						}
					}
					else if( output == 2 ) //Server goes first.
					{
						for( int i = 0; i < MAX_PARTY; i++ )
						{
							std::string serialisedPokemon;

							while( !MPhandler->Recieve( serialisedPokemon ) )
							{
								SDL_PumpEvents();
								Sleep( 10 );
							}
							if( serialisedPokemon != "ERR" )
							{
								Pokemon *poke = new Pokemon();
								poke->side = 1;
								poke->DeSerialisePokemon( serialisedPokemon );
								//Failsafe:
								poke->LoadSprite();
								trainer->AddToParty( poke );
							}

							if( m_World->thePlayer->m_pkmParty[i] != NULL )
							{
								MPhandler->Transmit( m_World->thePlayer->m_pkmParty[i]->SerialisePokemon() );
							}
							else
							{
								MPhandler->Transmit( "ERR" );
							}
						}
					}

					//Open battle scene with the trainer:
					m_Battle->Initialise( trainer->m_pkmParty[0], m_World->thePlayer->m_pkmParty[0], false, trainer );
					m_Battle->SetMultiplayer( true );

					battleScene = SCENE_BATTLE;

					FadeToBlack();
					//Dirty, dirty hack:
					PseudoRunEngine();
				}
			}
		}
		i++;
	}

	fclose( file );
}

bool debounce;
extern bool pressingEsc;
void PseudoRunEngine()
{
	pressingEnter = false;
	debounce = true;

	//Run battle scene while running script. Hacky, but needed.
	while( battleScene != SCENE_OVERWORLD )
	{
		if( battleScene == SCENE_BATTLE )
		{
			if( !m_Battle->Tick() )
			{
				//Kill engine if all else fails:
				//Deactivate SDL
				SDL_Quit();
				IMG_Quit();
				exit(0);
				break;
			}
		}
		//Party
		else if( battleScene == SCENE_PARTY )
		{
			if( !m_Party->Tick() )
			{
				//Kill engine if all else fails:
				//Deactivate SDL
				SDL_Quit();
				IMG_Quit();
				exit(0);
				break;
			}
		}
		//Summary
		else if( battleScene == SCENE_SUMMARY )
		{
			if( !m_summary->Tick() )
			{
				//Kill engine if all else fails:
				//Deactivate SDL
				SDL_Quit();
				IMG_Quit();
				exit(0);
				break;
			}
		}
		//Bag
		else if( battleScene == SCENE_BAG )
		{
			if( !m_Bag->Tick() )
			{
				//Kill engine if all else fails:
				//Deactivate SDL
				SDL_Quit();
				IMG_Quit();
				exit(0);
				break;
			}
		}

		//Hacky, but it does help...
		const Uint8 *keystate = SDL_GetKeyboardState( NULL );
		SDL_PumpEvents();
		if ( ( keystate[SDL_GetScancodeFromKey(SDLK_RETURN)] || keystate[SDL_GetScancodeFromKey(SDLK_SPACE)] ) )
		{
			if( !debounce )
			{
				pressingEnter = true;
			}
			else
			{
				pressingEnter = false;
			}
			debounce = true;
		}
		else if ( keystate[SDL_GetScancodeFromKey(SDLK_ESCAPE)] )
		{
			if( !debounce )
			{
				pressingEsc = true;
			}
			else
			{
				pressingEsc = false;
			}
			debounce = true;
		}
		else
		{
			debounce = false;
		}
	}
}

void ScriptableObject::MoveSelfTo( int X, int Y, bool Running, bool YFirst )
{
	m_GoalX = X;
	m_GoalY = Y;

	m_bIsMoving = true;
}

void ScriptableObject::HandleMotion( int GoalX, int GoalY , bool YFirst)
{
	//These are used for the movement and sprites:
	bool negativeX = m_iX > GoalX;
	bool negativeY = m_iY > GoalY;
	if( !YFirst )
	{
		//Check if we are already at disired X axis:
		if( m_iX == GoalX )
		{
			//Handle Y transition:
			if( negativeY )
				iDirection = 1;
			else
				iDirection = 0;

			if( (m_iY*TILE_SIZE) + AnimStepY != GoalY*TILE_SIZE )
			{
				AnimStepY += negativeY ? -1 : 1;
			}
			if( AnimStepY >= TILE_SIZE || AnimStepY <= -TILE_SIZE )
			{
				m_iY += negativeY ? -1 : 1;
				AnimStepY = 0;
			}
		}
		else
		{
			//Handle X transition:
			if( negativeX )
			{
				iDirection = 2;
				flip = false;
			}
			else
			{
				iDirection = 2;
				flip = true;
			}

			if( (m_iX*TILE_SIZE) + AnimStepX != GoalX*TILE_SIZE )
			{
				AnimStepX += negativeX ? -1 : 1;
			}
			if( AnimStepX >= TILE_SIZE || AnimStepX <= -TILE_SIZE )
			{
				m_iX += negativeX ? -1 : 1;
				AnimStepX = 0;
			}
		}
	}
	else
	{
		if( negativeY )
			iDirection = 1;
		else
			iDirection = 0;
	}
}