#include "core/manager.h"
#include "core/dispatcher.h"
#include "systems/writing/system.h"
#include "systems/filesystem/system.h"

// +===----- Functions -----===+ //

t_Manager	*manager_init(void)
{
	t_Manager		*manager;
	size_t			_size;

	manager = malloc(sizeof(t_Manager));
	TEST_NULL(manager, NULL);
	_size = WRITING_COMMANDS_COUNT + FS_COMMANDS_COUNT;
	if (false == dispatcher_init(manager, _size))
		return (manager_clean(manager), NULL);
	if (false == writing_init(manager))
		return (manager_clean(manager), NULL);
	if (false == fs_init(manager))
		return (manager_clean(manager), NULL);
	return (manager);
}

void		manager_clean(t_Manager *manager)
{
	if (NULL == manager)
		return ;

	dispatcher_clean(manager->dispatcher);
	writing_clean(manager->writing_ctx);
	fs_clean(manager->fs_ctx);
	free(manager);
}

t_ErrorCode	manager_exec(t_Manager *manager, t_Command *cmd)
{
	TEST_NULL(manager, ERR_INVALID_MANAGER);
	TEST_NULL(cmd, ERR_INVALID_COMMAND);
	return (dispatcher_exec(manager, cmd));
}
