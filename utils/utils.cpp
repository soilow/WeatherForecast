#include "utils.h"

std::string DateFormatting(int day) {
    std::stringstream stream;

    auto t = std::time(nullptr) + (3600 * 24 * day);
    stream << std::put_time(std::localtime(&t), "%a. %d %b") << std::endl;

    std::string temp = stream.str();
    temp.pop_back();  // Delete \n in temp

    return temp;
}

std::string SpaceToPlus(std::string city_name) {
    std::replace(city_name.begin(), city_name.end(), ' ', '+');
    
    return city_name;
}

 char getch() {
    static struct termios old, current;
    char ch;
    
    tcgetattr(0, &old);
    current = old;
    current.c_lflag &= ~ICANON;
     current.c_lflag &= ~ECHO;
    tcsetattr(0, TCSANOW, &current);
    
    ch = getchar();
    
    tcsetattr(0, TCSANOW, &old);
    
    return ch;
}
