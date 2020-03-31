/*
 * TODO: complete this file comment.
 */
#include <iostream>
#include <fstream>
#include <string>
#include <cmath>
#include <chrono>
#include "SimpleGraph.h"

using namespace std;

void Welcome();
void readFile(ifstream& file);
void readMicroseconds(int& microseconds);
void readNode(ifstream& file, int& node);
void createNode(SimpleGraph& graph, const int& n);
void readAndCreateEdge(ifstream& file, SimpleGraph& graph);
void attractiveForce(SimpleGraph& graph, vector<double>& dx, vector<double>& dy);
void repulsiveForce(SimpleGraph& graph, vector<double>& dx, vector<double>& dy);
void addDxDy(SimpleGraph& graph, vector<double>& dx, vector<double>& dy);

// Main method
int main() {
    Welcome();
    /* TODO: your implementation here */

    ifstream file;
    int microseconds;
    int node_count;
    bool flag = true;

    SimpleGraph graph;

    while (flag){
        readFile(file);
        readMicroseconds(microseconds);
        readNode(file, node_count);
        createNode(graph, node_count);
        readAndCreateEdge(file, graph);

        InitGraphVisualizer(graph);
        DrawGraph(graph);

        auto start = std::chrono::high_resolution_clock::now();

        vector<double> dx(node_count, 0);
        vector<double> dy(node_count, 0);

        for (int i = 0; i < microseconds; i++){
            auto end = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
            //int milliseconds = elapsed.count();

            if (i%10 == 0) cout << i << "itertion" << endl;
            attractiveForce(graph, dx, dy);
            repulsiveForce(graph, dx, dy);
            addDxDy(graph, dx, dy);
            DrawGraph(graph);

            for (int j = 0; j < node_count; j++){
                dx[j] = dy[j] = 0;
            }
        }

        cout << "Do you want to load another file?" << endl;
        string ans;
        getline(cin, ans);
        if (ans != "yes") flag = false;
    }
    return 0;
}

/* Prints a message to the console welcoming the user and
 * describing the program. */
void Welcome() {
    cout << "Welcome to CS106L GraphViz!" << endl;
    cout << "This program uses a force-directed graph layout algorithm" << endl;
    cout << "to render sleek, snazzy pictures of various graphs." << endl;
    cout << endl;
}

void readFile(ifstream& file){
    string filename;

    while(true){
        cout << "Enter an filename" << endl;
        cin >> filename;
        file.open(filename);
        if (file.is_open()) break;
    }

    cout << "Your filename is: " << filename << endl;
}

void readMicroseconds(int& microseconds){
    while(true){
        cout << "Enter an positive number" << endl;
        cin >> microseconds;
        if (microseconds > 0) break;
    }

    cout << "Your filename is: " << microseconds << endl;
}

void readNode(ifstream& file, int& node){
    string content;
    getline(file, content);
    node = atoi(content.c_str());
    cout << node << " nodes in this file." << endl;
}

void createNode(SimpleGraph& graph, const int& n){
    const double kPi = 3.14159265358979323;

    graph.nodes.clear();
    for (int i = 0; i < n; i++){
        Node node;
        node.x = cos(2*kPi*i/n);
        node.y = sin(2*kPi*i/n);
        graph.nodes.push_back(node);
    }
}

void readAndCreateEdge(ifstream& file, SimpleGraph& graph){
    int st, ed;
    Edge edge;

    graph.edges.clear();
    while (file >> st){
        file >> ed;
        edge.start = st;
        edge.end = ed;
        graph.edges.push_back(edge);
        cout << st << ' ' << ed << endl;
    }
    getchar();
    file.close();
}

void attractiveForce(SimpleGraph& graph, vector<double>& dx, vector<double>& dy){
    const double kattract = 0.001;
    for (auto edge : graph.edges){
        auto st = edge.start;
        auto ed = edge.end;

        double x0 = graph.nodes[st].x;
        double x1 = graph.nodes[ed].x;
        double y0 = graph.nodes[st].y;
        double y1 = graph.nodes[ed].y;

        double Fattract = kattract * ((y1-y0)*(y1-y0) + (x1-x0)*(x1-x0));
        double theta = atan2(y1 - y0, x1 - x0);

        dx[st] += Fattract * cos(theta);
        dy[st] += Fattract * sin(theta);
        dx[ed] -= Fattract * cos(theta);
        dy[ed] -= Fattract * sin(theta);
    }
}

void repulsiveForce(SimpleGraph& graph, vector<double>& dx, vector<double>& dy){
    const double krepel  = 0.001;
    for (size_t i = 0; i < graph.nodes.size(); i++){
        double x0 = graph.nodes[i].x;
        double y0 = graph.nodes[i].y;
        for (size_t j = i+1; j < graph.nodes.size(); j++){
            if (i == j) continue;
            double x1 = graph.nodes[j].x;
            double y1 = graph.nodes[j].y;
            double Frepel = krepel / sqrt((y1-y0)*(y1-y0) + (x1-x0)*(x1-x0));
            double theta = atan2(y1 - y0, x1 - x0);

            dx[i] -= Frepel * cos(theta);
            dy[i] -= Frepel * sin(theta);
            dx[j] += Frepel * cos(theta);
            dy[j] += Frepel * sin(theta);
        }
    }
}

void addDxDy(SimpleGraph& graph, vector<double>& dx, vector<double>& dy){
    for (size_t i = 0; i < dx.size(); i++){
        graph.nodes[i].x += dx[i];
        graph.nodes[i].y += dy[i];
    }
}

