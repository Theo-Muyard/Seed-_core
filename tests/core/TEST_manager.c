#include "tools.h"
#include "core/manager.h"
#include "core/dispatcher.h"

static int	test_manager_init_state(void)
{
	t_Manager	*manager;

	print_section("MANAGER INITIALIZATION");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	if (NULL == manager->dispatcher)
		return (manager_clean(manager), print_error("Dispatcher is NULL"), 1);
	print_success("Dispatcher initialized");
	if (NULL == manager->writing_ctx)
		return (manager_clean(manager), print_error("Writing context is NULL"), 1);
	print_success("Writing context initialized");
	if (NULL == manager->fs_ctx)
		return (manager_clean(manager), print_error("Filesystem context is NULL"), 1);
	print_success("Filesystem context initialized");
	if (manager->dispatcher->count != 19)
		return (manager_clean(manager), print_error("Expected 19 registered commands"), 1);
	print_success("All commands registered");
	manager_clean(manager);
	return (0);
}

static int	test_manager_exec_errors(void)
{
	t_Manager	*manager;
	t_Command	cmd;

	print_section("MANAGER EXEC ERROR HANDLING");
	cmd.id = CMD_WRITING_CREATE_BUFFER;
	cmd.payload = NULL;
	if (assert_error_code(manager_exec(NULL, &cmd), ERR_INVALID_MANAGER, "Reject NULL manager"))
		return (1);
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	if (assert_error_code(manager_exec(manager, NULL), ERR_INVALID_COMMAND, "Reject NULL command"))
		return (manager_clean(manager), 1);
	cmd.id = (t_CommandId)9999;
	cmd.payload = NULL;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_INVALID_COMMAND_ID, "Reject unknown command id"))
		return (manager_clean(manager), 1);
	manager_clean(manager);
	return (0);
}

static int	test_manager_exec_integration(void)
{
	t_Manager			*manager;
	t_Command			cmd;
	t_CmdCreateBuffer	create_payload;
	t_CmdOpenRoot		open_payload;
	char				*tmp_root;

	print_section("MANAGER EXEC INTEGRATION");
	manager = manager_init();
	if (NULL == manager)
		return (print_error("Failed to initialize manager"), 1);
	create_payload.out_buffer_id = 0;
	cmd.id = CMD_WRITING_CREATE_BUFFER;
	cmd.payload = &create_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Create buffer through manager"))
		return (manager_clean(manager), 1);
	tmp_root = test_tmpdir_create("/tmp/seed_manager_root");
	if (NULL == tmp_root)
		return (manager_clean(manager), print_error("Failed to create tmp root"), 1);
	open_payload.path = tmp_root;
	cmd.id = CMD_FS_OPEN_ROOT;
	cmd.payload = &open_payload;
	if (assert_error_code(manager_exec(manager, &cmd), ERR_SUCCESS, "Open root through manager"))
		return (free(tmp_root), manager_clean(manager), 1);
	if (assert_error_code(manager_exec(manager, &(t_Command){CMD_FS_CLOSE_ROOT, NULL}), ERR_SUCCESS, "Close root through manager"))
		return (test_tmpdir_remove(tmp_root), free(tmp_root), manager_clean(manager), 1);
	test_tmpdir_remove(tmp_root);
	free(tmp_root);
	manager_clean(manager);
	return (0);
}

int	main(void)
{
	int	status;

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║         MANAGER TEST SUITE         ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);
	status = 0;
	status |= test_manager_init_state();
	status |= test_manager_exec_errors();
	status |= test_manager_exec_integration();
	print_status(status);
	return (status);
}
