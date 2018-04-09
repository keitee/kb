#include <string>
#include <fstream>
#include <vector>

void tail_with_no_reverse()
{
    std::vector<std::string> svec;
    ifstrem ifs{"README", ifstream::in};
    std::string line;

    const size_t MAX_LINE{10};

    size_t current_line = 0;

    while (getline(ifs, line)) {
        if (current_line < MAX_LINE)
            svec.push_back(line);
        else
            svec[current_line % MAX_LINE] = line;

        ++current_line;
    }

    for (size_t i = 0;  i < MAX_LINE; ++i) {
       current_line %= MAX_LINE;

       cout << "(" << current_line << ")" << svec[current_line] << endl;

       ++current_line;
    }
}

void tail_win_reverse()
{
    std::vector<std::string> svec;
    std::ifstream ifs{"README", std::ifstream::in};
    std::string line;

    const size_t MAX_LINE{10};

    size_t current_line=0;

    while (getline(ifs, line))
    {
        if(current_line < MAX_LINE)
            svec.push_back(line);
    }

    for(const auto & e : svec)
    {
        std::cout << "x" << std::endl;
    }

    line.find_first_not_of()
}

