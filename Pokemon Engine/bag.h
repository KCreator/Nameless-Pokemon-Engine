#include "graphics.h"
#include "item.h"
#include <vector>

class BagScene
{
public:
	//Initialiser
	void Init();

	//Think func!
	bool Tick();

	void Render();
	void AddItem( CBaseItem *item, int count );
	bool HasItem( int ID );
	int GetItemCount( int ID );

	void DeleteUsedItems();

	bool IsBattle;
	int PreviousScene;

	std::vector< CBaseItem * > items; //Hacky!

private:
	SDL_Texture *BagUI;

	int m_iCurPocket;
	bool isMovingItem;
	int numItems;

	int selection;

	bool debouncer;
};