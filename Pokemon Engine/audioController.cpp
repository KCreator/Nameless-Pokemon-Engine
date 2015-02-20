#include "stdafx.h"
#include "audioController.h"

Mix_Music *music2;
double looppoint;

void SetMusPos2()
{
	Mix_RewindMusic( );
	Mix_PlayMusic( music2, 0 );
	Mix_SetMusicPosition( looppoint ); //Temp!
	Mix_HookMusicFinished( SetMusPos2 );
}

void AudioController::PlayMusic( const char *filename )
{
	std::string temp = filename;
	if( temp == lastPath )
		return;
	
	if( PlayingMusic )
	{
		Mix_FreeMusic( music );
		Mix_FreeMusic( music2 );
	}

	//Open the file "as a reabable"
	FILE *fp = fopen( filename, "r" );

	long lSize;
	// obtain file size:
	fseek (fp , 0 , SEEK_END);
	lSize = ftell(fp);
	rewind(fp);

	int buffpos = 0;
	int buffEOF;
	char * buffer;

	buffer = (char*) malloc (sizeof(char)*lSize);

	buffEOF = fread( buffer, 1, lSize, fp);

	std::string nameStr;
	bool found = false;
	bool reading = false;
	while( true )
	{
		if( !found )
		{
			if( buffer[buffpos] == 'L' && buffer[buffpos + 1] == 'O' && buffer[buffpos + 2] == 'O' && buffer[buffpos + 3] == 'P' && buffer[buffpos + 4] == 'S' )
			{
				found = true;
			}
		}
		else
		{
			if( buffer[buffpos] == '=' )
			{
				buffpos++;
				reading = true;
			}
		}
		if( reading )
		{
			if( buffer[buffpos] == ' ' )
				break;

			nameStr+=buffer[buffpos];
		}
		buffpos++;

		if( buffpos > buffEOF )
			break;
	}

	fclose( fp );

	double seconds = ( atof( nameStr.c_str() ) / 44100 );
	looppoint = seconds;
	music = Mix_LoadMUS( filename );
	music2 = music;
	Mix_PlayMusic( music, 0 );
	Mix_HookMusicFinished( SetMusPos2 );

	PlayingMusic = true;
}