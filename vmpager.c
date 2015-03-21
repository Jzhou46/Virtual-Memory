//
// Jeffrey Zhou
// JZhou46
// CS 361
// Simulation of a Memory Paging System
// 10/19/14
//
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <fcntl.h>
#include <stdint.h>
#include <stdbool.h>
#include <sys/mman.h>
#include <math.h>
#define PAGETABLESIZE 256
#define NPROCESSES 256

int hitCounter = 0;//hit counter global variable
int missCounter = 0; //miss counter global variable
unsigned long int upperLimit = 0;//upper limit of MemoryAccess array

typedef struct {//Memory Access struct 
	uint8_t pid; 
	uint8_t page; 
} MemoryAccess; 

typedef struct { //Page table entry struct
	int frame; // -1 if not currently loaded 
	int pageHits; 
	int pageMisses; 
} PageTableEntry; 

typedef struct { //Frame table entry struct
	uint8_t pid; 
	uint8_t page; 
	bool vacant; 
	bool ref;
} FrameTableEntry; 

//Function to initialize the page table
void initPageTables(PageTableEntry pageTables[NPROCESSES][PAGETABLESIZE]){
	int i, j;
	hitCounter = 0;
	missCounter = 0;
	//Initializing every field in the pageTableEnty
	for(i = 0; i < PAGETABLESIZE; i++){
		for(j = 0; j < NPROCESSES; j++){
			pageTables[i][j].frame = -1;
			pageTables[i][j].pageHits = 0;
			pageTables[i][j].pageMisses = 0;
		}
	}
}

//Function to initialize the frame table
void initFrameTable(FrameTableEntry frameTableArray[], int frameTableSize){
	//Initializing every field in the frameTableEnty
	int i;
	for(i = 0; i < frameTableSize; i++){
		frameTableArray[i].pid = 0;
		frameTableArray[i].page = 0;
		frameTableArray[i].vacant = true;
		frameTableArray[i].ref = false;
	}
}

//Function to determine a random victim
int randomVictimPage(int frameTableSize){
	int victim = rand() % frameTableSize;
	return victim; 
}

//Function to display the final results of the simulation
void finalResults(int memoryAccesses){
	float hitPercentage = (float)hitCounter/memoryAccesses;
	float missPercentage = (float)missCounter/memoryAccesses;

	printf("-----FINAL RESULTS-----\n");
	printf("The total number of attempts = %d\n", memoryAccesses);
	printf("The total number of page hits = %d\n", hitCounter);
	printf("Percentage of page hits = %f\n", hitPercentage);
	printf("The total number of page misses = %d\n", missCounter);
	printf("Percentage of page misses = %f\n", missPercentage);
}

//Infinite Memory Access Algorithm
void IMA(MemoryAccess *memoryAccessArray, int memoryAccesses, PageTableEntry pageTables[NPROCESSES][PAGETABLESIZE]){
	// loop through the memory accesses in the input data file,
	//determine whether each one is a page hit or a page miss
	int i, pid, page;
	initPageTables(pageTables);//calling function to initialize everything
	printf("------------------------------\n");
	printf("  INFINITE MEMORY ALGORITHM   \n");
	printf("------------------------------\n");
	for(i = 0; i < memoryAccesses; i++){//memoryAccesses is the number that the user enters in the command line. If <= 0, runs until upperLimit
		pid = memoryAccessArray[i].pid;
		page = memoryAccessArray[i].page;
		if(pageTables[pid][page].frame > 0){//Page hit
			pageTables[pid][page].pageHits++;//Increment pageHits field
			hitCounter++;//Increment global hit counter
		}
		else{//Page miss
			pageTables[pid][page].pageMisses++;//Increment pageMisses field
			missCounter++;//Increment global miss counter
			pageTables[pid][page].frame = 1;//The page table has been accessed, now set frame number to 1
		}
	}
	finalResults(memoryAccesses);//Function to print out final results
}

//FIFO algorithm
void FIFO(MemoryAccess *memoryAccessArray, int memoryAccesses, PageTableEntry pageTables[NPROCESSES][PAGETABLESIZE], FrameTableEntry frameTableArray[], int frameTableSize){
	int victimPage = 0;//Victim Page variable
	int i, pid, page;
	initPageTables(pageTables);//call function to initialize Page Tables
	initFrameTable(frameTableArray, frameTableSize);//call function to initialize Frame Table
	printf("------------------------------\n");
	printf("        FIFO ALGORITHM        \n");
	printf("------------------------------\n");
	for(i = 0; i < memoryAccesses; i++){
		pid = memoryAccessArray[i].pid;
		page = memoryAccessArray[i].page; 
		if(pageTables[pid][page].frame > 0){//Page hit
			pageTables[pid][page].pageHits++;//Increment page hits
			hitCounter++;//Increment global hit counter
		}
		else{//Page miss
			pageTables[pid][page].pageMisses++;//Increment page misses
			missCounter++;//Increment global miss counter

			if(victimPage == frameTableSize){//If the victimPage index is equal to the size of the frame table
				victimPage = 0;//Reset it back to 0
			}
			if(frameTableArray[victimPage].vacant == false){//If not vacant
				pageTables[frameTableArray[victimPage].pid][frameTableArray[victimPage].page].frame = -1;
				//Load the page
				pageTables[pid][page].frame = victimPage; 
				frameTableArray[victimPage].pid = pid;
				frameTableArray[victimPage].page = page;
			}
			else{//else if vacant
				//load the page
				pageTables[pid][page].frame = victimPage;
				frameTableArray[victimPage].pid = pid;
                                frameTableArray[victimPage].page = page;
				frameTableArray[victimPage].vacant = false;
			}
			victimPage++;//next victim page
		}
	}
	finalResults(memoryAccesses);//Display final results
}

//Function to execute the random victim algorithm
void randAlg(MemoryAccess *memoryAccessArray, int memoryAccesses, PageTableEntry pageTables[NPROCESSES][PAGETABLESIZE], FrameTableEntry frameTableArray[], int frameTableSize){
	int i, pid, page;
	initPageTables(pageTables);//call function to initialize Page Tables
	initFrameTable(frameTableArray, frameTableSize);//call function to initialize Frame Table
	printf("------------------------------\n");
	printf("   RANDOM VICTIM ALGORITHM        \n");
	printf("------------------------------\n");
	for(i = 0; i < memoryAccesses; i++){
		int victimPage = randomVictimPage(frameTableSize);
		pid = memoryAccessArray[i].pid;
		page = memoryAccessArray[i].page;
		if(pageTables[pid][page].frame > 0){//Page hit
			pageTables[pid][page].pageHits++;//Increment page hits
			hitCounter++;//Increment global hit counter
		}
		else{//Page miss
			pageTables[pid][page].pageMisses++;//Increment page misses
			missCounter++;//Increment global miss counter

			if(victimPage == frameTableSize){//If the victimPage index is equal to the size of the frame table
				victimPage = 0;//Reset it back to 0
			}
			if(frameTableArray[victimPage].vacant == false){//If not vacant
				pageTables[frameTableArray[victimPage].pid][frameTableArray[victimPage].page].frame = -1;
				//Load the page
				pageTables[pid][page].frame = victimPage;
				frameTableArray[victimPage].pid = pid;
				frameTableArray[victimPage].page = page;
			}
			else{//else if vacant
				//load the page
				pageTables[pid][page].frame = victimPage;
				frameTableArray[victimPage].pid = pid;
				frameTableArray[victimPage].page = page;
				frameTableArray[victimPage].vacant = false;
			}
		}
	}
	finalResults(memoryAccesses);//Display final results
}

//Function to execute the second chance algorithm
void secondChance(MemoryAccess *memoryAccessArray, int memoryAccesses, PageTableEntry pageTables[NPROCESSES][PAGETABLESIZE], FrameTableEntry frameTableArray[], int frameTableSize){
	int i, pid, page;
	int victimPage = 0;//victim page variable
	initPageTables(pageTables);//call function to initialize Page Tables
	initFrameTable(frameTableArray, frameTableSize);//call function to initialize Frame Table
	printf("------------------------------\n");
	printf("   SECOND CHANCE ALGORITHM        \n");
	printf("------------------------------\n");
	for(i = 0; i < memoryAccesses; i++){
		pid = memoryAccessArray[i].pid;
		page = memoryAccessArray[i].page;
		if(pageTables[pid][page].frame > 0){//Page hit
			frameTableArray[pageTables[pid][page].frame].ref = true;
			pageTables[pid][page].pageHits++;//Increment page hits
			hitCounter++;//Increment global hit counter
		}
		else{//Page miss
			pageTables[pid][page].pageMisses++;//Increment page misses
			missCounter++;//Increment global miss counter

			if(victimPage == frameTableSize){//If the victimPage index is equal to the size of the frame table
				victimPage = 0;//Reset it back to 0
			}
			if(frameTableArray[victimPage].vacant == false){//If occupied
				while(frameTableArray[victimPage].ref == true){
					frameTableArray[victimPage].ref = false;
					victimPage++;//next victim page
					if(victimPage == frameTableSize){//If the victimPage index is equal to the size of the frame table
						victimPage = 0;//Reset it back to 0
					}
				}
				pageTables[frameTableArray[victimPage].pid][frameTableArray[victimPage].page].frame = -1;
				//load the page
				pageTables[pid][page].frame = victimPage;
				frameTableArray[victimPage].pid = pid;
				frameTableArray[victimPage].page = page;
			}
			else{//else if vacant
				//load the page
				pageTables[pid][page].frame = victimPage;
				frameTableArray[victimPage].pid = pid;
				frameTableArray[victimPage].page = page;
				frameTableArray[victimPage].vacant = false;
			}
			victimPage++;//next victim page
		}
	}
	finalResults(memoryAccesses);//Display final results
}

int main(int argc, char* argv[]){
	struct stat buf;
	int fd = 0;//File descriptor
	PageTableEntry pageTables[PAGETABLESIZE][NPROCESSES];//Page Table
	MemoryAccess *memoryAccessArray, memoryAccessEntry; 
	int i, j, memoryAccesses, frameTableSize; 
	printf("-------------------------\n");
	printf("     Jeffrey Zhou\n     Jzhou46\n");
	printf("-------------------------\n");
	printf("Here is what you entered in the command line:\n");
	for(i = 0; i < argc; i++){//Print out what each argument from the command line is
		printf("argv[%d]: %s\n", i, argv[i]);
	}	
	if (argc < 2){//If user enters arguments less than 2
		printf("\nERROR: NOT THE CORRECT AMOUNT OF ARGUMENTS\nTHE NUMBER OF REQUIREMENTS REQUIRED: MINIMUM 2\n");
		printf("REMINDER: your command line should look like this:\n");
		printf("vmpager inputDataFile [ #memoryAccesses frameTableSize . . . ] \n");
		exit(EXIT_FAILURE);
	}
	else if (argc == 2){//If user enters only 2 arguments
		memoryAccesses = 0;
		frameTableSize = 0;
	}
	else if(argc == 3){//If user enters only 3 arguments
		memoryAccesses = atoi(argv[2]);
		frameTableSize = 0;		
	}	
	else if(argc == 4){//If user enters only 4 arguments
		memoryAccesses = atoi(argv[2]);
		frameTableSize = atoi(argv[3]);
	}

	//Open the file
	if ((fd = open(argv[1], O_RDONLY, S_IRUSR | S_IRGRP | S_IROTH)) == -1){
		fprintf(stderr, "Cannot open file. Try again later.\n");
		exit(0);
	}

	//Determine the size
	if (fstat(fd, &buf) == -1) {
		perror("fstat failed");
		exit(EXIT_FAILURE);
	}

	//Print out the size
	printf("\nSize of %s = %jd\n", argv[1], buf.st_size);

	//memory map the file to a MemoryAccess pointer
	memoryAccessArray = mmap(NULL, buf.st_size, PROT_READ, MAP_PRIVATE, fd, 0);
	if(memoryAccessArray == MAP_FAILED){
		printf("Unable to map the file %s to memory\n", argv[1]);
		exit(0);
	}
		
	//Dividing the size of file by the size of a MemoryAccess
	upperLimit = buf.st_size/sizeof(MemoryAccess);
	printf("The upper limit on the size of the array of MemoryAccesses = %lu\n", upperLimit);	

	/** If user enters a number for #memoryAccesses 
	  that is either less than or equal to 0 or 
	  greater than the upperLimit **/
	if(memoryAccesses <= 0 || memoryAccesses > upperLimit){
		memoryAccesses = upperLimit;//default it to the upperLimit
		printf("\nNOTE:PROGRAM WILL READ AND PROCESS THE ENTIRE INPUT DATA FILE\n");
	}
	/** If user enters a number for frameTableSize
          that is either less than or equal to 0 or
          greater than the 256 **/
	if(frameTableSize <= 0 || frameTableSize > 256){//If user enters 0 or less for the frameTableSize
		frameTableSize = 256;//Default to 256	
		printf("\nNOTE:FRAME TABLE SIZE DEFAULTED TO %d\n\n", frameTableSize);
	}

	FrameTableEntry *frameTableArray = malloc(sizeof(FrameTableEntry) *frameTableSize);//Array of the Frame TableEntrys	

	/**EXECUTE THE ALGORITHMS**/
	IMA(memoryAccessArray, memoryAccesses, pageTables);//Calling the IMA algorithm
	printf("\n");
	FIFO(memoryAccessArray, memoryAccesses, pageTables, frameTableArray, frameTableSize);//Calling the FIFO algorithm
	printf("\n");
	randAlg(memoryAccessArray, memoryAccesses, pageTables, frameTableArray, frameTableSize);//Calling the random victim algorithm
	printf("\n");
        secondChance(memoryAccessArray, memoryAccesses, pageTables, frameTableArray, frameTableSize);//Calling the second chance algorithm
	
	free(frameTableArray);//Deallocate frameTable array
	close(fd);//Close file

	return 0;
}//End of main
