#pragma once

#include "Misc.h"
#include "Entity.h"


class idLifeboatScripted : public idEntity
{
public:
	CLASS_PROTOTYPE(idLifeboatScripted);

							idLifeboatScripted(void);
	virtual					~idLifeboatScripted(void);

	void					Spawn(void);

	void					Save( idSaveGame *savefile ) const;
	void					Restore( idRestoreGame *savefile );

	virtual void			Think(void);
	void					Event_Activate(idEntity* activator);

	void					UpdateWaitingForPlayer();

private:

	virtual void			Event_PostSpawn(void);

	int						state;
	enum					{LFS_NONE, LFS_WAITINGFORPLAYER, LFS_ACTIVE};

	int						stateTimer;
	int						lookCounter;


	bool					IsInPlayerLOS(idVec3 _pos);

	void					LaunchPod();
	
	idEntity*				beamEnd;
	idEntity*				beamStart;
	

	

};
//#pragma once