#include "stdafx.h"
//#include "pokemon.h"
#include "graphics.h"

//Clamp function.
int Clamp( int in, int min, int max );

SDL_Rect GetRect( int x, int y, int w, int h );

float lerp(float a, float b, float f);

bool AwaitUserInput();

void FadeToBlack(  );

bool FileExists( const char *input );

bool strReplace(std::string& str, const std::string& from, const std::string& to);

 //range: [min, max].
int randomMinMax(int min, int max);