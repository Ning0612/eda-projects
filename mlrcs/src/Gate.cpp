#include <iostream>
#include <fstream>
#include <string>
#include <vector>

#include "Gate.hpp"

Gate::Gate()
{
    this->outputName = "";
    this->inputNames = {};
    this->pirority = 0;
    this->gateType = "";
}

Gate::Gate(std::string outputName, std::vector<std::string> inputNames, int pirority, std::string gateType)
{
    this->outputName = outputName;
    this->inputNames = inputNames;
    this->pirority = 0;
    this->gateType = gateType;
}

Gate::Gate(std::string outputName, std::vector<std::string> inputNames, std::string gateType)
{
    this->outputName = outputName;
    this->inputNames = inputNames;
    this->pirority = 0;
    this->gateType = gateType;
}

std::string Gate::getOutputName() const
{
    return this->outputName;
}

std::vector<std::string> Gate::getInputNames() const
{
    return this->inputNames;
}

int Gate::getPirority() const
{
    return this->pirority;
}

std::string Gate::getGateType() const
{
    return this->gateType;
}

void Gate::setPirority(int pirority)
{
    this->pirority = pirority;
}

std::ostream& operator<<(std::ostream& os, const Gate& gate)
{
    os << gate.gateType << " : ";
    for (const auto& inputName : gate.inputNames)
    {
        os << inputName << " ";
    }
    os << "<==> " << gate.outputName << " | " << gate.pirority;

    return os;
}