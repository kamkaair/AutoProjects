#include <iostream>
#include <string>

#include <fstream>
#include <filesystem>

// Get the desktop location
#include <Windows.h>
#include <shlobj.h>

#include <unordered_map>
#include <vector>

// For utf-8
#include <codecvt>
#include <locale>

using namespace std;

struct Characters {
    string keyName;
    string shortName;
    bool showSprite;
    //unordered_map<string, string> emotions;
};

string getWinDesktopPath() {
    string outPath;
    char winPath[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathA(NULL, CSIDL_DESKTOP, NULL, 0, winPath))) {
        outPath = std::string(winPath) + "/";
    }
    std::cout << "Current Windows desktop path: " << outPath << endl;
    std::cout << "" << endl;

    return outPath;
}

string correctChars(string& inLine, const unordered_map<string, string> unacceptables) {
    string outString = inLine;
    
    for (auto entry : unacceptables) {
        int pos = 0;
        while ((pos = outString.find(entry.first, pos)) != string::npos) {
            outString.replace(pos, entry.first.length(), entry.second); // index, old char, new char
            pos += entry.second.length();
            //cout << "Position: " << pos << " - " << entry.first << " size: " << entry.first.length() << endl;
        }
        
    }

    return outString;
}

void getTalkerAsStrings(string inLine, string& outStr, string& outStr2, string& outStr3) { // I want to make this better, this is ugly
    bool emotionStr = false, isolatedStr = false;
    string characterCache, emotionCache, isolatedLines;

    for (char chara : inLine) {
        if (chara == ':' || chara == ')') {
            isolatedStr = true;
            emotionStr = false;
        }
        else if (isolatedStr && chara != ':')
            isolatedLines += chara;   
        else if (emotionStr)
            emotionCache += chara;
        else if (chara == '(')
            emotionStr = true;
        else if(chara != ' ')
            characterCache += chara;
    }
    outStr = characterCache;
    outStr2 = emotionCache;
    outStr3 = isolatedLines;
}

string findCorresponding(string& inCharacter, const unordered_map<string, string> map) {
    for (auto entry : map) {
        if (inCharacter == entry.first)
            return entry.second;
    }

    return "error";
}

void initCharacters(unordered_map<string, Characters>& map) {
    //map["Protagonist-kun"] = { "mc", { {"angry", "angry"}, {"sad", "sad"}, {"happy", "happy"}, {"sad", "sad"}, {"surprised", "surprised"}, {"neutral", ""} } };
    //map["Narrator"] = { "", { {"happy", "smile"}, {"sad", "frown"}, {"angry", "angry"} } };
    
    // 1.Keyname (only used in sprite rendering), 2.shortname and 3.if has a sprite.
    map["Protagonist-kun"] = { "protagonist mask", "mc ", true};
    map["Protagonist - kun"] = { "protagonist mask", "mc ", true };
    map["Narrator"] = { "", "", false};
    map["Mask"] = { "", "mk ", false };

    map["Slime"] = { "susan", "sg ", true };
    map["Susan"] = { "susan", "sg ", true };

    map["Elf"] = { "ayla", "pg ", true };
    map["Ayla"] = { "ayla", "pg ", true };

    map["Fairy"] = { "evelyn", "lg ", true };
    map["Evelyn"] = { "evelyn", "lg ", true };

    map["Tessa"] = { "tessa", "cg ", true };
    map["Tiger"] = { "tessa", "cg ", true };

    map["Truck-kun"] = { "truck-kun", "tk ", true };

    map["Writer"] = { "", "wr ", false };
    map["Announcer"] = { "", "ar ", false };
    
    //vec.push_back({ "Protagonist-kun", "mc", {"angry", "happy", "sad", "surprised"}});
}

int main() {
    string textIn, textOut, path;
    path = getWinDesktopPath();

    ifstream inputFile;
    ofstream outputFile;

    const unordered_map<string, string> characterCorrectMap{
        {"&", "..."},
        { "\x1c", "" },
        { "\x1d", "" },
        { "\x19", "'" }, // Added the UTF-8 conversion codes, since they are being printed into the output. Should've probably used something else...

        //{"“", ""},
        //{"”", ""},
        //{"‘", "'"},
        //{"’", "'"},
        //{"…", "..."},
    };

    const unordered_map<string, string> nameColorCorrectMap{
        { "Protagonist-kun", "[mc_color]" },
        { "Protagonist", "[mc_color]" },
        { "Mask", "[mk_color]" },

        { "Susan", "[sg_color]" },
        { "Slime", "[sg_color]" },

        { "Ayla", "[pg_color]" },
        { "Elf", "[pg_color]" },

        { "Evelyn", "[lg_color]" },
        { "Fairy", "[lg_color]" },

        { "Tessa", "[cg_color]" },
        { "Tiger", "[cg_color]" },

        { "Truck-kun", "[tk_color]" },
        { "Writer", "[wr_color]" },
        { "Announcer", "[ar_color]" },
    };
    

    unordered_map<string, Characters> allCharacters;
    initCharacters(allCharacters);

    //string test = "Protagonist-kun (sad): “Sigh… Another long day of studying.”"; Protagonist-kun (sad): “I know I’m not the smartest… Or the strongest…”
    //test = correctChars(test);
    //cout << test << endl;
    
    inputFile.open(path + "chatInput.txt");
    inputFile.imbue(locale(locale(), new codecvt_utf8<char>)); // UTF-8 imbue
    if (!inputFile.is_open()) {
        std::cout << "Blyat" << endl;
        return -1;
    }
    
    string earlierEmotion, earlierCharacter;
    while (getline(inputFile, textIn)) {
        if (textIn[0] != '\0') {
            // Read the original lines and extract them
            string repairedText = correctChars(textIn, characterCorrectMap);
            string targetCharacter, targetEmotion, isolatedLines, characterID, keyName;
            bool showSprite = false;

            getTalkerAsStrings(repairedText, targetCharacter, targetEmotion, isolatedLines);

            isolatedLines = correctChars(isolatedLines, nameColorCorrectMap);

            // Transform original lines into the renpy code
            for (auto [first, second] : allCharacters) {
                if (targetCharacter == first) {
                    characterID = second.shortName;
                    keyName = second.keyName;
                    showSprite = second.showSprite;
                    break;
                }
            }

            // Write as lines
            if (((earlierEmotion != targetEmotion) || (targetCharacter != earlierCharacter)) && (showSprite))
                textOut.append("show " + keyName + " " + targetEmotion + '\n');
            
            isolatedLines.erase(0, isolatedLines.find_first_not_of(" \t")); // workaround for my spaghetti... getTalkerAsStrings adds a space in the beginning
            textOut.append(characterID + "\"" + isolatedLines + "\"" + '\n');
            textOut.append("\n");

            earlierEmotion = targetEmotion;
            earlierCharacter = targetCharacter;
        }
    }
    inputFile.close();


    outputFile.open(path + "chatOutput.txt");
    if (!outputFile.is_open()) {
        std::cout << "Blyat" << endl;
        return -1;
    }
    outputFile << textOut;
    outputFile.close();


    std::cout << textOut << endl;
    
    return 0;
}
