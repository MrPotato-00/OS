#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>

#define LOWER 0          // LOWER value for the randomIntegerGenerator method
#define UPPER 8589934592 // UPPER value for the randomIntegerGenerator method

char *addressOfMapBuffer; // The address of the buffer created by the mmap() method call is stored

void printLine()
{
    printf("---------------------------------------------\n");
}
int readInteger(unsigned long offset)
{
    unsigned long readValue;
    sscanf(addressOfMapBuffer + offset, "%lu", &readValue);
    printf("The Value Read is : %-10lu\n", readValue);
    return readValue;
}

int writeInteger(unsigned long offset, unsigned long value)
{
    printf("The Value Written is : %-10lu\n", value);
    sprintf(addressOfMapBuffer + offset, "%lu", value);
}

unsigned long randomIntegerGenerator(unsigned long low, unsigned high)
{
    unsigned long lowerRange = low;
    unsigned long upperRange = high;
    unsigned long randomNumber = lowerRange + (rand() % (upperRange - lowerRange + 1));
    return randomNumber;
}

int main(int argc, char **argv)
{
    if (argc != 2)
    {
        printf("Sufficient number of arguments are not provided!\n");
        printf("The expected execution command is : ./a.out <file_name>\n");
        exit(1);
    }

    // Opening the user specified file
    int FILE = open(argv[1], O_RDWR, S_IRUSR | S_IWUSR);

    // The information about the user specified file is stored in this structure
    struct stat sb;

    // Getting the information of the file in the sb variable
    if (fstat(FILE, &sb) == -1)
    {
        perror("Some ERROR occured while getting the file size!");
        exit(1);
    }

    // Mapping the file in the virtual address space
    if ((addressOfMapBuffer = (char *)mmap(NULL, sb.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, FILE, 0)) == MAP_FAILED)
    {
        perror("Some ERROR occured while mapping the file!");
        exit(2);
    }

    // Setting the seed value for the rand() function
    srand(time(NULL));
    unsigned long offset, writtenValue, readValue;

    while (1)
    {
        writtenValue = randomIntegerGenerator(0, 255); // Getting a random integer in the weittenValue variable from the randomIntegerGenerator method

        offset = randomIntegerGenerator(LOWER, UPPER); // Getting a random integer in the offset variable from the randomIntegerGenerator method

        writeInteger(offset, writtenValue); // Writing the value in writtenValue varible in the offset position of the addressOfMapBuffer

        readValue = readInteger(offset); // Reading a value in readValue from the offset position of the addressOfMapBuffer

        // Checking whether readValue and writtenValue are same or not
        if (writtenValue == readValue)
        {
            printf("The value that is read and written are the same!\n");
            printLine();
        }
        else
        {
            printf("The value that is read and written are NOT the same!\n");
            printLine();
        }
    }

    return 0;
}
