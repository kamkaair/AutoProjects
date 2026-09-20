#include <iostream>
#include <filesystem>
#include <string>

using namespace std;

//const string path = "D:/Koulu/OmatProjektit/AutoProjects";
const string path = "D:/Koulu/OpenGL/FantaInMySystem/assets/Saves";
//const string path = "D:/Koulu/OpenGL/FantaInMySystem/assets/textures";

string convertToFormat(const uintmax_t& fileSize) {
    const string types = "BKMGTPE";
    double mantissa = fileSize;

    int index = 0;
    for (; mantissa >= 1024.0; index++) {
        mantissa = mantissa /= 1024.0;  
        cout << "Mantissa: " << mantissa << " - ";
    }
    
    string result = to_string(ceil(mantissa * 10.0) / 10.0); // Shift the decimal by one and shift it back after std::ceiling (1.47 -> 14.7 -> ceil(15) -> 1.5)
    result.erase(result.find(".") + 2); // Obliterate the trailing numbers
    result += types[index];
    if (index != 0) {     
        result += "B";
    }

    return result;
}

int main()
{
    for (std::filesystem::directory_entry const& entry : std::filesystem::directory_iterator(path))
        if (entry.is_regular_file())
            cout << convertToFormat(entry.file_size()) << endl;
}