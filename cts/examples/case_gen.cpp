#include <cstdlib>
#include <ctime>
#include <fstream>
#include <iostream>
#include <set>
#include <string>

struct Point {
    int x;
    int y;

    Point() : x(0), y(0) {}
    Point(int x, int y) : x(x), y(y) {}

    bool operator<(const Point& other) const {
        if (x != other.x) {
            return x < other.x;
        }
        return y < other.y;
    }
};

int main(int argc, char** argv) {
    if (argc != 6) {
        std::cerr << "usage: " << argv[0] << " <dimX> <dimY> <point_count> <output.cts> <seed>\n";
        return 1;
    }

    const int dimX = std::atoi(argv[1]);
    const int dimY = std::atoi(argv[2]);
    const int pointCount = std::atoi(argv[3]);
    const std::string outputPath = argv[4];
    const unsigned int seed = static_cast<unsigned int>(std::strtoul(argv[5], nullptr, 10));

    if (dimX <= 2 || dimY <= 2 || pointCount < 2) {
        std::cerr << "dimensions must be > 2 and point_count must be >= 2\n";
        return 1;
    }

    if ((dimX - 1) * (dimY - 1) < pointCount) {
        std::cerr << "not enough unique grid points for requested point_count\n";
        return 1;
    }

    std::srand(seed);

    std::set<Point> sinks;
    while (static_cast<int>(sinks.size()) < pointCount - 1) {
        sinks.insert(Point(std::rand() % (dimX - 1) + 1, std::rand() % (dimY - 1) + 1));
    }

    Point source;
    do {
        source = Point(std::rand() % (dimX - 1) + 1, std::rand() % (dimY - 1) + 1);
    } while (sinks.count(source) != 0);

    std::ofstream out(outputPath);
    if (!out) {
        std::cerr << "failed to open output file: " << outputPath << "\n";
        return 1;
    }

    out << ".p " << pointCount << '\n';
    out << ".dimx " << dimX << '\n';
    out << ".dimy " << dimY << '\n';
    out << source.x << ' ' << source.y << '\n';
    for (const auto& sink : sinks) {
        out << sink.x << ' ' << sink.y << '\n';
    }
    out << ".e";
}
