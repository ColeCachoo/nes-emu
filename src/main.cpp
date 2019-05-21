// main.cpp : Defines the entry point for the application.
//

#include "main.h"

using namespace std;

void parse_results();
void compare_logs();

int main() {
    cout << "Hello nes-emu." << "\n";

    RAM ram;

    Mapper mapper(&ram);
    CPU cpu(&ram);

    // parse_results();
    compare_logs();

    return 0;
}

void parse_results() {
    const string test_log = "nestest.log.txt";
    // TODO: Get it to work without the full address on Windows.
    ifstream cpu_test_log("../resources/" + test_log);
    // ifstream cpu_test_log("E:/Documents/programming/nes-emu/resources/" + test_log);
    if (!cpu_test_log.is_open()) {
        cout << "Failed to open " + test_log + "\n";
        return;
    }

    ofstream parsed_log("../resources/parsed_log.txt");
    if (!parsed_log.is_open()) {
        puts("Failed to open file for writing.");
        exit(1);
    }


    string line;
    while(getline(cpu_test_log, line)) {
        line += "\n";
        vector<string> tokens;
        size_t tok_beg = line.find_first_not_of(" \t\n");
        size_t tok_end = line.find_first_of(" \t\n");
        while (tok_beg != string::npos || tok_end != string::npos) {
            string tok = "";
            while (tok_beg != tok_end) {
                tok += line.at(tok_beg);
                tok_beg++;
            }
            tokens.push_back(tok);

        tok_beg = line.find_first_not_of(" \t\n", tok_beg);
        tok_end = tok_beg;
        tok_end = line.find_first_of(" \t\n", tok_end);
        }

        string result = "";
        result += tokens[0] + " " + tokens[1] + " ";
        for (size_t i = 2; i < tokens.size(); i++) {
            // printf("%s ", s.c_str());
            if (tokens.at(i).find("A:") != string::npos) {
                result += tokens.at(i) + " ";
                result += tokens.at(i + 1) + " ";
                result += tokens.at(i + 2) + " ";
                result += tokens.at(i + 3) + " ";
                result += tokens.at(i + 4);
                break;
            }
        }
        parsed_log << result << "\n";
    }

    cpu_test_log.close();
    parsed_log.close();
}

void compare_logs() {
    // TODO: Get it to work without the full address on Windows.
    ifstream my_log("../resources/my_log.txt");
    ifstream parsed_log("../resources/parsed_log.txt");
    // ifstream my_log("E:/Documents/programming/nes-emu/resources/my_log.txt");
    // ifstream parsed_log("E:/Documents/programming/nes-emu/resources/parsed_log.txt");
    if (!my_log.is_open() || !parsed_log.is_open()) {
        puts("Failed to open log for comparison");
        exit(1);
    }

    string my_log_line = "";
    string parsed_log_line = "";
    string old_my_line = "";
    string old_parsed_line = "";
    while (getline(my_log, my_log_line) && getline(parsed_log, parsed_log_line)) {
        if (my_log_line != parsed_log_line) {
            cout << "Expected\n" << 
                old_parsed_line << "\n" << parsed_log_line << "\n";
            cout << "Got\n" << 
                old_my_line << "\n" <<  my_log_line << "\n";
            break;
        }
        old_my_line = my_log_line;
        old_parsed_line = parsed_log_line;
    }
}