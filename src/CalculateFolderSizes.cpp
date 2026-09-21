#include <iostream>
#include <filesystem>
#include <string>
#include <functional>

using namespace std;

const string path = "D:/Blender";

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

void tryCatch(const string& path, function<void()> func) {
    try {
        func();
    }
    catch (const filesystem::filesystem_error& error) {
        cerr << "SKIPPED: unreadable file: " << path << endl;
    }
}

uintmax_t iterateDirectory(const string& path) {
    uintmax_t totalResult = 0;

    filesystem::directory_options settings = filesystem::directory_options::skip_permission_denied | filesystem::directory_options::follow_directory_symlink;
    for (filesystem::directory_entry const& entry : filesystem::directory_iterator(path, settings)) {     
        tryCatch(path, [&entry, &totalResult]() {
            if (entry.is_regular_file()) {
                totalResult += entry.file_size();
            }
            else if (entry.is_directory()) {
                totalResult += iterateDirectory(entry.path().string());
            }
        });
    }

    return totalResult;
}

int main() {
    uintmax_t totalSpaceTaken = 0, totalSizeOfFiles = 0;

    filesystem::directory_options settings = filesystem::directory_options::skip_permission_denied | filesystem::directory_options::follow_directory_symlink;
    for (filesystem::directory_entry const& entry : filesystem::directory_iterator(path, settings)) {
        tryCatch(path, [&totalSizeOfFiles, &entry] {
            if (entry.is_regular_file()) {
                totalSizeOfFiles += entry.file_size();
            }
            else if (entry.is_directory()) {
                uintmax_t dirTotal = iterateDirectory(entry.path().string());
                totalSizeOfFiles += dirTotal;

                tuple<double, int> resultTotal = convertToDouble(dirTotal);
                cout << entry.path().string() << " - " << convertToString(get<0>(resultTotal), get<1>(resultTotal)) << endl;
            }
        });
    }

    cout << endl;
    tuple<double, int> resultTotal = convertToDouble(totalSpaceTaken);
    cout << "Total used space in " << path << " - " << convertToString(get<0>(resultTotal), get<1>(resultTotal)) << endl;
}