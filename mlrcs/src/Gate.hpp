#ifndef GATE_HPP
#define GATE_HPP

#include <string>
#include <vector>
#include <iostream>

class Gate
{
    private:
    std::string outputName;
    std::vector<std::string> inputNames;
    int pirority;

    std::string gateType;

    public:
    Gate();
    Gate(std::string outputName, std::vector<std::string> inputNames, int pirority, std::string gateType);
    Gate(std::string outputName, std::vector<std::string> inputNames, std::string gateType);

    std::string getOutputName() const; 
    std::vector<std::string> getInputNames() const;
    int getPirority() const;
    std::string getGateType() const;

    void setPirority(int pirority);
    friend std::ostream& operator<<(std::ostream& os, const Gate& gate);
};


#endif