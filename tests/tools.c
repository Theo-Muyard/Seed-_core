#include "tests/tools.h"

int		handle_errors(t_ErrorCode code)
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

t_ErrorCode	test_buffer_create(t_Manager *manager, t_CmdCreateBuffer *payload)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);
	RETURN_IF_NULL(payload, ERR_INVALID_PAYLOAD);

	t_Command	_cmd = {
		.id = CMD_WRITING_CREATE_BUFFER,
		.payload = payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_buffer_destroy(t_Manager *manager, size_t id)
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

t_ErrorCode	test_line_insert(t_Manager *manager, size_t buffer_id, ssize_t line)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdInsertLine	_payload = {
		.buffer_id = buffer_id,
		.line = line
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_INSERT_LINE,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_line_delete(t_Manager *manager, size_t buffer_id, ssize_t line)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdDeleteLine	_payload = {
		.buffer_id = buffer_id,
		.line = line
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_DELETE_LINE,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_get_line(t_Manager *manager, t_CmdGetLine *payload)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);
	RETURN_IF_NULL(payload, ERR_INVALID_PAYLOAD);

	t_Command	_cmd = {
		.id = CMD_WRITING_GET_LINE,
		.payload = payload
	};

	return (manager_exec(manager, &_cmd));
}


t_ErrorCode	test_line_split(t_Manager *manager, size_t buffer_id, ssize_t line, size_t index)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdSplitLine	_payload = {
		.buffer_id = buffer_id,
		.line = line,
		.index = index
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_SPLIT_LINE,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_line_join(t_Manager *manager, size_t buffer_id, size_t dst, size_t src)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdJoinLine	_payload = {
		.buffer_id = buffer_id,
		.dst = dst,
		.src = src
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_JOIN_LINE,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_insert_data(
	t_Manager *manager,
	size_t buffer_id, size_t line,
	size_t index, size_t size, const char *data
)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdInsertData	_payload = {
		.buffer_id = buffer_id,
		.line = line,
		.index = index,
		.size = size,
		.data = data
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_INSERT_TEXT,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}

t_ErrorCode	test_delete_data(
	t_Manager *manager, size_t buffer_id,
	size_t line, size_t index, size_t size
)
{
	RETURN_IF_NULL(manager, ERR_INVALID_MANAGER);

	t_CmdDeleteData	_payload = {
		.buffer_id = buffer_id,
		.line = line,
		.index = index,
		.size = size
	};

	t_Command	_cmd = {
		.id = CMD_WRITING_DELETE_TEXT,
		.payload = &_payload
	};

	return (manager_exec(manager, &_cmd));
}
