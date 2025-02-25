#include "sys/platform.h"
#include "gamesys/SysCvar.h"
//#include "physics/Physics_RigidBody.h"
#include "Entity.h"
//#include "Light.h"
#include "Player.h"
#include "trigger.h"
#include "framework/DeclEntityDef.h"


#include "bc_meta.h"
#include "bc_yarnboard.h"

#define LOOKTIME 300
#define PAUSETIME 300
#define FOV_AMOUNT -20

#define LOCBOXRADIUS 1.5

const idEventDef EV_SetYarnInfo("setyarninfo", "ssvs");


//Haiku for the Yarn Board

//This is the yarn board
//a lotta this is hard coded
//oh well, video game


CLASS_DECLARATION(idStaticEntity, idYarnBoard)
	EVENT(EV_SetYarnInfo, idYarnBoard::SetYarnInfo)
END_CLASS


idYarnBoard::idYarnBoard(void)
{
	sparkleParticles = NULL;
	hasFrobbed = false;
	state = YB_IDLE;
	stateTimer = 0;
	voName = "";
}

idYarnBoard::~idYarnBoard(void)
{
}

void idYarnBoard::Spawn(void)
{
	//spawn soundwave particle.
	idAngles particleAng = GetPhysics()->GetAxis().ToAngles();
	particleAng.pitch += 90;
	
	idDict args;	
	args.Clear();
	args.Set("model", spawnArgs.GetString("model_particle"));
	args.SetBool("start_off", true);
	sparkleParticles = static_cast<idFuncEmitter *>(gameLocal.SpawnEntityType(idFuncEmitter::Type, &args));
	sparkleParticles->SetOrigin(GetPhysics()->GetOrigin());
	sparkleParticles->SetAngles(particleAng);
	sparkleParticles->Hide();

	isFrobbable = false;

	//Create the loc boxes.
	for (int i = 0; i < YARNLOCBOXCOUNT; i++)
	{
		idStr definitionName = GetLocboxDefinitionViaIndex(i);
		if (definitionName.Length() <= 0)
		{
			common->Error("yarnboard: failed to find definition for index '%d'", i);
			return;
		}

		const idDeclEntityDef* yarninfoDef = gameLocal.FindEntityDef(definitionName.c_str());
		if (!yarninfoDef)
		{
			common->Error("yarnboard: couldn't find yarn definition '%s'\n", definitionName.c_str());
			return;
		}

		idStr yarnText = yarninfoDef->dict.GetString("text");
		idVec3 yarnPos = yarninfoDef->dict.GetVector("position");

		if (yarnText.Length() <= 0 || yarnPos == vec3_zero)
		{
			common->Error("yarnboard: text or position for '%s' is empty.", definitionName.c_str());
			return;
		}

		idVec3 right, up;
		GetPhysics()->GetAxis().ToAngles().ToVectors(NULL, &right, &up);
		idVec3 pos = GetPhysics()->GetOrigin() + (right * yarnPos.y) + (up * yarnPos.z);

		args.Clear();
		args.Set("text", yarnText.c_str());
		args.SetVector("origin", pos);
		args.SetBool("playerlook_trigger", true);		
		args.SetVector("mins", idVec3(-LOCBOXRADIUS, -LOCBOXRADIUS, -LOCBOXRADIUS));
		args.SetVector("maxs", idVec3(LOCBOXRADIUS, LOCBOXRADIUS, LOCBOXRADIUS));
		locboxes[i] = static_cast<idTrigger_Multi*>(gameLocal.SpawnEntityType(idTrigger_Multi::Type, &args));

		
	}
}

// hack,  hard code the definition names.
idStr idYarnBoard::GetLocboxDefinitionViaIndex(int index)
{
	switch (index)
	{
		case LB_ZENAMASKED:
			return "info_yarnboard_zenamasked";
		case LB_IDO:
			return "info_yarnboard_ido";
		case LB_ZENACLONE:
			return "info_yarnboard_zenaclone";
		case LB_NINA:
			return "info_yarnboard_nina";
		case LB_LEADSPIRATES:
			return "info_yarnboard_leadspirates";
		case LB_PIRATESKILL:
			return "info_yarnboard_pirateskill";
		case LB_SKULLSAVER:
			return "info_yarnboard_skullsaver";
		case LB_MADABANDON:
			return "info_yarnboard_madabandon";
		case LB_WHEREISSHE:
			return "info_yarnboard_whereisshe";
		case LB_BEASTBUG:
			return "info_yarnboard_beastbug";
		case LB_FINALMISSION:
			return "info_yarnboard_finalmission";
		case LB_NEWSPAPER:
			return "info_yarnboard_newspaper";
		case LB_LITTLELION:
			return "info_yarnboard_littlelion";
		case LB_PARTNERCRIME:
			return "info_yarnboard_partnercrime";
		case LB_RICHRUDE:
			return "info_yarnboard_richrude";
		case LB_LIKESZENAMORE:
			return "info_yarnboard_likeszenamore";
	}

	return "";
}


void idYarnBoard::Save(idSaveGame *savefile) const
{
}

void idYarnBoard::Restore(idRestoreGame *savefile)
{

}

bool idYarnBoard::DoFrob(int index, idEntity * frobber)
{
	//Player frobbed the yarnboard. Lerp the player to look at the  yarnboard...

	isFrobbable = false;

	StartSound("snd_look", SND_CHANNEL_ANY);

	idVec3 up, right;
	GetPhysics()->GetAxis().ToAngles().ToVectors(NULL, &right, &up);
	idVec3 lookPosition = GetPhysics()->GetOrigin() + (right * lookOffset.y) + (up * lookOffset.z);
	gameLocal.GetLocalPlayer()->SetViewLerp(lookPosition, LOOKTIME);
	
	sparkleParticles->SetActive(false); //deactivate particles.

	state = YB_LOOKLERP;
	stateTimer = gameLocal.time + LOOKTIME;

	gameLocal.GetLocalPlayer()->Event_setPlayerFrozen(1);
	gameLocal.GetLocalPlayer()->Event_SetFOVLerp(FOV_AMOUNT, LOOKTIME / 2);

	BecomeActive(TH_THINK);

	return true;
}

void idYarnBoard::Think(void)
{	
	if (state == YB_LOOKLERP && gameLocal.time > stateTimer)
	{
		//Player viewangle lerp is now done.
		//Pause on the yarnboard for a little bit...

		state = YB_SHORTPAUSE;
		stateTimer = gameLocal.time + PAUSETIME;
	}
	else if (state == YB_SHORTPAUSE && gameLocal.time > stateTimer)
	{
		//First pause is done.
		//Reveal the new items, and do another pause...

		state = YB_SHORTPAUSE2;

		StartSound("snd_reveal", SND_CHANNEL_ANY);
		SetModel(model_endName.c_str());

		SetLocboxVisibility(model_endName);

		stateTimer = gameLocal.time + PAUSETIME;


		if (voName.Length() > 0)
		{
			gameLocal.voManager.SayVO(gameLocal.GetLocalPlayer(), voName.c_str(), VO_CATEGORY_NARRATIVE);
		}
	}
	else if (state == YB_SHORTPAUSE2 && gameLocal.time > stateTimer)
	{
		state = YB_DONE;
		gameLocal.GetLocalPlayer()->Event_SetFOVLerp(0, LOOKTIME);
		gameLocal.GetLocalPlayer()->Event_setPlayerFrozen(0);
		BecomeInactive(TH_THINK);
	}

	idStaticEntity::Think();
}

//Gets called via script when map is loaded.
void idYarnBoard::SetYarnInfo(const char* startModel, const char* endModel, idVec3 _lookOffset, const char* voString )
{
	SetModel(startModel);
	model_endName = endModel;
	lookOffset = _lookOffset;

	//Yarn board active.
	sparkleParticles->Show();
	sparkleParticles->SetActive(true); //Display particle effects.
	isFrobbable = true;

	voName = voString;

	SetLocboxVisibility(startModel);
	

	BecomeActive(TH_THINK);
}

//hack, hard code the visibility of the locbox signage
void idYarnBoard::SetLocboxVisibility(idStr modelname)
{
	//first, make all locboxes hidden.
	for (int i = 0; i < YARNLOCBOXCOUNT; i++)
	{
		locboxes[i]->Hide();
	}

	if (idStr::FindText(modelname.c_str(), "phase1.ase") >= 0)
	{
		locboxes[LB_ZENAMASKED]->Show();
		locboxes[LB_LEADSPIRATES]->Show();
		locboxes[LB_PIRATESKILL]->Show();
		locboxes[LB_SKULLSAVER]->Show();
	}
	else if (idStr::FindText(modelname.c_str(), "phase4.ase") >= 0)
	{
		locboxes[LB_IDO]->Show();
		locboxes[LB_NINA]->Show();
		locboxes[LB_LEADSPIRATES]->Show();
		locboxes[LB_PIRATESKILL]->Show();
		locboxes[LB_SKULLSAVER]->Show();
	}
	else if (idStr::FindText(modelname.c_str(), "phase4b.ase") >= 0)
	{
		locboxes[LB_IDO]->Show();
		locboxes[LB_NINA]->Show();
		locboxes[LB_LEADSPIRATES]->Show();
		locboxes[LB_PIRATESKILL]->Show();
		locboxes[LB_SKULLSAVER]->Show();
		locboxes[LB_LITTLELION]->Show();
	}
	else if (idStr::FindText(modelname.c_str(), "phase7.ase") >= 0)
	{
		locboxes[LB_IDO]->Show();
		locboxes[LB_NINA]->Show();
		locboxes[LB_LEADSPIRATES]->Show();
		locboxes[LB_PIRATESKILL]->Show();
		locboxes[LB_SKULLSAVER]->Show();
		locboxes[LB_LITTLELION]->Show();
		locboxes[LB_PARTNERCRIME]->Show();
		locboxes[LB_RICHRUDE]->Show();
		locboxes[LB_FINALMISSION]->Show();
		locboxes[LB_NEWSPAPER]->Show();
	}
	else if (idStr::FindText(modelname.c_str(), "phase11.ase") >= 0)
	{
		locboxes[LB_ZENACLONE]->Show();
		locboxes[LB_NINA]->Show();
		locboxes[LB_LEADSPIRATES]->Show();
		locboxes[LB_PIRATESKILL]->Show();
		locboxes[LB_SKULLSAVER]->Show();
		locboxes[LB_LITTLELION]->Show();
		locboxes[LB_PARTNERCRIME]->Show();
		locboxes[LB_RICHRUDE]->Show();
		locboxes[LB_FINALMISSION]->Show();
		locboxes[LB_NEWSPAPER]->Show();
	}
	else if (idStr::FindText(modelname.c_str(), "yarnboard_master.ase") >= 0)
	{
		for (int i = 0; i < YARNLOCBOXCOUNT; i++)
		{
			if (i == LB_IDO || i == LB_ZENAMASKED)
				continue;

			locboxes[i]->Show();
		}
	}
}