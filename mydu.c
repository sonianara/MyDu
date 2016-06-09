#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>

#define GIGABYTE (1 << 21)
#define MEGABYTE (1 << 11)
#define KILOBYTE (1 << 1) 
#define BYTESPERBLOCK 512

void blockMath(char *entryName, blkcnt_t numBlocks, int hrFlag) {
	int size;
	char label;
	char sizeBuf[10];

	if (!hrFlag) {
		printf("%-8ld%s\n", (numBlocks + 1) / 2, entryName);
	}
	else {
		if (numBlocks >= GIGABYTE) {
			size = numBlocks / GIGABYTE;
			label = 'G';
		}
		else if (numBlocks >= MEGABYTE) {
			size = numBlocks / MEGABYTE;
			label = 'M';
		}
		else if (numBlocks >= KILOBYTE) {
			size = numBlocks / KILOBYTE;
			label = 'K';
		}
		else {
			size = numBlocks * BYTESPERBLOCK;
			label = ' ';
		}

		snprintf(sizeBuf, 10, "%d%c", size, label);
		printf("%-8s%s\n", sizeBuf, entryName);
 	}
}

blkcnt_t TraverseDir(char *entryName, int printFileFlag, int maxDepthFlag, int maxDepthValue, int hrFlag) {
	blkcnt_t totalSize = 0;
	struct stat st;
	char buffer[PATH_MAX];
	DIR *dir;
	struct dirent *newEntry;

	if (lstat(entryName, &st)) {
		perror(entryName);
		exit(-1);
	}

	if (S_ISDIR(st.st_mode) && (dir = opendir(entryName))) {
		while ((newEntry = readdir(dir)))
			if (strcmp(newEntry->d_name, ".") && strcmp(newEntry->d_name, "..")) {
				snprintf(buffer, PATH_MAX, "%s/%s", entryName, newEntry->d_name);
				totalSize += TraverseDir(buffer, printFileFlag, maxDepthFlag, maxDepthValue - 1, hrFlag);
			}
		closedir(dir);
	}

	totalSize += st.st_blocks;

	if ((printFileFlag || S_ISDIR(st.st_mode)) && (maxDepthValue >= 0 || !maxDepthFlag))
		blockMath(entryName, totalSize, hrFlag);

	return totalSize;
}

void SetUp(int argc, char **argv) {
	int argNdx, letterNdx;
	int totalFlag = 0, maxDepthFlag = 0, maxDepthVal = 0;
	int printFileFlag = 0, fileDirFlag = 0, hrFlag = 0;
	blkcnt_t total = 0;	


	for (argNdx = 1; argNdx < argc; argNdx++) {
		if (strncmp(argv[argNdx], "--max-depth=", 12) == 0) {
			maxDepthVal = atoi(&(argv[argNdx][12]));
			maxDepthFlag = 1;
		}
		else if (argv[argNdx][0] == '-') {
			for (letterNdx = 1; letterNdx < strlen(argv[argNdx]); letterNdx++) {
				if (argv[argNdx][letterNdx] != 'h' && argv[argNdx][letterNdx] != 'c' && argv[argNdx][letterNdx] != 'a') {
					printf("du: invalid option -- '%c'\n", argv[argNdx][letterNdx]);
					printf("Try `du --help' for more information.\n");
					exit(0);
				}
				if (argv[argNdx][letterNdx] == 'a') 
					printFileFlag = 1;
	
				if (argv[argNdx][letterNdx] == 'c') 
					totalFlag = 1;
				
				if (argv[argNdx][letterNdx] == 'h') 
					hrFlag = 1;	
			}
		}
		else 
			fileDirFlag = 1;
	}

	if (!fileDirFlag) {
		total += TraverseDir(".", printFileFlag, maxDepthFlag, maxDepthVal, hrFlag);
	}

	else {
		for (argNdx = 1; argNdx < argc; argNdx++) {
			if ((argv[argNdx][0] != '-')) {
				total += TraverseDir(argv[argNdx], printFileFlag, maxDepthFlag, maxDepthVal, hrFlag);
			} 
		}
	}
	if (totalFlag)
		blockMath("total", total, hrFlag);
}

int main(int argc, char **argv) {

	SetUp(argc, argv);
	
	return 0;
}
