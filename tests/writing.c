#include "seed.h"

#include "core/manager.h"
#include "systems/writing/writing_system.h"

#define RED	"\033[31m"
#define GREEN	"\033[32m"
#define YELLOW	"\033[33m"
#define BLUE	"\033[34m"
#define WHITE	"\033[37m"
#define RESET	"\033[0m"

#define print_error_code(msg) \
	do { printf("%sError:%s %s\n", RED, RESET, msg); } while (0)

#define print_section(msg) \
	do { printf("\n%s=== %s ===%s\n", BLUE, msg, WHITE); } while (0)

#define print_success(msg) \
	do { printf("%s✓ %s%s\n", GREEN, msg, RESET); } while (0)

#define print_error(msg) \
	do { printf("%s✗ %s%s\n", RED, msg, RESET); } while (0)

static int	handle_errors(t_ErrorCode code)
{
	if (code == 0)
		return (0);

	if (code == 1)
		print_error_code("Internal memory.");
	else if (code == 2)
		print_error_code("Operation failed.");
	else if (code == 3)
		print_error_code("Permission denied.");
	else if (code == 4)
		print_error_code("Invalid manager.");
	else if (code == 5)
		print_error_code("Invalid payload.");
	else if (code == 6)
		print_error_code("Invalid command.");
	else if (code == 7)
		print_error_code("Invalid command ID.");
	else if (code == 8)
		print_error_code("Dispatcher not initialized.");
	else if (code == 9)
		print_error_code("Writing context not initialized.");
	else if (code == 10)
		print_error_code("Filesystem context not initialized.");
	else if (code == 11)
		print_error_code("Buffer not found.");
	else if (code == 12)
		print_error_code("Line not found.");
	else if (code == 13)
		print_error_code("Folder not found.");
	else if (code == 14)
		print_error_code("Folder access denied.");
	else if (code == 15)
		print_error_code("Folder already exists.");
	else if (code == 16)
		print_error_code("File not found.");
	else if (code == 17)
		print_error_code("File access denied.");
	else if (code == 18)
		print_error_code("File already exists.");

	return (1);
}

t_ErrorCode	create_buffer(t_Manager *manager, t_CmdCreateBuffer *payload)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);
	RETURN_IF_NULL(payload, ERR_INVALID_PAYLOAD);

	t_Command	_cmd = {
		.id = CMD_WRITING_CREATE_BUFFER,
		.payload = payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	destroy_buffer(t_Manager *manager, size_t id)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdDestroyBuffer	_payload = {
		.buffer_id = id
	};

	t_Command			_cmd = {
		.id = CMD_WRITING_DELETE_BUFFER,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

int	test_buffer_command(t_Manager *manager)
{
	t_Command	_cmd;		// Our command
	t_ErrorCode	_err_code;	// The error code returned by manager

	/* CREATE COMMAND */
	// In this first test, we will create 5 buffers
	t_CmdCreateBuffer	_buffer_create_payload = {0};	// The payload does not require any optins

	// _cmd.id = CMD_WRITING_CREATE_BUFFER;	// The command ID
	// _cmd.payload = &_buffer_create_payload;	// We provide the adress of our payload so that the manager
	// 					// can modify it.

	// for (size_t _i = 0; _i < 5; _i++)
	// {
	// 	_err_code = manager_exec(manager, &_cmd); // We retrieve the return code

	// 	// If there is an error, we display it and stop the test here
	// 	if (_err_code)
	// 		return (handle_errors(_err_code));

	// 	// If the buffer ID is incorrect, then the test fails
	// 	if (_buffer_create_payload.out_buffer_id != _i)
	// 		return (1);
	// }

	for (size_t _i = 0; _i < 5; _i++)
	{
		_err_code = create_buffer(manager, &_buffer_create_payload);
		if (_err_code)
			return (handle_errors(_err_code));

		if (_buffer_create_payload.out_buffer_id != _i)
			return (1);
	}

	print_success("Buffers created correctly.");

	/* DELETE COMMAND */
	// In this test, we will delete a valid buffer and invalid buffer
	t_CmdDestroyBuffer	_buffer_destroy_payload = {
		.buffer_id = 2 // The ID of the buffer that will be destroyed
	};

	_cmd.id = CMD_WRITING_DELETE_BUFFER;
	_cmd.payload = &_buffer_destroy_payload;

	_err_code = manager_exec(manager, &_cmd);
	if (_err_code)
		return(handle_errors(_err_code));

	// We check whether the buffer has been deleted
	if (manager->writing_ctx->buffers[2])
	{
		print_error("Buffers has not been deleted.");
		return (1);
	}

	print_success("Buffer deleted correctly.");

	_buffer_destroy_payload.buffer_id = -4;	// Invalid buffer

	_err_code = manager_exec(manager, &_cmd);
	if (!_err_code)
	{
		print_error("Invalid buffer should not pass.");
		return (1);
	}

	print_success("Invalid buffer does not pass.");

	return (0);
}

int	test_line_command(t_Manager *manager)
{
	t_Command	_cmd;
	t_ErrorCode	_err_code;

	t_CmdCreateBuffer	_buffer_payload = {0};

	_err_code = create_buffer(manager, &_buffer_payload);
	if (_err_code)
		handle_errors(_err_code);

	size_t	_buffer_id = _buffer_payload.out_buffer_id;

	t_CmdInsertLine	_line_insert_payload = {
		.buffer_id = _buffer_id,
		.line = -1
	};

	_cmd.id = CMD_WRITING_INSERT_LINE;
	_cmd.payload = &_line_insert_payload;

	_err_code = manager_exec(manager, &_cmd);
	if (_err_code)
		return(handle_errors(_err_code));

	print_success("Line created correctly.");

	t_CmdInsertData	_data_insert_payload = {
		.buffer_id = _buffer_id,
		.line = 0,
		.data = "Salut 📍 tout le monde c'est la salle ! 🫡 ⬜",
		.size = 51,
		.index = -1
	};

	_cmd.id = CMD_WRITING_INSERT_TEXT;
	_cmd.payload = &_data_insert_payload;

	_err_code = manager_exec(manager, &_cmd);
	if (_err_code)
		return(handle_errors(_err_code));

	t_CmdGetLine	_line_get_payload = {
		.buffer_id = _buffer_id,
		.line = 0
	};

	_cmd.id = CMD_WRITING_GET_LINE;
	_cmd.payload = &_line_get_payload;

	_err_code = manager_exec(manager, &_cmd);
	if (_err_code)
		return(handle_errors(_err_code));

	printf("Line %zd: %s (%zd)\n", _line_get_payload.line,
		_line_get_payload.out_data, _line_get_payload.out_size);

	_data_insert_payload.data = "🆕 Deuxieme insert ici 🆕";
	_data_insert_payload.size = 30;
	_data_insert_payload.index = 10;

	_cmd.id = CMD_WRITING_INSERT_TEXT;
	_cmd.payload = &_data_insert_payload;

	_err_code = manager_exec(manager, &_cmd);
	if (_err_code)
		return(handle_errors(_err_code));

	_cmd.id = CMD_WRITING_GET_LINE;
	_cmd.payload = &_line_get_payload;
	_err_code = manager_exec(manager, &_cmd);
	if (_err_code)
		return(handle_errors(_err_code));

	printf("Line %zd: %s (%zd)\n", _line_get_payload.line,
		_line_get_payload.out_data, _line_get_payload.out_size);

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

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║     WRITING COMMANDS TEST SUITE    ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);

	int	_status = 0;

	print_section("BUFFER COMMANDS");
	_status |= test_buffer_command(_manager);

	print_section("LINE COMMANDS");
	_status |= test_line_command(_manager);

	_status |= test_data_command(_manager);

	manager_clean(_manager);
	return (0);
}
