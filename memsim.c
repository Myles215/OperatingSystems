#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct {
    int pageNo;
    int modified;
} page;

enum repl { rando, fifo, lru, clock};
int createMMU(int);
int checkInMemory(int );
int allocateFrame(int );
page selectVictim(int, enum repl);
const int pageoffset = 12;            /* Page size is fixed to 4 KB */
int numFrames;


page* MMU; // page table structure
int* timeAdded; // stores when the frame was added
int* clockBit; // store the bits for the clock algo
int clockIndex = 0; // index to keep track of where we are along the clockbit array
int time = 0;
enum repl algo;

/* Creates the page table structure to record memory allocation */
int createMMU (int frames)
{
    // to do
    MMU = (page*) malloc(frames * sizeof(page));
    timeAdded = (int*) malloc(frames * sizeof(int));
    clockBit = (int*) malloc(frames * sizeof(int));

    for (int i = 0;i<frames;i++)
    {
        timeAdded[i] = -1;
        clockBit[i] = 0;
        MMU[i].pageNo = -1;
    }
    return 0;
}

/* Checks for residency: returns frame no or -1 if not found */
int checkInMemory(int page_number)
{
    int result = -1;

    // to do
    for (int i = 0;i<numFrames;i++)
    {
        if (page_number == MMU[i].pageNo)
        {   
            // by updating the frame on use to a new time, we can increase its priority
            // and not get it kicked out when we swap
            if (algo!=fifo){
                timeAdded[i] = time++;
            }
            clockBit[i]=1;
            return i;
        }
    }

    return result;
}

/* allocate page to the next free frame and record where it put it */
int allocateFrame(int page_number)
{

    int ret = 0;

    for (int i = 0;i<numFrames;i++)
    {
        if (timeAdded[i] == -1)
        {
            page newPage;
            newPage.pageNo = page_number;
            newPage.modified = 0;
            MMU[i] = newPage;
            timeAdded[i] = time++;
            clockBit[i]=1;
            ret = i;
            break;
        }
    }
    // to do
    return ret;
}

/* Selects a victim for eviction/discard according to the replacement algorithm,  returns chosen frame_no  */
page selectVictim(int page_number, enum repl mode)
{
    page victim;

    if (mode == lru)
    {

        int mini = 1000000000;
        int index = -1;
        // by updating the frame on last access, finding the minimum of timeAdded
        // will give us the least recently use for swap
        for (int i = 0;i<numFrames;i++)
        {
            if (timeAdded[i] < mini) 
            {
                mini = timeAdded[i];
                index = i;
            }
        }

        victim = MMU[index];
        timeAdded[index] = time++;
        page newP;
        newP.pageNo = page_number;
        newP.modified = 0;
        MMU[index] = newP;

    }
    else if (mode == rando)
    {
        int index = rand() % numFrames;

        timeAdded[index] = time++;
        MMU[index].pageNo = page_number;
        MMU[index].modified = 1;
        victim = MMU[index];
    }
    else if (mode == fifo){
        int min = 10000000;
        int index;
        // find the frame which came in first by finding min of timeAdded and swap
        for (int i = 0;i<numFrames;i++)
        {
            if (timeAdded[i] < min) 
            {
                min = timeAdded[i];
                index = i;
            }
        }
        victim = MMU[index];
        timeAdded[index] = time++;
        page newP;
        newP.pageNo = page_number;
        newP.modified = 0;
        MMU[index] = newP;

    }
    else if (mode == clock){
        /*
        move around the clock array like a circle until we find one which has value 0
        while we move around, if current value is 1, make it 0
        keep index as we circle through
        */
        int index = -1;
        int tempIndex = clockIndex;
        while (tempIndex < numFrames){
            if(clockBit[tempIndex]==0){
                index = tempIndex;
                clockIndex = (index+1>=numFrames)? 0: index+1;
                break;
            }
            else{
                clockBit[tempIndex]=0;
            }
            tempIndex+=1;
            if (tempIndex >= numFrames){
                tempIndex=0;
            }
        }
       
        if (index != -1){
            victim = MMU[index];
            timeAdded[index] = time++;
            page newP;
            newP.pageNo = page_number;
            newP.modified = 0;
            MMU[index] = newP;
            clockBit[index]=1;
        }
    }
    // to do 
    return (victim);
}

		
int main(int argc, char *argv[])
{
  
	char *tracename;
	int	page_number,frame_no, done ;
	int	do_line, i;
	int	no_events, disk_writes, disk_reads;
	int debugmode;
 	enum repl replace;
	int	allocated=0; 
	int	victim_page;
    unsigned address;
    char 	rw;
	page	Pvictim;
	FILE	*trace;


    if (argc < 5) 
    {
        printf( "Usage: ./memsim inputfile numberframes replacementmode debugmode \n");
        exit ( -1);
	}
	else 
    {
        tracename = argv[1];	
	    trace = fopen( tracename, "r");
	    if (trace == NULL ) 
        {
            printf( "Cannot open trace file %s \n", tracename);
            exit ( -1);
	    }
	    numFrames = atoi(argv[2]);
        if (numFrames < 1) {
            printf( "Frame number must be at least 1\n");
            exit ( -1);
        }
        if (strcmp(argv[3], "lru\0") == 0) replace = lru;
	    else if (strcmp(argv[3], "rand\0") == 0) replace = rando;
	    else if (strcmp(argv[3], "clock\0") == 0) replace = clock;		 
	    else if (strcmp(argv[3], "fifo\0") == 0) replace = fifo;		 
        else 
	    {
            printf( "Replacement algorithm must be rand/fifo/lru/clock  \n");
            exit ( -1);
	    }
        algo = replace;
        if (strcmp(argv[4], "quiet\0") == 0) debugmode = 0;
	    else if (strcmp(argv[4], "debug\0") == 0) debugmode = 1;
        else 
	    {
            printf( "Replacement algorithm must be quiet/debug  \n");
            exit ( -1);
	    }
	}
	
	done = createMMU (numFrames);
	if ( done == -1 ) 
    {
		printf( "Cannot create MMU" ) ;
		exit(-1);
    }
	no_events = 0;
	disk_writes = 0;
	disk_reads = 0;

    do_line = fscanf(trace,"%x %c",&address,&rw);
	while (do_line == 2)
	{
		page_number =  address >> pageoffset;
		frame_no = checkInMemory(page_number) ;    /* ask for physical address */

		if (frame_no == -1 )
		{
		    disk_reads++;			/* Page fault, need to load it into memory */
		    if (debugmode) 
            {
		        printf( "Page fault %8d \n", page_number) ;
            }

		    if (allocated < numFrames)  			/* allocate it to an empty frame */
		    {
                frame_no = allocateFrame(page_number);
		        allocated++;
            }
            else
            {
		        Pvictim = selectVictim(page_number, replace) ;   /* returns page number of the victim  */
		        frame_no = checkInMemory(page_number) ;    /* find out the frame the new page is in */
		        if (Pvictim.modified)           /* need to know victim page and modified  */
	 	        {
                    disk_writes++;			    
                    if (debugmode) printf( "Disk write %8d \n", Pvictim.pageNo) ;
		        }
		        else if (debugmode) printf( "Discard    %8d \n", Pvictim.pageNo) ;
		    }
		}

		if (rw == 'R')
        {
		    if (debugmode) printf( "reading    %8d \n", page_number) ;
		}
		else if (rw == 'W')
        {
		    // mark page in page table as written - modified  
		    if (debugmode) printf( "writting   %8d \n", page_number) ;
            MMU[frame_no].modified = 1;
		}
		else 
        {
		    printf( "Badly formatted file. Error on line %d\n", no_events+1); 
		    exit (-1);
		}

		no_events++;
        do_line = fscanf(trace,"%x %c",&address,&rw);
	}

	printf( "total memory frames:  %d\n", numFrames);
	printf( "events in trace:      %d\n", no_events);
	printf( "total disk reads:     %d\n", disk_reads);
	printf( "total disk writes:    %d\n", disk_writes);
	printf( "page fault rate:      %.4f\n", (float) disk_reads/no_events);
    return 0;
}
				