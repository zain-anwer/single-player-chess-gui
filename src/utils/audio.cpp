#include "audio.hpp"
#include <iostream>
using namespace std;

const char* sound_name[5] = {
    "ChessAudio/illegal.wav",
    "ChessAudio/move.wav",
    "ChessAudio/capture.wav",
    "ChessAudio/capture.wav",
    "ChessAudio/capture.wav"
};

Audio::Audio()
{
	for (int type = 0; type < 3; type++)
	{
		if (SDL_LoadWAV(sound_name[type], &sound_specs[type], &sound_buffers[type], &sound_lengths[type]) == nullptr)
		{
			cerr << "Unable to load sound " << sound_name[type] << ": " << SDL_GetError() << '\n';
			continue;
		}

		if (device_spec.freq == 0)
			device_spec = sound_specs[type];
	}

	if (device_spec.freq == 0)
	{
		cerr << "No sound effects could be loaded.\n";
		return;
	}

	device_id = SDL_OpenAudioDevice(nullptr, 0, &device_spec, nullptr, 0);
	if (device_id == 0)
	{
		cerr << "Unable to open audio device: " << SDL_GetError() << '\n';
		return;
	}

	SDL_PauseAudioDevice(device_id, 0);
}

Audio::~Audio()
{
	shutdown();
}

void Audio::shutdown()
{
	if (device_id != 0)
	{
		SDL_CloseAudioDevice(device_id);
		device_id = 0;
	}

	for (int type = 0; type < 3; type++)
	{
		if (sound_buffers[type] != nullptr)
		{
			SDL_FreeWAV(sound_buffers[type]);
			sound_buffers[type] = nullptr;
		}
	}
}

void Audio::playSound (int type)
{
	if (type < 0 || type > 2 || device_id == 0 || sound_buffers[type] == nullptr)
	    return;

	if (sound_specs[type].freq != device_spec.freq ||
		sound_specs[type].format != device_spec.format ||
		sound_specs[type].channels != device_spec.channels)
	{
		cerr << "Sound format does not match the audio device: " << sound_name[type] << '\n';
		return;
	}

	if (SDL_QueueAudio(device_id, sound_buffers[type], sound_lengths[type]) < 0)
		cerr << "Could not queue sound: " << SDL_GetError() << '\n';
}