#include "tests/tools.h"

#include "core/manager.h"
#include "systems/writing/writing_system.h"

int	test_buffer_command(t_Manager *manager)
{
	// t_ErrorCode	_err_code;	// The error code returned by manager

	/* CREATE COMMAND */
	// In this first test, we will create 5 buffers
	t_CmdCreateBuffer	_buffer_create_payload = {0};	// The payload does not require any optins

	for (size_t _i = 0; _i < 5; _i++)
	{
		RETURN_IF_ERR(test_buffer_create(manager, &_buffer_create_payload));

		if (_buffer_create_payload.out_buffer_id != _i)
			return (1);
	}

	print_success("Buffers created correctly.");

	/* DELETE COMMAND */
	RETURN_IF_ERR(test_buffer_destroy(manager, 2));

	// We check whether the buffer has been deleted
	if (manager->writing_ctx->buffers[2])
	{
		print_error("Buffers has not been deleted.");
		return (1);
	}

	print_success("Buffer deleted correctly.");

	t_ErrorCode	_err_code = test_buffer_destroy(manager, -4);
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
	t_CmdCreateBuffer	_buffer_payload = {0};

	RETURN_IF_ERR(test_buffer_create(manager, &_buffer_payload));

	size_t	_buffer_id = _buffer_payload.out_buffer_id;

	RETURN_IF_ERR(test_line_insert(manager, _buffer_id, -1));
	RETURN_IF_ERR(test_line_insert(manager, _buffer_id, -1));
	RETURN_IF_ERR(test_line_insert(manager, _buffer_id, -1));

	
	RETURN_IF_ERR(
		test_insert_data(
			manager,
			_buffer_id, 0, -1, 12,
			"First line!"
		)
	);

	RETURN_IF_ERR(
		test_insert_data(
			manager,
			_buffer_id, 1, -1, 13,
			"Second line!"
		)
	);

	RETURN_IF_ERR(
		test_insert_data(
			manager,
			_buffer_id, 2, -1, 12,
			"Third line!"
		)
	);

	print_success("Lines created correctly.");

	RETURN_IF_ERR(test_line_join(manager, _buffer_id, 0, 1));

	t_CmdGetLine	_line_get_payload = {
		.buffer_id = _buffer_id,
		.line = 0
	};

	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));

	if (strncmp(
		_line_get_payload.out_data,
		"First line!Second line!",
		_line_get_payload.out_size
	) != 0)
	{
		print_error("The lines has not been correctly joined.");
		return (1);
	}

	print_success("Lines joined correctly.");

	RETURN_IF_ERR(test_line_split(manager, _buffer_id, 0, 11));

	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));
	
	if (strncmp(
		_line_get_payload.out_data,
		"First line!",
		_line_get_payload.out_size
	) != 0)
	{
		print_error("The line has not been correctly splited.");
		return (1);
	}

	_line_get_payload.line = 1;
	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));
	
	if (strncmp(
		_line_get_payload.out_data,
		"Second line!",
		_line_get_payload.out_size
	) != 0)
	{
		print_error("The line has not been correctly splited.");
		return (1);
	}

	print_success("Lines splited correctly.");

	RETURN_IF_ERR(test_line_delete(manager, _buffer_id, 2));

	_line_get_payload.line = 2;
	if (!test_get_line(manager, &_line_get_payload))
	{
		print_error("The line has not been deleted.");
		return (1);
	}

	print_success("Lines deleted correctly.");

	return (0);
}

int	test_data_command(t_Manager *manager)
{
	t_CmdCreateBuffer	_buffer_payload = {0};

	RETURN_IF_ERR(test_buffer_create(manager, &_buffer_payload));

	size_t	_buffer_id = _buffer_payload.out_buffer_id;

	RETURN_IF_ERR(test_line_insert(manager, _buffer_id, -1));

	RETURN_IF_ERR(
		test_insert_data(
			manager,
			_buffer_id, 0, -1, 20,
			"Hello,\tWorld ! 🙂"
		)
	);

	t_CmdGetLine	_line_get_payload = {
		.buffer_id = _buffer_id,
		.line = 0
	};

	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));

	if (strncmp(
		_line_get_payload.out_data,
		"Hello,\tWorld ! 🙂",
		_line_get_payload.out_size
	) != 0)
	{
		print_error("The data has not been written correctly.");
		return (1);
	}

	RETURN_IF_ERR(
		test_insert_data(
			manager,
			_buffer_id, 0, -1, 23,
			" Second insertion here."
		)
	);

	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));

	if (strncmp(
		_line_get_payload.out_data,
		"Hello,\tWorld ! 🙂 Second insertion here.",
		_line_get_payload.out_size
	) != 0)
	{
		print_error("The data has not been written correctly.");
		return (1);
	}

	print_success("Data writted correctly.");

	RETURN_IF_ERR(test_delete_data(manager, _buffer_id, 0, 14, 5));
	RETURN_IF_ERR(test_get_line(manager, &_line_get_payload));

	if (strncmp(
		_line_get_payload.out_data,
		"Hello,\tWorld ! Second insertion here.",
		_line_get_payload.out_size
	) != 0)
	{
		print_error("The data has not been written correctly.");
		return (1);
	}

	print_success("Data deleted correctly.");

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

	print_section("DATA COMMANDS");
	_status |= test_data_command(_manager);

	manager_clean(_manager);
	return (0);
}
