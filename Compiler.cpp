#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <stack>
#include <unordered_map>

#ifdef _WIN32
    #include <windows.h>
#endif

using namespace std;

struct CodeBlock {
    int indentLevel;
    string code;
};

int getIndentLevel(const string& line) {
    int count = 0;
    for (char c : line) {
        if (c == ' ') count++;
        else if (c == '\t') count += 4;
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
        string content = line.substr(indent);
        if (!content.empty()) {
            blocks.push_back({indent, content});
        }
    }
    return blocks;
}

string mapTypeToCpp(const string& type) {
    if (type == "integer") return "int";
    if (type == "string") return "std::string";
    if (type == "char") return "char";
    if (type == "bool") return "bool";
    if (type == "float") return "float";
    return type;
}

void compileToCpp(const vector<CodeBlock>& indentedCode) {
    
    //Comment
    regex commentRegex(R"(^#.*)");
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
    
    //looping
        //while-do
    regex traversalRegex(R"((\w+)\s+traversal\s+\[(.+)\.\.(.+)\]:)");
    regex whileDoRegex(R"(while\s*\((.+)\)\s*do:)");
        //do-while
    regex doRegex(R"(^do:$)");
    regex whileRegex(R"(^while\s*\((.+)\)$)");
        //repeat-until
    regex repeatRegex(R"(^repeat:$)");
    regex untilRegex(R"(^until\s*\((.+)\)$)");

    //function (not implemented yet)
    regex returnRegex(R"(^->\s+(.+)$)");
    regex functionRegex(R"(def\s+(\w+)\s*\(\s*\)\s*:)");

    ofstream outFile("output.cpp");
    outFile << "#include <iostream>\n";
    outFile << "#include <string>\n";
    outFile << "using namespace std;\n\n";
    outFile << "int main() {\n";

    unordered_map<string, string> variables;
    bool inAlgorithm = false;
    stack<pair<int, string>> indentStack; // Store indent level and block type

    //waiting convention
    int waitingCloseComment = 0;

    for (size_t i = 0; i < indentedCode.size(); i++) {
        const auto& block = indentedCode[i];
        
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
                string cppType = mapTypeToCpp(match[2]);
                outFile << "    " << cppType << " " << match[1] << ";\n";
            }
            continue;
        }

        smatch match;
        
        // Handle comments first (but don't output them yet if they're inline)
        bool hasInlineComment = false;
        string mainCode = block.code;
        string inlineComment = "";
        
        // Check for inline comments {comment}
        size_t commentStart = block.code.find('{');
        if (commentStart != string::npos && commentStart > 0) {
            size_t commentEnd = block.code.find('}', commentStart);
            if (commentEnd != string::npos) {
                hasInlineComment = true;
                mainCode = block.code.substr(0, commentStart);
                mainCode.erase(mainCode.find_last_not_of(" \t") + 1);
                inlineComment = " /* " + block.code.substr(commentStart + 1, commentEnd - commentStart - 1) + " */";
            }
        }

        // Check if we need to close blocks BEFORE processing current line
        // Special handling for while/until that close do/repeat blocks
        bool isClosingStatement = false;
        if (regex_search(mainCode, match, whileRegex) && !indentStack.empty() && indentStack.top().second == "do") {
            isClosingStatement = true;
        } else if (regex_search(mainCode, match, untilRegex) && !indentStack.empty() && indentStack.top().second == "repeat") {
            isClosingStatement = true;
        } else if (mainCode == "else" && !indentStack.empty() && indentStack.top().second == "if") {
            isClosingStatement = true;
        }

        // Handle closing braces for indentation changes (but not for closing statements)
        if (!isClosingStatement) {
            while (!indentStack.empty() && block.indentLevel <= indentStack.top().first) {
                auto top = indentStack.top();
                indentStack.pop();
                outFile << string(top.first + 4, ' ') << "}\n";
            }
        }
        
        if (regex_search(mainCode, match, commentRegex)) {
            outFile << string(block.indentLevel + 4, ' ') << "//" << mainCode.substr(1) << "\n";

        //  Looping
        } else if (regex_search(mainCode, match, traversalRegex)) {
            outFile << string(block.indentLevel + 4, ' ') << "for (int " << match[1] << " = " << match[2] << "; " << match[1] << " <= " << match[3] << "; " << match[1] << "++) {";
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";
            indentStack.push({block.indentLevel, "for"});
            
        } else if (regex_search(mainCode, match, whileDoRegex)) {
            outFile << string(block.indentLevel + 4, ' ') << "while (" << match[1] << ") {";
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";
            indentStack.push({block.indentLevel, "while"});
            
        } else if (regex_search(mainCode, match, doRegex)) {
            outFile << string(block.indentLevel + 4, ' ') << "do {";
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";
            indentStack.push({block.indentLevel, "do"});
            
        } else if (regex_search(mainCode, match, whileRegex)) {
            // For do-while, we need to close the do block first
            if (!indentStack.empty() && indentStack.top().second == "do") {
                auto top = indentStack.top();
                indentStack.pop();
                outFile << string(top.first + 4, ' ') << "} while (" << match[1] << ");";
            } else {
                // Standalone while statement (shouldn't happen in well-formed code)
                outFile << string(block.indentLevel + 4, ' ') << "while (" << match[1] << ");";
            }
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";
            
        } else if (regex_search(mainCode, match, repeatRegex)) {
            outFile << string(block.indentLevel + 4, ' ') << "do {";
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";
            indentStack.push({block.indentLevel, "repeat"});
            
        } else if (regex_search(mainCode, match, untilRegex)) {
            // For repeat-until: repeat waits UNTIL condition is true
            // So we need: } while (!(condition));
            if (!indentStack.empty() && indentStack.top().second == "repeat") {
                auto top = indentStack.top();
                indentStack.pop();
                outFile << string(top.first + 4, ' ') << "} while (!(" << match[1] << "));";
            } else {
                // Standalone until statement (shouldn't happen in well-formed code)
                outFile << string(block.indentLevel + 4, ' ') << "// until (" << match[1] << "); - orphaned until statement";
            }
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";

        //  Input/output
        } else if (regex_search(mainCode, match, inputRegex)) {
            outFile << string(block.indentLevel + 4, ' ') << "std::cin >> " << match[1] << ";";
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";
            
        } else if (regex_search(mainCode, match, outputRegex)) {
            outFile << string(block.indentLevel + 4, ' ') << "std::cout << " << match[1] << " << std::endl;";
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";

        //  Variable assignment
        } else if (regex_search(mainCode, match, assignRegex)) {
            outFile << string(block.indentLevel + 4, ' ') << match[1] << " = " << match[2] << ";";
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";
            
        } else if (regex_search(mainCode, match, returnRegex)) {
            outFile << string(block.indentLevel + 4, ' ') << "return " << match[1] << ";";
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";

        //  Conditional
        } else if (regex_search(mainCode, match, ifThenRegex)) {
            outFile << string(block.indentLevel + 4, ' ') << "if (" << match[1] << ") {";
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";
            indentStack.push({block.indentLevel, "if"});
            
        } else if (mainCode == "else") {
            // For else, we need to close the if block first, then open else block
            if (!indentStack.empty() && indentStack.top().second == "if") {
                auto top = indentStack.top();
                indentStack.pop();
                outFile << string(top.first + 4, ' ') << "} else {";
                if (hasInlineComment) outFile << inlineComment;
                outFile << "\n";
                indentStack.push({block.indentLevel, "else"});
            }
            
        } else if (regex_search(mainCode, match, functionRegex)) {
            // Skip function definitions for now
            continue;

        //  Handle multi-line comments
        } else if (regex_search(mainCode, match, openCommentRegex) && !hasInlineComment) {
            size_t pos = mainCode.find('{');
            if (pos != string::npos) {
                if (mainCode.find('}') != string::npos) {
                    // Single line comment
                    size_t endPos = mainCode.find('}');
                    outFile << string(block.indentLevel + 4, ' ') << "/* " << mainCode.substr(pos + 1, endPos - pos - 1) << " */\n";
                } else {
                    // Multi-line comment start
                    outFile << string(block.indentLevel + 4, ' ') << "/* " << mainCode.substr(pos + 1) << "\n";
                    waitingCloseComment++;
                }
            }
        } else if (waitingCloseComment > 0) {
            if(regex_search(mainCode, match, closeCommentRegex)){
                size_t endPos = mainCode.find('}');
                outFile << string(block.indentLevel + 4, ' ') << mainCode.substr(0, endPos) << " */\n";
                waitingCloseComment--;
            } else {
                outFile << string(block.indentLevel + 4, ' ') << mainCode << "\n";
            }

        //  Misc - treat as regular statements
        } else if (!mainCode.empty() && waitingCloseComment == 0) {
            outFile << string(block.indentLevel + 4, ' ') << mainCode << ";";
            if (hasInlineComment) outFile << inlineComment;
            outFile << "\n";
        }
    }

    // Close any remaining open braces
    while (!indentStack.empty()) {
        auto top = indentStack.top();
        indentStack.pop();
        outFile << string(top.first + 4, ' ') << "}\n";
    }

    outFile << "    return 0;\n";
    outFile << "}\n";
    outFile.close();

    cout << "C++ code generated successfully!" << endl;
}

bool compileAndRun() {
    cout << "Compiling..." << endl;
    
#ifdef _WIN32
    string compileCmd = "g++ -o output.exe output.cpp";
    string runCmd = "output.exe";
#else
    string compileCmd = "g++ -o output output.cpp";
    string runCmd = "./output";
#endif

    // Compile
    int result = system(compileCmd.c_str());
    if (result != 0) {
        cerr << "Compilation failed!" << endl;
        return false;
    }
    
    cout << "Compilation successful. Running program..." << endl;
    cout << "=================== OUTPUT ===================" << endl;
    
    // Run
    result = system(runCmd.c_str());
    
    cout << "=============================================" << endl;
    return true;
}

int main(int argc, char* argv[]) {
    if (argc < 2) {
#ifdef _WIN32
        cerr << "Usage: compiler.exe <filename.notal>" << endl;
#else
        cerr << "Usage: ./compiler <filename.notal>" << endl;
#endif
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

    compileToCpp(indentedCode);
    compileAndRun();

    return 0;
}