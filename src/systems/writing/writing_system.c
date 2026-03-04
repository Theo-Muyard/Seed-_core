#include "systems/writing/writing_system.h"
#include "systems/writing/writing_commands.h"
#include "common/systems_registry.h"

// +===----- Commands entries -----===+ //

const t_CommandEntry	writing_commands[] = {
	{ CMD_WRITING_CREATE_BUFFER,	cmd_buffer_create},
	{ CMD_WRITING_DELETE_BUFFER,	cmd_buffer_destroy},
	
	{ CMD_WRITING_INSERT_LINE,	cmd_buffer_line_insert},
	{ CMD_WRITING_DELETE_LINE,	cmd_buffer_line_delete},
	{ CMD_WRITING_SPLIT_LINE,	cmd_buffer_line_split},
	{ CMD_WRITING_JOIN_LINE,	cmd_buffer_line_join},
	{ CMD_WRITING_GET_LINE,		cmd_buffer_get_line},
	
	{ CMD_WRITING_INSERT_TEXT,	cmd_line_insert_data},
	{ CMD_WRITING_DELETE_TEXT,	cmd_line_delete_data}
};

// +===----- Functions -----===+ //

bool	writing_init(t_Manager	*manager)
{
	RETURN_IF_NULL(manager, false);
	RETURN_IF_NULL(manager->dispatcher, false);

	t_WritingCtx	*_ctx = malloc(sizeof(t_WritingCtx));
	RETURN_IF_NULL(_ctx, false);

	_ctx->buffers = NULL;
	_ctx->count = 0;
	_ctx->capacity = 0;

	GOTO_IF_FALSE(
		register_commands(
			manager->dispatcher,
			writing_commands, WRITING_COMMANDS_COUNT
		),
		exit_free_ctx
	);

	manager->writing_ctx = _ctx;
	return (true);

	/* GOTO EXIT */
	exit_free_ctx:
		return (free(_ctx), false);
}

void	writing_clean(t_WritingCtx *ctx)
{
	if (NULL == ctx)
		return ;

	size_t	_i = 0;
	while (_i < ctx->count)
	{
		buffer_destroy(ctx->buffers[_i]);
		_i++;
	}

	free(ctx->buffers);
	ctx->buffers = NULL;
	ctx->count = 0;
	ctx->capacity = 0;
	free(ctx);
}
