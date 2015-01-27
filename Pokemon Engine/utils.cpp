#include "stdafx.h"
#include "utils.h"
#include "include\SDL_events.h"

//Clamp function.
int Clamp( int in, int min, int max )
{
	int out = in;
	if( in > max )
	{
		out = max;
	}
	else if( in < min )
	{
		out = min;
	}

	return in;
}

SDL_Rect GetRect( int x, int y, int w, int h )
{
	SDL_Rect input = { x,y,w,h };

	return input;
}

float lerp(float a, float b, float f)
{
    return a + f * (b - a);
}

bool AwaitUserInput()
{
	//Await user confirmation:

	const Uint8 *keystate = SDL_GetKeyboardState( NULL );
	SDL_PumpEvents();
	if ( keystate[SDL_GetScancodeFromKey(SDLK_RETURN)] || keystate[SDL_GetScancodeFromKey(SDLK_SPACE)] )
	{
		return true;
	}

	return false;
}

extern SDL_Renderer *gRenderer;
void FadeToBlack( )
{
	int progress = 0;
	while( true )
	{
		SDL_Surface *surf = SDL_CreateRGBSurface( SDL_SWSURFACE, 600, 480, 1, 0,0,0, progress );
		SDL_Texture *texture = SDL_CreateTextureFromSurface( gRenderer, surf );
		//Get rid of old loaded surface
		SDL_FreeSurface( surf );

		SDL_SetTextureBlendMode( texture, SDL_BLENDMODE_BLEND );
		SDL_SetTextureAlphaMod( texture, progress );
		SDL_SetTextureColorMod( texture, 0, 0, 0 );

		SDL_RenderCopy( gRenderer, texture, NULL, NULL );
		SDL_RenderPresent( gRenderer );

		progress+=5;

		if( progress >= 100 )
		{
			break;
		}

		SDL_Delay( 1 );
	}
}

bool FileExists( const char *input )
{
	FILE *file = fopen( input, "r" );
	if( !file )
		return false;
	fclose( file );

	return true;
}

bool strReplace(std::string& str, const std::string& from, const std::string& to)
{
    size_t start_pos = str.find(from);
    if(start_pos == std::string::npos)
        return false;
    str.replace(start_pos, from.length(), to);
    return true;
}

 //range: [min, max].
int randomMinMax(int min, int max)
{
   return min + rand() % (max - min + 1);
};
