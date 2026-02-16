# include "tools.h"
# include <dirent.h>

// +===----- Test Utilities -----===+ //

void	print_section(const char *section)
{
	printf("\n%s=== %s ===%s\n", BLUE, section, WHITE);
}

void	print_success(const char *message)
{
	printf("%s✓ %s%s\n", GREEN, message, WHITE);
}

void	print_error(const char *message)
{
	printf("%s✗ %s%s\n", RED, message, WHITE);
}

void	print_status(int status)
{
	if (0 == status)
	{
		printf("\n%s╔════════════════════════════════════╗%s\n", GREEN, WHITE);
		printf("%s║         ALL TESTS PASSED ✓         ║%s\n", GREEN, WHITE);
		printf("%s╚════════════════════════════════════╝%s\n", GREEN, WHITE);
	}
	else
	{
		printf("\n%s╔════════════════════════════════════╗%s\n", RED, WHITE);
		printf("%s║         SOME TESTS FAILED ✗        ║%s\n", RED, WHITE);
		printf("%s╚════════════════════════════════════╝%s\n", RED, WHITE);
	}
	printf("\n");
}

int		assert_error_code(t_ErrorCode got, t_ErrorCode expected, const char *message)
{
	if (got != expected)
	{
		printf(
			"%s✗ %s (got=%d expected=%d)%s\n",
			RED,
			message,
			(int)got,
			(int)expected,
			WHITE
		);
		return (1);
	}
	print_success(message);
	return (0);
}

char	*test_tmpdir_create(const char *prefix)
{
	char	*path;
	size_t	prefix_len;

	if (NULL == prefix)
		return (NULL);
	prefix_len = strlen(prefix);
	path = malloc(prefix_len + sizeof("_XXXXXX"));
	if (NULL == path)
		return (NULL);
	memcpy(path, prefix, prefix_len);
	memcpy(path + prefix_len, "_XXXXXX", sizeof("_XXXXXX"));
	if (NULL == mkdtemp(path))
		return (free(path), NULL);
	return (path);
}

static bool	remove_tree_recursive(const char *path)
{
	DIR				*dir;
	struct dirent	*entry;
	char			*entry_path;
	size_t			base_len;
	size_t			name_len;
	struct stat		st;

	dir = opendir(path);
	if (NULL == dir)
		return (false);
	base_len = strlen(path);
	entry = readdir(dir);
	while (entry)
	{
		if (0 == strcmp(entry->d_name, ".") || 0 == strcmp(entry->d_name, ".."))
		{
			entry = readdir(dir);
			continue ;
		}
		name_len = strlen(entry->d_name);
		entry_path = malloc(base_len + 1 + name_len + 1);
		if (NULL == entry_path)
			return (closedir(dir), false);
		memcpy(entry_path, path, base_len);
		entry_path[base_len] = '/';
		memcpy(entry_path + base_len + 1, entry->d_name, name_len);
		entry_path[base_len + 1 + name_len] = '\0';
		if (-1 == stat(entry_path, &st))
			return (free(entry_path), closedir(dir), false);
		if (S_ISDIR(st.st_mode))
		{
			if (false == remove_tree_recursive(entry_path))
				return (free(entry_path), closedir(dir), false);
			if (-1 == rmdir(entry_path))
				return (free(entry_path), closedir(dir), false);
		}
		else
		{
			if (-1 == remove(entry_path))
				return (free(entry_path), closedir(dir), false);
		}
		free(entry_path);
		entry = readdir(dir);
	}
	closedir(dir);
	return (true);
}

bool	test_tmpdir_remove(const char *path)
{
	if (NULL == path)
		return (false);
	if (false == remove_tree_recursive(path))
		return (false);
	if (-1 == rmdir(path))
		return (false);
	return (true);
}
