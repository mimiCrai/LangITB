#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <stack>
#include <unordered_map>
using namespace std;

struct CodeBlock {
    int indentLevel;
    string code;
};

int getIndentLevel(const string& line) {
    int count = 0;
    for (char c : line) {
        if (isspace(c)) count++;
        else break;
    }
    return count;
}

vector<CodeBlock> readIndentedCode(ifstream &file) {
    vector<CodeBlock> blocks;
    string line;
    while (getline(file, line)) {
        if (!line.empty() && line.back() == '\r') {
            line.pop_back();
        }
        int indent = getIndentLevel(line);
        blocks.push_back({indent, line.substr(indent)});
    }
    return blocks;
}

void compileToPy(const vector<CodeBlock>& indentedCode) {
    regex traversalRegex(R"((\w+)\s+traversal\s+\[(\w+)\.\.(\w+)\]:)");
    regex returnRegex(R"(->\s+(\w+))");
    regex outputRegex(R"(output\((.+)\))");
    regex inputRegex(R"(input\((.+)\))");
    regex assignRegex(R"((\w+)\s+<-\s+(.+))");
    regex ifThenRegex(R"(if\s*\((.+)\)\s*then)");
    regex elseRegex(R"(^\s*else)");
    regex variableDeclarationRegex(R"((\w+)\s*:\s*(integer|string|char|bool|float))");

    ofstream outFile("output.py");
    outFile << "# Generated Python code\n";

    unordered_map<string, string> variables;
    bool inAlgorithm = false;
    stack<int> indentStack;
    bool waitingForElse = false;

    for (const auto& block : indentedCode) {
        if (block.code == "KAMUS") {
            continue;
        } else if (block.code == "ALGORITMA") {
            inAlgorithm = true;
            continue;
        }

        if (!inAlgorithm) {
            smatch match;
            if (regex_search(block.code, match, variableDeclarationRegex)) {
                variables[match[1]] = match[2];
                outFile << "# Declared " << match[1] << " as " << match[2] << "\n";
            }
            continue;
        }

        while (!indentStack.empty() && block.indentLevel <= indentStack.top()) {
            indentStack.pop();
        }

        smatch match;
        if (regex_search(block.code, match, traversalRegex)) {
            outFile << string(block.indentLevel, ' ') << "for " << match[1] << " in range(int(" << match[2] << "), int(" << match[3] << ") + 1):\n";
            indentStack.push(block.indentLevel);
        } else if (regex_search(block.code, match, assignRegex)) {
            outFile << string(block.indentLevel, ' ') << match[1] << " = " << match[2] << "\n";
        } else if (regex_search(block.code, match, inputRegex) && block.code.find("<-") == string::npos) {
            outFile << string(block.indentLevel, ' ') << match[1] << " = input()\n";
        } else if (regex_search(block.code, match, outputRegex)) {
            outFile << string(block.indentLevel, ' ') << "print(" << match[1] << ")\n";
        } else if (regex_search(block.code, match, returnRegex)) {
            outFile << string(block.indentLevel, ' ') << "return " << match[1] << "\n";
        } else if (regex_search(block.code, match, ifThenRegex)) {
            outFile << string(block.indentLevel, ' ') << "if " << match[1] << ":\n";
            indentStack.push(block.indentLevel);
            waitingForElse = true;
        } else if (waitingForElse && regex_search(block.code, match, elseRegex)) {
            outFile << string(block.indentLevel, ' ') << "else:\n";
            indentStack.push(block.indentLevel);
            waitingForElse = false;
        }else {
            outFile << string(block.indentLevel, ' ') << block.code << "\n";
        }
    }

    outFile.close();

    cout << "Python code generated in output.py" << endl;
    cout << "Run using: python output.py" << endl;
}


int main(int argc, char* argv[]) {
    if (argc < 2) {
        cerr << "Usage: ./compiler <filename.notal>" << endl;
        return 1;
    }

    string filename = argv[1];
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Failed to open " << filename << endl;
        return 1;
    }

    vector<CodeBlock> indentedCode = readIndentedCode(file);
    file.close();

    compileToPy(indentedCode);
    
    system("python output.py");

    return 0;
}
