#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <mcheck.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PATH_LEN 2048
#define MAP_SIZE (64*1024)

void save_self_maps()
{
	int fout = 0;
	int fin = 0;
	int ret = 0;
	char *mtrace_env = NULL;
	char path[PATH_LEN] = {0};
	char buf[MAP_SIZE] = {0};

	//fprintf(stderr, "%s work\n", __FUNCTION__);
	mtrace_env = getenv("MALLOC_TRACE");
	if (mtrace_env == NULL)
		return;

	memset(path, 0x00, sizeof(path));
	ret = snprintf(path, sizeof(path), "%s/catchleak_mtrace_%jd", mtrace_env, (intmax_t)getpid());
	if (ret < 0)
		return;

	ret = setenv("MALLOC_TRACE", path, 1);
	if (ret < 0)
		return;

	memset(path, 0x00, sizeof(path));
	ret = snprintf(path, sizeof(path), "%s/catchleak_maps_%jd", mtrace_env, (intmax_t)getpid());
	if (ret < 0)
		return;

	fout = open(path, O_CREAT | O_WRONLY | O_TRUNC, 0400);
	if (fout < 0)
		goto out;

	fin = open("/proc/self/maps", O_RDONLY);
	if (fin < 0)
		goto out;

	ret = read(fin, buf, sizeof(buf));
	if (ret < 0)
		goto out;

	ret = write(fout, buf, ret);
	if (ret < 0)
		goto out;

	ret = fsync(fout);
	if (ret < 0)
		goto out;

	fprintf(stderr, "save self maps to %s\n", path);

out :
	if (fin > 0)
		close(fin);

	if (fout > 0)
		close(fout);
}

static void __attribute__ ((constructor)) catchleak (void)
{
	fprintf(stderr, "%s work\n", __FUNCTION__);
	save_self_maps();
	mtrace();
}
