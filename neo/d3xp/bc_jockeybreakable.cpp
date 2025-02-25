//#include "sys/platform.h"
//#include "Entity.h"

#include "framework/DeclEntityDef.h"
#include "Fx.h"

#include "Item.h"
#include "bc_frobcube.h"
#include "bc_jockeybreakable.h"


CLASS_DECLARATION(idStaticEntity, idJockeyBreakable)
END_CLASS

idJockeyBreakable::idJockeyBreakable(void)
{
}

idJockeyBreakable::~idJockeyBreakable(void)
{
}

void idJockeyBreakable::Spawn(void)
{
	GetPhysics()->SetContents(CONTENTS_SOLID);
	GetPhysics()->SetClipMask(MASK_SOLID);

	fl.takedamage = true;
}

void idJockeyBreakable::Save(idSaveGame *savefile) const
{
}

void idJockeyBreakable::Restore(idRestoreGame *savefile)
{
}

//void idJockeyBreakable::Think(void)
//{	
//	idStaticEntity::Think();
//}

//void idJockeyBreakable::Damage(idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location, const int materialType)
//{
//}

void idJockeyBreakable::Killed(idEntity *inflictor, idEntity *attacker, int damage, const idVec3 &dir, int location)
{
	if (!fl.takedamage)
		return;

	fl.takedamage = false;

	idStr brokenSkin = spawnArgs.GetString("skin_broken");
	if (brokenSkin.Length() > 0)
	{
		SetSkin(declManager->FindSkin(brokenSkin.c_str()));
	}

	idStr scriptName = spawnArgs.GetString("call_killed");
	if (scriptName.Length() > 0)
	{
		idEntity *caller;
		if (inflictor != NULL)
			caller = inflictor;
		else
			caller = attacker;

		gameLocal.RunMapScriptArgs(scriptName.c_str(), caller, this);
	}
}