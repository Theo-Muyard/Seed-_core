#include "tools.h"
#include "systems/writing/_internal.h"

static int	test_line_core(void)
{
	t_Line	*line;
	char	msg[] = "Hello";

	print_section("INTERNAL LINE CORE");
	line = line_create();
	if (NULL == line)
		return (print_error("line_create failed"), 1);
	if (false == line_insert_data(line, 0, 5, msg))
		return (free(line), print_error("line_insert_data failed"), 1);
	if (line->len != 5 || 0 != strcmp(line->data, "Hello"))
		return (free(line->data), free(line), print_error("Unexpected line content"), 1);
	print_success("Insert data in empty line");
	if (false == line_insert_data(line, 2, 1, "_"))
		return (free(line->data), free(line), print_error("Insert in middle failed"), 1);
	if (0 != strcmp(line->data, "He_llo"))
		return (free(line->data), free(line), print_error("Middle insert mismatch"), 1);
	print_success("Insert in middle");
	if (false == line_delete_data(line, 2, 1))
		return (free(line->data), free(line), print_error("Delete in middle failed"), 1);
	if (0 != strcmp(line->data, "Hello"))
		return (free(line->data), free(line), print_error("Delete result mismatch"), 1);
	print_success("Delete in line");
	free(line->data);
	free(line);
	return (0);
}

static int	test_buffer_core(void)
{
	t_Buffer	*buffer;
	t_Line		*l0;
	t_Line		*l1;
	t_Line		*l2;
	t_Line		*split;
	t_Line		*joined;

	print_section("INTERNAL BUFFER CORE");
	buffer = buffer_create();
	if (NULL == buffer)
		return (print_error("buffer_create failed"), 1);
	l0 = line_create();
	l1 = line_create();
	l2 = line_create();
	if (NULL == l0 || NULL == l1 || NULL == l2)
		return (buffer_destroy(buffer), print_error("line allocation failed"), 1);
	if (false == buffer_line_insert(buffer, l0, 0)
		|| false == buffer_line_insert(buffer, l1, -1)
		|| false == buffer_line_insert(buffer, l2, 1))
		return (buffer_destroy(buffer), print_error("buffer_line_insert failed"), 1);
	if (buffer->size != 3)
		return (buffer_destroy(buffer), print_error("Invalid buffer size after inserts"), 1);
	print_success("Insert lines at beginning/middle/end");
	if (NULL == buffer_get_line(buffer, 0)
		|| NULL == buffer_get_line(buffer, 1)
		|| NULL == buffer_get_line(buffer, -1))
		return (buffer_destroy(buffer), print_error("buffer_get_line failed"), 1);
	print_success("Get lines by index");
	if (false == line_insert_data(buffer_get_line(buffer, 0), 0, 8, "ABCD1234"))
		return (buffer_destroy(buffer), print_error("line insert for split failed"), 1);
	split = buffer_line_split(buffer, buffer_get_line(buffer, 0), 4);
	if (NULL == split)
		return (buffer_destroy(buffer), print_error("buffer_line_split failed"), 1);
	if (0 != strcmp(buffer_get_line(buffer, 0)->data, "ABCD")
		|| 0 != strcmp(buffer_get_line(buffer, 1)->data, "1234"))
		return (buffer_destroy(buffer), print_error("split data mismatch"), 1);
	print_success("Split line");
	joined = buffer_line_join(buffer, buffer_get_line(buffer, 0), buffer_get_line(buffer, 1));
	if (NULL == joined)
		return (buffer_destroy(buffer), print_error("buffer_line_join failed"), 1);
	if (0 != strcmp(joined->data, "ABCD1234"))
		return (buffer_destroy(buffer), print_error("join data mismatch"), 1);
	print_success("Join lines");
	buffer_destroy(buffer);
	return (0);
}

static int	test_internal_errors(void)
{
	t_Buffer	*buffer;
	t_Line		*line;

	print_section("INTERNAL ERROR CASES");
	if (false != line_insert_data(NULL, 0, 1, "x"))
		return (print_error("line_insert_data should reject NULL line"), 1);
	print_success("Reject NULL line on insert");
	if (false != line_delete_data(NULL, 0, 1))
		return (print_error("line_delete_data should reject NULL line"), 1);
	print_success("Reject NULL line on delete");
	buffer = buffer_create();
	line = line_create();
	if (NULL == buffer || NULL == line)
		return (buffer_destroy(buffer), free(line), print_error("Allocation failed"), 1);
	if (false != buffer_line_insert(buffer, line, 2))
		return (buffer_destroy(buffer), print_error("Insert should fail with out-of-range index"), 1);
	print_success("Reject out-of-range line index");
	free(line);
	if (NULL != buffer_get_line(buffer, 0))
		return (buffer_destroy(buffer), print_error("Get line should fail on empty buffer"), 1);
	print_success("Reject get line on empty buffer");
	buffer_destroy(buffer);
	return (0);
}

int	test_internal_main(void)
{
	int	status;

	printf("\n%s╔════════════════════════════════════╗%s\n", BLUE, WHITE);
	printf("%s║    WRITING INTERNAL TEST SUITE     ║%s\n", BLUE, WHITE);
	printf("%s╚════════════════════════════════════╝%s\n", BLUE, WHITE);
	status = 0;
	status |= test_line_core();
	status |= test_buffer_core();
	status |= test_internal_errors();
	print_status(status);
	return (status);
}
