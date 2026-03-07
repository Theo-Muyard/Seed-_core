#include "systems/filesystem/fs_commands.h"
#include "systems/filesystem/fs_common.h"
#include "systems/filesystem/fs_vfs.h"
#include "systems/filesystem/fs_os.h"
#include "systems/filesystem/fs_system.h"
#include "systems/filesystem/fs_watcher.h"
#include "common/memory.h"

/**
 * @brief Initialize the VFS root. Retrive all subdirs and files.
 * 
 * @param root The root must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @return A specific `ErrorCode`.
*/
t_ErrorCode	vfs_register_root(t_Directory *root, const char *path)
{
	DIR	*_dir = opendir(path);
	if (NULL == _dir)
		return (handle_dir_error());
	
	char	*_entry_path = NULL;

	struct dirent	*_entry;
	while (NULL != (_entry = readdir(_dir)))
	{
		if (strcmp(_entry->d_name, ".") == 0 || strcmp(_entry->d_name, "..") == 0)
			continue ;
	
		_entry_path = join_path(path, _entry->d_name);
		GOTO_IF_NULL(_entry_path, exit_clean);

		struct stat	_st;
		if (stat(_entry_path, &_st) == -1)
			goto exit_clean_errno;

		if (S_ISDIR(_st.st_mode))
		{
			t_Directory	*_subdir = vfs_dir_create(root, _entry->d_name);
			GOTO_IF_NULL(_dir, exit_clean);

			t_ErrorCode	_err = vfs_register_root(_subdir, _entry_path);
			if (_err)
			{
				goto free_path_and_close_dir;
				return (_err);
			}
		}
		else if (S_ISREG(_st.st_mode))
			GOTO_IF_FALSE(vfs_file_create(root, _entry->d_name), exit_clean);

		free(_entry_path);
	}

	closedir(_dir);
	return (ERR_SUCCESS);

	/* GOTO EXIT */
	free_path_and_close_dir:
		closedir(_dir);
		free(_entry_path);
	
	exit_clean:
		goto free_path_and_close_dir;
		return (ERR_INTERNAL_MEMORY);

	exit_clean_errno:
		goto free_path_and_close_dir;
		return (handle_dir_error());
}

// +===----- Path -----===+ //

/**
 * @brief Retrieve the parent folder by its path.
 * 
 * @param root The root must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @retval The parent folder.
 * @retval NULL if `root` or `path` is NULL or an error occurred.
*/
static t_Directory	*resolve_parent_dir(t_Directory *root, const char *path)
{
	RETURN_IF_NULL(root, NULL);
	RETURN_IF_NULL(path, NULL);

	char	*_cpy = ft_strdup(path);
	RETURN_IF_NULL(_cpy, NULL);

	char	*_slash = strrchr(_cpy, '/');
	if (NULL == _slash)
		return (free(_cpy), root);

	*_slash = '\0';
	t_Directory	*_dir = vfs_subdir_resolve(root, _cpy);
	free(_cpy);

	return (_dir);
}

// +===----- Root functions -----===+ //

t_ErrorCode	cmd_root_open(t_Manager *manager, const t_Command *cmd)
{
	t_FileSystemCtx	*_ctx = manager->fs_ctx;
	RETURN_IF_NULL(_ctx, ERR_FS_CONTEXT_NOT_INITIALIZED);
	t_CmdOpenRoot	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_payload->path, ERR_INVALID_PAYLOAD);

	char	*_normalized = ft_strdup(_payload->path);
	RETURN_IF_NULL(_normalized, ERR_INTERNAL_MEMORY);

	struct stat	_st;
	if (stat(_normalized, &_st) == -1)
		return (free(_normalized), ERR_OPERATION_FAILED);
	if (!S_ISDIR(_st.st_mode))
		return (free(_normalized), ERR_DIR_NOT_FOUND);

	char	*_dirname = strrchr(_normalized, '/');
	if (NULL == _dirname)
		return (free(_normalized), ERR_INVALID_PAYLOAD);

	t_Directory	*_root = vfs_dir_create(NULL, _dirname + 1);
	GOTO_IF_NULL(_root, exit_free_and_destroy);

	t_ErrorCode	_err = vfs_register_root(_root, _normalized);
	if (_err)
		goto exit_free_and_destroy;

	if (_ctx->root)
		cmd_root_close(manager, NULL);

	_ctx->root = _root;
	_ctx->root_path = _normalized;
	_ctx->path_len = strlen(_ctx->root_path);

	return (ERR_SUCCESS);

	/* GOTO EXIT */
	exit_free_and_destroy:
		free(_normalized);
		vfs_dir_destroy(_root);
		return (ERR_INTERNAL_MEMORY);
}

t_ErrorCode	cmd_root_close(t_Manager *manager, const t_Command *cmd)
{
	(void)cmd;

	t_FileSystemCtx	*_ctx = manager->fs_ctx;
	RETURN_IF_NULL(_ctx->root, ERR_FS_CONTEXT_NOT_INITIALIZED);

	vfs_dir_destroy(_ctx->root);
	free(_ctx->root_path);
	_ctx->root = NULL;
	_ctx->root_path = NULL;
	_ctx->path_len = 0;

	return (ERR_SUCCESS);
}

// +===----- Directory functions -----===+ //

t_ErrorCode	cmd_directory_create(t_Manager *manager, const t_Command *cmd)
{
	t_FileSystemCtx	*_ctx = manager->fs_ctx;
	RETURN_IF_NULL(_ctx, ERR_FS_CONTEXT_NOT_INITIALIZED);
	t_CmdCreateDir	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_payload->path, ERR_INVALID_PAYLOAD);

	char	*_abs_path = join_path(_ctx->root_path, _payload->path);
	RETURN_IF_NULL(_abs_path, ERR_INTERNAL_MEMORY);

	GOTO_IF_FALSE(os_dir_create(_abs_path, 0755), exit_free_path_errno);

	t_Directory	*_parent_dir = resolve_parent_dir(_ctx->root, _payload->path);
	if (NULL == _parent_dir)
		GOTO_IF_FALSE(os_dir_delete(_abs_path), exit_free_path_errno);
	free(_abs_path);

	char	*_dirname = strrchr(_payload->path, '/');
	_dirname = _dirname ? _dirname + 1 : (char *)_payload->path;
	RETURN_IF_NULL(vfs_dir_create(_parent_dir, _dirname), ERR_INTERNAL_MEMORY);

	return (ERR_SUCCESS);

	/* GOTO EXIT */
	exit_free_path_errno:
		free(_abs_path);
		return (handle_dir_error());
}

t_ErrorCode	cmd_directory_delete(t_Manager *manager, const t_Command *cmd)
{
	t_FileSystemCtx	*_ctx = manager->fs_ctx;
	RETURN_IF_NULL(_ctx, ERR_FS_CONTEXT_NOT_INITIALIZED);
	t_CmdDeleteDir	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_payload->path, ERR_INVALID_PAYLOAD);

	char	*_abs_path = join_path(_ctx->root_path, _payload->path);
	RETURN_IF_NULL(_abs_path, ERR_INTERNAL_MEMORY);

	GOTO_IF_FALSE(os_dir_delete(_abs_path), exit_free_path_errno);
	free(_abs_path);

	t_Directory	*_dir = vfs_subdir_resolve(_ctx->root, _payload->path);
	RETURN_IF_NULL(_dir, ERR_SUCCESS);

	RETURN_IF_FALSE(
		vfs_remove_subdir_to_dir(_dir->parent, _dir),
		ERR_OPERATION_FAILED
	);

	vfs_dir_destroy(_dir);
	return (ERR_SUCCESS);

	/* GOTO EXIT */
	exit_free_path_errno:
		free(_abs_path);
		return (handle_dir_error());
}

t_ErrorCode	cmd_directory_move(t_Manager *manager, const t_Command *cmd)
{
	t_FileSystemCtx	*_ctx = manager->fs_ctx;
	RETURN_IF_NULL(_ctx, ERR_FS_CONTEXT_NOT_INITIALIZED);
	t_CmdMoveDir	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_payload->old_path, ERR_INVALID_PAYLOAD);
	RETURN_IF_NULL(_payload->new_path, ERR_INVALID_PAYLOAD);

	char	*_old_abs_path = join_path(_ctx->root_path, _payload->old_path);
	RETURN_IF_NULL(_old_abs_path, ERR_INTERNAL_MEMORY);

	char	*_new_abs_path = join_path(_ctx->root_path, _payload->new_path);
	RETURN_IF_NULL(_new_abs_path, ERR_INTERNAL_MEMORY);

	GOTO_IF_FALSE(
		os_dir_move(_old_abs_path, _new_abs_path),
		exit_free_paths_errno
	);

	free(_old_abs_path);
	free(_new_abs_path);
	
	t_Directory	*_dir = vfs_subdir_resolve(_ctx->root, _payload->old_path);
	RETURN_IF_NULL(_dir, ERR_DIR_NOT_FOUND);

	t_Directory	*_new_parent_dir = resolve_parent_dir(_ctx->root, _payload->new_path);
	RETURN_IF_NULL(_new_parent_dir, ERR_DIR_NOT_FOUND);
	
	char	*_slash = strrchr(_payload->new_path, '/');
	char	*_dirname = _slash ? _slash + 1 : (char *)_payload->new_path;

	RETURN_IF_FALSE(
		vfs_dir_rename(_dir, _dirname),
		ERR_OPERATION_FAILED
	);

	RETURN_IF_FALSE(
		vfs_subdir_move(_new_parent_dir, _dir->parent, _dir),
		ERR_OPERATION_FAILED
	);

	return (ERR_SUCCESS);

	/* GOTO EXIT */
	exit_free_paths_errno:
		free(_old_abs_path);
		free(_new_abs_path);
		return (handle_dir_error());
}

// +===----- File functions -----===+ //

t_ErrorCode	cmd_file_create(t_Manager *manager, const t_Command *cmd)
{
	t_FileSystemCtx	*_ctx = manager->fs_ctx;
	RETURN_IF_NULL(_ctx, ERR_FS_CONTEXT_NOT_INITIALIZED);
	t_CmdCreateFile	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_payload->path, ERR_INVALID_PAYLOAD);

	char	*_abs_path = join_path(_ctx->root_path, _payload->path);
	RETURN_IF_NULL(_abs_path, ERR_INTERNAL_MEMORY);

	FILE	*_file = os_file_create(_abs_path, "w");
	if (NULL == _file)
		return (free(_abs_path), ERR_OPERATION_FAILED);
	fclose(_file);

	t_Directory	*_parent_dir = resolve_parent_dir(_ctx->root, _payload->path);
	if (NULL == _parent_dir)
		GOTO_IF_FALSE(
			os_file_delete(_abs_path),
			exit_free_path_errno
		);
	free(_abs_path);

	char	*_filename = strrchr(_payload->path, '/');
	_filename = _filename ?  _filename + 1 : _payload->path;
	
	RETURN_IF_NULL(vfs_file_create(_parent_dir, _filename), ERR_OPERATION_FAILED);

	return (ERR_SUCCESS);

	/* GOTO EXIT */
	exit_free_path_errno:
		return (free(_abs_path), handle_file_error());
}

t_ErrorCode	cmd_file_delete(t_Manager *manager, const t_Command *cmd)
{
	t_FileSystemCtx	*_ctx = manager->fs_ctx;
	RETURN_IF_NULL(_ctx, ERR_FS_CONTEXT_NOT_INITIALIZED);
	t_CmdDeleteFile	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_payload->path, ERR_INVALID_PAYLOAD);

	char	*_abs_path = join_path(_ctx->root_path, _payload->path);
	RETURN_IF_NULL(_abs_path, ERR_INTERNAL_MEMORY);

	GOTO_IF_FALSE(
		os_file_delete(_abs_path),
		exit_free_path_errno
	);
	free(_abs_path);

	t_File	*_file = vfs_file_resolve(_ctx->root, _payload->path);
	RETURN_IF_NULL(_file, ERR_SUCCESS);

	RETURN_IF_FALSE(
		vfs_remove_file_to_dir(_file->parent, _file),
		ERR_OPERATION_FAILED
	);

	vfs_file_destroy(_file);
	return (ERR_SUCCESS);

	/* GOTO EXIT */
	exit_free_path_errno:
		return (free(_abs_path), handle_file_error());
}

t_ErrorCode	cmd_file_move(t_Manager *manager, const t_Command *cmd)
{
	t_FileSystemCtx	*_ctx = manager->fs_ctx;
	RETURN_IF_NULL(_ctx, ERR_FS_CONTEXT_NOT_INITIALIZED);
	t_CmdMoveFile	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_payload->old_path, ERR_INVALID_PAYLOAD);
	RETURN_IF_NULL(_payload->new_path, ERR_INVALID_PAYLOAD);

	char	*_old_abs_path = join_path(_ctx->root_path, _payload->old_path);
	RETURN_IF_NULL(_old_abs_path, ERR_INTERNAL_MEMORY);

	char	*_new_abs_path = join_path(_ctx->root_path, _payload->new_path);
	RETURN_IF_NULL(_new_abs_path, ERR_INTERNAL_MEMORY);

	GOTO_IF_FALSE(
		os_file_move(_old_abs_path, _new_abs_path),
		exit_free_paths_errno
	);

	free(_old_abs_path);
	free(_new_abs_path);

	t_File	*_file = vfs_file_resolve(_ctx->root, _payload->old_path);
	RETURN_IF_NULL(_file, ERR_FILE_NOT_FOUND);

	t_Directory	*_new_parent_dir = resolve_parent_dir(_ctx->root, _payload->new_path);
	RETURN_IF_NULL(_new_parent_dir, ERR_DIR_NOT_FOUND);

	char	*_slash = strrchr(_payload->new_path, '/'); 
	char	*_filename = NULL;
	_filename = _slash ? _slash + 1 : (char *)_payload->new_path;

	RETURN_IF_FALSE(
		vfs_file_rename(_file, _filename),
		ERR_OPERATION_FAILED
	);

	RETURN_IF_FALSE(
		vfs_file_move(_new_parent_dir, _file->parent, _file),
		ERR_OPERATION_FAILED
	);

	return (ERR_SUCCESS);

	/* GOTO EXIT */
	exit_free_paths_errno:
		free(_old_abs_path);
		free(_new_abs_path);
		return (handle_file_error());
}

t_ErrorCode	cmd_file_read(t_Manager *manager, const t_Command *cmd)
{
	t_FileSystemCtx	*_ctx = manager->fs_ctx;
	RETURN_IF_NULL(_ctx, ERR_FS_CONTEXT_NOT_INITIALIZED);
	t_CmdReadFile	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_payload->path, ERR_INVALID_PAYLOAD);

	char	*_abs_path = join_path(_ctx->root_path, _payload->path);
	RETURN_IF_NULL(_abs_path, ERR_INTERNAL_MEMORY);

	FILE	*_file = os_file_open(_abs_path, "r");
	free(_abs_path);
	RETURN_IF_NULL(_file, ERR_OPERATION_FAILED);

	char	*_data = os_file_get_data(_file);
	fclose(_file);
	RETURN_IF_NULL(_data, ERR_OPERATION_FAILED);

	_payload->out_data = _data;
	_payload->out_len = strlen(_data);

	return (ERR_SUCCESS);
}

t_ErrorCode	cmd_file_write(t_Manager *manager, const t_Command *cmd)
{
	t_FileSystemCtx	*_ctx = manager->fs_ctx;
	RETURN_IF_NULL(_ctx, ERR_FS_CONTEXT_NOT_INITIALIZED);
	t_CmdWriteFile	*_payload = cmd->payload;
	RETURN_IF_NULL(_payload, ERR_INVALID_PAYLOAD);

	RETURN_IF_NULL(_payload->path, ERR_INVALID_PAYLOAD);
	RETURN_IF_NULL(_payload->data, ERR_INVALID_PAYLOAD);

	char	*_abs_path = join_path(_ctx->root_path, _payload->path);
	RETURN_IF_NULL(_abs_path, ERR_INTERNAL_MEMORY);

	FILE	*_file = os_file_open(_abs_path, "w");
	free(_abs_path);
	RETURN_IF_NULL(_file, ERR_OPERATION_FAILED);

	GOTO_IF_FALSE(
		os_file_write(_file, _payload->data),
		exit_close_file
	);

	fclose(_file);
	return (ERR_SUCCESS);

	/* GOTO EXIT */
	exit_close_file:
		fclose(_file);
		return (ERR_OPERATION_FAILED);
}
