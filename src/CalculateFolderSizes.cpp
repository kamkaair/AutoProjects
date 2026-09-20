#include <iostream>
#include <filesystem>
#include <string>

using namespace std;

//const string path = "D:/Koulu/OmatProjektit/AutoProjects";
//const string path = "D:/Koulu/OpenGL/FantaInMySystem/assets/Saves";
//const string path = "D:/Koulu/OpenGL/FantaInMySystem/assets/textures";
const string path = "D:/Koulu/OpenGL/FantaInMySystem";

tuple<double, int> convertToDouble(const uintmax_t& fileSize) {
    double mantissa = fileSize;
    int index = 0;

    for (; mantissa >= 1024.0; index++) { // Determine size, index will determine the unit of digital information
        mantissa = mantissa /= 1024.0;
    }
    return make_tuple(mantissa, index);
}

string convertToString(double& mantissa, int& index) {
    const string types = "BKMGTPE";

    string result = to_string(ceil(mantissa * 10.0) / 10.0); // Shift the decimal by one and shift it back after std::ceiling (1.47 -> 14.7 -> ceil(15) -> 1.5)
    result.erase(result.find(".") + 2); // Obliterate the trailing numbers
    result += types[index];
    if (index != 0) {
        result += "B";
    }

    return result;
}

uintmax_t iterateDirectoryChild(const string& path, bool couting = false) {
    uintmax_t totalResult = 0;
    for (std::filesystem::directory_entry const& entry : std::filesystem::directory_iterator(path)) {
        if (entry.is_regular_file())
            totalResult += entry.file_size();
        else if (entry.is_directory())
            totalResult += iterateDirectoryChild(entry.path().string());
    }
    
    if (couting) {
        tuple<double, int> result = convertToDouble(totalResult);
        cout << path << " - " << convertToString(get<0>(result), get<1>(result)) << endl;
    }
        
    return totalResult;
}

void iterateDirectoryMain(const string& path) {
    vector<uintmax_t> totalSpace;
    uintmax_t totalResult = 0, totality = 0;

    filesystem::directory_options settings = filesystem::directory_options::skip_permission_denied | filesystem::directory_options::follow_directory_symlink;
    for (filesystem::directory_entry const& entry : filesystem::directory_iterator(path, settings)) {
        if (entry.is_regular_file())
            totalResult += entry.file_size();
        else if (entry.is_directory())
            totalSpace.push_back(iterateDirectoryChild(entry.path().string(), true));
    }

    for (const uintmax_t& total : totalSpace) {
        tuple<double, int> result = convertToDouble(total);
        cout << path << " - " << convertToString(get<0>(result), get<1>(result)) << endl;
        totality += total;
    }

    tuple<double, int> resultTotal = convertToDouble(totality);
    cout << "Total used space: " << convertToString(get<0>(resultTotal), get<1>(resultTotal)) << endl;
}

int main() {
    iterateDirectoryMain(path);
}