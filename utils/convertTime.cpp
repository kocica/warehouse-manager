#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    std::ifstream infile(argv[1]);
    std::string line;

    if(std::getline(infile, line))
    {
        std::stringstream ss(line);

        for (double i; ss >> i;)
        {
            std::cout << i / 60.0 << ",";

            if (ss.peek() == ',')
            {
                ss.ignore();
            }
        }
    }

    std::cout << std::endl;

    return 0;
}
