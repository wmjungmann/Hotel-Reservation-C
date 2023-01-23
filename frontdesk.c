#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

/*file for currnet guest information*/
#define FILE_NAME "guests"
/*max length of the name for a guest*/
#define NAME_LEN_MAX 21
/*total number of rooms */
#define NUM_ROOMS 10

/* print guest name in each room*/
void print_guests()
{
	int fd;
	off_t offset;
	ssize_t nread;

	int i;
	char namebuf[NAME_LEN_MAX+1];

	if((fd = open(FILE_NAME, O_RDONLY)) == -1)
	{
		printf("fail to open file\n");	exit(0);
	}

	for(i=1; i<=NUM_ROOMS; i++)
	{

		offset = (i - 1) * NAME_LEN_MAX;

		if(lseek(fd, offset, SEEK_SET) == -1)
		{
			printf("faile to seek position\n"); exit(0);
		}

		nread = read(fd, namebuf, NAME_LEN_MAX-1);
		namebuf[NAME_LEN_MAX] = '\0';

		if(nread > 0)
		{
			printf("\nroom # %d: %s", i, namebuf);
		}
		else
		{
			printf("\nroom # %d:", i);
		}

		memset(namebuf, 0, NAME_LEN_MAX);
	}
	
	printf("\n");
	close(fd);
}


int check_vacancies()
{
	int empty_room = 0;
	int smallest = 11;

	//add your code here
        int fd;
        off_t offset;
        ssize_t nread;

        int i;
        char namebuf[NAME_LEN_MAX+1];

        if((fd = open(FILE_NAME, O_RDWR)) == -1)
        {
                printf("fail to open file\n");  exit(0);
        }


        for(i=1; i<=NUM_ROOMS; i++)
        {

                offset = (i - 1) * NAME_LEN_MAX;

                if(lseek(fd, offset, SEEK_SET) == -1)
                {
                        printf("faile to seek position\n"); exit(0);
                }

                nread = read(fd, namebuf, NAME_LEN_MAX);
                namebuf[NAME_LEN_MAX] = '\0';
                if(namebuf[0] == 32)
                {
			if( i < smallest)
			{
				smallest = i;
			}
			
			empty_room += 1;
			
                }

                memset(namebuf, 0, NAME_LEN_MAX);
        }
	
        close(fd);
	

	if(empty_room)
	{
		printf("\nthe number of empty rooms is %d\n",empty_room);
		return smallest;
	}
	else
	{
		printf("\nthere are no available rooms\n");
		return 0;
	}
}


int checkin_guest(char *guestname, int roomnum)
{
        off_t offset;
        ssize_t nread;

        int i;
        char namebuf[NAME_LEN_MAX+1];
	int flag = 0;
	int counter = 1;
	int fd, temp;

	if((fd = open(FILE_NAME, O_RDWR)) == -1){
		printf("Guest file fail to open");
	}
	
	if((temp = open("temp", O_CREAT | O_RDWR, 0644)) == -1){
		printf("Temp file fail to open");
	}

	if(roomnum > NUM_ROOMS)
	{
		printf("room # %d does not exist\n", roomnum);
		return -1;
	}
	
	for(i=1; i<=NUM_ROOMS; i++)
        {	
                offset = (i - 1) * NAME_LEN_MAX;

                if(lseek(fd, offset, SEEK_SET) == -1)
                {
                        printf("failed to seek position\n"); exit(0);
                }

                nread = read(fd, namebuf, NAME_LEN_MAX);
                if(roomnum == i){
			if(nread < 0){
                                printf("room # %d is occupied by %s", i, namebuf);				
        		}
			else{
				printf("room # %d is occupied by %s", i, namebuf);
                                strncpy(namebuf, guestname, NAME_LEN_MAX);
				namebuf[NAME_LEN_MAX-1] = '\n';
                                flag = 1;

			}
		}

		write(temp, namebuf, NAME_LEN_MAX);
                memset(namebuf, 0, NAME_LEN_MAX);
        }

			
       	


	remove(FILE_NAME);
	rename("temp", FILE_NAME);

        close(fd);
	close(temp);

	if(flag == 1)
	{
		return 0;
	}
	else
	{
		return -1;
	}
}


int checkout_guest(char *guestname)
{
        off_t offset;
        ssize_t nread;

        int i, roomnum;
        char namebuf[NAME_LEN_MAX+1];
        int flag = 0;
        int fd, temp;
	
	if((fd = open(FILE_NAME, O_RDWR)) == -1){
                printf("Guest file fail to open");
        }

        if((temp = open("temp", O_CREAT | O_RDWR, 0644)) == -1){
                printf("Temp file fail to open");
        }
	
	//Cycle through room numbers until searched name = name of guest
        for(i=1; i<=NUM_ROOMS; i++)
        {
                offset = (i - 1) * NAME_LEN_MAX;

                if(lseek(fd, offset, SEEK_SET) == -1)
                {
                        printf("failed to seek position\n"); exit(0);
                }

                nread = read(fd, namebuf, NAME_LEN_MAX);
		
		//If searched name = name of guest
                if(strcmp(guestname, namebuf) == 0){
                        flag = 1;
			memset(namebuf, 0, NAME_LEN_MAX);
			namebuf[NAME_LEN_MAX - 1] = '\n';
			roomnum = i;
                }

                write(temp, namebuf, NAME_LEN_MAX);
                memset(namebuf, 0, NAME_LEN_MAX);
        }

        remove(FILE_NAME);
        rename("temp", FILE_NAME);

        close(fd);
        close(temp);

	if(flag == 1)
	{
		return roomnum;
	}
	else
	{
		printf("guest %s does not exist in the room\n",guestname);
		return -1;
	}
}



int main(int argc, char *argv[])
{
	char guestname[NAME_LEN_MAX];
	int roomnum;
	int result;

	/*print out the current status*/
	print_guests();


	//check vacancies
	result = -1;
	if((result = check_vacancies()) > 0) 	
	{
		printf("the smallest number of empty rooms is %d\n", result);
	}

	//add guestname to the position of roomnum
	result = -1;
	memcpy(guestname, "test guest name", NAME_LEN_MAX);
	roomnum = 4;
	if((result = checkin_guest(guestname, roomnum)) != -1)
	{
		print_guests();
	}

	//remove guestname from where it actually is
	result = -1;
	memcpy(guestname, "test guest name", NAME_LEN_MAX);
	if((result = checkout_guest(guestname)) != -1)
	{
		print_guests();
	}



	return 0;
}


