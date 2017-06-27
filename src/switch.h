#include <vector>
#include <string>

typedef int Address;

class Switch {
    static const int bufferSize = 256;

    // Each "port" on a Switch has side-by-side incoming and outgoing data and strobe lines 
    struct Port {
        char txData;
        char txStrobe;
        char rxData;
        char rxStrobe;

        Port* link;
        Switch* parent;
    } Port_t;

    protected:
        // Personal
        std::string name;
        Address address;

        // Networking
        // Let's have an arbitrary number of ports that will be built at the beginning of the simulation
        std::vector<Port*> ports;

    public:
        // Personal
        Switch();
        Switch(const std::string& name, Address address);
        ~Switch();

        // Accessors
        const std::string& getName();
        Address getAddress();

        int setName(const std::string& name);
        int setAddress(Address address);

        // Networking
        Port* link(Switch* targetSwitch); // Create ports for both this and the recipient, then link them
        Port* link(Switch* targetSwitch, Switch::Port* targetPort);
        int step(); // Process data when prompted by the simulator

        // Simulation
        int printStatus();
};
