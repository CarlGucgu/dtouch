#include <stdio.h>
#include <io.h>
#include <stdlib.h>
#include <fcntl.h>
#include <time.h>
#include <dir.h>
#include <dos.h>

int convert_digits(const char *str, int offset, int len) {
    char buf[5];
    int i;
    for (i = 0; i < len; i++) {
        buf[i] = str[offset + i];
    }
    buf[len] = '\0';
    return atoi(buf);
}


int get_pc_time(struct ftime *t1, const char *time_str){
	/* They told me to use pointers, whatever*/
	time_t now_t;
	struct tm *now;
	int year, month, day, hour, min, sec;

	if (time_str == NULL){
		/*See dos manual about time, year from 1980*/
		/*Dos Sec ticks every 2 sec*/
		time(&now_t);
		now = localtime(&now_t);

		(*t1).ft_tsec = (*now).tm_sec /2;
		(*t1).ft_min = (*now).tm_min;
		(*t1).ft_hour = (*now).tm_hour;
		(*t1).ft_day = (*now).tm_mday;
		(*t1).ft_month = (*now).tm_mon + 1;

		if ((*now).tm_year >= 80){
			(*t1).ft_year = (*now).tm_year - 80;
		} else {
			(*t1).ft_year=0;
				}

			/* I tried nest a shit, but tcc sucks */

	} else {
		if (strlen(time_str) != 14) {
            printf("Error: Time format must be YYYYMMDDHHMMSS (14 digits)\n");
            return -1;
        }

		/*Oh fuck fault tolerance*/
		year  = convert_digits(time_str, 0, 4);
    	month = convert_digits(time_str, 4, 2);
    	day   = convert_digits(time_str, 6, 2);
    	hour  = convert_digits(time_str, 8, 2);
    	min   = convert_digits(time_str, 10, 2);
    	sec   = convert_digits(time_str, 12, 2);
	/*Ugly offset written by people, dirty 1980*/
		if (year < 1980) {
            printf("ERROR: Year cannot be earlier than 1980 for DOS!\n");
            return -1;
        }

	
		(*t1).ft_tsec  = sec / 2;
        (*t1).ft_min   = min;
        (*t1).ft_hour  = hour;
        (*t1).ft_day   = day;
        (*t1).ft_month = month;
        (*t1).ft_year  = year - 1980;
		/*Maybe I'll delete this stuff next time*/
	}

	return 0;

	}

int touch_afile(const char *filename, const char *custom_time){
	int handle = 1;
	int status = -1;
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
    /* （use get_pc_time） */
    if (get_pc_time(&ft, custom_time) != 0) {
        goto cleanup;
    }
	/*So confusing Nestting oh cock */

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
    int file_start_index = 1;
    char *custom_time = NULL;

	if (argc < 2){
		printf("DTOUCH, A STUPID DOS TOUCH\n") ;
		printf("USEAGE: TOUCH [-t YYYYMMDDHHMMSS] <filename> \n");
		return 1;
		}

    if (strcmp(argv[1], "-t") == 0) {
        if (argc < 4) {
            printf("ERROR: -t option requires time string and filename\n");
            printf("USEAGE: TOUCH [-t YYYYMMDDHHMMSS] <filename> \n");
            return 1;
        }
        custom_time = argv[2];
        file_start_index = 3; 
    }
	    if (strcmp(argv[1], "-h") == 0) {
        if (argc < 4) {
            printf("USEAGE: TOUCH [-t YYYYMMDDHHMMSS] <filename> \n");
            return 1;
        }
        custom_time = argv[2];
        file_start_index = 3; 
    }
	/*Again, maybe i'll make it less bloat*/

	for (i = 1; i < argc; i++){
		touch_afile(argv[i], custom_time);
		}
	return 0;
	}