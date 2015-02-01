#pragma once

#include "stdafx.h"
#include "TileMap.h"
#include "player.h"

//Tile solid types:
#define TILE_SOLID_NONE 0
#define TILE_SOLID_BLOCK 1
#define TILE_SOLID_GRASS 2
#define TILE_SOLID_CLIFF_UP 3

class OverworldController
{
public:
	void Initialise();
	bool Tick();
	void Render();

	void RenderTxtBox();

	void EditorInit();
	void EditorThink();

	bool CheckEncounter();

	void TryInteract();

	int GetPlayerFacing( ){ return Player_Facing; };
	void SetPlayerFacing( int in ){ Player_Facing = in; };

	void MovePlayer( int x, int y, bool yFirst = false );

	void SetMapPos( std::string path, int x, int y );

	void FadeIn();

	Player *thePlayer;

private:
	TileMap *tm;

	//Adjacent maps:
	TileMap *adjacentMapPosY;
	TileMap *adjacentMapNegY;
	TileMap *adjacentMapPosX;
	TileMap *adjacentMapNegX;

	int Player_X, Player_Y;
	int Player_Facing;

	int cameraProgressY, cameraProgressX;
	int MoveCoolDown;

	bool AnimStep;

	bool m_bIsRunning;

	bool IsAnimating;

	void SetEditorTexture();
	void LoadAdjMaps();
	bool CheckCollision();
	void ChecKTrigger();

	SDL_Texture *Player_Texture;

	//Tilemap editor:
	SDL_Window *editor;
	SDL_Renderer *editorRenderer;
	SDL_Texture *editorTexture;

	SDL_Texture *OWTextBox;

	int editorSelection;
	int editorCollisionSelection;

	int mapX, mapY;

	int editorCameraX, editorCameraY;

	bool m_bMainMenuOpen;

	std::string mapPrefix;
};