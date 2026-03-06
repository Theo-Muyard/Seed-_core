#include "common/memory.h"
#include "systems/filesystem/fs_vfs.h"

// +===----- Resolving functions -----===+ //

char		*vfs_get_dir_relative_path(const t_Directory *dir)
{
	RETURN_IF_NULL(dir, NULL);

	t_Directory	*_tmp = (t_Directory *)dir;
	size_t	_size = 1;

	while (_tmp)
	{
		RETURN_IF_NULL(_tmp->dirname, NULL);

		_size += strlen(_tmp->dirname) + 1;
		_tmp = _tmp->parent;
	}

	char	*path = malloc(_size * sizeof(char));
	RETURN_IF_NULL(path, NULL);

	path[_size - 1] = '\0';
	_tmp = (t_Directory *)dir;
	size_t	_start = _size - 1;
	while (_tmp)
	{
		size_t	_len = strlen(_tmp->dirname);
		_start -= _len;
		memcpy(path + _start, _tmp->dirname, _len);
		path[--_start] = '/';
		_tmp = _tmp->parent;
	}

	return (path);
}

char		*vfs_get_file_relative_path(const t_File *file)
{
	RETURN_IF_NULL(file, NULL);

	char	*path = vfs_get_dir_relative_path(file->parent);
	RETURN_IF_NULL(path, NULL);

	size_t	_path_len = strlen(path);
	size_t	_filename_len = strlen(file->filename);
	size_t	_size = _path_len + 1 + _filename_len + 1;

	char		*_tmp = realloc(path, _size * sizeof(char));
	GOTO_IF_NULL(_tmp, exit_free_path);

	path = _tmp;
	path[_path_len++] = '/';
	memcpy(path + _path_len, file->filename, _filename_len);
	path[_path_len + _filename_len] = '\0';

	return (path);

	/* GOTO EXIT */
	exit_free_path:
		return (free(path), NULL);
}

// +===----- Directory functions -----===+ //

t_Directory	*vfs_dir_create(t_Directory *parent, const char *dirname)
{
	RETURN_IF_NULL(dirname, NULL);

	t_Directory	*dir = malloc(sizeof(t_Directory));
	RETURN_IF_NULL(dir, NULL);

	dir->dirname = ft_strdup(dirname);
	GOTO_IF_NULL(dir->dirname, exit_free_dir);

	vfs_add_subdir_to_dir(parent, dir);
	dir->parent = parent;
	
	dir->files = NULL;
	dir->files_count = 0;
	dir->files_capacity = 0;

	dir->subdirs = NULL;
	dir->subdirs_count = 0;
	dir->subdirs_capacity = 0;

	return (dir);

	/* GOTO EXIT */
	exit_free_dir:
		return (free(dir), NULL);
}

void		vfs_dir_destroy(t_Directory *dir)
{
	if (NULL == dir)
		return ;

	for (size_t _i = 0; _i < dir->files_count; _i++)
		vfs_file_destroy(dir->files[_i]);	
	
	for (size_t _i = 0; _i < dir->subdirs_count; _i++)
		vfs_dir_destroy(dir->subdirs[_i]);	

	free(dir->dirname);
	dir->dirname = NULL;
	free(dir->files);
	dir->files = NULL;
	free(dir->subdirs);
	dir->subdirs = NULL;
	free(dir);
}

bool		vfs_dir_rename(t_Directory *dir, const char *dirname)
{
	RETURN_IF_NULL(dir, false);
	RETURN_IF_NULL(dirname, false);

	if (strcmp(dirname, dir->dirname) == 0)
		return (false);

	char	*_new_dirname = ft_strdup(dirname);
	RETURN_IF_NULL(_new_dirname, false);

	free(dir->dirname);
	dir->dirname = _new_dirname;

	return (true);
}

// +===----- File functions-----===+ //

t_File	*vfs_file_create(t_Directory *parent, const char *filename)
{
	RETURN_IF_NULL(filename, NULL);

	t_File	*file = malloc(sizeof(t_File));
	RETURN_IF_NULL(file, NULL);

	file->filename = ft_strdup(filename);
	file->parent = NULL;
	GOTO_IF_NULL(file->filename, exit_free_file);

	vfs_add_file_to_dir(parent, file);

	return (file);

	/* GOTO EXIT */
	exit_free_file:
		return (free(file), NULL);
}

void		vfs_file_destroy(t_File *file)
{
	if (NULL == file)
		return ;

	free(file->filename);
	free(file);
}

bool		vfs_add_file_to_dir(t_Directory *dir, t_File *file)
{
	RETURN_IF_NULL(dir, NULL);
	RETURN_IF_NULL(file, NULL);

	if (dir->files_capacity == 0)
	{
		dir->files = malloc(FILE_ALLOC * sizeof(t_File *));
		RETURN_IF_NULL(dir->files, false);

		dir->files_capacity = FILE_ALLOC;
	}

	if (dir->files_count >= dir->files_capacity)
	{
		t_File	**_tmp = realloc(
			dir->files,
			(FILE_ALLOC + dir->files_capacity) * sizeof(t_File *)
		);
		RETURN_IF_NULL(_tmp, false);

		dir->files = _tmp;
		dir->files_capacity += FILE_ALLOC;
	}

	file->parent = dir;
	dir->files[dir->files_count] = file;
	dir->files_count++;

	return (true);
}

bool		vfs_remove_file_to_dir(t_Directory *dir, t_File *file)
{
	RETURN_IF_NULL(dir, NULL);
	RETURN_IF_NULL(file, NULL);

	size_t	_i = 0;
	while (_i < dir->files_count)
	{
		if (dir->files[_i] == file)
		{
			memmove(
				dir->files + _i,
				dir->files + _i + 1,
				(dir->files_count - _i - 1) * sizeof(t_File *)
			);
			dir->files[_i] = NULL;
			dir->files_count--;
			file->parent = NULL;
			return (true);
		}
		_i++;
	}

	return (false);
}

t_File	*vfs_file_find(t_Directory *parent, const char *filename)
{
	RETURN_IF_NULL(parent, NULL);
	RETURN_IF_NULL(filename, NULL);

	size_t	_i = 0;
	while (_i < parent->files_count)
	{
		if (strcmp(filename, parent->files[_i]->filename) == 0)
			return (parent->files[_i]);
		_i++;
	}

	return (NULL);
}

t_File	*vfs_file_resolve(t_Directory *root, const char *path)
{
	RETURN_IF_NULL(root, NULL);
	RETURN_IF_NULL(path, NULL);

	char	*_cpy = ft_strdup(path);
	RETURN_IF_NULL(_cpy, NULL);

	char	*_slash = strrchr(_cpy, '/');
	if (NULL == _slash)
	{
		t_File	*file = vfs_file_find(root, _cpy);
		return (free(_cpy), file);
	}

	*_slash = '\0';
	t_Directory	*_dir = vfs_subdir_resolve(root, _cpy);
	GOTO_IF_NULL(_dir, exit_free_cpy);

	t_File	*file = vfs_file_find(_dir, _slash + 1);
	free(_cpy);

	return (file);

	/* GOTO EXIT */
	exit_free_cpy:
		return (free(_cpy), NULL);
}

bool		vfs_file_move(t_Directory *dst, t_Directory *src, t_File *file)
{
	RETURN_IF_NULL(dst, false);
	RETURN_IF_NULL(src, false);
	RETURN_IF_NULL(file, false);

	RETURN_IF_FALSE(vfs_remove_file_to_dir(src, file), false);
	RETURN_IF_FALSE(vfs_add_file_to_dir(dst, file), false);

	return (true);
}

bool		vfs_file_rename(t_File *file, const char *filename)
{
	RETURN_IF_NULL(file, false);
	RETURN_IF_NULL(filename, false);

	if (strcmp(filename, file->filename) == 0)
		return (false);

	char	*_new_filename = ft_strdup(filename);
	RETURN_IF_NULL(_new_filename, false);

	free(file->filename);
	file->filename = _new_filename;

	return (true);
}

bool		vfs_file_is_in_dir(t_Directory *dir, t_File *file)
{
	RETURN_IF_NULL(dir, NULL);
	RETURN_IF_NULL(file, NULL);

	size_t	_i = 0;
	while (_i < dir->files_count)
	{
		if (dir->files[_i] == file)
			return (true);
		_i++;
	}

	return (false);
}

// +===----- Subdir functions -----===+ //

bool		vfs_add_subdir_to_dir(t_Directory *dir, t_Directory *subdir)
{
	if (dir->subdirs_capacity == 0)
	{
		dir->subdirs = malloc(DIR_ALLOC * sizeof(t_Directory *));
		RETURN_IF_NULL(dir->subdirs, false);

		dir->subdirs_capacity = DIR_ALLOC;
	}

	if (dir->subdirs_count >= dir->subdirs_capacity)
	{
		t_Directory	**_tmp = realloc(
			dir->subdirs,
			(DIR_ALLOC + dir->subdirs_capacity) * sizeof(t_Directory *)
		);
		RETURN_IF_NULL(_tmp, false);
		
		dir->subdirs = _tmp;
		dir->subdirs_capacity += DIR_ALLOC;
	}

	subdir->parent = dir;
	dir->subdirs[dir->subdirs_count] = subdir;
	dir->subdirs_count++;

	return (true);
}

bool		vfs_remove_subdir_to_dir(t_Directory *dir, t_Directory *subdir)
{
	RETURN_IF_NULL(dir, NULL);
	RETURN_IF_NULL(subdir, NULL);

	size_t	_i = 0;
	while (_i < dir->subdirs_count)
	{
		if (dir->subdirs[_i] == subdir)
		{
			memmove(
				dir->subdirs + _i,
				dir->subdirs + _i + 1,
				(dir->subdirs_count - _i - 1) * sizeof(t_Directory *)
			);
			dir->subdirs[_i] = NULL;
			dir->subdirs_count--;
			subdir->parent = NULL;
			return (true);
		}
		_i++;
	}
	return (false);
}

t_Directory	*directory_find_subdir(t_Directory *parent, const char *dirname)
{
	RETURN_IF_NULL(parent, NULL);
	RETURN_IF_NULL(dirname, NULL);

	size_t	_i = 0;
	while (_i < parent->subdirs_count)
	{
		if (strcmp(dirname, parent->subdirs[_i]->dirname) == 0)
			return (parent->subdirs[_i]);
		_i++;
	}

	return (NULL);
}

t_Directory	*directory_resolve(t_Directory *root, const char *path)
{
	RETURN_IF_NULL(root, NULL);
	RETURN_IF_NULL(path, NULL);

	char	**_tokens = ft_split(path, '/');
	RETURN_IF_NULL(_tokens, NULL);

	size_t	_i = 0;
	t_Directory	*current = root;
	while (_tokens[_i])
	{
		if (strcmp(_tokens[_i], ".") == 0)
		{
			_i++;
			continue;
		}
		else if (strcmp(_tokens[_i], "..") == 0)
		{
			current = current->parent ? current->parent : current;
			_i++;
			continue;
		}

		current = directory_find_subdir(current, _tokens[_i]);
		GOTO_IF_NULL(current, exit_free_tokens);
		_i++;
	}

	ft_free_split(_tokens);
	return (current);

	/* GOTO EXIT */
	exit_free_tokens:
		return (ft_free_split(_tokens), NULL);
}

bool		directory_subdir_move(t_Directory *dst, t_Directory *src, t_Directory *subdir)
{
	RETURN_IF_NULL(dst, false);
	RETURN_IF_NULL(src, false);
	RETURN_IF_NULL(subdir, false);

	RETURN_IF_FALSE(vfs_remove_subdir_to_dir(src, subdir), false);
	RETURN_IF_FALSE(vfs_add_subdir_to_dir(dst, subdir), false);

	return (true);
}

bool		directory_contains_subdir(t_Directory *dir, t_Directory *subdir)
{
	RETURN_IF_NULL(dir, false);
	RETURN_IF_NULL(subdir, false);

	size_t	_i = 0;
	while (_i < dir->subdirs_count)
	{
		if (dir->subdirs[_i] == subdir)
			return (true);
		_i++;
	}

	return (false);
}
