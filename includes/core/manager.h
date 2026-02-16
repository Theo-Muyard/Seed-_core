#ifndef SEED_MANAGER_H
# define SEED_MANAGER_H

# include "dependency.h"

// +===----- Types -----===+ //

typedef enum e_ErrorCode		t_ErrorCode;
typedef struct s_Command		t_Command;
typedef struct s_Dispatcher		t_Dispatcher;
typedef struct s_WritingCtx		t_WritingCtx;
typedef struct s_FileSystemCtx	t_FileSystemCtx;

/* The seed API manager */
typedef struct	s_Manager
{
	t_Dispatcher		*dispatcher;	/* The dispatcher */
	t_WritingCtx		*writing_ctx;	/* The writing context */
	t_FileSystemCtx		*fs_ctx;	/* The filesysten context */
}	t_Manager;

// +===----- Functions -----===+ //

/**
 * @brief Initialize the seed core manager.
 * @return The manager who was created.
*/
t_Manager	*manager_init(void);

/**
 * @brief Clean the seed core manager.
 * @param manager The manager.
*/
void		manager_clean(t_Manager *manager);

/**
 * @brief Clean the seed core manager.
 * @param manager The manager.
 * @param cmd The command content.
 * @return An error code or SUCCESS (=0).
*/
t_ErrorCode	manager_exec(t_Manager *manager, t_Command *cmd);

#endif