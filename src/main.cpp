/*--------------------------------------------------------------------------
 Copyright (c) 2020 Mark Elrod. All rights reserved. This file is
 distributed under the MIT License. See LICENSE.TXT for details.
 --------------------------------------------------------------------------*/

#include <getopt.h>
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <fstream>
#include <iomanip>

using namespace std;

char chars[256] = {
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 0
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0, // 1
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, // 2
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, // 3
    
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, // 4
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, // 5
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,1, // 6
    1,1,1,1, 1,1,1,1, 1,1,1,1, 1,1,1,0, // 7
    
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0,
    0,0,0,0, 0,0,0,0, 0,0,0,0, 0,0,0,0
};

inline char cleanChar(char c) {
    if (chars[(unsigned char)c])
        return c;
    
    return '.';
}

class App {
public:
    
    App() {
        std::stringstream ss;
        
        ss << "Usage: mbr-tool -f <filename>\n";
        ss << "\n";
        ss << "  -v, --version    Displays the version info for the app.\n";
        ss << "  -h, --help       Displays help info for the app.\n";
        ss << "  -f, --filename   Input filename.\n";
        ss << "  -c, --color      Output colored text to console.\n";
        ss << "  -t, --tilde      Use tilde to mask extra data.\n";
        ss << "  -b, --blank      Use blank to mask extra data.\n";
        ss << "\n";
        
        _usage = ss.str();
    }
    
    int parseArgs(int argc, char **argv) {
        std::string optLong;
        std::string optArg;
        std::vector<char> charBuf(2);
        
        static std::vector<struct option> long_options = {
            { "version",    no_argument,            nullptr,        'v' },
            { "help",       no_argument,            nullptr,        'h' },
            { "color",      no_argument,            nullptr,        'c' },
            { "tilde",      no_argument,            nullptr,        't' },
            { "blank",      no_argument,            nullptr,        'b' },
            { "file",       required_argument,      nullptr,        'f' },
            { nullptr,      0,                      nullptr,        0   }
        };
        
        int option_index = 0;
        
        while (true) {
            int opt = getopt_long(argc, argv, "+:vhctbf:", long_options.data(), &option_index);
            
            if (opt == -1)
                break;
            
            switch (opt) {
                    
                case 0:
                    break;
                    
                case 'v':
                    _version = true;
                    break;
                    
                case 'h':
                    _help = true;
                    break;
                    
                case 't':
                    _tilde = true;
                    _replaceBytes = true;
                    _byteReplacement = "~~";
                    _charReplacement = "~";
                    break;
                    
                case 'b':
                    _blank = true;
                    _replaceBytes = true;
                    _byteReplacement = "  ";
                    _charReplacement = " ";
                    break;
                    
                case 'c':
                    _color = true;
                    break;
                    
                case 'f':
                    optLong = long_options[static_cast<size_t>(option_index)].name;
                    _filename = optarg;
                    break;
                    
                case '?': {
                    charBuf[0] = static_cast<char>(optopt);
                    std::cerr << "error: bad option: " << charBuf.data() << "\n";
                    std::cerr << _usage << "\n";
                    _error = 1;
                }
                    return 0;
                    
                case ':': {
                    charBuf[0] = static_cast<char>(optopt);
                    std::cerr << "error: mission argument for option: " << charBuf.data() << "\n";
                    std::cerr << _usage << "\n";
                    _error = 1;
                }
                    return 0;
                    
                default:
                    std::cerr << "error: invalid argument\n";
                    std::cerr << _usage;
                    _error = 1;
                    return 1;
            }
        }
        
        int rc = 0;
        
        if (_help)
            std::cout << _usage;
        else if (_version)
            std::cout << "version " << _versionNum << " " << _devStage << "\n";
        
        return rc;
    }
    
    void dumpBuf(char *buf, size_t start, size_t end, const string &label = "") {
        cout << "          " << label << "\n";
        
        if (_color)
            cout << "\033[0m";
        
        cout << hex << setw(2) << setfill('0') << right << uppercase;
        
        size_t bytesPerLine = 16;
        size_t spaceInterval = 1;
        size_t spaceInterval2 = 8;
        size_t addrLength = 8;
        
        size_t _lineStart = start/bytesPerLine * bytesPerLine;
        size_t count = (end + bytesPerLine - 1)/bytesPerLine*bytesPerLine;
        
        for (size_t lineStart = _lineStart; lineStart < count; lineStart += bytesPerLine) {
            size_t lineEnd = lineStart + bytesPerLine;
            
            cout << hex << setw(addrLength) << setfill('0') << uppercase << lineStart << "  ";
            
            for (size_t i = lineStart; i < lineEnd; ++i) {
                if ((i != lineStart) && (i % spaceInterval) == 0)
                    cout << " ";
                if ((i != lineStart) && (i % spaceInterval2) == 0)
                    cout << " ";
                
                if (_color) {
                    if (i >= start && i < end)
                        cout << "\033[1;36m";
                    else
                        cout << "\033[2;37m";
                }
                
                if (_replaceBytes && (i < start || i >= end))
                    cout << _byteReplacement;
                else
                    cout << setw(2) << setfill('0') <<(unsigned)(unsigned char)buf[i];
                
                if (_color)
                    cout << "\033[0m";
            }
            
            cout << "  |";
            
            for (size_t i = lineStart; i < lineEnd; ++i) {
                
                if (_color) {
                    if (i >= start && i < end)
                        cout << "\033[0m";
                    else
                        cout << "\033[2;37m";
                }
                
                if (_replaceBytes && (i < start || i >= end))
                    cout << _charReplacement;
                else
                    cout << cleanChar(buf[i]);
                
                if (_color)
                    cout << "\033[0m";
            }
            
            cout << "|\n";
        }
        
        cout << "\n";
    }
    
    int run(int argc, char **argv) {
        
        int rc = parseArgs(argc, argv);
        
        if (rc != 0)
            return rc;
        
        if (_help || _version)
            return 0;
        
        if (!_filename.size()) {
            cout << "error: filename is a required argument\n";
            cout << _usage;
            return 1;
        }
        
        std::ifstream file;
        file.open(_filename, ios::in | ios::binary);
        
        if (!file.is_open()) {
            cout << "error: could not open file '" << _filename << "'\n";
            return 1;
        }
        
        file.seekg(0, std::ios::end);
        size_t size = file.tellg();
        file.seekg(0);
        
        if (size > 512) {
            cout << "error: file size is larger than 512: " << size << "'\n";
            file.close();
            return 1;
        }
        
        vector<char> vec(size);
        file.read(vec.data(), size);
        file.close();
        
        cout << "\n";
        
        dumpBuf(vec.data(), 0,      3,      "Jump Instruction");
        dumpBuf(vec.data(), 3,      11,     "OEM Name");
        dumpBuf(vec.data(), 11,     101,    "BPB");
        dumpBuf(vec.data(), 101,    446,    "Code");
        dumpBuf(vec.data(), 446,    509,    "Partition Table");
        dumpBuf(vec.data(), 509,    510,    "Physical Drive Number");
        dumpBuf(vec.data(), 510,    512,    "Boot Signature");
        
        return rc;
    }
    
private:
    string  _filename;
    bool    _version = false;
    bool    _help = false;
    string  _usage;
    int     _error = 0;
    string  _versionNum = "0.0.0";
    string  _devStage = "beta";
    bool    _color = false;
    bool    _tilde = false;
    bool    _blank = false;
    bool    _replaceBytes = false;
    string  _byteReplacement;
    string  _charReplacement;
};

int main(int argc, char **argv) {
    App app;
    return app.run(argc, argv);
}

