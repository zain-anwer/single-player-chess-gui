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

void Audio::playSound (int type)
{
	if (type < 0 || type > 2)
	    return;
			
	SDL_LoadWAV(sound_name[type],&wavSpec,&wavStart,&wavLength);
	SDL_AudioDeviceID deviceId = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, 0);
	if (deviceId == 0) {
		    cout << "Could not open audio device SDL_Error: " << SDL_GetError() << endl;
		    return;
		}
	
	int success = SDL_QueueAudio(deviceId, wavStart, wavLength);
	if (success < 0) 
	{
	    cout << "Could not queue audio, SDL_Error: " <<  SDL_GetError() << endl;
		return;
	}
	SDL_PauseAudioDevice(deviceId, 0);

	SDL_Delay(100);

	SDL_CloseAudioDevice(deviceId);
    SDL_FreeWAV(wavStart);
}