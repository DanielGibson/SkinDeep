#include "Trigger.h"

class idTrigger_gascloud : public idTrigger_Multi
{
public:
	CLASS_PROTOTYPE(idTrigger_gascloud);

	idTrigger_gascloud();
	void				Spawn();

	virtual void		Think(void);

private:
	void				Event_Touch(idEntity* other, trace_t* trace);
	int					lastUpdatetime;	

	idFuncEmitter		*particleEmitter;

	int					maxlifetime;

	void				Despawn();

	bool				active;
	bool				touchedBySomeone;
};
