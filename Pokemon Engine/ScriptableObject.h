#pragma once
#include "stdafx.h"
#include "graphics.h"
#include "Overworld.h"
#include "pokemon.h"
#include "player.h"
#include "TileMap.h"
#include <map> //Datamaps

#define MAX_OBJECT_TYPES 255

class ObjectFlags
{
public:
	bool Objflags[MAX_OBJECT_TYPES];
	int intVars[ MAX_OBJECT_TYPES ];
};

class ScriptableObject
{
public:
	void SetUp(int index, int x, int y );
	void LoadData();
	void PlaceAt( int x, int y );
	void Render( int xofs, int yofs );
	void AnimatedRender( int xofs, int yofs );

	void Interact();

	int GetX() { return m_iX; };
	int GetY() { return m_iY; };

	int GetIndex() { return m_iIndex; };
	
	int Type;

private:
	int m_iIndex;

	int m_iX, m_iY;
	int m_GoalX, m_GoalY;
	int AnimStepX, AnimStepY;

	int m_xSize, m_ySize;

	bool m_bIsMoving;
	bool m_bRenderable;
	SDL_Texture *texture;

	bool flip;

	int iDirection;

	std::map< std::string, int > Variables;

	void MoveSelfTo( int X, int Y, bool Running, bool YFirst );
	void HandleMotion(int GoalX, int GoalY, bool YFirst);
};