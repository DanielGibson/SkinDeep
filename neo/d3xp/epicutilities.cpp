
//Epic Game Store functionality

#pragma hdrstop

#include "epicutilities.h"

#ifdef EPICSTORE

//Try to initialize steam.
bool CEpicUtilities::InitializeSteam()
{
	return false;
}

//Returns whether steam has successfully initialized.
bool CEpicUtilities::IsSteamInitialized()
{
	return false;
}

//Call this when shutting down the game.
void CEpicUtilities::SteamShutdown()
{
	
}


bool CEpicUtilities::IsOnSteamDeck()
{
	return false;
}

bool CEpicUtilities::IsInBigPictureMode()
{
	return false;
}

//This should be called every frame for steam callbacks.
void CEpicUtilities::RunCallbacks()
{

}

//Save savegame file to steam cloud.
bool CEpicUtilities::SteamCloudSave(const char* savefile, const char* descFile)
{
#ifndef DEMO
	return false;
#else
	//if it's the demo build, don't do the cloud save.
	return false;
#endif // DEMO
}

bool CEpicUtilities::SteamCloudSaveConfig(const char* configFileName)
{
#ifndef DEMO
	return false;
#else
	//if it's demo, don't do cloud save.
	return false;
#endif // DEMO
}

//Delete a file from the steam cloud.
bool CEpicUtilities::SteamCloudDeleteFile(const char* filename)
{
	return false;
}

bool CEpicUtilities::SteamCloudLoad()
{
#ifndef DEMO
	return false;
#else
	//if it's demo, don't do cloud load.
	return false;
#endif // DEMO
}

//Opens a steam overlay page URL.
void CEpicUtilities::OpenSteamOverlaypage(const char* pageURL)
{

}

//Open store page of current game.
void CEpicUtilities::OpenSteamOverlaypageStore()
{

}

//Set a Steam achievement.
bool CEpicUtilities::SetAchievement(const char *achievementName)
{
	return false;
}

void CEpicUtilities::ResetAchievements()
{

}

int CEpicUtilities::GetSteamWorkshopAmount()
{
	return 0;
}

const char* CEpicUtilities::GetWorkshopPathAtIndex(int index)
{
	return "";
}

//Note: This returns the language set up in the Steam game properties (NOT the Steam interface language).
//In Steam, this is accessed via right-click game > Properties > General > Language
//In the Steam backend, the languages are set in Steampipe > Depots > Managing Base Languages
const char* CEpicUtilities::GetSteamLanguage()
{
	return "english";
}

idStr CEpicUtilities::GetSteamID()
{
	return "";
}

//NOTE: this requires localized text in the steam backend. The text should be "#nameOfLocString"
void CEpicUtilities::SetSteamRichPresence(const char* text)
{

}

void CEpicUtilities::SetSteamTimelineEvent(const char* icon)
{

}

#endif
