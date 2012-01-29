# include <stdio.h>
# include <stdlib.h>
# include <unistd.h>
# include <fcntl.h>
# include <sys/inotify.h>

void errexit(const char* str)
{
        fprintf(stderr,str);
        exit(1);
}

int main(int argc, char** argv)
{	
        int fd, notify;
        size_t readchars= 1, inotifysize = sizeof(struct inotify_event);
        const char* path = argv[1];
        char* buf = calloc(256,1);
        struct inotify_event evt;

        notify = inotify_init();

        if ( argc != 2 )
        {
                fprintf(stderr,"Usage: %s FILENAME\n",argv[0]);
                exit(1);
        }

        if ( -1 == (fd = open(path, O_RDONLY)) )
                        errexit("Couldn't open file!\n");
    
        while ( readchars > 0 )
        {
                readchars = read(fd,buf,255);
                write(1,buf,readchars);
        }

        inotify_add_watch(notify,path,IN_MODIFY);

        while ( 1 )
        {
                read(notify,&evt,inotifysize); // Blocks until event

                if ( evt.mask == IN_MODIFY )
                {
                        readchars = read(fd,buf,255);
//                        printf("Return: %i\n",readchars);
                        if ( readchars > 0 )
                                write(1,buf,readchars);
                }
        }

	return 0;
}
