#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <dir.h>
#include <dos.h>

void get_pc_time(struct ftime *t1){
	time_t now_t;
	struct tm *now;
	time(&now_t);
	now = localtime(&now_t);
	/*See dos manual about time, year from 1980*/
	/*Dos Sec ticks every 2 sec*/

	(*t1).ft_tsec = (*now).tm_sec /2;
	(*t1).ft_min = (*now).tm_min;
	(*t1).ft_hour = (*now).tm_hour;
	(*t1).ft_day = (*now).tm_mday;
	(*t1).ft_month = (*now).tm_mon + 1;

	/* I tried nest a shit, but tcc sucks */

	if ((*now).tm_year >= 80){
		(*t1).ft_year = (*now).tm_year - 80;
		} else {
			(*t1).ft_year=0;
			}
	}

int touch_afile(const char *filename){
	int handle = 1;
	int status = -1;
	unsigned dos_err = 0;
	struct ftime ft;

/* 1. Readonly First (O_RDWR) */
/*Abandon: &handle/
/*TC works like an idiot for dos calls, so i use posix instead*/
    handle = open(filename, O_RDWR);

    if(handle == -1) {
	/* _dos_open ==2 (File Not Found) ==1 Failure */
		handle = creat(filename, 0);
        if (handle == -1) {
			printf("ERROR: Cannot create file %s\n", filename);
			goto cleanup;
		}
	    /*Abandon: File Not Found ; _dos_creat creat no file  (_A_NORMAL as a normal file property) */
	    /* I hate Macros caz i need to RTFM */
	    /*Abandon _A_NORMAL is just like xattr i guess*/
		printf("Created file: %s\n", filename);
	}else {
		printf("Updating timestamp: %s\n", filename);
	}

		/*Only goto knows me*/



    get_pc_time(&ft);
    if(setftime(handle, &ft) != 0) {
        printf("ERROR: Failed to set time for %s\n", filename);
        goto cleanup;
    }

    status = 0;

	/*fuck dos no mutex */
cleanup:
    /* if handle assigined and avaliable，do force quit ，no JFT deadlock */

    if (handle != -1) {
        close(handle);
		/*dosclose->posix close*/
    }
    return status;
}




int main(int argc, char *argv[]){
	int i;

	if (argc < 2){
		printf("DTOUCH, A STUPID DOS TOUCH\n") ;
		printf("USEAGE: TOUCH <filename> \n");
		return 1;
		}

	for (i = 1; i < argc; i++){
		touch_afile(argv[i]);
		}
	return 0;
	}