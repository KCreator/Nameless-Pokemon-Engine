#include "graphics.h"
#include "particle.h"
#include <vector>

class OpeningScreen
{
public:
	void Init();
	bool Think();

private:
	void Render();
	void AnimationRender();
	void MenuRender();

	void OpeningAnimationThink();
	void MenuThink();

	//Variables:
	bool m_bIsTitleMenu;
	int m_iAnimationState;
	int m_iMenuSelection;

	int m_iTimer1;

	//Needed textures?
	SDL_Texture *m_texTexture_Title_BG;
	SDL_Texture *m_texTexture_Title_Pokemon;

	//Particle emitters:
	std::vector<CBaseEmitter *> Particles;
	void RenderAllParticles();
};