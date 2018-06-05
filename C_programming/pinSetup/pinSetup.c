// Blinky "Hello World"-like program for Raspberry Pi using the sysfs method.
// By Jacob Bechmman Pedersen

#include <sys/stat.h> // For Linux system status
#include <sys/types.h> // For Linux system types
#include <fcntl.h> // Possibly used for allowing the user to manipulate GPIO in the first place.
#include <stdio.h> // Easy. For the standard I/O streams.
#include <stdlib.h> // Contains methods for file I/O and more.
#include <unistd.h> // Contains UNIX system calls.
#include <stdbool.h> // For proper handling of Boolean values.
#include <string.h> // For handling string functions
#include <getopt.h> // For getopt_long().

// Some defines to ease readability:

#define IN 0 // The pin direction is set to input by writing 0 to the relevant file
#define OUT 1 // The pin direction is output when 1 is written.

#define LOW 0 // Arduino-like definition of a LOW logic level to mean 0
#define HIGH 1 // Similarly, HIGH is equivalent to 1.

#define BUFFER_MAX 3 // Maximum needed chars in a file write for pin setup.
#define GPIO_AMNT 26 // Amount of usable GPIOs on the Raspberry Pi
#define MAX_ARGLEN 2 // How many chars can be in an argument?
#define MAX_ARGNUM 4 // Maximum amount of arguments to take.

char *addPins = 0, *removePins = 0, *inputPins = 0, *outputPins = 0;

int opts; // To hold the picked options from getopt_long().
	// Is an int type in order to hodle the return value of -1, when options are over.

static struct option long_options[] = 	{
					{"add", required_argument,	0, 'a'},
					{"remove", required_argument,	0, 'r'},
					{"input", required_argument,	0, 'i'},
					{"output", required_argument,	0, 'o'}
					};
int option_index = 0;

const int legalPins[GPIO_AMNT] = {2, 3, 4, 14, 15, 17, 18, 27, 22, 23, 24, 10, 9, 25,
					11, 8, 7, 5, 6, 12, 13, 19, 16, 26, 20, 21};

static int GPIOExport(int pin);
static int GPIOUnexport(int pin);
static void printHelp(void);

int main (int argc, char ** argv){

	// -a for adding pins
	// -r for removing pins
	// -i for setting them as inputs
	// -o for setting them as outputs

	while(1){
		// The opts variable is set to the value parsed using getopt_long:
		opts = getopt_long(argc/*passed by main*/, argv/*passed by main*/, "a:r:i:o:"/*Valid short opts*/, long_options, &option_index);

		// If the end of options passed is reached, the opts variable is assigned -1
		// This is used to break the eternal while loop.
		if(opts = -1){
			break;
		}

		// If not, then the options can be used to set the relevant flags for program execution:
		switch(opts){
			case 'a':
				addPins = optarg;
				printf("%s\n", optarg);
				break;
			case 'r':
				removePins = optarg;
				printf("%s\n", optarg);
				break;
			case 'i':
				inputPins = optarg;
				printf("%s\n", optarg);
				break;
			case 'o':
				outputPins = optarg;
				printf("%s\n", optarg);
				break;
			default:
				printHelp();
				break;
		}
	}
	printf("All Args:\n");
	printf("-a: %s\n-r: %s\n-i: %s\n-o: %s\n", addPins, removePins, inputPins, outputPins);
}

static int GPIOExport(int pin){
	char buffer[BUFFER_MAX]; // The amount of characters allowed as maximum in the write buffer
				// The write buffer is used in order to support the UNIX low level file
				// system library, (sys/stat.h) for writing to files.

	ssize_t bytes_to_write; // Holds the amount of bytes that write() will expect to write to the opened file.

	int fd; // The UNIX file descriptor for the opened file.

	fd = open("/sys/class/gpio/export", O_WRONLY);  // Opens up the export file, as write only.
							// In order to export (initialize for use) the appropriate
							//GPIO pin

	// If the file descriptor is assigned the return value -1 (Failure to open the file).
	if(fd == -1){
		fprintf(stderr, "Failed to open export for writing pin %d!\n", pin); // Prints error message to stderr.
		return -1; // Returns -1 for failure.
	}

	// The following will write the bytes to the buffer, and assign the amount of chars to bytes_to_write.
	bytes_to_write = snprintf(buffer/*The char buffer*/, BUFFER_MAX /*The max amount in buffer*/, "%d"/*int*/, pin);
	// Now to actually write to the file itself:
	write(fd/*File descriptor*/, buffer/*char buffer*/, bytes_to_write/*amount of chars to write*/);
	// fsync() will synchronize the file with memory.
	fsync(fd);
	// close() of course closes the file.
	close(fd);
	return 0;
}

static int GPIOUnexport(int pin){
	char buffer[BUFFER_MAX]; // Max chars to write to gpio number to file. Two figures usually results in 3 chars.
	ssize_t bytes_to_write; // Bytes to write, usually between one and three
	int fd; // UNIX File Descriptor

	fd = open("/sys/class/gpio/unexport", O_WRONLY); // Open the gpio unexport file as write only.
	if(fd == -1) {
		fprintf(stderr, "Failed to open unexport for pin %d!\n", pin);
		return -1;
	}

	bytes_to_write = snprintf(buffer, BUFFER_MAX, "%d", pin); // Write the pin number to the buffer.
	write(fd, buffer, bytes_to_write); // Write the message to the file.
	fsync(fd); // Sync to memory/driver
	close(fd); // Close the file.
	return 0;
}

static void printHelp(void){
	printf("Options:\n");
	printf("-a, --add [n,m,o..]:\tAdds specified pin(s) to the GPIOs of Raspberry Pi\n");
	printf("-r, --remove [n,m,o..]\tRemoves specified pin(s) form the GPIOs of Raspberry Pi\n");
	printf("-i, --input [n,m,o..]\tSets specified pin(s) up as input\n");
	printf("-o, --output [n,m,o..]\tSets specified pin(s) up as output\n");
}
