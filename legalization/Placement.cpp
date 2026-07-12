#include "Placement.hpp"
#include <iomanip>
#include <algorithm> 
#include <cctype> 
#include <cmath>

Placement::Placement(char *inFileName, char *outFileName)
{
    this->inFileName = inFileName;
    this->outFileName = outFileName;
    this->FixedCells = 0;

    this->readPlFile();
    this->readNodesFile();
    this->readSclFile();
}

void Placement::readNodesFile()
{
    std::ifstream inFile(this->inFileName + ".nodes");

    if (!inFile)
    {
        std::cerr << "Unable to open file " << this->inFileName + ".nodes" << std::endl;
        exit(1);
    }

    std::string line;
    std::string name;
    double width, height;
    std::regex pattern(R"(^\s*(\S+)\s+([-+]?[0-9]*\.?[0-9]+)\s+([-+]?[0-9]*\.?[0-9]+)(?:\s+(\S+))?\s*$)");

    while (std::getline(inFile, line))
    {
        std::smatch match;

        if (std::regex_match(line, match, pattern)) {
            std::string name = match[1].str();
            double width = std::stod(match[2].str());
            double height = std::stod(match[3].str());

            std::string terminal;
            if (match[4].matched) {  
                terminal = match[4].str();  
                terminal.erase(0, terminal.find_first_not_of(" \t"));
                if (terminal.find("terminal") != std::string::npos){
                    this->components[name].setIsFixed(true);
                    this->FixedCells++;
                }
            }
            
            this->componentsCount++;

            this->components[name].setSizes(width, height);
        }
    }

    inFile.close();
}

void Placement::readPlFile()
{
    std::ifstream inFile(this->inFileName + ".pl");

    if (!inFile)
    {
        std::cerr << "Unable to open file " << this->inFileName + ".pl" << std::endl;
        exit(1);
    }

    std::string line;
    std::string name;
    double x, y;
    std::string direction;
    std::regex pattern(R"(^\s*(\S+)\s+([-+]?[0-9]*\.?[0-9]+)\s+([-+]?[0-9]*\.?[0-9]+)\s+:\s+(\S+)(?:\s+/FIXED)?\s*$)");

    while (std::getline(inFile, line))
    {
        std::smatch match;
        if (std::regex_match(line, match, pattern)) {
            std::string name = match[1].str();
            double x = std::stod(match[2].str());
            double y = std::stod(match[3].str());
            std::string direction = match[4].str();

            Component component(name, Point(x, y), direction);
            this->components.emplace(name, component);
        }
    }

    inFile.close();
}

void Placement::readSclFile()
{
    std::ifstream inFile(this->inFileName + ".scl");

    if (!inFile)
    {
        std::cerr << "Unable to open file " << this->inFileName + ".scl" << std::endl;
        exit(1);
    }

    std::string line;
    bool inCoreRowSection = false;

    while (std::getline(inFile, line))
    {
        if (line.find("CoreRow") != std::string::npos) {
            inCoreRowSection = true;
            bool isHorizontal = line.find("Horizontal") != std::string::npos;

            double coordinate = 0.0, height = 0.0, siteWidth = 0.0, siteSpacing = 0.0;
            std::string siteorient, sitesymmetry;
            std::vector<std::pair<double, int>> subRows;

            while (line.find("End") == std::string::npos && std::getline(inFile, line)) {
                std::istringstream iss(line);
                std::string token;
                while (iss >> token) {
                    if (token == "Coordinate") {
                        iss >> token; // Skip ":"
                        iss >> coordinate;
                    } else if (token == "Height") {
                        iss >> token;
                        iss >> height;
                    } else if (token == "Sitewidth") {
                        iss >> token;
                        iss >> siteWidth;
                    } else if (token == "Sitespacing") {
                        iss >> token;
                        iss >> siteSpacing;
                    } else if (token == "Siteorient") {
                        iss >> token;
                        iss >> siteorient;
                    } else if (token == "Sitesymmetry") {
                        iss >> token;
                        iss >> sitesymmetry;
                    } else if (token == "SubrowOrigin") {
                        double subrowOrigin;
                        int numSites;
                        iss >> token;
                        iss >> subrowOrigin;
                        iss >> token; // Skip "NumSites"
                        iss >> token; // Skip ":"
                        iss >> numSites;
                        subRows.emplace_back(subrowOrigin, numSites);
                    }
                }
            }

            this->cellRows.emplace(coordinate, std::make_pair(CellRow(isHorizontal, coordinate, height, siteWidth, siteSpacing, siteorient, sitesymmetry, subRows), std::vector<Component*>()));
        }
    }

    inFile.close();
}

void Placement::writeAuxFile()
{
    std::ofstream outFile(this->outFileName + ".aux");

    if (!outFile)
    {
        std::cerr << "Unable to open file " << this->outFileName + ".aux" << std::endl;
        exit(1);
    }

    outFile << "RowBasedPlacement : " 
            << this->outFileName + ".nodes "
            << this->outFileName + ".nets "
            << this->outFileName + ".wts "
            << this->outFileName + ".pl "
            << this->outFileName + ".scl "
            << std::endl;

    outFile.close();
}

void Placement::writePlFile()
{
    std::ofstream outFile(this->outFileName + ".pl");

    if (!outFile)
    {
        std::cerr << "Unable to open file " << this->outFileName + ".pl" << std::endl;
        exit(1);
    }

    outFile << "UCLA pl 1.0" << std::endl;
    outFile << std::endl;

    for (auto &component : this->components)
    {
        component.second.outPlFile(outFile);
    }

    outFile.close();
}

void Placement::copyNodesFile()
{
    std::ifstream inFile(this->inFileName + ".nodes");
    std::ofstream outFile(this->outFileName + ".nodes");

    if (!inFile)
    {
        std::cerr << "Unable to open file " << this->inFileName + ".nodes" << std::endl;
        exit(1);
    }

    if (!outFile)
    {
        std::cerr << "Unable to open file " << this->outFileName + ".nodes" << std::endl;
        exit(1);
    }

    outFile << inFile.rdbuf();

    inFile.close();
    outFile.close();
}

void Placement::copySclFile()
{
    std::ifstream inFile(this->inFileName + ".scl");
    std::ofstream outFile(this->outFileName + ".scl");

    if (!inFile)
    {
        std::cerr << "Unable to open file " << this->inFileName + ".scl" << std::endl;
        exit(1);
    }

    if (!outFile)
    {
        std::cerr << "Unable to open file " << this->outFileName + ".scl" << std::endl;
        exit(1);
    }

    outFile << inFile.rdbuf();

    inFile.close();
    outFile.close();
}

void Placement::copyNetsFile()
{
    std::ifstream inFile(this->inFileName + ".nets");
    std::ofstream outFile(this->outFileName + ".nets");

    if (!inFile)
    {
        std::cerr << "Unable to open file " << this->inFileName + ".nets" << std::endl;
        exit(1);
    }

    if (!outFile)
    {
        std::cerr << "Unable to open file " << this->outFileName + ".nets" << std::endl;
        exit(1);
    }

    outFile << inFile.rdbuf();

    inFile.close();
    outFile.close();
}

void Placement::copyWtsFile()
{
    std::ifstream inFile(this->inFileName + ".wts");
    std::ofstream outFile(this->outFileName + ".wts");

    if (!inFile)
    {
        std::cerr << "Unable to open file " << this->inFileName + ".wts" << std::endl;
        exit(1);
    }

    if (!outFile)
    {
        std::cerr << "Unable to open file " << this->outFileName + ".wts" << std::endl;
        exit(1);
    }

    outFile << inFile.rdbuf();

    inFile.close();
    outFile.close();
}

void Placement::calculateBenchmark()
{
    double totalDisplacement = 0;
    double maximumDisplacement = 0;

    for (auto &component : this->components)
    {
        Point origin_position_LD = component.second.getOriginPosition();
        Point legal_position_LD = component.second.legal_position_LD;

        double displacement = Point::distance(origin_position_LD, legal_position_LD);
        totalDisplacement += displacement;

        if (displacement > maximumDisplacement)
        {
            maximumDisplacement = displacement;
        }
    }

    this->totalDisplacement = totalDisplacement;
    this->maximumDisplacement = maximumDisplacement;
}

void Placement::printComponents()
{
    for (auto &component : this->components)
    {
        std::cout << component.second.getName() << " " << component.second.getOriginPosition().x << " " << component.second.getOriginPosition().y << std::endl;
    }
}

void Placement::printBenchmark()
{
    this->calculateBenchmark();

    std::cout << "Total displacement: " << std::fixed << std::setprecision(2) << this->totalDisplacement << std::endl;
    std::cout << "Maximum displacement: " << std::fixed << std::setprecision(2) << this->maximumDisplacement << std::endl;
}

void Placement::outputAllFiles()
{
    this->writeAuxFile();
    this->writePlFile();
    this->copyNodesFile();
    this->copySclFile();
    this->copyNetsFile();
    this->copyWtsFile();
}

void Placement::plotComponents() {
    // Open files for fixed and non-fixed components
    std::ofstream fixedFile("fixed_components.dat");
    std::ofstream nonFixedFile("non_fixed_components.dat");

    if (!fixedFile || !nonFixedFile) {
        std::cerr << "Error: Unable to open files for plotting data." << std::endl;
        return;
    }

    // Write each component's bounding box to the appropriate file
    for (const auto& component : this->components) {
        const std::string& name = component.first;
        const auto& boundingBox = component.second.getLegalBoundingBox();
        bool isFixed = component.second.getIsFixed();

        double x_min = boundingBox.first.x;
        double y_min = boundingBox.first.y;
        double x_max = boundingBox.second.x;
        double y_max = boundingBox.second.y;

        std::ofstream& outFile = isFixed ? fixedFile : nonFixedFile;

        // Write the rectangle's corner points to form a closed loop
        outFile << x_min << " " << y_min << "\n"
                << x_max << " " << y_min << "\n"
                << x_max << " " << y_max << "\n"
                << x_min << " " << y_max << "\n"
                << x_min << " " << y_min << "\n\n";

        // Optional: add labels for each component's name at the center of the rectangle
        outFile << "# " << name << " label at ("
                << (x_min + x_max) / 2 << ", "
                << (y_min + y_max) / 2 << ")\n";
    }

    fixedFile.close();
    nonFixedFile.close();
}

void Placement::plotCellRows() {
    std::ofstream dataFile("cellRows.dat");

    if (!dataFile) {
        std::cerr << "Error: Unable to open file for plotting data." << std::endl;
        return;
    }

    for (const auto& cellRowPair : this->cellRows) {
        const std::string& name = "Row_" + std::to_string(cellRowPair.first);
        const auto& subRows = cellRowPair.second.first.getSubRows();

        for (const auto& subRowPair : subRows) {
            std::string subRowName = name + "_SubRow_" + std::to_string(subRowPair.first);

            double x_min, y_min, x_max, y_max;
            if(cellRowPair.second.first.getIsHorizontal()){
                x_min = subRowPair.second.getBegin();
                y_min = cellRowPair.first;
                x_max = subRowPair.second.getEnd();
                y_max = cellRowPair.first + cellRowPair.second.first.getHeight();
            }else{
                x_min = cellRowPair.first;
                y_min = subRowPair.second.getBegin();
                x_max = cellRowPair.first + cellRowPair.second.first.getHeight();
                y_max = subRowPair.second.getEnd();
            }

            dataFile << x_min << " " << y_min << "\n"
                     << x_max << " " << y_min << "\n"
                     << x_max << " " << y_max << "\n"
                     << x_min << " " << y_max << "\n"
                     << x_min << " " << y_min << "\n\n";

            dataFile << "# " << subRowName << " label at ("
                     << (x_min + x_max) / 2 << ", "
                     << (y_min + y_max) / 2 << ")\n";
        }
    }

    dataFile.close();
}

void Placement::alignComponents(int times)
{
    std::map<double, double> remainSpace;

    for(auto &cellRow : this->cellRows){
        remainSpace[cellRow.first] = cellRow.second.first.getTotalSpace();
    }

    std::vector<Component*> componmentPlacementOrder;
    for(auto &component : this->components){
        componmentPlacementOrder.push_back(&component.second);
    }

    std::sort(componmentPlacementOrder.begin(), componmentPlacementOrder.end(), [](Component* a, Component* b) {
        if (a->getWidth() == b->getWidth()) {
            return a->getOriginPosition().y > b->getOriginPosition().y;
        }
        return a->getWidth() > b->getWidth();
    });

    for (auto &component : componmentPlacementOrder){
        if (component->getIsFixed())
        {
            continue;
        }

        Point align_position_LD = component->getOriginPosition();

        std::map<double, std::pair<CellRow, std::vector<Component *>>>::iterator it_upper;
        std::map<double, std::pair<CellRow, std::vector<Component *>>>::iterator it_lower;
        std::map<double, std::pair<CellRow, std::vector<Component *>>>::iterator it_tarrget;
        
        // find the nearest row
        if(this->cellRows.begin()->second.first.getIsHorizontal()){
            it_upper = this->cellRows.upper_bound(component->getOriginPosition().y);
            it_lower = it_upper == this->cellRows.begin() ? it_upper : std::prev(it_upper);

            if (it_upper == this->cellRows.end() || (it_lower != this->cellRows.end() && std::abs(it_lower->first - component->getOriginPosition().y) < std::abs(it_upper->first - component->getOriginPosition().y))) {
                it_tarrget = it_lower;
            } else {
                it_tarrget = it_upper;
            }
        }else{
            it_upper = this->cellRows.upper_bound(component->getOriginPosition().x);
            it_lower = it_upper == this->cellRows.begin() ? it_upper : std::prev(it_upper);

            if (it_upper == this->cellRows.end() || (it_lower != this->cellRows.end() && std::abs(it_lower->first - component->getOriginPosition().x) < std::abs(it_upper->first - component->getOriginPosition().x))) {
                it_tarrget = it_lower;
            } else {
                it_tarrget = it_upper;
            }
        }

        // row full, find the nearest remain spacing row
        while(true){
            if(remainSpace[it_tarrget->second.first.getCoordinate()] >= component->getWidth() && it_tarrget->second.first.getHeight() == component->getHeight()){
                break;
            }

            it_lower = (it_lower == this->cellRows.begin()) ? it_lower : std::prev(it_lower);
            it_tarrget = it_lower;

            if(remainSpace[it_tarrget->second.first.getCoordinate()] >= component->getWidth() && it_tarrget->second.first.getHeight() == component->getHeight()){
                break;
            }

            it_upper = (it_upper == this->cellRows.end()) ? it_upper : std::next(it_upper);
            it_tarrget = it_upper;

            if(it_lower == this->cellRows.begin() && it_upper == this->cellRows.end()){
                break;
            }
        }

        CellRow row = it_tarrget->second.first;

        remainSpace[row.getCoordinate()] -= component->getWidth();

        if(row.getIsHorizontal()){
            align_position_LD.y = row.getCoordinate();
            align_position_LD.x = (int)component->getOriginPosition().x;
        }else{
            align_position_LD.x = row.getCoordinate();
            align_position_LD.y = (int)component->getOriginPosition().y;
        }

        this->cellRows[row.getCoordinate()].second.push_back(component);
        component->legal_position_LD = align_position_LD;
    }

    // balance the space
    for(int i = 0 ; i < times; ++i){
        std::vector<CellRow*> cellRowOrder;
        for(auto &cellRow : this->cellRows){
            cellRowOrder.push_back(&cellRow.second.first);
        }

        std::sort(cellRowOrder.begin(), cellRowOrder.end(), [remainSpace](CellRow* a, CellRow* b) {
            return remainSpace.at(a->getCoordinate()) < remainSpace.at(b->getCoordinate());
        });


        for(auto &cellRow : cellRowOrder){
            auto it = this->cellRows.find(cellRow->getCoordinate());
            if (it == this->cellRows.end()) {
                return; 
            }

            auto it_next = (std::next(it) == this->cellRows.end()) ? it : std::next(it);
            auto it_prev = (it == this->cellRows.begin()) ? it : std::prev(it);

            if (it != this->cellRows.end()) {
                std::sort(it->second.second.begin(), it->second.second.end(), [](Component* a, Component* b) {
                    return a->getWidth() > b->getWidth(); 
                });
            }

            if (it_next != it) {
                std::sort(it_next->second.second.begin(), it_next->second.second.end(), [](Component* a, Component* b) {
                    return a->getWidth() > b->getWidth(); 
                });
            }

            if (it_prev != it) {
                std::sort(it_prev->second.second.begin(), it_prev->second.second.end(), [](Component* a, Component* b) {
                    return a->getWidth() > b->getWidth(); 
                });
            }


            bool flag = true;

            while (flag && (remainSpace.at(it->first) < remainSpace.at(it_next->first) || remainSpace.at(it->first) < remainSpace.at(it_prev->first))) {
                flag = false;
                // 比較並調整至 it -> next
                if (!it->second.second.empty() && remainSpace.at(it->first) <= remainSpace.at(it_next->first)) {
                    Component* comp = it->second.second.back();
                    if (remainSpace.at(it_next->first) > comp->getWidth()) {
                        flag = true;
                        it->second.second.pop_back();
                        it_next->second.second.push_back(comp);
                        remainSpace[it->first] += comp->getWidth();
                        remainSpace[it_next->first] -= comp->getWidth();
                        if(it->second.first.getIsHorizontal()){
                            comp->legal_position_LD = Point(comp->legal_position_LD.x, it_next->first);
                        }else{
                            comp->legal_position_LD = Point(it_next->first, comp->legal_position_LD.y);
                        }
                    }
                }

                // 比較並調整至 it -> prev
                if (!it->second.second.empty() && remainSpace.at(it->first) <= remainSpace.at(it_prev->first)) {
                    Component* comp = it->second.second.back();
                    if (remainSpace.at(it_prev->first) > comp->getWidth()) {
                        flag = true;
                        it->second.second.pop_back();
                        it_prev->second.second.push_back(comp);
                        remainSpace[it->first] += comp->getWidth();
                        remainSpace[it_prev->first] -= comp->getWidth();
                        if(it->second.first.getIsHorizontal()){
                            comp->legal_position_LD = Point(comp->legal_position_LD.x, it_prev->first);
                        }else{
                            comp->legal_position_LD = Point(it_prev->first, comp->legal_position_LD.y);
                        }
                    }
                }
            }
        }
    }
}

void Placement::placeInRows(){
    for(auto cellRow : this->cellRows){

        if(cellRow.second.second.size() == 0){
            continue;
        }

        if(cellRow.second.first.getIsHorizontal()){
            std::sort(cellRow.second.second.begin(), cellRow.second.second.end(), [](Component* a, Component* b){
                return a->legal_position_LD.x < b->legal_position_LD.x;
            });
        }else{
            std::sort(cellRow.second.second.begin(), cellRow.second.second.end(), [](Component* a, Component* b){
                return a->legal_position_LD.y < b->legal_position_LD.y;
            });
        }

        std::vector<Component*> components;
        components.empty();

        for(auto component : cellRow.second.second){
            Point legal_position_LD = component->legal_position_LD;

            for(auto &comp : components){
                if(isOverlap(*component, *comp)){
                    if(cellRow.second.first.getIsHorizontal()){
                        legal_position_LD.x = components.back()->getLegalBoundingBox().second.x;
                        break;
                    }else{
                        legal_position_LD.y = components.back()->getLegalBoundingBox().second.y;
                        break;
                    }
                }
            }

            component->legal_position_LD = legal_position_LD;
            components.push_back(component);
        }

        if(cellRow.second.first.getIsHorizontal()){
            std::sort(cellRow.second.second.begin(), cellRow.second.second.end(), [](Component* a, Component* b){
                return a->legal_position_LD.x > b->legal_position_LD.x;
                });
        }else{
            std::sort(cellRow.second.second.begin(), cellRow.second.second.end(), [](Component* a, Component* b){
                return a->legal_position_LD.y > b->legal_position_LD.y;
            });
        }

        if(cellRow.second.first.getIsHorizontal()){
            
            if(cellRow.second.second.front()->getLegalBoundingBox().second.x > cellRow.second.first.getBoundingBox().second.x){
                double offset = cellRow.second.second.front()->getLegalBoundingBox().second.x - cellRow.second.first.getBoundingBox().second.x;
                
                int i = 0;

                while(offset >= 0 && i < cellRow.second.second.size() - 1){
                    double componentSpace = cellRow.second.second.at(i)->getLegalBoundingBox().first.x - cellRow.second.second.at(i + 1)->getLegalBoundingBox().second.x;

                    if(componentSpace > 0){
                        for (int j = 0 ; j <= i ; ++j){
                            cellRow.second.second.at(j)->legal_position_LD.x -= componentSpace;
                        }
                        offset -= componentSpace;
                    }
                    i++;
                }

                if(offset > 0){
                    for(auto component : cellRow.second.second){
                        component->legal_position_LD.x -= offset;
                    }
                }
            }


        }else{
            if(cellRow.second.second.front()->getLegalBoundingBox().second.y > cellRow.second.first.getBoundingBox().second.y){
                double offset = cellRow.second.second.front()->getLegalBoundingBox().second.y - cellRow.second.first.getBoundingBox().second.y;
                
                int i = 0;

                while(offset >= 0 && i < cellRow.second.second.size() - 1){
                    double componentSpace = cellRow.second.second.at(i)->getLegalBoundingBox().first.y - cellRow.second.second.at(i + 1)->getLegalBoundingBox().second.y;

                    if(componentSpace > 0){
                        for (int j = 0 ; j <= i ; ++j){
                            cellRow.second.second.at(j)->legal_position_LD.y -= componentSpace;
                        }
                        offset -= componentSpace;
                    }
                    i++;
                }

                if(offset > 0){
                    for(auto component : cellRow.second.second){
                        component->legal_position_LD.y -= offset;
                    }
                }
            }
        }
    }
}

void Placement::legalize(int times)
{
    this->alignComponents(times);

    this->placeInRows();
}

std::pair<double, double> Placement::getBenchmark() const
{
    return std::make_pair(this->totalDisplacement, this->maximumDisplacement);
}

bool Placement::detectLegalization()
{
    for(auto cellRow : this->cellRows){
        for(auto component : cellRow.second.second){
            if(component->getIsFixed()){
                continue;
            }

            if(component->getHeight() != cellRow.second.first.getHeight()){
                std::cout << "component " << component->getName() << " height is not equal to cellRow " << cellRow.second.first.getCoordinate() << std::endl;
            }

            if(cellRow.second.first.getIsHorizontal()){
                if(component->getLegalBoundingBox().first.x < cellRow.second.first.getBoundingBox().first.x || component->getLegalBoundingBox().second.x > cellRow.second.first.getBoundingBox().second.x){
                    std::cout << "component " << component->getName() << " x is out of cellRow " << cellRow.second.first.getCoordinate() << std::endl;
                }
            }else{
                if(component->getLegalBoundingBox().first.y < cellRow.second.first.getBoundingBox().first.y || component->getLegalBoundingBox().second.y > cellRow.second.first.getBoundingBox().second.y){
                    std::cout << "component " << component->getName() << " y is out of cellRow " << cellRow.second.first.getCoordinate() << std::endl;
                }
            }

            for(auto component2 : cellRow.second.second){
                if(component == component2){
                    continue;
                }

                if(isOverlap(*component, *component2)){
                    std::cout << component->getName() << " overlap " << component2->getName() << std::endl;
                    std::cout << component->getLegalBoundingBox().first.x << " " << component->getLegalBoundingBox().first.y << " " << component->getLegalBoundingBox().second.x << " " << component->getLegalBoundingBox().second.y << std::endl;
                    std::cout << component2->getLegalBoundingBox().first.x << " " << component2->getLegalBoundingBox().first.y << " " << component2->getLegalBoundingBox().second.x << " " << component2->getLegalBoundingBox().second.y << std::endl;
                }
            }
        }
    }

    return true;
}
