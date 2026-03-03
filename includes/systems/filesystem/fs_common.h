/**
 * @file "systems/filesystem/fs_common.h"
 * 
 * @brief Includes all global dependencies to the
 * 		filesystem.
*/

#ifndef SEED_FILESYSTEM_COMMONL_H
# define SEED_FILESYSTEM_COMMONL_H

/**
 * @brief Join two paths.
 * 
 * @details The paths provided are not modified or free.
 * 		the new path is created from scratch.
 * 
 * @param base The base path must not be NULL.
 * @param path The path must not be NULL.
 * 
 * @retval The newly allocated joined path.
 * @retval NULL if `base` or `path` is NULL, if allocation fails
 * 		or an error occurred.
 * 
 * @warning Caller must free returned pointer with `free()`.
*/
char	*join_path(const char *base, const char *path);

#endif