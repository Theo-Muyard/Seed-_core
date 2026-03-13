/**
 * @file "tests/commands.h"
 * 
 * @brief Includes easier-to-use commands for testing.
*/

#ifndef SEED_TESTS_COMMMANDS_H
# define SEED_TESTS_COMMMANDS_H

# include "seed.h"

// +===----- Macros -----===+ //

#define RED		"\033[31m"
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

#define RETURN_IF_ERR(code) \
	do { if (code) return (handle_errors(code)); } while (0)

// +===----- Functions -----===+ //

int		handle_errors(t_ErrorCode code);


t_ErrorCode	test_buffer_create(t_Manager *manager, t_CmdCreateBuffer *payload);

t_ErrorCode	test_buffer_destroy(t_Manager *manager, size_t id);

t_ErrorCode	test_line_insert(t_Manager *manager, size_t buffer_id, ssize_t line);

t_ErrorCode	test_line_delete(t_Manager *manager, size_t buffer_id, ssize_t line);

t_ErrorCode	test_get_line(t_Manager *manager, t_CmdGetLine *payload);

t_ErrorCode	test_line_split(t_Manager *manager, size_t buffer_id, ssize_t line, size_t index);

t_ErrorCode	test_line_join(t_Manager *manager, size_t buffer_id, size_t dst, size_t src);

t_ErrorCode	test_insert_data(
	t_Manager *manager,
	size_t buffer_id, size_t line,
	size_t index, size_t size, const char *data
);

t_ErrorCode	test_delete_data(
	t_Manager *manager, size_t buffer_id,
	size_t line, size_t index, size_t size
);

#endif