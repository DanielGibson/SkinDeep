#include "Misc.h"
#include "Target.h"

class idBeaconLogic : public idEntity
{
public:
	CLASS_PROTOTYPE(idBeaconLogic);

							idBeaconLogic();
							~idBeaconLogic();
	void					Spawn();

	virtual void			Think(void);

	void					FlashConfirm();

	bool					validLandingPosition;
	idEntity				*placerEnt;

	bool					HasLockedOn();

	void					SetPodType(int _podType);

private:
	
	idBeam*					lasersightbeam;
	idBeam*					lasersightbeamTarget;

	

	enum                    { BEACONLOGIC_DELAY, BEACONLOGIC_ACTIVE, BEACONLOGIC_FASTBLINK, BEACONLOGIC_LOCKON, BEACONLOGIC_DORMANT };
	int						state;
	int						stateTimer;

	void					LaunchPod(idVec3 destinationPosition);

	void					DeployConfirmation(bool force);

	idEntity				*tubeEnt;

	idVec3					FindAlternateLaunchPosition(idVec3 destinationPoint);
	idVec3					podLaunchPosition;
	idVec3					podLaunch_lerpStart;
	idVec3					podLaunch_lerpEnd;
	bool					podLaunch_isLerping;
	int						podLaunch_lerpTimer;

	void					DestroySelf();
	idVec3					lastAimDir;
	idVec3					lastEyePosition;

	int						podType;

};
