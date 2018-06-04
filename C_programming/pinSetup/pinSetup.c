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

// Some defines to ease readability:

#define IN 0 // The pin direction is set to input by writing 0 to the relevant file
#define OUT 1 // The pin direction is output when 1 is written.

#define LOW 0 // Arduino-like definition of a LOW logic level to mean 0
#define HIGH 1 // Similarly, HIGH is equivalent to 1.

#define BUFFER_MAX 3 // Maximum needed chars in a file write for pin setup.
#define GPIO_AMNT 26 // Amount of usable GPIOs on the Raspberry Pi
#define MAX_ARGLEN 2 // How many chars can be in an argument?
#define MAX_ARGNUM 4 // Maximum amount of arguments to take.

enum argTypes {none, add, remove, input, output}; // The types of arguments that are legal. None will count as no argument.

const int legalPins[GPIO_AMNT] = {2, 3, 4, 14, 15, 17, 18, 27, 22, 23, 24, 10, 9, 25,
					11, 8, 7, 5, 6, 12, 13, 19, 16, 26, 20, 21};

static int GPIOExport(int pin);
static int GPIOUnexport(int pin);
static enum argTypes evaluteArg(char * arg);

int main (int argc, char ** argv){

	// -a for adding pins
	// -r for removing pins
	// -i for setting them as inputs
	// -o for setting them as outputs

	// Arg flags keep track of which arguments were passed.
	enum argTypes argFlags[MAX_ARGNUM] = {none, none, none, none};

	int i = 1; // Iterator variable for do-while loop:

	// Do-while will evaluate the arguments, and assign them to the argFlags array.
	do{
		argFlags[i] = evaluateArg(argv[i]); // Assigns the option to the flag index.
		i++; // Up iterator.
	}
	while(argFlags[i] != none && i <= MAX_ARGNUM); // When there are no more 



	// An array for the pins to be exported is set:
	int pins[argc-1];

	// The arguments are all converted to integers and evaluated:
	for(int i = 1; i < argc; i++){
		pins[i] = atoi(argv[i]); // First the conversion.
		bool legalityCheck = false; 	// Then a flag is set to false, and must be flipped to true for a pin to
						// Be considered legal (existing on the board as GPIO) for usage.

		// Then the pin entry is compared to the list of viable pins for the board:
		for(int j = 0; j < GPIO_AMNT; j++){
			// If it exists:
			if(pins[i] == legalPins[j]){
				//TODO: Change the order of these two statements, and add a check for success.
				fprintf(stdout, "Exported pin %d\n", pins[i]); 	// A message is passed to stdout to inform
										// the user!
				GPIOExport(pins[i]); // And the pin is exported
				legalityCheck = true; // The pins exists, and thus no warning will be issued.
				break; // No need to traverse the rest of the array.
			}
		}

		// If the legality check fails, the user will be notified through stderr:
		if(!legalityCheck){
			fprintf(stderr, "Warning, pin %d does not exist on the board!\nIt will be ignored.\n", pins[i]);
			pins[i] = -1; // If array is used for further processing, the pin is marked as unusable.
		}
	}

	if ()
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

static enum argTypes evaluateArg(char * arg){
	// -a for adding pins
	// -r for removing pins
	// -i for setting them as inputs
	// -o for setting them as outputs

	// If the argument -a calls for and addition of the pin to the raspberry pi's active pins:
	if(strncmp(arg, "-a", MAX_ARGLEN) == 0){
		return add;
	}

	// If the argument -r wants it removed:
	else if(strncmp, "-r", MAX_ARGLEN) == 0){
		return remove;
	}

	// If the argument -i is set, the pins mentioned are set as inputs.
	else if(strncmp, "-i", MAX_ARGLEN) == 0){
		return input;
	}

	// If the argument -o is set, the pins will be set as outputs instead.
	else if(strncmp, "-o", MAX_ARGLEN) == 0){
		return output;
	}

	// If the argument is neither of these, the function will return none.
	// Then it is probably a pin number, and will be treated as such.
	else return none;
}
