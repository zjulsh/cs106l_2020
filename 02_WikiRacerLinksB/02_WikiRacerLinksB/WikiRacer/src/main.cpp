
#include <iostream>
#include <unordered_set>
#include <vector>
#include <queue>
#include <unordered_map>
#include "wikiscraper.h"


using std::cout;            using std::endl;
using std::string;          using std::vector;
using std::priority_queue;  using std::unordered_map;
using std::unordered_set;



/*
 * This is the function you will be implementing. It takes
 * two string representing the names of a start_page and
 * end_page and is supposed to return a ladder, represented
 * as a vector<string>, of links that can be followed from
 * start_page to get to the end_page.
 *
 * For the purposes of this algorithm, the "name" of a Wikipedia
 * page is what shows at the end of the URL when you visit that page
 * in your web browser. For ex. the name of the Stanford University
 * Wikipedia page is "Stanford_University" since the URL that shows
 * in your browser when you visit this page is:
 *
 *       https://en.wikipedia.org/wiki/Stanford_University
 */
vector<string> findWikiLadder(const string& start_page, const string& end_page) {
    WikiScraper scraper;

    auto target_set = scraper.getLinkSet(end_page);

    auto cmpFn = [&](const vector<string>& a, const vector<string>& b) -> bool{
        string a_last = a.back(), b_last = b.back();
        vector<string> a_common, b_common;

        auto a_last_link = scraper.getLinkSet(a_last);
        auto b_last_link = scraper.getLinkSet(b_last);

        std::set_intersection(a_last_link.begin(), a_last_link.end(),
                              target_set.begin(), target_set.end(),
                              std::back_inserter(a_common));
        std::set_intersection(b_last_link.begin(), b_last_link.end(),
                              target_set.begin(), target_set.end(),
                              std::back_inserter(b_common));

        return a_common.size() < b_common.size();
    };

    priority_queue<vector<string>, vector<vector<string>>, decltype(cmpFn)> ladderQueue(cmpFn);
    unordered_set<string> seen;
    vector<string> ladder = {start_page};
    ladderQueue.push(ladder);
    seen.insert(start_page);

    while(ladderQueue.size()){
        ladder = ladderQueue.top();
        ladderQueue.pop();
        auto current_page = ladder.back();
        auto links = scraper.getLinkSet(current_page);

        //seen.insert(current_page);

        for (auto i : ladder)
            cout << i << ' ';
        cout << endl;

        cout << "=========================" << endl;
        cout << ladder.back() << ":" << links.size() << endl;
        cout << "=========================" << endl;

        if (links.count(end_page)){
            ladder.push_back(end_page);
            return ladder;
        }else{
            seen.insert(current_page);

            for (string i : links){
                if (!seen.count(i)){
                    auto test = scraper.getLinkSet(i);
                    vector<string> test_commom;
                    std::set_intersection(test.begin(), test.end(),
                                          target_set.begin(), target_set.end(),
                                          std::back_inserter(test_commom));

                    if (test_commom.size() > 0)
                        cout << i << ':' << test_commom.size() << endl;
                    auto new_ladder = ladder;
                    new_ladder.push_back(i);
                    ladderQueue.push(new_ladder);
                    seen.insert(i);
                }
            }
        }
    }

    vector<string> empty_ladder;
    return empty_ladder;
}



int main() {
    auto ladder = findWikiLadder("Milkshake", "Gene");
    cout << endl;

    if(ladder.empty()) {
        cout << "No ladder found!" << endl;
    } else {
        cout << "Ladder found:" << endl;
        cout << "\t";

        for (auto i : ladder)
            cout << i << ' ';
        cout << endl;
    }

    return 0;
}




