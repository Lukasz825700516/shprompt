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

int git_repo_prompt(git_buf* repo_root) {
	char padding[256];
	memset(padding, ' ', 256);

	struct str_span repo_name = shprompt_get_repo(repo_root->ptr);
	struct winsize dimensions;

	ioctl(0, TIOCGWINSZ, &dimensions);

	git_repository *repo;
	if (git_repository_open(&repo, "./")) {
		fprintf(stderr, "Failed to get repo!\n");
		exit(EXIT_FAILURE);
	}

	git_reference *ref;
	if (git_repository_head(&ref, repo)) {
		fprintf(stderr, "Failed to get ref!\n");
		exit(EXIT_FAILURE);
	}


	const char* branch_name;
	git_branch_name(&branch_name, ref);

	size_t branch_name_width = strlen(branch_name);

	int repo_name_width = (repo_name.str_end - repo_name.str);
	printf("%.*s%.*s%s\n", repo_name_width, repo_name.str, dimensions.ws_col - repo_name_width - (int)branch_name_width, padding, branch_name);


	git_reference_free(ref);

	return 0;
}

int main() {
	const char* user_color = "\e[32m";


	const char* user = getenv("USER");
	const char* directory = getenv("PWD");
	git_buf repo_root = {0};
	char host[HOST_NAME_MAX + 1];
	struct str_span directory_span = shprompt_get_directory(directory);


	gethostname(host, HOST_NAME_MAX + 1);

	git_libgit2_init();

	int error = git_repository_discover(&repo_root, directory, 0, NULL);


	// print GIT's part of prompt
	const int in_git_repo = repo_root.size > 0;
	if (in_git_repo) {
		git_repo_prompt(&repo_root);
	}


	printf("%s@%s %.*s $ ", user, host, (int)(directory_span.str_end - directory_span.str), directory_span.str);

	git_buf_free(&repo_root);
	git_libgit2_shutdown();
	return 0;
}
