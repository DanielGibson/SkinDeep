#include "Misc.h"
#include "target.h"

class idVacuumSpline : public idEntity
{
public:
	CLASS_PROTOTYPE(idVacuumSpline);

							idVacuumSpline();
							~idVacuumSpline();
	void					Spawn();

	virtual void			Think(void);


private:
	

	enum                    { VS_VACUUMING, VS_DONE };
	int						state;
	
	idEntityPtr<idEntity>	targetActor;

	idCurve_Spline<idVec3> *spline;

	idEntity				*mover;

	int						startTime;
	idEntity				*splineEnt;

	idVec3					flingDirection;

	void					Detach();

	bool					is_player;
};