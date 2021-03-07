#include "sound.h"
#include "system_SDL.h" 
#include "SDL_mixer.h"
#include <string>
using namespace std;

//adding some verbosity for SDL
#define SOUND_CHANNEL_ALL -1
#define SOUND_CHANNEL_NEW -1
#define SOUND_CHANNEL_NONE -1
#define SOUND_LOOPS_FOREVER -1

#define SOUND_CHUNK_SIZE 1024

static Mix_Music* gMusCurrent=NULL;

#define CACHE_MAXIMUM 32
static Mix_Chunk*   cache_sounds  [CACHE_MAXIMUM]={0};
static std::string  cache_filename[CACHE_MAXIMUM];
static int          cache_index=0;

//-----------------------------------------------------------------------------

inline int Sound_Cache_Add(Mix_Chunk* pSnd, std::string file){
	if(cache_index>=CACHE_MAXIMUM) return false;
	cache_sounds[cache_index]=pSnd;
	cache_filename[cache_index]=file;
	int i=cache_index;
	cache_index++;
	return i;
}
inline Mix_Chunk* Sound_Cache_Find(std::string file, int& iCached){
	for(int i=0;i<CACHE_MAXIMUM;i++)
		if(file==cache_filename[i]){
			iCached=i;
			return cache_sounds[i];
		}

	iCached=-1;
	return NULL;
}
Mix_Chunk* Sound_Cache_Load(std::string file, int& iCached){
	iCached=-1;
	Mix_Chunk* pSnd=Sound_Cache_Find(file,iCached);
	if(pSnd==NULL){//cached sound not found, try to load a new one (if it did succeed, skip to the return)
		pSnd=Mix_LoadWAV(file.c_str());
		if(pSnd!=NULL){//loading new sound did not fail, if it does fail, pSnd will be returned as NULL (per docs)
			iCached=Sound_Cache_Add(pSnd, file);
			if(iCached==-1){//caching new sound failed.
				Mix_FreeChunk(pSnd);//no need to check pSnd!=NULL since this was a precondition to get here.
				return NULL;
			}
		}
	}
	return pSnd;
}

void Sound_Cache_Clear(){
	for(int i=0;i<CACHE_MAXIMUM;i++)
		if(cache_sounds[i]!=NULL){
			Mix_FreeChunk(cache_sounds[i]);//release all the cached sounds we took responsibility for.
			cache_sounds[i]=NULL;
			cache_filename[i]="";
		}
	cache_index=0;
}
//-----------------------------------------------------------------------------





class SoundInfo
{
public:
	int audio_rate,audio_channels;
	Uint16 audio_format;
	Uint32 t;
	Mix_Music *music;
	int volume;
};

/*
class ClipInfo{
public:
	int iCached;
	Mix_Chunk* chunk;
	int channel;
};
*/



void* Sound_Startup(){
	SoundInfo* si=new SoundInfo();
	si->volume=SDL_MIX_MAXVOLUME;

	int flags=0;//0=WAV support. MIX_INIT_OGG|MIX_INIT_MOD;
	int initted=Mix_Init(flags);
	if((initted&flags) != flags) {
		System_Alert(string("Sound Error\n Mix_Init: Failed to init!\n")+"Mix_Init: %s\n", Mix_GetError());
		// handle error
	}

	if(Mix_OpenAudio(44100,MIX_DEFAULT_FORMAT,2,SOUND_CHUNK_SIZE)==-1) {
		System_Alert(string("Mix_OpenAudio: \n")+ Mix_GetError());
	}

	//Mix_AllocateChannels(CACHE_MAXIMUM);

	return si;
}
void Sound_Shutdown(void* pS){
	if(pS==NULL) return;
	Mix_HaltChannel(SOUND_CHANNEL_ALL);//halt playing on all channels
	Mix_HaltMusic();
	Sound_Cache_Clear();
	if(gMusCurrent!=NULL) { Mix_FreeMusic(gMusCurrent); gMusCurrent=NULL; }
	if(pS!=NULL){
		Mix_CloseAudio();
		Mix_Quit();
		delete (SoundInfo*) pS;
	}
}

bool Sound_SetMusic(void* pS, const string& filename,int fadems){
	if(pS==NULL) return false;
	Mix_Music* musNew=NULL;
	if(!filename.empty()){
		musNew=Mix_LoadMUS(filename.c_str());
		if(fadems<1)  Mix_PlayMusic  (musNew, SOUND_LOOPS_FOREVER);//per the docs, this halts and plays the new clip
		else          Mix_FadeInMusic(musNew, SOUND_LOOPS_FOREVER, fadems);
	}
	if(gMusCurrent!=NULL) Mix_FreeMusic(gMusCurrent);
	gMusCurrent=musNew;
	return (gMusCurrent!=NULL);
}
/*
void* Sound_ClipLoad(void* pS, const string& filename){
	SoundInfo* si=(SoundInfo*) pS;
	ClipInfo*  ci=new ClipInfo();
	ci->channel=-1;
	ci->chunk=Sound_Cache_Load(filename, ci->isCached);
	if(ci->chunk==NULL){
		delete ci;
		ci=NULL;
	}
	return ci;
}
bool Sound_ClipUnload(void* pS, void* pClip){
	SoundInfo* si=(SoundInfo*) pS;
	ClipInfo*  ci=(ClipInfo*) pClip;
	if(!ci->isCached) Mix_FreeChunk(ci->chunk);
	delete ci;
	return true;
}
bool Sound_ClipPlay(void* pS, void* pClip, POINT* direction){
	SoundInfo* si=(SoundInfo*) pS;
	ClipInfo*  ci=(ClipInfo*) pClip;
	//ci->channel=Mix_PlayChannel(-1, ci->chunk, 0);
	return (ci->channel > -1);
}
*/
bool Sound_ClipPlay(void* pS, const string& filename, int distance, int distmax)
{
	
	return false;


	if(pS==NULL) return false;
	int iCached=-1;
	Mix_Chunk* chunk=Sound_Cache_Load(filename, iCached);
	if(iCached>-1){

		int chan=Mix_PlayChannel(SOUND_CHANNEL_NEW,chunk,0);//while this works, we need a way to get a new unreserved channel, set its volume, then play the clip. playing on a static channel plays everything same-volume and resets track position.   playing on -1 channel is unpredictable / cannot set before.
		
		if(chan!=SOUND_CHANNEL_NONE){
			if(distance>0){
				int volume=255;
				if(distance<distmax){
					float f=(float)distance / (float)distmax;
					volume = f*255.0f;
				}
				volume+=random(-20,+20);
				if(volume>-1 && volume<256) Mix_SetDistance(chan,(int)volume);
			}else Mix_SetDistance(chan,0);
		}
		
	}

	return true;
}