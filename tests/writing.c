#include "seed.h"

#define print_error(msg) \
	do { printf("Error: %s\n", msg); } while (0)

static void	handle_errors(t_ErrorCode code)
{
	if (code == 0)
		return ;

	if (code == 1)
		print_error("Internal memory.");
	else if (code == 2)
		print_error("Operation failed.");
	else if (code == 3)
		print_error("Permission denied.");
	else if (code == 4)
		print_error("Invalid manager.");
	else if (code == 5)
		print_error("Invalid payload.");
	else if (code == 6)
		print_error("Invalid command.");
	else if (code == 7)
		print_error("Invalid command ID.");
	else if (code == 8)
		print_error("Dispatcher not initialized.");
	else if (code == 9)
		print_error("Writing context not initialized.");
	else if (code == 10)
		print_error("Filesystem context not initialized.");
	else if (code == 11)
		print_error("Buffer not found.");
	else if (code == 12)
		print_error("Line not found.");
	else if (code == 13)
		print_error("Folder not found.");
	else if (code == 14)
		print_error("Folder access denied.");
	else if (code == 15)
		print_error("Folder already exists.");
	else if (code == 16)
		print_error("File not found.");
	else if (code == 17)
		print_error("File access denied.");
	else if (code == 18)
		print_error("File already exists.");
}

int	test_buffer_command(t_Manager *manager)
{
	t_Command	_cmd;
	t_ErrorCode	_err_code;

	/* CREATE COMMAND */
	t_CmdCreateBuffer	_buffer_create_payload = {0};

	_cmd.id = CMD_WRITING_CREATE_BUFFER;
	_cmd.payload = &_buffer_create_payload;

	for (size_t _i = 0; _i < 5; _i++)
	{
		_err_code = manager_exec(manager, &_cmd);
		if (_err_code)
		{
			handle_errors(_err_code);
			return (1);
		}
		printf("Buffer ID: %ld\n", _buffer_create_payload.out_buffer_id);
	}

	/* DELETE COMMAND */
	t_CmdDestroyBuffer	_buffer_destroy_payload = {
		.buffer_id = 2
	};

	_cmd.id = CMD_WRITING_DELETE_BUFFER;
	_cmd.payload = &_buffer_destroy_payload;

	_err_code = manager_exec(manager, &_cmd);
	if (_err_code)
	{
		handle_errors(_err_code);
		return (1);
	}

	printf("Destroyed buffer ID: %ld\n", _buffer_destroy_payload.buffer_id);

	printf("\nBuffers correctly created.\n");
	return (0);
}

int	test_line_command(t_Manager *manager)
{
	(void)manager;
	return (0);
}

int	test_data_command(t_Manager *manager)
{
	(void)manager;
	return (0);
}

int	main(void)
{
	t_Manager	*_manager = manager_init();
	RETURN_IF_NULL(_manager, 1);

	if (test_buffer_command(_manager))
		goto exit_free_manager;

	if (test_line_command(_manager))
		goto exit_free_manager;

	if (test_data_command(_manager))
		goto exit_free_manager;

	manager_clean(_manager);
	return (0);

	/* GOTO EXIT */
	exit_free_manager:
		return (manager_clean(_manager), 1);
}
