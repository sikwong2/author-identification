# author-identification
Author Identification using RegEx

# Make
To make the main program type `make` into the command line.

# Usage  
## Identify  
`./identify [-k matches] [-n noise] [-l limit] [-e] [-m] [-c] [-h]`  
### Flags  
* `-k [matches]` Set number of top matches to display [default: 5]  
* `-n [noise]` Set file of words to ignore [default: noise.txt]  
* `-l [limit]` Set noise word limit [default: 100]  
* `-e` Set distance metric as Euclidean distance [default]  
* `-m` Set distance metric as Manhattan distance  
* `-c` Set distance metric as Cosine distance  
* `-h` Display program help and usage  

# Cleanup  
To cleanup all created files, type `make clean` into the command line
