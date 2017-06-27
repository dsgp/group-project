#include <stdio.h>
#include <stdlib.h>
#include <vector>
#include <string>

#include "switch.h"

// Prototypes
int simulateStep();
int printAll();
Switch* addSwitch(const std::string& name, char address);

int testingPrompt();
void printClear();
void printNetwork();
void printMainOptions();

// List of switches (including nodes)
std::vector<Switch*> switches;
int step = 0;

/*
    Main program
*/
int main(int argc, char** argv) {

    // Set up two-node network
    Switch* node1 = addSwitch("Jack", 0);
    Switch* node2 = addSwitch("Wanda", 1);
    node1->link(node2);

    printAll();
    simulateStep();
    printAll();

    // Temporary command prompt for testing purposes
    //testingPrompt();

    printf("Finished!\n");
}


Switch* addSwitch(const std::string& name, char address) {
    Switch* newSwitch = new Switch(name, address);
    switches.push_back(newSwitch);
    return newSwitch;
}

int simulateStep() {

    ++step;

    // Step through all switches
    int n = switches.size();
    for (int i = 0; i < n; ++i) {
        Switch* iSwitch = switches[i];
        iSwitch->step();
    }
}

int printAll() {

    printf("\n---------- Step %d ----------\n\n", step);

    // Print all switches
    int n = switches.size();
    for (int i = 0; i < n; ++i) {
        Switch* iSwitch = switches[i];
        iSwitch->printStatus();
    }
}

int testingPrompt() {

    char* cmd = (char*)malloc(32);

    while (1) {
        printClear();
        printf("- Welcome to SpAcEwIrE -\n");
        printMainOptions();

        scanf("%s", cmd);

        if (cmd[0] == 'q')
            break;
    }

    free(cmd);

}

// Clears the terminal
void printClear() {
    printf("\033[2J\033[1;1H");
}

void printNetwork() {
}

void printMainOptions() {

    printf("[R]estart\n");
    printf("[Q]uit\n");

    printf("\nChoice: ");
}
