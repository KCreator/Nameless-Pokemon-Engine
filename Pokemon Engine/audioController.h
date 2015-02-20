#pragma once
#include "stdafx.h"
#include <string>
#include "include\SDL_mixer.h"

class AudioController
{
public:
	AudioController::AudioController()
	{
		PlayingMusic = false;
	};
	void PlayMusic( const char *filename );
private:
	Mix_Music *music;

	std::string lastPath;

	bool PlayingMusic;
};
