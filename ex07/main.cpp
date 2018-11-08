#include <iostream>
#include <string>
#include <cstdint>
#include <fstream>
#include <streambuf>

using namespace std;

namespace Brainfuck
{
constexpr int CELLS_COUNT = 30000;

class BrainfuckInterpreter
{
private:
    char cells[CELLS_COUNT];
    uint16_t cellIndex = 0;

public:
    BrainfuckInterpreter()
    {
        memset(cells, 0, sizeof(cells));
    }

    void execute(const string& program)
    {
        for (auto it = program.cbegin(); it != program.cend(); it++)
        {
            switch (*it)
            {
                case '>':
                cellIndex++;
                break;

                case '<':
                cellIndex--;
                break;

                case '+':
                cells[cellIndex]++;
                break;

                case '-':
                cells[cellIndex]--;
                break;

                case '.':
                cout.put(cells[cellIndex]);
                break;

                case ',':
                cin.get(cells[cellIndex]);
                break;

                case '[':
                if (cells[cellIndex] == 0)
                {
                    int status = 0;
                    auto it2 = it + 1;
                    while (*it2 != ']' || status != 0)
                    {
                        if (*it2 == ']')
                            status++;
                        else if (*it2 == '[')
                            status--;
                        it2++;
                    }
                    it = it2;
                }
                break;

                case ']':
                if (cells[cellIndex] != 0)
                {
                    int status = 0;
                    auto it2 = it - 1;
                    while (*it2 != '[' || status != 0)
                    {
                        if (*it2 == ']')
                            status++;
                        else if (*it2 == '[')
                            status--;
                        it2--;
                    }
                    it = it2;
                }
                break;
            }
        }
    }
};
}

int main()
{
    const string filename = "mandelbrot.b";
    ifstream file(filename);
    if (!file)
    {
        cout << "Could not open " << filename << endl;
        return 0;
    }

    std::string program((std::istreambuf_iterator<char>(file)),
                         std::istreambuf_iterator<char>());

    Brainfuck::BrainfuckInterpreter interpreter;
    interpreter.execute(program);
    //interpreter.execute("+[-[<<[+[--->]-[<<<]]]>>>-]>-.---.>..>.<<<<-.<+.>>>>>.>.<<.<-.");
    return 0;
}
