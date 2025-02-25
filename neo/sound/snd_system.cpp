/*
===========================================================================

Doom 3 GPL Source Code
Copyright (C) 1999-2011 id Software LLC, a ZeniMax Media company.

This file is part of the Doom 3 GPL Source Code ("Doom 3 Source Code").

Doom 3 Source Code is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Doom 3 Source Code is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Doom 3 Source Code.  If not, see <http://www.gnu.org/licenses/>.

In addition, the Doom 3 Source Code is also subject to certain additional terms. You should have received a copy of these additional terms immediately following the terms and conditions of the GNU General Public License which accompanied the Doom 3 Source Code.  If not, please request a copy in writing from id Software at the address below.

If you have questions concerning this license or the applicable additional terms, you may contact in writing id Software LLC, c/o ZeniMax Media Inc., Suite 120, Rockville, Maryland 20850 USA.

===========================================================================
*/

#include "sys/platform.h"
#include "idlib/LangDict.h"


#include "sound/snd_local.h"

#ifdef ID_DEDICATED
idCVar idSoundSystemLocal::s_noSound( "s_noSound", "1", CVAR_SOUND | CVAR_BOOL | CVAR_ROM, "" );
#else
idCVar idSoundSystemLocal::s_noSound( "s_noSound", "0", CVAR_SOUND | CVAR_BOOL | CVAR_NOCHEAT, "" );
#endif
idCVar idSoundSystemLocal::s_device( "s_device", "default", CVAR_SOUND | CVAR_NOCHEAT | CVAR_ARCHIVE, "the audio device to use ('default' for the default audio device)" );
idCVar idSoundSystemLocal::s_quadraticFalloff( "s_quadraticFalloff", "1", CVAR_SOUND | CVAR_BOOL, "" );
idCVar idSoundSystemLocal::s_drawSounds( "s_drawSounds", "0", CVAR_SOUND | CVAR_INTEGER, "", 0, 2, idCmdSystem::ArgCompletion_Integer<0,2> );
idCVar idSoundSystemLocal::s_showStartSound( "s_showStartSound", "0", CVAR_SOUND | CVAR_INTEGER, "Print sounds when they play. 1 = basic, 2 = detailed." );
idCVar idSoundSystemLocal::s_useOcclusion( "s_useOcclusion", "1", CVAR_SOUND | CVAR_BOOL, "" );
idCVar idSoundSystemLocal::s_maxSoundsPerShader( "s_maxSoundsPerShader", "0", CVAR_SOUND | CVAR_ARCHIVE, "", 0, 10, idCmdSystem::ArgCompletion_Integer<0,10> );
idCVar idSoundSystemLocal::s_showLevelMeter( "s_showLevelMeter", "0", CVAR_SOUND | CVAR_BOOL, "" );
idCVar idSoundSystemLocal::s_constantAmplitude( "s_constantAmplitude", "-1", CVAR_SOUND | CVAR_FLOAT, "" );
idCVar idSoundSystemLocal::s_minVolume6( "s_minVolume6", "0", CVAR_SOUND | CVAR_FLOAT, "" );
idCVar idSoundSystemLocal::s_dotbias6( "s_dotbias6", "0.8", CVAR_SOUND | CVAR_FLOAT, "" );
idCVar idSoundSystemLocal::s_minVolume2( "s_minVolume2", "0.25", CVAR_SOUND | CVAR_FLOAT, "" );
idCVar idSoundSystemLocal::s_dotbias2( "s_dotbias2", "1.1", CVAR_SOUND | CVAR_FLOAT, "" );
idCVar idSoundSystemLocal::s_spatializationDecay( "s_spatializationDecay", "2", CVAR_SOUND | CVAR_ARCHIVE | CVAR_FLOAT, "" );
idCVar idSoundSystemLocal::s_reverse( "s_reverse", "0", CVAR_SOUND | CVAR_ARCHIVE | CVAR_BOOL, "" );
idCVar idSoundSystemLocal::s_meterTopTime("s_meterTopTime", "2000", CVAR_SOUND | CVAR_ARCHIVE | CVAR_INTEGER, "");
idCVar idSoundSystemLocal::s_volumeCurve("s_volumeCurve", "0.75", CVAR_SOUND | CVAR_FLOAT, "Curve for volume slider. Higher value = steeper. (internal)", 0.0, 1.0);
idCVar idSoundSystemLocal::s_volumeBoost("s_volumeBoost", "0.5", CVAR_SOUND | CVAR_FLOAT, "Scale for volume slider. (internal)", 0.0,4.0);
idCVar idSoundSystemLocal::s_volumeAll("s_volumeAll", "0.6", CVAR_SOUND | CVAR_ARCHIVE | CVAR_FLOAT, "Master volume. (0.0 - 1.0)",0.0,1.0);
idCVar idSoundSystemLocal::s_volumeEffects("s_volumeEffects", "1.0", CVAR_SOUND | CVAR_ARCHIVE | CVAR_FLOAT, "volume for effects. (0.0 - 1.0)",0.0,1.0);
idCVar idSoundSystemLocal::s_volumeMusic("s_volumeMusic", "1.0", CVAR_SOUND | CVAR_ARCHIVE | CVAR_FLOAT, "volume for music. (0.0 - 1.0)",0.0,1.0);
idCVar idSoundSystemLocal::s_volumeVoice("s_volumeVoice", "1.0", CVAR_SOUND | CVAR_ARCHIVE | CVAR_FLOAT, "volume for voice. (0.0 - 1.0)",0.0,1.0);
idCVar idSoundSystemLocal::s_playDefaultSound( "s_playDefaultSound", "1", CVAR_SOUND | CVAR_ARCHIVE | CVAR_BOOL, "play a beep for missing sounds" );
idCVar idSoundSystemLocal::s_subFraction( "s_subFraction", "0.75", CVAR_SOUND | CVAR_ARCHIVE | CVAR_FLOAT, "volume to subwoofer in 5.1" );
idCVar idSoundSystemLocal::s_globalFraction( "s_globalFraction", "0.8", CVAR_SOUND | CVAR_ARCHIVE | CVAR_FLOAT, "volume to all speakers when not spatialized" );
idCVar idSoundSystemLocal::s_doorDistanceAdd( "s_doorDistanceAdd", "200", CVAR_SOUND | CVAR_FLOAT, "reduce sound volume with this distance when going through a door" );
idCVar idSoundSystemLocal::s_singleEmitter( "s_singleEmitter", "0", CVAR_SOUND | CVAR_INTEGER, "mute all sounds but this emitter" );
idCVar idSoundSystemLocal::s_numberOfSpeakers( "s_numberOfSpeakers", "2", CVAR_SOUND | CVAR_ARCHIVE, "number of speakers" );
idCVar idSoundSystemLocal::s_force22kHz( "s_force22kHz", "0", CVAR_SOUND | CVAR_BOOL, ""  );
idCVar idSoundSystemLocal::s_clipVolumes( "s_clipVolumes", "1", CVAR_SOUND | CVAR_BOOL, ""  );
idCVar idSoundSystemLocal::s_realTimeDecoding( "s_realTimeDecoding", "1", CVAR_SOUND | CVAR_BOOL | CVAR_INIT, "" );

idCVar idSoundSystemLocal::s_slowAttenuate( "s_slowAttenuate", "1", CVAR_SOUND | CVAR_BOOL, "slowmo sounds attenuate over shorted distance" );
idCVar idSoundSystemLocal::s_reverbTime( "s_reverbTime", "1000", CVAR_SOUND | CVAR_FLOAT, "" );
idCVar idSoundSystemLocal::s_reverbFeedback( "s_reverbFeedback", "0.333", CVAR_SOUND | CVAR_FLOAT, "" );
idCVar idSoundSystemLocal::s_skipHelltimeFX( "s_skipHelltimeFX", "0", CVAR_SOUND | CVAR_BOOL, "" );

// SW: Exposing these parameters on the envirosuit EFX low-pass filter so that we can fiddle around with them
//idCVar idSoundSystemLocal::s_enviroSuitLPFOcclusion("s_enviroSuitLPFOcclusion", "2000", CVAR_SOUND | CVAR_INTEGER, "Occlusion value for the low-pass filter that gets applied in airless environments");
//idCVar idSoundSystemLocal::s_enviroSuitLPFOcclusionRatio("s_enviroSuitLPFOcclusionRatio", "0.3", CVAR_SOUND | CVAR_FLOAT, "Occlusion ratio for the low-pass filter that gets applied in airless environments");
idCVar idSoundSystemLocal::s_enviroSuitHighpass("s_enviroSuitHighpass", ".01", CVAR_SOUND | CVAR_FLOAT, "High-pass gain value for zero-g audio filter. Smaller numbers = more muted.");
idCVar idSoundSystemLocal::s_enviroSuitCutoffFreq("s_enviroSuitCutoffFreq", "2000", CVAR_SOUND | CVAR_FLOAT, "OBSOLETE");
idCVar idSoundSystemLocal::s_enviroSuitCutoffQ("s_enviroSuitCutoffQ", "2", CVAR_SOUND | CVAR_FLOAT, "OBSOLETE");
idCVar idSoundSystemLocal::s_enviroSuitVolumeScale("s_enviroSuitVolumeScale", "0.9", CVAR_SOUND | CVAR_FLOAT, "OBSOLETE");

#if !defined(ID_DEDICATED)
idCVar idSoundSystemLocal::s_useEAXReverb( "s_useEAXReverb", "1", CVAR_SOUND | CVAR_BOOL | CVAR_ARCHIVE, "use EFX reverb" );
idCVar idSoundSystemLocal::s_decompressionLimit( "s_decompressionLimit", "6", CVAR_SOUND | CVAR_INTEGER | CVAR_ARCHIVE, "specifies maximum uncompressed sample length in seconds" );
#else
idCVar idSoundSystemLocal::s_useEAXReverb( "s_useEAXReverb", "0", CVAR_SOUND | CVAR_BOOL | CVAR_ROM, "EFX not available in this build" );
idCVar idSoundSystemLocal::s_decompressionLimit( "s_decompressionLimit", "6", CVAR_SOUND | CVAR_INTEGER | CVAR_ROM, "specifies maximum uncompressed sample length in seconds" );
#endif

//BC
idCVar idSoundSystemLocal::s_debug("s_debug", "0", CVAR_SOUND | CVAR_BOOL, "show audio debug errors." );
idCVar idSoundSystemLocal::s_drawoffset("s_drawoffset", "0", CVAR_SOUND | CVAR_BOOL, "show audio sound origin offset ('sound_offset' keyvalue). Note: only appears at map start.");
idCVar idSoundSystemLocal::s_voDebug("s_voDebug", "0", CVAR_SOUND | CVAR_INTEGER, "show VO manager debug.");

// SW: Display info on how we're currently blending two reverb settings
idCVar idSoundSystemLocal::s_showReverbBlend("s_showReverbBlend", "0", CVAR_SOUND | CVAR_BOOL, "shows current reverbs in the primary and secondary slots and their respective blend factors");
idCVar idSoundSystemLocal::s_muteInBackground("s_muteInBackground", "1", CVAR_SOUND | CVAR_BOOL | CVAR_ARCHIVE, "mute the game when window loses focus");

idCVar idSoundSystemLocal::s_debugSlowmo("s_debugSlowmo", "0", CVAR_BOOL | CVAR_SYSTEM, "Display debug messages for the slow-mo manager.");
//SM
idCVar idSoundSystemLocal::s_autoduck_db( "s_autoduck_db", "-8.0", CVAR_SOUND | CVAR_FLOAT, "How many dB to fade sounds when autoducking" );
idCVar idSoundSystemLocal::s_autoduck_fadeTime( "s_autoduck_fadeTime", "1.0", CVAR_SOUND | CVAR_FLOAT, "How long the fade in/out takes for auto ducking" );

bool idSoundSystemLocal::useEFXReverb = false;
int idSoundSystemLocal::EFXAvailable = -1;

idSoundSystemLocal	soundSystemLocal;
idSoundSystem	*soundSystem  = &soundSystemLocal;

/*
===============
SoundReloadSounds_f

  this is called from the main thread
===============
*/
void SoundReloadSounds_f( const idCmdArgs &args ) {
	if ( !soundSystemLocal.soundCache ) {
		return;
	}
	common->Printf("Checking for changed sound files...\n");
	bool force = false;
	if ( args.Argc() == 2 ) {
		force = true;
	}
	soundSystem->SetMute( true );
	soundSystemLocal.soundCache->ReloadSounds( force );

	//DARKMOD
	if (soundSystemLocal.useEFXReverb)
	{
		bool ok = soundSystemLocal.EFXDatabase.Reload();
		if (ok)
			common->Printf("Checking for changed EFX files...\n");
	}


	soundSystem->SetMute( false );	
}

//BC this is listsounds, but pares it down to just the file name and nothing else. This is so we can more easily plug things into blendo diff deleter for demo builds.
void ListSoundsSimple_f(const idCmdArgs& args)
{
	int i;
	int	totalSounds = 0;
	const char* snd = args.Argv(1);

	if (!soundSystemLocal.soundCache) {
		common->Printf("No sound.\n");
		return;
	}

	for (i = 0; i < soundSystemLocal.soundCache->GetNumObjects(); i++) {
		const idSoundSample* sample = soundSystemLocal.soundCache->GetObject(i);
		if (!sample) {
			continue;
		}
		if (snd && sample->name.Find(snd, false) < 0) {
			continue;
		}

		common->Printf("%s\n", sample->name.c_str());
		totalSounds++;
	}
	common->Printf("\n%8d total sounds\n", totalSounds);	
}
	

/*
===============
ListSounds_f

Optional parameter to only list sounds containing that string
===============
*/
void ListSounds_f( const idCmdArgs &args ) {
	int i;
	const char	*snd = args.Argv( 1 );

	if ( !soundSystemLocal.soundCache ) {
		common->Printf( "No sound.\n" );
		return;
	}

	int	totalSounds = 0;
	int totalSamples = 0;
	int totalMemory = 0;
	int totalPCMMemory = 0;
	for( i = 0; i < soundSystemLocal.soundCache->GetNumObjects(); i++ ) {
		const idSoundSample *sample = soundSystemLocal.soundCache->GetObject(i);
		if ( !sample ) {
			continue;
		}
		if ( snd && sample->name.Find( snd, false ) < 0 ) {
			continue;
		}

		const waveformatex_t &info = sample->objectInfo;

		const char *stereo = ( info.nChannels == 2 ? "ST" : "  " );
		const char *format = ( info.wFormatTag == WAVE_FORMAT_TAG_OGG ) ? "OGG" : "WAV";
		const char *defaulted = ( sample->defaultSound ? "(DEFAULTED)" : sample->purged ? "(PURGED)" : "" );

		common->Printf( "%s %dkHz %6dms %5dkB %4s %s%s\n", stereo, sample->objectInfo.nSamplesPerSec / 1000,
					soundSystemLocal.SamplesToMilliseconds( sample->LengthIn44kHzSamples() ),
					sample->objectMemSize >> 10, format, sample->name.c_str(), defaulted );

		if ( !sample->purged ) {
			totalSamples += sample->objectSize;
			if ( info.wFormatTag != WAVE_FORMAT_TAG_OGG )
				totalPCMMemory += sample->objectMemSize;
			if ( !sample->hardwareBuffer )
				totalMemory += sample->objectMemSize;
		}
		totalSounds++;
	}
	common->Printf( "%8d total sounds\n", totalSounds );
	common->Printf( "%8d total samples loaded\n", totalSamples );
	common->Printf( "%8d kB total system memory used\n", totalMemory >> 10 );
}

/*
===============
ListSoundDecoders_f
===============
*/
void ListSoundDecoders_f( const idCmdArgs &args ) {
	int i, j, numActiveDecoders, numWaitingDecoders;
	idSoundWorldLocal *sw = soundSystemLocal.currentSoundWorld;

	numActiveDecoders = numWaitingDecoders = 0;

	for ( i = 0; i < sw->emitters.Num(); i++ ) {
		idSoundEmitterLocal *sound = sw->emitters[i];

		if ( !sound ) {
			continue;
		}

		// run through all the channels
		for ( j = 0; j < SOUND_MAX_CHANNELS; j++ ) {
			idSoundChannel	*chan = &sound->channels[j];

			if ( chan->decoder == NULL ) {
				continue;
			}

			idSoundSample *sample = chan->decoder->GetSample();

			if ( sample != NULL ) {
				continue;
			}

			const char *format = ( chan->leadinSample->objectInfo.wFormatTag == WAVE_FORMAT_TAG_OGG ) ? "OGG" : "WAV";
			common->Printf( "%3d waiting %s: %s\n", numWaitingDecoders, format, chan->leadinSample->name.c_str() );

			numWaitingDecoders++;
		}
	}

	for ( i = 0; i < sw->emitters.Num(); i++ ) {
		idSoundEmitterLocal *sound = sw->emitters[i];

		if ( !sound ) {
			continue;
		}

		// run through all the channels
		for ( j = 0; j < SOUND_MAX_CHANNELS; j++ ) {
			idSoundChannel	*chan = &sound->channels[j];

			if ( chan->decoder == NULL ) {
				continue;
			}

			idSoundSample *sample = chan->decoder->GetSample();

			if ( sample == NULL ) {
				continue;
			}

			const char *format = ( sample->objectInfo.wFormatTag == WAVE_FORMAT_TAG_OGG ) ? "OGG" : "WAV";

			int localTime = soundSystemLocal.GetCurrent44kHzTime() - chan->trigger44kHzTime;
			int sampleTime = sample->LengthIn44kHzSamples() * sample->objectInfo.nChannels;
			int percent;
			if ( localTime > sampleTime ) {
				if ( chan->parms.soundShaderFlags & SSF_LOOPING ) {
					percent = ( localTime % sampleTime ) * 100 / sampleTime;
				} else {
					percent = 100;
				}
			} else {
				percent = localTime * 100 / sampleTime;
			}

			common->Printf( "%3d decoding %3d%% %s: %s\n", numActiveDecoders, percent, format, sample->name.c_str() );

			numActiveDecoders++;
		}
	}

	common->Printf( "%d decoders\n", numWaitingDecoders + numActiveDecoders );
	common->Printf( "%d waiting decoders\n", numWaitingDecoders );
	common->Printf( "%d active decoders\n", numActiveDecoders );
	common->Printf( "%d kB decoder memory in %d blocks\n", idSampleDecoder::GetUsedBlockMemory() >> 10, idSampleDecoder::GetNumUsedBlocks() );
}

/*
===============
TestSound_f

  this is called from the main thread
===============
*/
void TestSound_f( const idCmdArgs &args ) {
	if ( args.Argc() != 2 ) {
		common->Printf( "Usage: testSound <file>\n" );
		return;
	}
	if ( soundSystemLocal.currentSoundWorld ) {
		soundSystemLocal.currentSoundWorld->PlayShaderDirectly( args.Argv( 1 ) );
	}
}

/*
===============
SoundSystemRestart_f

restart the sound thread

  this is called from the main thread
===============
*/
void SoundSystemRestart_f( const idCmdArgs &args ) {
	soundSystem->SetMute( true );
	soundSystemLocal.ShutdownHW();
	soundSystemLocal.InitHW();
	soundSystem->SetMute( false );
}

/*
===============
idSoundSystemLocal::Init

initialize the sound system
===============
*/
void idSoundSystemLocal::Init() {
	common->Printf( "----- Initializing OpenAL -----\n" );

	isInitialized = false;
	muted = false;
	shutdown = false;

	currentSoundWorld = NULL;
	soundCache = NULL;

	olddwCurrentWritePos = 0;
	buffers = 0;
	CurrentSoundTime = 0;

	nextWriteBlock = 0xffffffff;

	soundcardname = ""; //BC

	memset( meterTops, 0, sizeof( meterTops ) );
	memset( meterTopsTime, 0, sizeof( meterTopsTime ) );

	for( int i = -600; i < 600; i++ ) {
		float pt = i * 0.1f;
		volumesDB[i+600] = pow( 2.0f,( pt * ( 1.0f / 6.0f ) ) );
	}

	// make a 16 byte aligned finalMixBuffer
	finalMixBuffer = (float *) ( ( ( (intptr_t)realAccum ) + 15 ) & ~15 );

	graph = NULL;

	if ( !s_noSound.GetBool() ) {
		idSampleDecoder::Init();
		soundCache = new idSoundCache();
	}

	// set up openal device and context
	common->Printf( "Setup OpenAL device and context\n" );

	const char *device = s_device.GetString();
	if (strlen(device) < 1)
		device = NULL;
	else if (!idStr::Icmp(device, "default"))
		device = NULL;

	if (alcIsExtensionPresent(NULL, "ALC_ENUMERATE_ALL_EXT")) {
		const char *devs = alcGetString(NULL, ALC_ALL_DEVICES_SPECIFIER);
		bool found = false;

		while (devs && *devs) {
			common->Printf("OpenAL: found device '%s'", devs);

			if (device && !idStr::Icmp(devs, device)) {
				common->Printf(" (ACTIVE)\n");
				found = true;
			} else {
				common->Printf("\n");
			}

			devs += strlen(devs) + 1;
		}

		if (device && !found) {
			common->Printf("OpenAL: device %s not found, using default\n", device);
			device = NULL;
		}
	}

	openalDevice = alcOpenDevice( device );
	if (!openalDevice && device) {
		common->Warning("OpenAL: failed to open device '%s' (0x%x), using default\n", device, alGetError());
		openalDevice = alcOpenDevice( NULL );
	}
	else if (openalDevice == nullptr && device == nullptr)
	{
		common->Warning("no sound devices found. Please check your system's sound devices.\n");
	}
	

	openalContext = alcCreateContext( openalDevice, NULL );
	alcMakeContextCurrent( openalContext );

	// log openal info
	common->Printf( "OpenAL vendor: %s\n", alGetString(AL_VENDOR));
	common->Printf( "OpenAL renderer: %s\n", alGetString(AL_RENDERER));
	common->Printf( "OpenAL version: %s\n", alGetString(AL_VERSION));

	//BC print audio info in the options menu.
	idStr renderername = alGetString(AL_RENDERER);
	idStr versionname = alGetString(AL_VERSION);
	if (renderername.IsEmpty() || versionname.IsEmpty())
	{
		soundcardname = common->GetLanguageDict()->GetString("#str_gui_mainmenu_noaudio");
	}
	else
	{
		soundcardname = idStr::Format("%s\n%s", renderername.c_str(), versionname.c_str());
	}

	// try to obtain EFX extensions
	if (alcIsExtensionPresent(openalDevice, "ALC_EXT_EFX")) {
		common->Printf( "OpenAL: found EFX extension\n" );
		EFXAvailable = 1;

		alGenEffects = (LPALGENEFFECTS)alGetProcAddress("alGenEffects");
		alDeleteEffects = (LPALDELETEEFFECTS)alGetProcAddress("alDeleteEffects");
		alIsEffect = (LPALISEFFECT)alGetProcAddress("alIsEffect");
		alEffecti = (LPALEFFECTI)alGetProcAddress("alEffecti");
		alEffectf = (LPALEFFECTF)alGetProcAddress("alEffectf");
		alEffectfv = (LPALEFFECTFV)alGetProcAddress("alEffectfv");
		alGenFilters = (LPALGENFILTERS)alGetProcAddress("alGenFilters");
		alDeleteFilters = (LPALDELETEFILTERS)alGetProcAddress("alDeleteFilters");
		alIsFilter = (LPALISFILTER)alGetProcAddress("alIsFilter");
		alFilteri = (LPALFILTERI)alGetProcAddress("alFilteri");
		alFilterf = (LPALFILTERF)alGetProcAddress("alFilterf");
		alGenAuxiliaryEffectSlots = (LPALGENAUXILIARYEFFECTSLOTS)alGetProcAddress("alGenAuxiliaryEffectSlots");
		alDeleteAuxiliaryEffectSlots = (LPALDELETEAUXILIARYEFFECTSLOTS)alGetProcAddress("alDeleteAuxiliaryEffectSlots");
		alIsAuxiliaryEffectSlot = (LPALISAUXILIARYEFFECTSLOT)alGetProcAddress("alIsAuxiliaryEffectSlot");;
		alAuxiliaryEffectSloti = (LPALAUXILIARYEFFECTSLOTI)alGetProcAddress("alAuxiliaryEffectSloti");
		alAuxiliaryEffectSlotf = (LPALAUXILIARYEFFECTSLOTF)alGetProcAddress("alAuxiliaryEffectSlotf");
	} else {
		common->Printf( "OpenAL: EFX extension not found\n" );
		EFXAvailable = 0;
		idSoundSystemLocal::s_useEAXReverb.SetBool( false );

		alGenEffects = NULL;
		alDeleteEffects = NULL;
		alIsEffect = NULL;
		alEffecti = NULL;
		alEffectf = NULL;
		alEffectfv = NULL;
		alGenFilters = NULL;
		alDeleteFilters = NULL;
		alIsFilter = NULL;
		alFilteri = NULL;
		alFilterf = NULL;
		alGenAuxiliaryEffectSlots = NULL;
		alDeleteAuxiliaryEffectSlots = NULL;
		alIsAuxiliaryEffectSlot = NULL;
		alAuxiliaryEffectSloti = NULL;
		alAuxiliaryEffectSlotf = NULL;
	}

	ALuint handle;
	openalSourceCount = 0;

	while ( openalSourceCount < 256 ) {
		alGetError();
		alGenSources( 1, &handle );
		if ( alGetError() != AL_NO_ERROR ) {
			break;
		} else {
			// store in source array
			openalSources[openalSourceCount].handle = handle;
			openalSources[openalSourceCount].startTime = 0;
			openalSources[openalSourceCount].chan = NULL;
			openalSources[openalSourceCount].inUse = false;
			openalSources[openalSourceCount].looping = false;

			// initialise sources
			alSourcef( handle, AL_ROLLOFF_FACTOR, 0.0f );

			// found one source
			openalSourceCount++;
		}
	}

	common->Printf( "OpenAL: found %d hardware voices\n", openalSourceCount );

	// adjust source count to allow for at least eight stereo sounds to play
	openalSourceCount -= 8;

	useEFXReverb = idSoundSystemLocal::s_useEAXReverb.GetBool();
	efxloaded = false;

	cmdSystem->AddCommand( "listSounds", ListSounds_f, CMD_FL_SOUND, "lists all sounds" );
	cmdSystem->AddCommand("listSoundsSimple", ListSoundsSimple_f, CMD_FL_SOUND, "lists all sounds");
	cmdSystem->AddCommand( "listSoundDecoders", ListSoundDecoders_f, CMD_FL_SOUND, "list active sound decoders" );
	cmdSystem->AddCommand( "reloadSounds", SoundReloadSounds_f, CMD_FL_SOUND|CMD_FL_CHEAT, "reloads all sounds" );
	cmdSystem->AddCommand( "testSound", TestSound_f, CMD_FL_SOUND | CMD_FL_CHEAT, "tests a sound", idCmdSystem::ArgCompletion_SoundName );
	cmdSystem->AddCommand( "s_restart", SoundSystemRestart_f, CMD_FL_SOUND, "restarts the sound system" );
}

/*
===============
idSoundSystemLocal::Shutdown
===============
*/
void idSoundSystemLocal::Shutdown() {
	ShutdownHW();

	// EFX or not, the list needs to be cleared
	EFXDatabase.Clear();

	efxloaded = false;

	// adjust source count back up to allow for freeing of all resources
	openalSourceCount += 8;

	for ( ALsizei i = 0; i < openalSourceCount; i++ ) {
		// stop source
		alSourceStop( openalSources[i].handle );
		alSourcei( openalSources[i].handle, AL_BUFFER, 0 );

		// delete source
		alDeleteSources( 1, &openalSources[i].handle );

		// clear entry in source array
		openalSources[i].handle = 0;
		openalSources[i].startTime = 0;
		openalSources[i].chan = NULL;
		openalSources[i].inUse = false;
		openalSources[i].looping = false;
	}

	// destroy all the sounds (hardware buffers as well)
	delete soundCache;
	soundCache = NULL;

	// destroy openal device and context
	alcMakeContextCurrent( NULL );

	alcDestroyContext( openalContext );
	openalContext = NULL;

	alcCloseDevice( openalDevice );
	openalDevice = NULL;

	idSampleDecoder::Shutdown();
}

/*
===============
idSoundSystemLocal::InitHW
===============
*/
bool idSoundSystemLocal::InitHW() {
	int numSpeakers = s_numberOfSpeakers.GetInteger();

	if (numSpeakers != 1 && numSpeakers != 2 && numSpeakers != 6) {
		common->Warning("invalid value for s_numberOfSpeakers. Use either 2 or 6");
		numSpeakers = 2;
		s_numberOfSpeakers.SetInteger(numSpeakers);
	}

	if ( s_noSound.GetBool() ) {
		return false;
	}

	// put the real number in there
	s_numberOfSpeakers.SetInteger(numSpeakers);

	isInitialized = true;
	shutdown = false;

	return true;
}

/*
===============
idSoundSystemLocal::ShutdownHW
===============
*/
bool idSoundSystemLocal::ShutdownHW() {
	if ( !isInitialized ) {
		return false;
	}

	shutdown = true;		// don't do anything at AsyncUpdate() time
	Sys_Sleep( 100 );		// sleep long enough to make sure any async sound talking to hardware has returned

	common->Printf( "Shutting down sound hardware\n" );

	isInitialized = false;

	if ( graph ) {
		Mem_Free( graph );
		graph = NULL;
	}

	return true;
}

/*
===============
idSoundSystemLocal::GetCurrent44kHzTime
===============
*/
int idSoundSystemLocal::GetCurrent44kHzTime( void ) const {
	if ( isInitialized ) {
		return CurrentSoundTime;
	} else {
		// NOTE: this would overflow 31bits within about 1h20
		//return ( ( Sys_Milliseconds()*441 ) / 10 ) * 4;
		return idMath::FtoiFast( (float)Sys_Milliseconds() * 176.4f );
	}
}

/*
===================
idSoundSystemLocal::AsyncMix
Mac OSX version. The system uses it's own thread and an IOProc callback
===================
*/
int idSoundSystemLocal::AsyncMix( int soundTime, float *mixBuffer ) {
	int	inTime, numSpeakers;

	if ( !isInitialized || shutdown ) {
		return 0;
	}

	inTime = Sys_Milliseconds();
	numSpeakers = s_numberOfSpeakers.GetInteger();

	// let the active sound world mix all the channels in unless muted or avi demo recording
	if ( !muted && currentSoundWorld && !currentSoundWorld->fpa[0] ) {
		currentSoundWorld->MixLoop( soundTime, numSpeakers, mixBuffer, s_muteInBackground.GetBool());
	}

	CurrentSoundTime = soundTime;

	return Sys_Milliseconds() - inTime;
}

/*
===================
idSoundSystemLocal::AsyncUpdate
called from async sound thread when com_asyncSound == 1 ( Windows )
===================
*/
int idSoundSystemLocal::AsyncUpdate( int inTime ) {

	if ( !isInitialized || shutdown ) {
		return 0;
	}

	ulong dwCurrentWritePos;
	dword dwCurrentBlock;

	// here we do it in samples ( overflows in 27 hours or so )
	dwCurrentWritePos = idMath::Ftol( (float)Sys_Milliseconds() * 44.1f ) % ( MIXBUFFER_SAMPLES * ROOM_SLICES_IN_BUFFER );
	dwCurrentBlock = dwCurrentWritePos / MIXBUFFER_SAMPLES;

	if ( nextWriteBlock == 0xffffffff ) {
		nextWriteBlock = dwCurrentBlock;
	}

	if ( dwCurrentBlock != nextWriteBlock ) {
		return 0;
	}

	soundStats.runs++;
	soundStats.activeSounds = 0;

	int	numSpeakers = s_numberOfSpeakers.GetInteger();

	nextWriteBlock++;
	nextWriteBlock %= ROOM_SLICES_IN_BUFFER;

	int newPosition = nextWriteBlock * MIXBUFFER_SAMPLES;

	if ( newPosition < olddwCurrentWritePos ) {
		buffers++;					// buffer wrapped
	}

	// nextWriteSample is in multi-channel samples inside the buffer
	int	nextWriteSamples = nextWriteBlock * MIXBUFFER_SAMPLES;

	olddwCurrentWritePos = newPosition;

	// newSoundTime is in multi-channel samples since the sound system was started
	int newSoundTime = ( buffers * MIXBUFFER_SAMPLES * ROOM_SLICES_IN_BUFFER ) + nextWriteSamples;

	// check for impending overflow
	// FIXME: we don't handle sound wrap-around correctly yet
	if ( newSoundTime > 0x6fffffff ) {
		buffers = 0;
	}

	if ( (newSoundTime - CurrentSoundTime) > (int)MIXBUFFER_SAMPLES ) {
		soundStats.missedWindow++;
	}

	// enable audio hardware caching
	alcSuspendContext( openalContext );

	// let the active sound world mix all the channels in unless muted or avi demo recording
	if ( !muted && currentSoundWorld && !currentSoundWorld->fpa[0] ) {
		currentSoundWorld->MixLoop( newSoundTime, numSpeakers, finalMixBuffer, s_muteInBackground.GetBool());
	}

	// disable audio hardware caching (this updates ALL settings since last alcSuspendContext)
	alcProcessContext( openalContext );

	CurrentSoundTime = newSoundTime;

	soundStats.timeinprocess = Sys_Milliseconds() - inTime;

	return soundStats.timeinprocess;
}

/*
===================
idSoundSystemLocal::AsyncUpdateWrite
sound output using a write API. all the scheduling based on time
we mix MIXBUFFER_SAMPLES at a time, but we feed the audio device with smaller chunks (and more often)
called by the sound thread when com_asyncSound is 3 ( Linux )
===================
*/
int idSoundSystemLocal::AsyncUpdateWrite( int inTime ) {

	if ( !isInitialized || shutdown ) {
		return 0;
	}

	unsigned int dwCurrentBlock = (unsigned int)( inTime * 44.1f / MIXBUFFER_SAMPLES );

	if ( nextWriteBlock == 0xffffffff ) {
		nextWriteBlock = dwCurrentBlock;
	}

	if ( dwCurrentBlock < nextWriteBlock ) {
		return 0;
	}

	if ( nextWriteBlock != dwCurrentBlock ) {
		Sys_Printf( "missed %d sound updates\n", dwCurrentBlock - nextWriteBlock );
	}

	int sampleTime = dwCurrentBlock * MIXBUFFER_SAMPLES;
	int numSpeakers = s_numberOfSpeakers.GetInteger();

	// enable audio hardware caching
	alcSuspendContext( openalContext );

	// let the active sound world mix all the channels in unless muted or avi demo recording
	if ( !muted && currentSoundWorld && !currentSoundWorld->fpa[0] ) {
		currentSoundWorld->MixLoop( sampleTime, numSpeakers, finalMixBuffer, s_muteInBackground.GetBool());
	}

	// disable audio hardware caching (this updates ALL settings since last alcSuspendContext)
	alcProcessContext( openalContext );

	// only move to the next block if the write was successful
	nextWriteBlock = dwCurrentBlock + 1;
	CurrentSoundTime = sampleTime;

	return Sys_Milliseconds() - inTime;
}

/*
===================
idSoundSystemLocal::dB2Scale
===================
*/
float idSoundSystemLocal::dB2Scale( const float val ) const {
	if ( val == 0.0f ) {
		return 1.0f;				// most common
	} else if ( val <= -60.0f ) {
		return 0.0f;
	} else if ( val >= 60.0f ) {
		return powf( 2.0f, val * ( 1.0f / 6.0f ) );
	}
	int ival = (int)( ( val + 60.0f ) * 10.0f );
	return volumesDB[ival];
}

/*
===================
idSoundSystemLocal::ImageForTime
===================
*/
cinData_t idSoundSystemLocal::ImageForTime( const int milliseconds, const bool waveform ) {
	cinData_t ret;
	int i, j;

	if ( !isInitialized ) {
		memset( &ret, 0, sizeof( ret ) );
		return ret;
	}

	Sys_EnterCriticalSection();

	if ( !graph ) {
		graph = (dword *)Mem_Alloc( 256*128 * 4);
	}
	memset( graph, 0, 256*128 * 4 );
	float *accum = finalMixBuffer;	// unfortunately, these are already clamped
	int time = Sys_Milliseconds();

	int numSpeakers = s_numberOfSpeakers.GetInteger();

	if ( !waveform ) {
		for( j = 0; j < numSpeakers; j++ ) {
			int meter = 0;
			for( i = 0; i < MIXBUFFER_SAMPLES; i++ ) {
				float result = idMath::Fabs(accum[i*numSpeakers+j]);
				if ( result > meter ) {
					meter = result;
				}
			}

			meter /= 256;		// 32768 becomes 128
			if ( meter > 128 ) {
				meter = 128;
			}
			int offset;
			int xsize;
			if ( numSpeakers == 6 ) {
				offset = j * 40;
				xsize = 20;
			} else {
				offset = j * 128;
				xsize = 63;
			}
			int x,y;
			dword color = 0xff00ff00;
			for ( y = 0; y < 128; y++ ) {
				for ( x = 0; x < xsize; x++ ) {
					graph[(127-y)*256 + offset + x ] = color;
				}
#if 0
				if ( y == 80 ) {
					color = 0xff00ffff;
				} else if ( y == 112 ) {
					color = 0xff0000ff;
				}
#endif
				if ( y > meter ) {
					break;
				}
			}

			if ( meter > meterTops[j] ) {
				meterTops[j] = meter;
				meterTopsTime[j] = time + s_meterTopTime.GetInteger();
			} else if ( time > meterTopsTime[j] && meterTops[j] > 0 ) {
				meterTops[j]--;
				if (meterTops[j]) {
					meterTops[j]--;
				}
			}
		}

		for( j = 0; j < numSpeakers; j++ ) {
			int meter = meterTops[j];

			int offset;
			int xsize;
			if ( numSpeakers == 6 ) {
				offset = j*40;
				xsize = 20;
			} else {
				offset = j*128;
				xsize = 63;
			}
			int x,y;
			dword color;
			if ( meter <= 80 ) {
				color = 0xff007f00;
			} else if ( meter <= 112 ) {
				color = 0xff007f7f;
			} else {
				color = 0xff00007f;
			}
			for ( y = meter; y < 128 && y < meter + 4; y++ ) {
				for ( x = 0; x < xsize; x++ ) {
					graph[(127-y)*256 + offset + x ] = color;
				}
			}
		}
	} else {
		dword colors[] = { 0xff007f00, 0xff007f7f, 0xff00007f, 0xff00ff00, 0xff00ffff, 0xff0000ff };

		for( j = 0; j < numSpeakers; j++ ) {
			int xx = 0;
			float fmeter;
			int step = MIXBUFFER_SAMPLES / 256;
			for( i = 0; i < MIXBUFFER_SAMPLES; i += step ) {
				fmeter = 0.0f;
				for( int x = 0; x < step; x++ ) {
					float result = accum[(i+x)*numSpeakers+j];
					result = result / 32768.0f;
					fmeter += result;
				}
				fmeter /= 4.0f;
				if ( fmeter < -1.0f ) {
					fmeter = -1.0f;
				} else if ( fmeter > 1.0f ) {
					fmeter = 1.0f;
				}
				int meter = (fmeter * 63.0f);
				graph[ (meter + 64) * 256 + xx ] = colors[j];

				if ( meter < 0 ) {
					meter = -meter;
				}
				if ( meter > meterTops[xx] ) {
					meterTops[xx] = meter;
					meterTopsTime[xx] = time + 100;
				} else if ( time>meterTopsTime[xx] && meterTops[xx] > 0 ) {
					meterTops[xx]--;
					if ( meterTops[xx] ) {
						meterTops[xx]--;
					}
				}
				xx++;
			}
		}
		for( i = 0; i < 256; i++ ) {
			int meter = meterTops[i];
			for ( int y = -meter; y < meter; y++ ) {
				graph[ (y+64)*256 + i ] = colors[j];
			}
		}
	}
	ret.imageHeight = 128;
	ret.imageWidth = 256;
	ret.image = (unsigned char *)graph;

	Sys_LeaveCriticalSection();

	return ret;
}

/*
===================
idSoundSystemLocal::GetSoundDecoderInfo
===================
*/
int idSoundSystemLocal::GetSoundDecoderInfo( int index, soundDecoderInfo_t &decoderInfo ) {
	int i, j, firstEmitter, firstChannel;
	idSoundWorldLocal *sw = soundSystemLocal.currentSoundWorld;

	if ( index < 0 ) {
		firstEmitter = 0;
		firstChannel = 0;
	} else {
		firstEmitter = index / SOUND_MAX_CHANNELS;
		firstChannel = index - firstEmitter * SOUND_MAX_CHANNELS + 1;
	}

	for ( i = firstEmitter; i < sw->emitters.Num(); i++ ) {
		idSoundEmitterLocal *sound = sw->emitters[i];

		if ( !sound ) {
			continue;
		}

		// run through all the channels
		for ( j = firstChannel; j < SOUND_MAX_CHANNELS; j++ ) {
			idSoundChannel	*chan = &sound->channels[j];

			if ( chan->decoder == NULL ) {
				continue;
			}

			idSoundSample *sample = chan->decoder->GetSample();

			if ( sample == NULL ) {
				continue;
			}

			decoderInfo.name = sample->name;
			decoderInfo.format = ( sample->objectInfo.wFormatTag == WAVE_FORMAT_TAG_OGG ) ? "OGG" : "WAV";
			decoderInfo.numChannels = sample->objectInfo.nChannels;
			decoderInfo.numSamplesPerSecond = sample->objectInfo.nSamplesPerSec;
			decoderInfo.num44kHzSamples = sample->LengthIn44kHzSamples();
			decoderInfo.numBytes = sample->objectMemSize;
			decoderInfo.looping = ( chan->parms.soundShaderFlags & SSF_LOOPING ) != 0;
			decoderInfo.lastVolume = chan->lastVolume;
			decoderInfo.start44kHzTime = chan->trigger44kHzTime;
			decoderInfo.current44kHzTime = soundSystemLocal.GetCurrent44kHzTime();

			return ( i * SOUND_MAX_CHANNELS + j );
		}

		firstChannel = 0;
	}
	return -1;
}

/*
===================
idSoundSystemLocal::AllocSoundWorld
===================
*/
idSoundWorld *idSoundSystemLocal::AllocSoundWorld( idRenderWorld *rw ) {
	idSoundWorldLocal	*local = new idSoundWorldLocal;

	local->Init( rw );

	return local;
}

/*
===================
idSoundSystemLocal::SetMute
===================
*/
void idSoundSystemLocal::SetMute( bool muteOn ) {
	muted = muteOn;
}

/*
===================
idSoundSystemLocal::SamplesToMilliseconds
===================
*/
int idSoundSystemLocal::SamplesToMilliseconds( int samples ) const {
	return ( samples / (PRIMARYFREQ/1000) );
}

/*
===================
idSoundSystemLocal::SamplesToMilliseconds
===================
*/
int idSoundSystemLocal::MillisecondsToSamples( int ms ) const {
	return ( ms * (PRIMARYFREQ/1000) );
}

/*
===================
idSoundSystemLocal::SetPlayingSoundWorld

specifying NULL will cause silence to be played
===================
*/
void idSoundSystemLocal::SetPlayingSoundWorld( idSoundWorld *soundWorld ) {
	currentSoundWorld = static_cast<idSoundWorldLocal *>(soundWorld);
}

/*
===================
idSoundSystemLocal::GetPlayingSoundWorld
===================
*/
idSoundWorld *idSoundSystemLocal::GetPlayingSoundWorld( void ) {
	return currentSoundWorld;
}

/*
===================
idSoundSystemLocal::BeginLevelLoad
===================
*/

void idSoundSystemLocal::BeginLevelLoad() {
	if ( !isInitialized ) {
		return;
	}
	soundCache->BeginLevelLoad();

	if ( efxloaded ) {
		EFXDatabase.Clear();
		efxloaded = false;
	}
}

/*
===================
idSoundSystemLocal::EndLevelLoad
===================
*/
void idSoundSystemLocal::EndLevelLoad( const char *mapstring ) {
	if ( !isInitialized ) {
		return;
	}
	soundCache->EndLevelLoad();

	if (!useEFXReverb)
		return;

	//bc FIRST LOAD THE .MAP SPECIFIC EFX FILE.

	idStr efxname( "efxs/" );
	idStr mapname( mapstring );

	mapname.SetFileExtension( ".efx" );
	mapname.StripPath();
	efxname += mapname;

	efxloaded = EFXDatabase.LoadFile( efxname );

	if (efxloaded) {
		common->Printf("sound: found custom .efx '%s'\n", efxname.c_str());
	}
	else {
		common->Printf("sound: no custom .efx '%s'\n", efxname.c_str());
	}

	//if (!efxloaded)
	{
		efxname = "efxs/room_templates.efx";
		efxloaded = EFXDatabase.LoadFile(efxname);
	}

	if ( efxloaded ) {
		common->Printf("sound: found .efx '%s'\n", efxname.c_str() );
	} else {
		common->Printf("sound: missing .efx '%s'\n", efxname.c_str() );
	}
}

/*
===================
idSoundSystemLocal::AllocOpenALSource
===================
*/
ALuint idSoundSystemLocal::AllocOpenALSource( idSoundChannel *chan, bool looping, bool stereo ) {
	int timeOldestZeroVolSingleShot = Sys_Milliseconds();
	int timeOldestZeroVolLooping = Sys_Milliseconds();
	int timeOldestSingle = Sys_Milliseconds();
	int iOldestZeroVolSingleShot = -1;
	int iOldestZeroVolLooping = -1;
	int iOldestSingle = -1;
	int iUnused = -1;
	int index = -1;
	ALsizei i;

	// Grab current msec time
	int time = Sys_Milliseconds();

	// Cycle through all sources
	for ( i = 0; i < openalSourceCount; i++ ) {
		// Use any unused source first,
		// Then find oldest single shot quiet source,
		// Then find oldest looping quiet source and
		// Lastly find oldest single shot non quiet source..
		if ( !openalSources[i].inUse ) {
			iUnused = i;
			break;
		}  else if ( !openalSources[i].looping && openalSources[i].chan->lastVolume < SND_EPSILON ) {
			if ( openalSources[i].startTime < timeOldestZeroVolSingleShot ) {
				timeOldestZeroVolSingleShot = openalSources[i].startTime;
				iOldestZeroVolSingleShot = i;
			}
		} else if ( openalSources[i].looping && openalSources[i].chan->lastVolume < SND_EPSILON ) {
			if ( openalSources[i].startTime < timeOldestZeroVolLooping ) {
				timeOldestZeroVolLooping = openalSources[i].startTime;
				iOldestZeroVolLooping = i;
			}
		} else if ( !openalSources[i].looping ) {
			if ( openalSources[i].startTime < timeOldestSingle ) {
				timeOldestSingle = openalSources[i].startTime;
				iOldestSingle = i;
			}
		}
	}

	if ( iUnused != -1 ) {
		index = iUnused;
	} else if ( iOldestZeroVolSingleShot != - 1 ) {
		index = iOldestZeroVolSingleShot;
	} else if ( iOldestZeroVolLooping != -1 ) {
		index = iOldestZeroVolLooping;
	} else if ( iOldestSingle != -1 ) {
		index = iOldestSingle;
	}

	if ( index != -1 ) {
		// stop the channel that is being ripped off
		if ( openalSources[index].chan ) {
			// stop the channel only when not looping
			if ( !openalSources[index].looping ) {
				openalSources[index].chan->Stop();
			} else {
				openalSources[index].chan->triggered = true;
			}

			// Free hardware resources
			openalSources[index].chan->ALStop();
		}

		// Initialize structure
		openalSources[index].startTime = time;
		openalSources[index].chan = chan;
		openalSources[index].inUse = true;
		openalSources[index].looping = looping;
		openalSources[index].stereo = stereo;

		return openalSources[index].handle;
	} else {
		return 0;
	}
}

/*
===================
idSoundSystemLocal::FreeOpenALSource
===================
*/
void idSoundSystemLocal::FreeOpenALSource( ALuint handle ) {
	ALsizei i;
	for ( i = 0; i < openalSourceCount; i++ ) {
		if ( openalSources[i].handle == handle ) {
			if ( openalSources[i].chan ) {
				openalSources[i].chan->openalSource = 0;
			}

			// Initialize structure
			openalSources[i].startTime = 0;
			openalSources[i].chan = NULL;
			openalSources[i].inUse = false;
			openalSources[i].looping = false;
			openalSources[i].stereo = false;
		}
	}
}

/*
============================================================
SoundFX and misc effects
============================================================
*/

/*
===================
idSoundSystemLocal::ProcessSample
===================
*/
void SoundFX_Lowpass::ProcessSample( float* in, float* out ) {
	float c, a1, a2, a3, b1, b2;
	float resonance = idSoundSystemLocal::s_enviroSuitCutoffQ.GetFloat();
	float cutoffFrequency = idSoundSystemLocal::s_enviroSuitCutoffFreq.GetFloat();

	Initialize();

	c = 1.0 / idMath::Tan16( idMath::PI * cutoffFrequency / 44100 );

	// compute coefs
	a1 = 1.0 / ( 1.0 + resonance * c + c * c );
	a2 = 2* a1;
	a3 = a1;
	b1 = 2.0 * ( 1.0 - c * c) * a1;
	b2 = ( 1.0 - resonance * c + c * c ) * a1;

	// compute output value
	out[0] = a1 * in[0] + a2 * in[-1] + a3 * in[-2] - b1 * out[-1] - b2 * out[-2];
}

void SoundFX_LowpassFast::ProcessSample( float* in, float* out ) {
	// compute output value
	out[0] = a1 * in[0] + a2 * in[-1] + a3 * in[-2] - b1 * out[-1] - b2 * out[-2];
}

void SoundFX_LowpassFast::SetParms( float p1, float p2, float p3 ) {
	float c;

	// set the vars
	freq = p1;
	res = p2;

	// precompute the coefs
	c = 1.0 / idMath::Tan( idMath::PI * freq / 44100 );

	// compute coefs
	a1 = 1.0 / ( 1.0 + res * c + c * c );
	a2 = 2* a1;
	a3 = a1;

	b1 = 2.0 * ( 1.0 - c * c) * a1;
	b2 = ( 1.0 - res * c + c * c ) * a1;
}

void SoundFX_Comb::Initialize() {
	if ( initialized )
		return;

	initialized = true;
	maxlen = 50000;
	buffer = new float[maxlen];
	currentTime = 0;
}

void SoundFX_Comb::ProcessSample( float* in, float* out ) {
	float gain = idSoundSystemLocal::s_reverbFeedback.GetFloat();
	int len = idSoundSystemLocal::s_reverbTime.GetFloat() + param;

	Initialize();

	// sum up and output
	out[0] = buffer[currentTime];
	buffer[currentTime] = buffer[currentTime] * gain + in[0];

	// increment current time
	currentTime++;
	if ( currentTime >= len )
		currentTime -= len;
}

/*
===================
idSoundSystemLocal::DoEnviroSuit
===================
*/
void idSoundSystemLocal::DoEnviroSuit( float* samples, int numSamples, int numSpeakers ) {
	float out[10000], *out_p = out + 2;
	float in[10000], *in_p = in + 2;


	assert( false );

	if ( !fxList.Num() ) {
		for ( int i = 0; i < 6; i++ ) {
			SoundFX* fx;

			// lowpass filter
			fx = new SoundFX_Lowpass();
			fx->SetChannel( i );
			fxList.Append( fx );

			// comb
			fx = new SoundFX_Comb();
			fx->SetChannel( i );
			fx->SetParameter( i * 100 );
			fxList.Append( fx );

			// comb
			fx = new SoundFX_Comb();
			fx->SetChannel( i );
			fx->SetParameter( i * 100 + 5 );
			fxList.Append( fx );
		}
	}

	for ( int i = 0; i < numSpeakers; i++ ) {
		int j;

		// restore previous samples
		memset( in, 0, 10000 * sizeof( float ) );
		memset( out, 0, 10000 * sizeof( float ) );

		// fx loop
		for ( int k = 0; k < fxList.Num(); k++ ) {
			SoundFX* fx = fxList[k];

			// skip if we're not the right channel
			if ( fx->GetChannel() != i )
				continue;

			// get samples and continuity
			fx->GetContinuitySamples( in_p[-1], in_p[-2], out_p[-1], out_p[-2] );
			for ( j = 0; j < numSamples; j++ ) {
				in_p[j] = samples[j * numSpeakers + i] * s_enviroSuitVolumeScale.GetFloat();
			}

			// process fx loop
			for ( j = 0; j < numSamples; j++ ) {
				fx->ProcessSample( in_p + j, out_p + j );
			}

			// store samples and continuity
			fx->SetContinuitySamples( in_p[numSamples-2], in_p[numSamples-3], out_p[numSamples-2], out_p[numSamples-3] );

			for ( j = 0; j < numSamples; j++ ) {
				samples[j * numSpeakers + i] = out_p[j];
			}
		}
	}
}

/*
=================
idSoundSystemLocal::PrintMemInfo
=================
*/
void idSoundSystemLocal::PrintMemInfo( MemInfo_t *mi ) {
	soundCache->PrintMemInfo( mi );
}

/*
===============
idSoundSystemLocal::IsEFXAvailable
===============
*/
int idSoundSystemLocal::IsEFXAvailable( void ) {
#if defined(ID_DEDICATED)
	return -1;
#else
	return EFXAvailable;
#endif
}
