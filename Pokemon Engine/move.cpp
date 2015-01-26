#include "stdafx.h"
#include "pokemon.h"
#include "move.h"
#include "particle.h"
#include "windows.h"

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

	std::string nameStr, ppStr, bpStr, accStr, typeStr, PhySpecStatStr;

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
			case 6: m_sMoveAnimation += buffer[buffpos]; break;
			case 7: m_sMoveAnimation2 += buffer[buffpos]; break;
			case 8: m_sMoveAnimation3 += buffer[buffpos]; break;
			case 9: m_sMoveAnimation4 += buffer[buffpos]; break;
			}
		}
		buffpos++;
	}

	m_sMoveName = nameStr;
	m_iMaxPP = atoi( ppStr.c_str() );
	m_iBP = atoi( bpStr.c_str() );
	m_iAcc = atoi( accStr.c_str() );
	m_iPhySpeStat = atoi( PhySpecStatStr.c_str() );
	moveType.type = atoi( typeStr.c_str() );
}

int Move::GetBP()
{
	return m_iBP;
}

extern SDL_Renderer *gRenderer;
extern BattleEngineGraphics *BattleUIGFX;
extern TTF_Font *gFont;

void Move::PlayAnimation( Pokemon *User, Pokemon *Target )
{
	//Time to do some haxory
	int EmitterType = 0, Amount;
	float SpeedX, SpeedY, SpeadVarianceX, SpeadVarianceY, StartSize, EndSize;

	std::string moveAnimString = "";

	for( int i = 0; i < 3; i++ )
	{
		moveAnimString = "";

		switch( i )
		{
		case 0: moveAnimString += m_sMoveAnimation;
		case 1: moveAnimString += m_sMoveAnimation2;
		case 2: moveAnimString += m_sMoveAnimation3;
		case 3: moveAnimString += m_sMoveAnimation4;
		}
		if( moveAnimString.c_str() == NULL )
			continue;

		int readMode = 0;
		const char *buffer = moveAnimString.c_str();

		std::string FilePath = "DATA/GFX/Particles/";

		std::string Output = "";

		for( int pos = 0; pos <= moveAnimString.size(); pos++ )
		{
			if( buffer[pos] == ';' )
			{
				if( readMode == 0 )
				{
					EmitterType = atoi( Output.c_str() );
					readMode++;
					Output = "";
					continue;
				}
				if( readMode == 1 )
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
						case 2: Amount = atoi( Output.c_str() ); readMode++; break;
						case 3: SpeedX = atof( Output.c_str() ); readMode++; break;
						case 4: SpeedY = atof( Output.c_str() ); readMode++; break;
						case 5: SpeadVarianceX = atof( Output.c_str() ); readMode++; break;
						case 6: SpeadVarianceY = atof( Output.c_str() ); readMode++; break;
						case 7: StartSize = atof( Output.c_str() ); readMode++; break;
						case 8: EndSize = atof( Output.c_str() ); readMode++; break;
					}
				}
				Output = "";
			}
			else
			{
				Output += buffer[ pos ];
			}
		}

		if( EmitterType == 1 ) //Burst
		{
			CBaseEmitter *emitter = new CBaseEmitter( FilePath.c_str(), Target->m_iPositionX + 80, Target->m_iPositionY + 80, SpeedX, SpeedY, SpeadVarianceX, SpeadVarianceY, StartSize, EndSize );
			emitter->Emit( Amount );
			for( int timer = 0; timer <= 100; timer++ )
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
}