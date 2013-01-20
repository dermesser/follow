# include <stdio.h>
# include <errno.h>
# include <string.h>
# include <stdlib.h>
# include <stdarg.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/inotify.h>
# include <time.h>

# define BUFSIZE 2048

void errexit(const char* str, ...)
{
    va_list args;

    va_start(args,str);
    vfprintf(stderr,str,args);
    va_end(args);

    exit(1);
}

int main(int argc, char** argv)
{
    int fd, notify, timestamps = 0;
    int i = 0;
    size_t readchars = 1, inotifysize = sizeof(struct inotify_event);
    const char* path;
    char buf[BUFSIZE], lastnewline;
    struct inotify_event evt;
    time_t current_time;
    struct tm* local;

    if ( argc < 2 )
	errexit("Usage: %s [-t] FILENAME\n",argv[0]);

    // Parse arguments
    while ( (i = getopt(argc,argv,"t")) > 0 )
    {
	if ( i == 't' )
	    timestamps = 1;
    }

    // Set file path
    path = argv[optind];

    if ( -1 == (fd = open(path,O_RDONLY)) )
	errexit("follow: %s\n",strerror(errno));

    if ( (notify = inotify_init()) == -1 )
	errexit("follow: %s\n",strerror(errno));

    // Read data being already in the file
    while ( readchars > 0 )
    {
	readchars = read(fd,buf,BUFSIZE-1);
	write(1,buf,readchars);

	buf[readchars] = 0;

	if ( strstr(buf,"\n") )
	    lastnewline = 1;
    }

    inotify_add_watch(notify,path,IN_MODIFY);

    while ( 1 )
    {
	read(notify,&evt,inotifysize); // Blocks until event

	if ( evt.mask == IN_MODIFY )
	{
	    while (0 < (readchars = read(fd,buf,BUFSIZE-1)))
	    {
		if ( lastnewline && timestamps ) // if last character was '\n' and we should produce timestamps (-t)
		{
		    time(&current_time);
		    local = localtime(&current_time);

		    printf("%02i.%02i.%4i %02i:%02i:%02i :: ",local->tm_mday,local->tm_mon+1,local->tm_year + 1900,local->tm_hour,local->tm_min,local->tm_sec);
		    fflush(stdout);

		    write(1,buf,readchars);

		    if ( buf[readchars-1] == '\n' )
			lastnewline = 1;
		    else
			lastnewline = 0;
		} else
		{
		    write(1,buf,readchars);

		    if ( buf[readchars-1] == '\n' )
			lastnewline = 1;
		    else
			lastnewline = 0;
		}

	    }
	}
    }
    return 0;
}
