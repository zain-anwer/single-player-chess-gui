#ifndef AUDIO_HPP
#define AUDIO_HPP


#include <SDL2/SDL.h>

extern const char* sound_name[5];

class Audio
{
	private:
		SDL_AudioSpec sound_specs[3]{};
		SDL_AudioSpec device_spec{};
		Uint8 *sound_buffers[3]{};
		Uint32 sound_lengths[3]{};
		SDL_AudioDeviceID device_id = 0;

    public:
		Audio();
		~Audio();
	    void playSound (int type);
		void shutdown();
};

#endif