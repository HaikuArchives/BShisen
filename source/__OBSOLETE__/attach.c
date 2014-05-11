#include <stdio.h>
#include <kernel/fs_attr.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <ansi_parms.h>



int main(int argc, char **argv)
{
int fd;
DIR *d;
dirent_t *ent;
attr_info info;  

	d = fs_open_attr_dir(argv[1]);

	if (d)
	{
		fd = open(argv[1], O_RDONLY);
		if (fd == -)
		{
			fprintf(stderr, "Couldn't open file!\n");
			exit(-1);
		}





	}




}
