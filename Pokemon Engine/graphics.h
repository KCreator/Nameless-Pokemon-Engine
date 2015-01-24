#pragma once

#include "stdafx.h"
#include "SDL.h"
#include "SDL_video.h"
#include "SDL_thread.h"
#include "SDL_image.h"
#include <SDL_ttf.h>
#include <string>
#include <stdio.h>

class BattleBackGround
{
public:
	//Initializes variables
	BattleBackGround();

	//Deallocates memory
	~BattleBackGround();

	void Render();
	bool LoadFromFile( std::string path );

	//Deallocates texture
	void free();

private:
	//The actual hardware texture
	SDL_Texture* mTexture;
};

class BattleMenu
{
	friend class Pokemon;
public:
	//Initializes variables
	BattleMenu();

	//Deallocates memory
	~BattleMenu();

	void Render();
	bool LoadFromFile( std::string path, int texNum );

	void SetCurrentPokemon( Pokemon *pkmn )
	{
		Poke = pkmn;
	};

	//Deallocates texture
	void free();

	int subMenu;
	int cursorPos;

private:
	//The actual hardware texture
	SDL_Texture* mTexture_1;
	SDL_Texture* mTexture_2;
	SDL_Texture* mTexture_3;

	Pokemon *Poke;
};

class HPDisplays
{
	friend class Pokemon;
public:
	//Initializes variables
	HPDisplays();

	//Deallocates memory
	~HPDisplays();

	void Render( );
	bool LoadFromFile( std::string path );

	//Deallocates texture
	void free();

	int subMenu;
	int cursorPos;
	
	void UpdateHP( int hp1, int hp2, int maxhp1, int maxhp2 )
	{
		m_iBattler1_MaxHealth = maxhp1;
		m_iBattler2_MaxHealth = maxhp2;

		m_iBattler1_Health = hp1;
		m_iBattler2_Health = hp2;
	};

	void SetPokes( Pokemon *poke1, Pokemon *poke2 )
	{
		pmkn1 = poke1;
		pmkn2 = poke2;
	};

private:
	//The actual hardware texture
	SDL_Texture* mTexture;
	TTF_Font *mFont;

	int m_iBattler1_Health;
	int m_iBattler2_Health;
	int m_iBattler1_MaxHealth;
	int m_iBattler2_MaxHealth;

	Pokemon *pmkn1;
	Pokemon *pmkn2;
};

//This class MUST be last!
class BattleEngineGraphics
{
	friend class Pokemon;
public:
	BattleBackGround *bg;
	BattleMenu *menu;
	HPDisplays *hpDisp;

	Pokemon *BattlePkmn[2];

	SDL_Renderer *gRender;

	void AddPoke( Pokemon *poke, int num )
	{
		BattlePkmn[num] = poke;
	};

	void RenderPokes();
};

void initSDL();

SDL_Window* GetScreen();
