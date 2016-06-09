#cpe357
https://github.com/sonianara

### An implementation of the UNIX command "du" in C

__Description__: 

    du - summarizes disk usage of each FILE, recursively for directories 

__Usage__: 

    >> make
    >> ./mydu [-c][-h][-a][--max-depth=N]

__Optional flags__:

    -a: writes counts for all files, not just directories
    -h: prints sizes in human readable format
    -c: produces a grand total
    --max-depth=N: prints the total for a directory only if it is N or fewer levels below the command line argument
