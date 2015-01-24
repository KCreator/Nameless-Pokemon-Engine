#include "stdafx.h"
#include "graphics.h"
#include "Overworld.h"

#define TEXTFLAG_AUTOSCALE 1
#define TEXTFLAG_SHADOWED 2

class CText
{
public:
	CText::CText( std::string text, SDL_Renderer *renderer, TTF_Font *mFont, int flags = 0, int red = 0, int green = 0, int blue = 0 ); //Initialiser
	CText::~CText(); //Deconstructor

	void Render(SDL_Rect *rect);
private:
	std::string textString;
	SDL_Renderer *gRenderer;
	SDL_Texture *texture;
	SDL_Texture *texture_2;

	int m_IFlags;
};

void BattleText( std::string input, SDL_Renderer *render, BattleEngineGraphics *BattleUIGFX, TTF_Font *font, bool renderHP = true );
void OWText( std::string input, SDL_Renderer *render, OverworldController *overworld, TTF_Font *font );