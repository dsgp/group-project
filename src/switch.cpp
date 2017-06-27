#include "switch.h"

#include <stdlib.h>
#include <stdio.h>
#include <iostream>

Switch::Switch(const std::string& name, Address address) {
    this->name = name;
    this->address = address;
}


Switch::~Switch() {
    // Ports were new'd
    for (int i = 0; i < ports.size(); ++i)
        delete ports[i];
}


const std::string& Switch::getName() {
    return name;
}


Address Switch::getAddress() {
    return address;
}


int Switch::setName(const std::string& name) {
    this->name = name;
}


int Switch::setAddress(Address address) {
    this->address = address;
}


/*
    link() connects two switches by creating ports and connecting them
*/
Switch::Port* Switch::link(Switch* targetSwitch, Switch::Port* targetPort) {
    // Create a new local port
    Switch::Port* newPort = new Port();
    newPort->parent = this;
    newPort->txData = 0;
    newPort->txStrobe = 0;
    newPort->rxData = 0;
    newPort->rxStrobe = 0;
    ports.push_back(newPort);

    // If a target port was not provided, create a corresponding port on the destination
    if (targetPort == NULL)
    {
        newPort->link = targetSwitch->link(this, newPort);
        newPort->txData = 1;
        newPort->txStrobe = 1;
    }
    // Otherwise, link the provided target port
    else
        newPort->link = targetPort;

}


// When the target port is not provided, it will be created automatically
Switch::Port* Switch::link(Switch* targetSwitch) {
    link(targetSwitch, NULL);
}


/*
    Step once through the simulation
*/
int Switch::step() {
    // Propagate information across linked ports
    int n = ports.size();
    for (int i = 0; i < n; ++i) {
        Port* port = ports[i];

        // INSERT SIMULATED ERRORS HERE
        char receiveData = port->link->txData;
        char receiveStrobe = port->link->txStrobe;

        // Receive information
        port->rxData = receiveData;
        port->rxStrobe = receiveStrobe;
    }
}


/*
    Display state
*/
int Switch::printStatus() {

    std::cout << "SWITCH: " << name << "\n";

    // Propagate information across ports
    int n = ports.size();
    for (int i = 0; i < n; ++i) {
        Port* port = ports[i];

        std::cout << " Port " << i << ":\n";

        // Print linkage
        if (port->link == NULL)
            std::cout << "\tNo link\n";
        else
            std::cout << "\tLinked to " << port->link->parent->getName() << "\n";

        // Print registers
        std::cout << "\trxData  \t" << (int)port->rxData << "\n";
        std::cout << "\trxStrobe\t" << (int)port->rxStrobe << "\n";
        std::cout << "\ttxData  \t" << (int)port->txData << "\n";
        std::cout << "\ttxStrobe\t" << (int)port->txStrobe << "\n";
    }
}
