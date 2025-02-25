#include "trigger.h"

class idTrigger_sneeze : public idTrigger_Multi
{
public:
	CLASS_PROTOTYPE(idTrigger_sneeze);

	idTrigger_sneeze();
	void				Spawn();

	virtual void		Think(void);

private:
	void				Event_Touch(idEntity* other, trace_t* trace);
	int					lastUpdatetime;
	float				multiplier;

	idFuncEmitter		*particleEmitter;

	int					maxlifetime;

	void				Despawn();

	bool				active;
	bool				touchedByAI;
};