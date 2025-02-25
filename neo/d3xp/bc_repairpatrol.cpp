#include "sys/platform.h"
#include "gamesys/SysCvar.h"
#include "Entity.h"

#include "bc_repairpatrol.h"

CLASS_DECLARATION(idEntity, idRepairPatrolNode)
END_CLASS

idRepairPatrolNode::idRepairPatrolNode(void)
{
	
}

idRepairPatrolNode::~idRepairPatrolNode(void)
{
	
}

void idRepairPatrolNode::Save(idSaveGame *savefile) const
{
}

void idRepairPatrolNode::Restore(idRestoreGame *savefile)
{
}

void idRepairPatrolNode::Spawn(void)
{
	gameLocal.repairpatrolEntities.Append(this);
}
