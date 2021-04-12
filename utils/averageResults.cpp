#include <fstream>
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

int main(int argc, char** argv)
{
    std::ifstream infile(argv[1]);
    std::vector<double> vect(1000, 0.0);
    std::string line;
    int lineCount = 0;

    while(std::getline(infile, line))
    {
        std::stringstream ss(line);

        int index = 0;

        for (double i; ss >> i;)
        {
            vect.at(index) += i;

            if (ss.peek() == ',')
            {
                ss.ignore();
            }

            index++;
        }

        lineCount++;
    }

    for (size_t i = 0; i < vect.size(); i++)
    {
        std::cout << vect[i] / static_cast<double>(lineCount) << ",";
    }

    return 0;
}
