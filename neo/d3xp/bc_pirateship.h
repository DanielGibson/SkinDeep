#include "Misc.h"
#include "Entity.h"
#include "Mover.h"


class idPirateship : public idMover
{
public:
	CLASS_PROTOTYPE(idPirateship);

							idPirateship(void);
	virtual					~idPirateship(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);
	virtual void			Damage(idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location, const int materialType = SURFTYPE_NONE);

    bool                    IsDormant();
    void                    StartEntranceSequence();

    virtual bool			DoFrob(int index = 0, idEntity * frobber = NULL);

	void					DebugFastForward();

private:


	virtual void			Event_PostSpawn(void);
	void					DestroyAccumulatorsInRoom(idLocationEntity *locationEnt);


	idFuncEmitter			*boosterEmitter[2];

	idBeam*					wireOrigin;
	idBeam*					wireTarget;

    int                     state;
    enum                   
    {
        PSH_DORMANT,
        PSH_SPLINEMOVING,
        PSH_DOCKROTATING,
        PSH_DOCKMOVING,
        PSH_DOORBREACHDELAY,
        PSH_SMOKEGRENADE_DELAY,
        PSH_SMOKEGRENADE_LAUNCHING,
		PSH_SPAWNDELAY,
		PSH_SPAWNFISH,
        PSH_SPAWNING,
		PSH_DONESPAWNING,
		PSH_REINFORCEMENTMESSAGEDONE
    };

    int                     moveStartTime;

    idEntity               *GetSplinePath();

    idEntityPtr<idEntity>   airlockEnt;
    idEntity                *FindMyAirlock();

    idEntity *              openFrobcube;

    int                     smokegrenadeCounter;
    int                     smokegrenadeTimer;
    void                    LaunchSmokeGrenade();

	int						spawnIndex;
	int						spawnIntervalTimer;
	idList<int>				spawnList;
	bool					SpawnPersonInAirlockViaEntNum(int idx);

	const idDeclParticle *	smoketrail;
	int						smoketrailFlyTime;

	idEntity				*ftlPortal;
	int						portalFadeTimer;
	bool					portalActive;

	void					UpdateCountdownText();
	void					SetCountdownStr(idStr text);
	int						countdownValue;
	int						countdownStartTime;

	int						keylocationTimer;
	void					UpdateKeyLocation();
	idEntity				*dynatipEnt;


	idVec3					GetAirlockClearPosition();

	
	
};
//#pragma once