#include <iostream>
#include <fstream>
#include <unordered_set>
#include <functional>
#include <algorithm>

using std::cout;            using std::endl;
using std::string;          using std::unordered_set;

void readFileName(string& fileName, string &fileContent);
void readFileContent(string& fileContent, unordered_set<string>& set);

int main() {
	
    /* TODO: Write code here! */

    /* Note if your file reading isn't working, please follow the
     * steps posted on the front page of the CS106L website. Essentially
     * go to the projects tab on the panel on the left, and in the run
     * section, uncheck the "Run in terminal" box and re-check it. This
     * should fix things.
     */
    string fileName, fileContent;
    unordered_set<string> set;

    readFileName(fileName, fileContent);
    //cout << fileContent << endl;
    readFileContent(fileContent, set);

    return 0;
}


void readFileName(string& fileName, string& fileContent){
    std::ifstream f;
    string tmp;

    cout << "Please input file name-" << endl;
    std::cin >> fileName;
    f.open(fileName);

    if (!f.is_open()){
        cout << "Fail open!" << endl;
        return;
    }

    while (std::getline(f, tmp)) fileContent += tmp;
}

void readFileContent(string& fileContent, unordered_set<string>& set){
    string pattern = "href=\"/wiki/";
    auto it_begin = fileContent.begin(),it_end = fileContent.end();
    std::string tmp;

    while(it_begin!=it_end){
        auto st = std::search(it_begin, it_end, pattern.begin(), pattern.end());
        if(st == it_end) break;
        else st += pattern.size();

        auto end = std::find(st, it_end,'\"');
        if(st==it_end) break;

        tmp = std::string(st, end);
        if(std::all_of(st, end, [](char c){return (c!=':')&&(c!='#');}))  set.insert(tmp);

        it_begin = end;
    }

    for (auto i : set){
        cout << i << endl;
    }
}
