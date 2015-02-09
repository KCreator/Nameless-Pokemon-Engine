#pragma once
#include "graphics.h" //Needs graphics!
#include "pokemon.h" //Just in case!

#define MAX_ITEMS 255 //Lots of potential items here!

class CBaseItem
{
public:
	CBaseItem::CBaseItem();
	CBaseItem::~CBaseItem();

	virtual void Use();

	void Add( int i ){ count += i; };

	virtual bool CanUse( bool IsBattle );

	int GetAmount(){ return count; };

	std::string GetName(){ return ItemName; };
	std::string GetDesc(){ return ItemDescription; };

	int ID;

protected:
	SDL_Texture *m_itemSprite;
	std::string ItemDescription, ItemName;

private:
	int count;
};

//Utility for save system:
CBaseItem* GetFromID(int id);

//BasePokeBall
class CBasePokeBall : public CBaseItem
{
	public:
		void Use();
		bool CanUse( bool IsBattle ){ return IsBattle; };

protected:
	float CatchMod;
};

class MasterBall : public CBasePokeBall
{
public:
	MasterBall::MasterBall()
	{
		ID = 1;
		ItemName = "Master Ball";
		ItemDescription = "The best Ball with the ultimate \nlevel of performance. It will catch any wild Pok�mon \nwithout fail. ";

		CatchMod = -1;
	}
};

class RareCandy : public CBaseItem
{
public:
	RareCandy::RareCandy()
	{
		ID = 50;
		ItemName = "Rare Candy";
		ItemDescription = "A strange candy that\nelevates the level of a\nsingle pokemon!";
	}

	void Use();
	bool CanUse( bool IsBattle ){ return !IsBattle; };
};