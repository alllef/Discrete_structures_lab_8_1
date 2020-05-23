#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include<Windows.h>
#include<limits>
#include <algorithm>
#include <iomanip>
#include<stack>

using namespace std;

struct Rib {
public:
    int start;
    int end;
    int bandwidth;
};

void initializeGraph(int &picks, int &ribs, vector<Rib> &structRibs);

void sortRibs(int &picks, int &ribs, vector<Rib> &structRibs);

int getS(vector<Rib> &structRibs, int picks);

int getT(vector<Rib> &structRibs, int picks);

void changeStream(vector<int> &stream, int reserve, int startPick, int endPick, vector<Rib> structRibs);

vector<int> fordFalkersonAlgorithm(int &picks, int &ribs, vector<Rib> &structRibs);

void printStreams(int s, vector<int>stream,vector<Rib>structRibs);

int main() {
    vector<Rib> ribsList;
    int n = 0, m = 0;
    initializeGraph(n, m, ribsList);
    sortRibs(n, m, ribsList);
    SetConsoleOutputCP(CP_UTF8);
    printStreams(getS(ribsList,n),fordFalkersonAlgorithm(n, m, ribsList),ribsList);
    return 0;
}

void initializeGraph(int &picks, int &ribs, vector<Rib> &structRibs) {
    Rib myRib{};
    ifstream inFile;
    inFile.open("GraphWithScales.txt");

    if (!inFile.is_open()) cout << "It is not open" << endl;
    inFile >> picks >> ribs;

    for (int i = 0; i < ribs; i++) {
        inFile >> myRib.start >> myRib.end >> myRib.bandwidth;

        structRibs.push_back(myRib);
    }
    inFile.close();
}

void sortRibs(int &picks, int &ribs, vector<Rib> &structRibs) {

    Rib tmp{};
    for (int i = 0; i < ribs - 1; i++) {
        for (int j = 0; j < ribs - 1; j++) {
            if ((structRibs[j].start + structRibs[j].end) > (structRibs[j + 1].start + structRibs[j + 1].end)) {

                tmp = structRibs[j];
                structRibs[j] = structRibs[j + 1];
                structRibs[j + 1] = tmp;

            }
        }
    }
}

int getS(vector<Rib> &structRibs, int picks) {
    int enterCounter = 0;
    for (int i = 1; i <= picks; i++) {
        for (auto &structRib : structRibs) {
            if (structRib.end == i)enterCounter++;
        }
        if (enterCounter == 0)return i;
        enterCounter = 0;
    }
    return -1;
}

int getT(vector<Rib> &structRibs, int picks) {
    int exitCounter = 0;
    for (int i = 1; i <= picks; i++) {
        for (auto &structRib : structRibs) {
            if (structRib.start == i)exitCounter++;
        }
        if (exitCounter == 0)return i;
        exitCounter = 0;
    }
    return -1;
}

void changeStream(vector<int> &stream, int reserve, int startPick, int endPick, vector<Rib> structRibs) {
    for (int i = 0; i < structRibs.size(); i++) {
        if (structRibs[i].start == startPick && structRibs[i].end == endPick) {
            stream[i] += reserve;
            break;
        }
        if (structRibs[i].end == startPick && structRibs[i].start == endPick) {
            stream[i] -= reserve;
            break;
        }
    }
}

vector<int> fordFalkersonAlgorithm(int &picks, int &ribs, vector<Rib> &structRibs) {
    int s = getS(structRibs, picks);
    int t = getT(structRibs, picks);
    vector<int> stream(ribs, 0);

    stack<int> myStack;
    myStack.push(s);

    while (!myStack.empty()) {
        vector<int> ancestors(picks, -1);
        vector<int> reserve(picks, -1);
        reserve[s - 1] = 1000000;
        while (myStack.top() != t) {
            int p = myStack.top();
            for (int i = 0; i < structRibs.size(); i++) {
                if (structRibs[i].start == myStack.top() && reserve[structRibs[i].end - 1] == -1 &&
                    stream[i] < structRibs[i].bandwidth) {
                    reserve[structRibs[i].end - 1] = min(structRibs[i].bandwidth - stream[i],
                                                         reserve[myStack.top() - 1]);
                    ancestors[structRibs[i].end - 1] = myStack.top();
                    myStack.push(structRibs[i].end);
                    break;

                }
                if (structRibs[i].end == myStack.top() && reserve[structRibs[i].start - 1] == -1 && stream[i] > 0) {
                    reserve[structRibs[i].start - 1] = min(stream[i], reserve[structRibs[i].end - 1]);
                    ancestors[structRibs[i].start - 1] = myStack.top();
                    myStack.push(structRibs[i].start);
                    break;
                }

            }
            if (myStack.top() == s) {
                break;
            }
            if (myStack.top() == p)myStack.pop();

        }
        if (myStack.top() == s)break;
        while (myStack.top() != s) {
            int chainReserve = reserve[t - 1];
            changeStream(stream, chainReserve, ancestors[myStack.top() - 1], myStack.top(), structRibs);
            myStack.pop();
        }
    }
return stream;
}

void printStreams(int s, vector<int>stream,vector<Rib>structRibs){
    int summaryStream=0;
    for(int i=0; i<structRibs.size(); i++){
        if(structRibs[i].start==s)summaryStream+=stream[i];
        cout<< "Потік в ребрі "<<structRibs[i].start<<" "<<structRibs[i].end<< " дорівнює "<<stream[i]<<endl;
    }
    cout<< "Суммарний потік дорівнює "<<summaryStream;
}
