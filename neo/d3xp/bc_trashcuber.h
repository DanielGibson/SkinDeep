#include "Misc.h"
#include "Entity.h"
#include "Mover.h"

class idTrashcuber : public idMover
{
public:
	CLASS_PROTOTYPE(idTrashcuber);

							idTrashcuber(void);
	virtual					~idTrashcuber(void);

	void					Save(idSaveGame *savefile) const;
	void					Restore(idRestoreGame *savefile);

	void					Spawn(void);

	virtual void			Think(void);

private:

	idLight *				interiorLight;
	idLight *				exteriorLights[4];
	idLight *				spotlight;

	int						stateTimer;
	int						cuberState;
	enum					{ CUBER_IDLE, CUBER_MOVING, CUBER_CHARGEDELAY, CUBER_CHARGINGUP, CUBER_SLAMMINGDOWN, CUBER_MAKINGCUBE, CUBER_RISINGUP };

	int						lastZPosition;
	void					UpdateSpotlightSize();

	
	const char *			targetAirlock;
	void					SpawnTrashcube();
	idVec3					FindValidPosition();
};
