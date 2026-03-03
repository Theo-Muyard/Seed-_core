/**
 * @file "systems/filesystem/fs_vfs.h"
 * 
 * @brief Managing virtual files systems.
*/

#ifndef SEED_FILESYSTEM_VFS_H
# define SEED_FILESYSTEM_VFS_H

# include "dependency.h"

# define FILE_ALLOC 32
# define DIR_ALLOC 32

// +===----- Types -----===+ //

/* A file in the filesystem */
typedef struct	s_File
{
	char				*filename;	/* The filename */
	struct s_Directory	*parent;	/* The parent directory */
}	t_File;

/* A directory in the filesystem */
typedef struct	s_Directory
{
	char				*dirname;	/* The dirname */
	struct s_Directory	*parent;	/* The parent directory */

	t_File				**files;	/* The files his contains */
	size_t				files_count;	/* The files count */
	size_t				files_capacity;	/* The file capacity */

	struct s_Directory	**subdirs;	/* The subdir his contains */
	size_t				subdirs_count;	/* The subdir count */
	size_t				subdirs_capacity;	/* The subdir capacity */
}	t_Directory;

// +===----- Resolving functions -----===+ //

/**
 * @brief Retrieve the relative path of the folder,
 * 		relative to the root.
 * 
 * @param dir The folder must not be NULL.
 * 
 * @retval The newly allocated relative path.
 * @retval NULL if `dir` is NULL, if allocation fails or
 * 		an error occurred.
 * 
 * @warning Caller must free returned pointer with `free()`.
*/
char		*vfs_get_dir_relative_path(const t_Directory *dir);

/**
 * @brief Retrieve the relative path of the file,
 * 		relative to the root.
 * 
 * @param file The file must not be NULL.
 * 
 * @retval The newly allocated relative path.
 * @retval NULL if `file` is NULL, if allocation fails or
 * 		an error occurred.
 * 
 * @warning Caller must free returned pointer with `free()`.
*/
char		*vfs_get_file_relative_path(const t_File *file);

// +===----- Directory functions -----===+ //

/**
 * @brief Creates a new empty directory.
 * 
 * @param parent The parent can be NULL.
 * @param dirname The dirname must not be NULL.
 * 
 * @retval The newly allocated directory.
 * @retval NULL if `dirname` is NULL, if allocation fails or
 * 		an error occurred.
 * 
 * @warning Caller must free returned pointer with `vfs_directory_destroy()`.
*/
t_Directory	*vfs_directory_create(t_Directory *parent, const char *dirname);

/**
 * @brief Destroys the directory.
 * 
 * @param dir The directory must not be NULL.
*/
void		vfs_directory_destroy(t_Directory *dir);

// +===----- File functions -----===+ //

/**
 * @brief Creates a new empty file.
 * 
 * @param parent The parent can be NULL.
 * @param filename The filename must not be NULL.
 * 
 * @retval The newly allocated file.
 * @retval NULL if `filename` is NULL, if allocation fails or
 * 		an error occurred.
 * 
 * @warning Caller must free returned pointer with `vfs_file_destroy()`.
*/
t_File		*vfs_file_create(t_Directory *parent, const char *filename);

/**
 * @brief Destroys the file.
 * 
 * @param file The file must not be NULL.
*/
void		vfs_file_destroy(t_File *file);

/**
 * @brief Add the file to the folder.
 * 
 * @param dir The directory must not be NULL.
 * @param file The file must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dir` or `file` is NULL or an error occurred.
*/
bool		vfs_add_file_to_dir(t_Directory *dir, t_File *file);

/**
 * @brief Remove the file to the folder.
 * 
 * @param dir The directory must not be NULL.
 * @param file The file must not be NULL.
 * 
 * @retval TRUE for success.
 * @retval FALSE if `dir` or `file` is NULL or an error occurred.
*/
bool		vfs_remove_file_to_dir(t_Directory *dir, t_File *file);

/**
 * @brief Search for a file by its filename ONLY in its parent directory.
 * 
 * @param parent The parent must not be NULL.
 * @param filename The filename must not be NULL.
 * 
 * @retval The file.
 * @retval NULL if `parent` or `filename` is NULL, if the file was not found
 * 		or an error occurred.
*/
t_File		*vfs_find_file(t_Directory *parent, const char *filename);

// TODO: Continuer a partir d'ici

/**
 * @brief Resolve a relative path.
 * @param root The root directory.
 * @param path The relative path of the file.
 * @return The file if it was find or NULL.
*/
t_File		*file_resolve(t_Directory *root, const char *path);

/**
 * @brief Move a file in src to dst directory.
 * @param dst The destination directory.
 * @param src The source directory.
 * @param file The file that will be moved.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_file_move(t_Directory *dst, t_Directory *src, t_File *file);

/**
 * @brief Rename a file.
 * @param subdir The file that will be renamed.
 * @param filename The new filename
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_file_rename(t_File *file, const char *filename);

/**
 * @brief Check if the directory contains a specific file.
 * @param dir The directory that contains files and sub directory.
 * @param file The file.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_contains_file(t_Directory *dir, t_File *file);

// +===----- Sub directory -----===+ //

/**
 * @brief Add the given sub directory to the directory.
 * @param dir The directory that contains files.
 * @param subdir The sub directory that will be added.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_subdir_add(t_Directory *dir, t_Directory *subdir);

/**
 * @brief Remove the given sub directory.
 * @param dir The directory that contains files.
 * @param file The subdirectory that will be removed.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_subdir_remove(t_Directory *dir, t_Directory *subdir);

/**
 * @brief Find a sub directory by its name in the given directory.
 * @param parent The parent directory of the subdir that contains files and sub directory.
 * @param dirname The name of the sub directory.
 * @return The subdir or NULL if not found.
*/
t_Directory	*directory_find_subdir(t_Directory *parent, const char *dirname);

/**
 * @brief Resolve a relative path.
 * @param root The root directory.
 * @param path The relative path of the dir.
 * @return The dir if it was find or NULL.
*/
t_Directory	*directory_resolve(t_Directory *root, const char *path);

/**
 * @brief Move a sub directory to src in dst directory.
 * @param dst The destination directory.
 * @param src The source directory.
 * @param subdir The sub directory that will be moved.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_subdir_move(t_Directory *dst, t_Directory *src, t_Directory *subdir);

/**
 * @brief Rename a sub directory.
 * @param subdir The sub directory that will be renamed.
 * @param dirname The new dirname
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_subdir_rename(t_Directory *dir, const char *dirname);

/**
 * @brief Check if the directory contains a specific sub directory.
 * @param dir The directory that contains files and sub directory.
 * @param subdir The subdir.
 * @return TRUE for success or FALSE if an error occured.
*/
bool		directory_contains_subdir(t_Directory *dir, t_Directory *subdir);

#endif