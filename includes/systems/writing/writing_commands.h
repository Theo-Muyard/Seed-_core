/**
 * @file "systems/writing/writing_commands.h"
 * 
 * @brief Manage the writing system commands. These are
 * 		the functions that are executed according to the command
 * 		given by the client.
 * 
 * @ingroup "writing system"
*/

#ifndef SEED_WRITING_COMMANDS_H
# define SEED_WRITING_COMMANDS_H

# include "seed.h"
# include "core/manager.h"

# define BUFFER_ALLOC 32

// +===----- Buffer functions -----===+ //

/**
 * @brief Creates a new empty buffer.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_buffer_create(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Destroys the given buffer.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_buffer_destroy(t_Manager *manager, const t_Command *cmd);

// +===----- Lines functions -----===+ //

/**
 * @brief Adds the line to the given index of the buffer.

 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_buffer_line_insert(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Delete the given line.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_buffer_line_delete(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Get the line of the given index.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_buffer_get_line(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Splits the given line in two lines.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_buffer_line_split(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Joins the 2 givens lines in one line.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_buffer_line_join(t_Manager *manager, const t_Command *cmd);

// +===----- Data functions -----===+ //

/**
 * @brief Insert the data to the given line.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_line_insert_data(t_Manager *manager, const t_Command *cmd);

/**
 * @brief Delete the data to the given line.
 * 
 * @param manager The manager must not be NULL.
 * @param cmd The command content.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	cmd_line_delete_data(t_Manager *manager, const t_Command *cmd);

#endif