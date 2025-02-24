#include <iostream>
#include <fstream>
#include <string>
#include <regex>
#include <vector>
#include <stack>
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
        int indent = getIndentLevel(line);
        blocks.push_back({indent, line.substr(indent)});
    }
    return blocks;
}

void compileToCpp(const vector<CodeBlock>& indentedCode) {
    regex traversalRegex(R"((\w+)\s+traversal\s+\[(\d+)\.\.(\d+)\]:)");
    regex outputRegex(R"(->\s+(\w+))");
    regex inputRegex(R"(<-\s+(\w+))");
    regex assignRegex(R"((\w+)\s+<-\s+(\w+))");

    ofstream outFile("output.cpp");
    outFile << "#include <iostream>\n";
    outFile << "using namespace std;\n\n";
    outFile << "int main() {\n";

    stack<int> indentStack;

    for (size_t i = 0; i < indentedCode.size(); ++i) {
        const auto& block = indentedCode[i];
        smatch match;

        while (!indentStack.empty() && block.indentLevel <= indentStack.top()) { // Close blocks on outdent
            outFile << string(indentStack.top(), ' ') << "}\n";
            indentStack.pop();
        }

        if (regex_search(block.code, match, traversalRegex)) { // Traversal loop
            string var = match[1];
            string start = match[2];
            string end = match[3];
            outFile << string(block.indentLevel, ' ') << "for(int " << var << " = " << start << "; " << var << " <= " << end << "; " << var << "++) {\n";
            indentStack.push(block.indentLevel);
        } else if (regex_search(block.code, match, outputRegex)) { // Output
            string var = match[1];
            outFile << string(block.indentLevel, ' ') << "cin >> " << var << ";\n";
        } else if (regex_search(block.code, match, inputRegex)) { // Input
            string var = match[1];
            outFile << string(block.indentLevel, ' ') << "cout << " << var << " << endl;\n";
        } else if (regex_search(block.code, match, assignRegex)) { // Assignment
            string var1 = match[1];
            string var2 = match[2];
            outFile << string(block.indentLevel, ' ') << var1 << " = " << var2 << ";\n";
        } else { // Regular code
            outFile << string(block.indentLevel, ' ') << block.code << "\n";
        }
    }

    while (!indentStack.empty()) { // Close any remaining open blocks
        outFile << string(indentStack.top(), ' ') << "}\n";
        indentStack.pop();
    }

    outFile << "    return 0;\n";
    outFile << "}\n";
    outFile.close();

    cout << "C++ code generated in output.cpp" << endl;
    cout << "Compile using: g++ output.cpp -o output" << endl;
    cout << "Run using: ./output" << endl;
}

int main() {
    string filename = "code.zed";
    ifstream file(filename);
    
    if (!file.is_open()) {
        cerr << "Failed to open " << filename << endl;
        return 1;
    }

    vector<CodeBlock> indentedCode = readIndentedCode(file);
    file.close();

    compileToCpp(indentedCode);
    return 0;
}
