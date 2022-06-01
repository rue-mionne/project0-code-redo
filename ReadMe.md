#Project 0: Project Cleanup
Simple project to put my newly acquired knowledge of building clean code to the test.

##About
The whole point of the project is to transform my older program in file "OriginPoint.c" into a new, clearer code using Robert C. Martin's "Clean Code: A Handbook of Agile Software Craftsmanship" as a reference. 
The program in its core has only one task: align text in a given txt and export it into a new file. Being a begginer I was, trying to write code to handle string manipulation resulted in a messy code - a perfect candidate for the cleanup project.

##Getting started
###Dependencies
1. GCC
###Compilation
####Linux
```
gcc -o align-text align-text.c
```

####Windows
Windows .exe file available after the project is finished

###Usage
```
./align-text <file_in> <aligned-file>
```

##ToDo:
1. Create clear namerules
2. Break up code into smaller chunks
3. Reduce number of passed argument
4. Hide library functions (clarity sake)
5. Remove comments
6. Improve error handling

##Known issues
Problems with output file if file was created in Windows with ANSI (Windows-1252) characters and program run on a Linux system. Double characters and uneven lines.

##Authors
@rue-mionne
