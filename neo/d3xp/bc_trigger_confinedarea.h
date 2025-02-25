#include "Trigger.h"

class idTrigger_confinedarea : public idTrigger_Multi
{
public:
	CLASS_PROTOTYPE(idTrigger_confinedarea);

						idTrigger_confinedarea();
	virtual				~idTrigger_confinedarea(void);
	void				Spawn();

	float				adjustedBaseAngle;
	float				baseAngle;
	float				playerEnterAngle;

private:
	void				Event_Touch(idEntity* other, trace_t* trace);
	int					lastUpdatetime;
	
};
