#include "Misc.h"
#include "Entity.h"
#include "Mover.h"

class idDoorBarricade : public idStaticEntity
{
public:
	CLASS_PROTOTYPE( idDoorBarricade );

							idDoorBarricade(void);
	virtual					~idDoorBarricade(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);
	virtual void			Think(void);
	virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);

	idEntityPtr<idDoor>		owningDoor;

	virtual void			DoHack(); //for the hackgrenade.

private:

	void					UnlockBarricade();

	void					UnlockAllBarricades();

	int						keyrequirementTimer;
	bool					keyrequirementLightActive;

	bool					UpdateProximity();
	int						proximityTimer;
	bool					proximityActive;
	bool					DoProxCheckPlayer();
	bool					DoProxCheckGround();
	bool					DoProxCheckAI();

	idBeam*					beamOrigin;
	idBeam*					beamTarget;
	void					UpdateLaserPosition(idVec3 targetPos);

	bool					particleDone;

};
//#pragma once