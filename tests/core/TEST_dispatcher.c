#include "tools.h"
#include "core/manager.h"
#include "core/dispatcher.h"

static t_ErrorCode	handler_ok(t_Manager *manager, const t_Command *cmd)
{
	(void)manager;
	(void)cmd;
	return (ERR_SUCCESS);
}

static t_ErrorCode	handler_fail(t_Manager *manager, const t_Command *cmd)
{
	(void)manager;
	(void)cmd;
	return (ERR_OPERATION_FAILED);
}

static void	free_dispatcher_manager(t_Manager *manager)
{
	if (NULL == manager)
		return ;
	dispatcher_clean(manager->dispatcher);
	free(manager);
}

static int	test_dispatcher_init_and_register(void)
{
	t_Manager		*manager;
	t_Dispatcher	*dispatcher;

	print_section("DISPATCHER INIT / REGISTER");
	manager = calloc(1, sizeof(t_Manager));
	if (NULL == manager)
		return (print_error("Failed to allocate manager"), 1);
	if (false == dispatcher_init(manager, 2))
		return (free(manager), print_error("Failed to init dispatcher"), 1);
	dispatcher = manager->dispatcher;
	if (dispatcher->count != 0 || dispatcher->capacity != 2 || NULL == dispatcher->commands)
		return (free_dispatcher_manager(manager), print_error("Invalid initial dispatcher state"), 1);
	print_success("Dispatcher initialized with expected state");
	if (false == dispatcher_register(dispatcher, CMD_WRITING_CREATE_BUFFER, sizeof(t_CmdCreateBuffer), handler_ok))
		return (free_dispatcher_manager(manager), print_error("First register failed"), 1);
	if (false == dispatcher_register(dispatcher, CMD_WRITING_DELETE_BUFFER, sizeof(t_CmdDestroyBuffer), handler_ok))
		return (free_dispatcher_manager(manager), print_error("Second register failed"), 1);
	print_success("Registered 2 commands");
	if (true == dispatcher_register(dispatcher, CMD_WRITING_GET_LINE, sizeof(t_CmdGetLine), handler_ok))
		return (free_dispatcher_manager(manager), print_error("Register should fail when full"), 1);
	print_success("Registration rejected when capacity is full");
	if (true == dispatcher_register(dispatcher, CMD_WRITING_GET_LINE, sizeof(t_CmdGetLine), NULL))
		return (free_dispatcher_manager(manager), print_error("Register should reject NULL handler"), 1);
	print_success("Registration rejected NULL handler");
	free_dispatcher_manager(manager);
	return (0);
}

static int	test_dispatcher_exec_paths(void)
{
	t_Manager	*manager;
	t_Command	cmd;

	print_section("DISPATCHER EXECUTION PATHS");
	manager = calloc(1, sizeof(t_Manager));
	if (NULL == manager)
		return (print_error("Failed to allocate manager"), 1);
	if (false == dispatcher_init(manager, 4))
		return (free(manager), print_error("Failed to init dispatcher"), 1);
	if (false == dispatcher_register(manager->dispatcher, CMD_WRITING_CREATE_BUFFER, sizeof(t_CmdCreateBuffer), handler_ok))
		return (free_dispatcher_manager(manager), print_error("Register success handler failed"), 1);
	if (false == dispatcher_register(manager->dispatcher, CMD_WRITING_DELETE_BUFFER, sizeof(t_CmdDestroyBuffer), handler_fail))
		return (free_dispatcher_manager(manager), print_error("Register fail handler failed"), 1);

	cmd.id = CMD_WRITING_CREATE_BUFFER;
	cmd.payload = NULL;
	if (assert_error_code(dispatcher_exec(manager, &cmd), ERR_SUCCESS, "Execute registered success handler"))
		return (free_dispatcher_manager(manager), 1);
	cmd.id = CMD_WRITING_DELETE_BUFFER;
	if (assert_error_code(dispatcher_exec(manager, &cmd), ERR_OPERATION_FAILED, "Propagate handler failure"))
		return (free_dispatcher_manager(manager), 1);
	cmd.id = CMD_WRITING_JOIN_LINE;
	if (assert_error_code(dispatcher_exec(manager, &cmd), ERR_INVALID_COMMAND_ID, "Reject unregistered command"))
		return (free_dispatcher_manager(manager), 1);
	if (assert_error_code(dispatcher_exec(NULL, &cmd), ERR_INVALID_MANAGER, "Reject NULL manager"))
		return (free_dispatcher_manager(manager), 1);
	if (assert_error_code(dispatcher_exec(manager, NULL), ERR_INVALID_COMMAND, "Reject NULL command"))
		return (free_dispatcher_manager(manager), 1);
	free_dispatcher_manager(manager);
	return (0);
}

static int	test_dispatcher_exec_no_dispatcher(void)
{
	t_Manager	manager;
	t_Command	cmd;

	print_section("DISPATCHER MISSING INSTANCE");
	memset(&manager, 0, sizeof(manager));
	cmd.id = CMD_WRITING_CREATE_BUFFER;
	cmd.payload = NULL;
	if (assert_error_code(dispatcher_exec(&manager, &cmd), ERR_DISPATCHER_NOT_INITIALIZED, "Reject exec without dispatcher"))
		return (1);
	return (0);
}

int	main(void)
{
	int	status;

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║       DISPATCHER TEST SUITE        ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);
	status = 0;
	status |= test_dispatcher_init_and_register();
	status |= test_dispatcher_exec_paths();
	status |= test_dispatcher_exec_no_dispatcher();
	print_status(status);
	return (status);
}
