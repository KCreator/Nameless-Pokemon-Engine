#include "stdafx.h"
#include "pokemon.h"
#include "move.h"
#include "particle.h"
#include "windows.h"
#include "PokemonBattle.h"
#include "text.h"

std::string Move::GetName()
{
	if( m_iID == 0 )
	{
		return "-";
	}
	return m_sMoveName;
}

void Move::LoadMove()
{
	std::string str = "DATA/Moves/";
	str += std::to_string( (_ULonglong)m_iID );
	str += ".txt";
	FILE *moveData = fopen( str.c_str() , "r" );

	if( moveData == NULL )
	{
		m_iID = 0;
		return;
	}

	long lSize;
	// obtain file size:
	fseek (moveData , 0 , SEEK_END);
	lSize = ftell(moveData);
	rewind(moveData);

	int buffpos = 0;
	int buffEOF;
	char * buffer;

	buffer = (char*) malloc (sizeof(char)*lSize);
    if (buffer == NULL)
	{
		m_iID = 0;
		return;
	}

	buffEOF = fread( buffer, 1, lSize, moveData);

	std::string nameStr, ppStr, bpStr, accStr, typeStr, PhySpecStatStr, effectStr;

	int ParseLine = 0;

	while( buffer[buffpos] != NULL )
	{
		if( buffpos >= buffEOF )
			break;

		if( buffer[buffpos] == '\n' )
		{
			ParseLine++;
		}
		else
		{
			switch( ParseLine )
			{
			case 0: nameStr += buffer[buffpos]; break;
			case 1: ppStr += buffer[buffpos]; break;
			case 2: bpStr += buffer[buffpos]; break;
			case 3: accStr += buffer[buffpos]; break;
			case 4: typeStr += buffer[buffpos]; break;
			case 5: PhySpecStatStr += buffer[buffpos]; break;
			case 6: effectStr += buffer[buffpos]; break;
			}
			if( ParseLine > 6 )
				m_sMoveScript += buffer[buffpos];
		}
		buffpos++;
	}

	m_sMoveName = nameStr;
	m_iMaxPP = atoi( ppStr.c_str() );
	m_iBP = atoi( bpStr.c_str() );
	m_iAcc = atoi( accStr.c_str() );
	m_iPhySpeStat = atoi( PhySpecStatStr.c_str() );
	moveType.type = atoi( typeStr.c_str() );
	m_iEffect = atoi( effectStr.c_str() );
}

int Move::GetBP()
{
	return m_iBP;
}

extern SDL_Renderer *gRenderer;
extern BattleEngineGraphics *BattleUIGFX;
extern TTF_Font *gFont;

void Move::DispatchParticle( Pokemon *User, Pokemon *Target, std::string m_sMoveAnimation, int EmitterType )
{
	//Time to do some haxory
	int Amount;
	float XOfs, YOfs, SpeedX, SpeedY, SpeadVarianceX, SpeadVarianceY, StartSize, EndSize, Life;

	std::string moveAnimString = m_sMoveAnimation;

	if( moveAnimString.c_str() == NULL )
		return;

	int readMode = 0;
	const char *buffer = moveAnimString.c_str();

	std::string FilePath = "DATA/GFX/Particles/";

	std::string Output = "";

	for( int pos = 0; pos <= moveAnimString.size(); pos++ )
	{
		if( buffer[pos] == ',' )
		{
			if( readMode == 0 )
			{
				FilePath += Output;
				FilePath += ".png";
                                    				readMode++;
				Output = "";
				continue;
			}
			if( EmitterType == 1 )
			{
				switch( readMode )
				{
					case 1: Amount = atoi( Output.c_str() ); readMode++; break;
					case 2: XOfs = atof( Output.c_str() ); readMode++; break;
					case 3: YOfs = atof( Output.c_str() ); readMode++; break;
					case 4: SpeedX = atof( Output.c_str() ); readMode++; break;
					case 5: SpeedY = atof( Output.c_str() ); readMode++; break;
					case 6: SpeadVarianceX = atof( Output.c_str() ); readMode++; break;
					case 7: SpeadVarianceY = atof( Output.c_str() ); readMode++; break;
					case 8: StartSize = atof( Output.c_str() ); readMode++; break;
					case 9: EndSize = atof( Output.c_str() ); readMode++; break;
					case 10: Life = atof( Output.c_str() ); readMode++; break;
				}
			}
			Output = "";
		}
		else
		{
			Output += buffer[ pos ];
		}
	}

	if( EmitterType == EMITTER_OTHER_BURST ) //Burst
	{
		CBaseEmitter *emitter = new CBaseEmitter( FilePath.c_str(), Target->m_iPositionX + 80, Target->m_iPositionY + 80, SpeedX, SpeedY, SpeadVarianceX, SpeadVarianceY, StartSize, EndSize, Life );
		emitter->Emit( Amount );
		for( int timer = 0; timer <= (Life*100); timer++ )
		{
			AwaitUserInput();

			SDL_RenderClear( gRenderer );

			BattleUIGFX->bg->Render();

			BattleUIGFX->RenderPokes();
		
			emitter->SimulateAndRender();


			BattleUIGFX->menu->Render();
			BattleUIGFX->hpDisp->Render();

			SDL_RenderPresent( gRenderer );
			Sleep( 10 );
		}
		delete emitter;

		EmitterType = 0;
	}
}

void Move::DoAttack( Pokemon *user, Pokemon* target, float damage )
{
	float InitialDamage = damage;

	std::string command;

	//Type effectiveness:

	float typeEffectiveness;

	typeEffectiveness = moveType.TestFor( target->types[0] );
	typeEffectiveness *= moveType.TestFor( target->types[1] );

	damage *= typeEffectiveness;
	floor( damage );

	int buffer = 0;
	while( m_sMoveScript[ buffer ] )
	{
		//Run script:
		//Semicolon or NewLine is a terminator:
		command = "";
		while( ( m_sMoveScript[ buffer ] != ';' || m_sMoveScript[ buffer ] != '\n' ) && m_sMoveScript[ buffer ] != NULL )
		{
			if( ( m_sMoveScript[ buffer ] == ';' || m_sMoveScript[ buffer ] == '\n' ) )
				break;

			command += m_sMoveScript[ buffer ];
			buffer++;
		}

		if( command != "" )
		{
			//Tokenise:
			char seps[] = " ";
			char *token;

			////Parse command:
			token = NULL;
			token = strtok( &command[0], seps );

			//FX commands:
			if( !strcmp(token, "Emit") )
			{
				//Get the first argument:
				token = strtok( NULL, seps );

				//Parse it:
				if( !strcmp(token, "BURST_OTHER") )
				{
					token = strtok( NULL, seps );
					std::string str = token;
					DispatchParticle( user, target, str, EMITTER_OTHER_BURST );
				}
				else if( !strcmp(token, "BURST_SELF") )
				{
					token = strtok( NULL, seps );
					std::string str = token;
					DispatchParticle( user, target, str, EMITTER_SELF_BURST );
				}
			}

			//Battle logic commands
			else if( !strcmp(token, "DoDamage") )
			{
				target->IncrementHealth( -damage, true, user, BattleUIGFX->hpDisp );

				if( typeEffectiveness > 1 )
				{
					BattleText( "Its super effective!", gRenderer, BattleUIGFX, gFont );
				}
				if( typeEffectiveness < 1 )
				{
					BattleText( "Its not very effective!", gRenderer, BattleUIGFX, gFont );
				}
			}
		}

		buffer++;
	}

	//DispatchParticle( user, target, m_sMoveAnimation );
}