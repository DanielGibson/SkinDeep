#include "trigger.h"

class idTrigger_healcloud : public idTrigger_Multi
{
public:
	CLASS_PROTOTYPE(idTrigger_healcloud);

						idTrigger_healcloud();
	virtual				~idTrigger_healcloud(void);
	
	void				Spawn();

	//virtual void		Damage(idEntity *inflictor, idEntity *attacker, const idVec3 &dir, const char *damageDefName, const float damageScale, const int location, const int materialType = SURFTYPE_NONE);

	virtual void		Think(void);
	

private:

	bool				active;

	void				Event_Touch(idEntity* other, trace_t* trace);

	idFuncEmitter		*particleEmitter;

	int					maxlifetime;
};