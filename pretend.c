/**********************************************************************
 *
 *	pretend	:	2013-03-20
 *		emptymonkey's tool for altering identity.
 *
 *	This is a simple program for changing uids, primary gids, and the
 *	supplimentary gid lists, regardless of if those ids exist on 
 *	this system. (E.g. uids for users that don't appear in /etc/passwd.)
 *	This program just calls the underlying syscalls that are responsible
 *	for this kind of management. Nothing fancy.
 *
 *	Example usage:
 *
 *	empty@monkey:~$ id
 *	uid=1000(empty) gid=1000(empty) groups=1000(empty),24(cdrom),25(floppy),29(audio)
 *	empty@monkey:~$ sudo ./pretend 23:19:555:1037 id
 *	uid=23 gid=19 groups=555,1037
 *
 **********************************************************************/

#define _GNU_SOURCE

#include <errno.h>
#include <error.h>
#include <grp.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SUPPLEMENTAL_GROUPS 128

int main(int argc, char **argv){
	uid_t uid;
	gid_t gid, gid_list[MAX_SUPPLEMENTAL_GROUPS];

	int i;
	char *end_ptr;

	if(argc < 3){
		fprintf(stderr, "Usage: %s uid:gid[:gid1:gid2:...] COMMAND [ARGS]\n", argv[0]);
		exit(-1);
	}

	memset(gid_list, 0, sizeof(gid_list));
	uid = (uid_t) strtol(argv[1], &end_ptr, 10);
	gid = (gid_t) strtol(end_ptr + 1, &end_ptr, 10);
	if(errno){
		fprintf(stderr, "Usage: %s uid:gid[:gid1:gid2:...] COMMAND [ARGS]\n", argv[0]);
		exit(-1);
	}

	i = 0;
	while(*end_ptr != 0){
		gid_list[i++] = (gid_t) strtol(end_ptr + 1, &end_ptr, 10);
	}

	if(setgroups((size_t) i, gid_list)){
		error(-1, errno, "setgroups()");
	}

	if(setresgid(gid, gid, gid)){
		error(-1, errno, "setresgid()");
	}

	if(setresuid(uid, uid, uid)){
		error(-1, errno, "setresuid()");
	}

	execvp(argv[2], argv + 2);
	error(-1, errno, "execv()");

	return(-1);
}
