#include <git2/repository.h>
#include <git2/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <git2.h>

#include <unistd.h>
#include <limits.h>
#include <sys/ioctl.h>

struct str_span {
	const char* str;
	const char* str_end;
};

struct str_span
shprompt_get_repo(const char* path) {
	struct str_span span;
	size_t path_length = strlen(path);
	char* repository_name = (char*)path + path_length;

	int depth = 0;
	while (repository_name >= path) {
		if (*repository_name == '/') depth++;
		if (depth == 3) break;
		repository_name--;
	}

	repository_name++;

	span.str = repository_name;
	span.str_end = path + path_length - sizeof(".git");

	return span;
}
struct str_span
shprompt_get_directory(const char* path) {
	struct str_span span;
	size_t path_length = strlen(path);
	char* repository_name = (char*)path + path_length;

	int depth = 0;
	while (repository_name >= path) {
		if (*repository_name == '/') depth++;
		if (depth == 2) break;
		repository_name--;
	}

	repository_name++;

	span.str = repository_name;
	span.str_end = path + path_length;

	return span;
}

int git_repo_prompt(const char* directory) {
	char padding[256];
	memset(padding, ' ', 256);

	git_buf repo_root = {0};
	if (git_repository_discover(&repo_root, directory, 0, NULL)) {

	} else {
		struct str_span repo_name = shprompt_get_repo(repo_root.ptr);
		struct winsize dimensions;

		ioctl(0, TIOCGWINSZ, &dimensions);


		git_repository *repo;
		if (git_repository_open(&repo,repo_root.ptr)) {
			// Current directory is not in any git repo

		}

		git_reference *ref;
		if (git_repository_head(&ref, repo)) {
			// HEAD is not set? Create initial commit!

		} else {
			const char* branch_name;
			git_branch_name(&branch_name, ref);

			size_t branch_name_width = strlen(branch_name);

			int repo_name_width = (repo_name.str_end - repo_name.str);
			printf("%.*s%.*s%s\n", repo_name_width, repo_name.str, dimensions.ws_col - repo_name_width - (int)branch_name_width, padding, branch_name);


			git_reference_free(ref);
		}
		git_buf_free(&repo_root);
	}
	return 0;
}

int main() {
	const char* user_color = "\e[32m";


	const char* user = getenv("USER");
	char* directory = getcwd(0, 0);
	char host[HOST_NAME_MAX + 1];
	struct str_span directory_span = shprompt_get_directory(directory);

	gethostname(host, HOST_NAME_MAX + 1);

	git_libgit2_init();

	git_repo_prompt(directory);
	printf("%s@%s %.*s $ ", user, host, (int)(directory_span.str_end - directory_span.str), directory_span.str);

	git_libgit2_shutdown();

	free(directory);
	return 0;
}
