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
    
    //Comment
    regex commentRegex(R"(#.*)");
    regex openCommentRegex(R"(\{.*)");
    regex closeCommentRegex(R"(.*\})");
    //Variable
    regex variableDeclarationRegex(R"((\w+)\s*:\s*(integer|string|char|bool|float))");
    regex assignRegex(R"((\w+)\s+<-\s+(.+))");
    
    //IO
    regex outputRegex(R"(output\((.+)\))");
    regex inputRegex(R"(input\((.+)\))");
    
    //Conditional
    regex ifThenRegex(R"(if\s*\((.+)\)\s*then)");
    regex elseRegex(R"(^\s*else)");
    
    //looping
    regex traversalRegex(R"((\w+)\s+traversal\s+\[(\w+)\.\.(\w+)\]:)");
    regex whileDoRegex(R"(while\s*\((.+)\)\s*do:)");
        //do-while
    regex doRegex(R"(do:)");
    regex whileRegex(R"(while\s*\((.+)\))");
        //repeat-until
    regex repeatRegex(R"(repeat:)");
    regex untilRegex(R"(until\s*\((.+)\))");

    //function
    regex returnRegex(R"(->\s+(\w+))");
    


    
    


    ofstream outFile("output.py");
    outFile << "# Generated Python code\n";

    unordered_map<string, string> variables;
    bool inAlgorithm = false;
    stack<int> indentStack;

    //waiting convention
    int waitingCloseComment = 0;
    int waitingForElse = 0;
    int waitingDoWhile = 0;
    int waitingRepeatUntil = 0;

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
        
        if (regex_search(block.code, match, commentRegex)) {
            outFile << string(block.indentLevel, ' ') << block.code;

        //  Looping
            
            //traversal
        } else if (regex_search(block.code, match, traversalRegex)) {
            outFile << string(block.indentLevel, ' ') << "for " << match[1] << " in range(int(" << match[2] << "), int(" << match[3] << ") + 1):";
            indentStack.push(block.indentLevel);
            //while-do
        } else if (regex_search(block.code, match, whileDoRegex)) {
            outFile << string(block.indentLevel, ' ') << "while " << match[1] << ":";
            indentStack.push(block.indentLevel);
            //do-while
        } else if (regex_search(block.code, match, doRegex)) {
            outFile << string(block.indentLevel, ' ') << "while True:";
            indentStack.push(block.indentLevel);
            waitingDoWhile++;
        } else if (waitingDoWhile > 0 && regex_search(block.code, match, whileRegex)) {
            outFile << "    " << string(block.indentLevel, ' ') << "if not (" << match[1] << "):\n";
            outFile << "    " << string(block.indentLevel, ' ') << "    break\n";
            waitingDoWhile--;
            //repeat-until
        } else if (regex_search(block.code, match, repeatRegex)) {
            outFile << string(block.indentLevel, ' ') << "while True:";
            indentStack.push(block.indentLevel);
            waitingRepeatUntil++;
        } else if (waitingRepeatUntil > 0 && regex_search(block.code, match, untilRegex)) {
            outFile << "    " << string(block.indentLevel, ' ') << "if (" << match[1] << "):\n";
            outFile << "    " << string(block.indentLevel, ' ') << "    break\n";
            waitingRepeatUntil--;


        //  Input/output
        } else if (regex_search(block.code, match, inputRegex)) {
            outFile << string(block.indentLevel, ' ') << match[1] << " = input()";
        } else if (regex_search(block.code, match, outputRegex)) {
            outFile << string(block.indentLevel, ' ') << "print(" << match[1] << ")";
        

        //  Var
        } else if (regex_search(block.code, match, assignRegex)) {
            outFile << string(block.indentLevel, ' ') << match[1] << " = " << match[2] << "";
        } else if (regex_search(block.code, match, returnRegex)) {
            outFile << string(block.indentLevel, ' ') << "return " << match[1] << "";
        

        //  Conditional
        } else if (regex_search(block.code, match, ifThenRegex)) {
            outFile << string(block.indentLevel, ' ') << "if " << match[1] << ":";
            indentStack.push(block.indentLevel);
            waitingForElse++;
        } else if ((waitingForElse > 0) && regex_search(block.code, match, elseRegex)) {
            outFile << string(block.indentLevel, ' ') << "else:";
            indentStack.push(block.indentLevel);
            waitingForElse--;
        

        //  Misc
        } else {
            if (block.code.find('{') == string::npos && waitingCloseComment == 0) {
                outFile << string(block.indentLevel, ' ') << block.code << "";
            }
            // outFile << string(block.indentLevel, ' ') << block.code << "\n";
        }

        //comment
        if (regex_search(block.code, match, openCommentRegex)) {
            size_t pos = block.code.find('{');
            if (pos != string::npos) {
                outFile << "# " << block.code.substr(pos);
            } else {
                outFile << string(block.indentLevel, ' ') << "# " << block.code;
            }
            if(!regex_search(block.code, match, closeCommentRegex)){
                waitingCloseComment++;
            }
            
        } else if (waitingCloseComment > 0) {
            if(regex_search(block.code, match, closeCommentRegex)){
                waitingCloseComment--;
            }
            outFile << string(block.indentLevel, ' ') << "# " << block.code;
        }
        outFile << "\n";
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
