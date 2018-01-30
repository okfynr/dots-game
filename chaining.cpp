#include "qmath.h"
#include <iostream>
#include <fstream>
#include <QDebug>
#include <vector>


#include "chaining.h"


using std::vector;

//orly static?
static QVector<QPair<int, int>> Dots;
static vector<int> color;
static int ncycle;
static vector<int> path;
static QVector<QVector<int>> Edges;
static vector<vector<int>> cycle;



Chaining::Chaining(int xP, int yP, int step, QObject *parent)
    : QThread(parent)
{
    this -> xDot = xP;
    this -> yDot = yP;
    this -> Step = step;
}


Chaining::~Chaining()
{
    qDebug() << "destroyed";
}

void Chaining::run()
{
    color.resize(Step);
    for (auto i : color) color[i] = 0;
    ncycle = 0;     //0 cycles found right here
    path.clear();
    path.resize(Step);
    Edges.clear();
    Edges.resize(Step);
    cycle.clear();  // here's error, when big cycle contents chained and not chained dots and
                    // littler cycle appears in that cycle


    //qDebug() << "no DFS is coming!" << Edges[0] << " " << Edges[1] << endl;
    //QVector<QPair<double, double>> redDots, blueDots;
    //vector<vector<int>> redEdges, blueEdges;


    if (Step & 1) {
         Dots.push_back(qMakePair(xDot, yDot));
     }
     else {
        Dots.push_back(qMakePair(xDot, yDot));
     }

    //qDebug() << "hello, i am here to chain smth: ";
    //qDebug() << "size of dots vector is " << Dots.size() << endl;


    //cycle to make edges instead of dots

    for (int i = 0; i < Step; i++) {
        for (int j = 0; j < Step; j++) {

            if (i & 1) {
                if ((    (Dots[i].first == Dots[j].first)
                      || (Dots[i].first == (Dots[j].first - 1))
                      || (Dots[i].first == (Dots[j].first + 1)) )
                 && (    (Dots[i].second == Dots[j].second)
                      || (Dots[i].second == (Dots[j].second - 1))
                      || (Dots[i].second == (Dots[j].second + 1)) )
                 && (i != j)
                 && (j & 1) ) {
                    Edges[i].push_back(j);  // adding neighbours for i-th dot
                    //qDebug() << "new red edge, between " << i << " and " << j << endl;
                }
            }
            else {
                if ((    (Dots[i].first == Dots[j].first)
                      || (Dots[i].first == (Dots[j].first - 1))
                      || (Dots[i].first == (Dots[j].first + 1)) )
                 && (    (Dots[i].second == Dots[j].second)
                      || (Dots[i].second == (Dots[j].second - 1))
                      || (Dots[i].second == (Dots[j].second + 1)) )
                 && (i != j)
                 && !(j & 1) ) {
                    Edges[i].push_back(j);     // adding neighbours for i-th dot
                    //qDebug() << "new blue edge, between " << i << " and " << j << endl;
                }
            }

        }
    }



    //qDebug() << "then DFS is coming! " << Edges;
    dfs(Step - 1);               // iterator Step begins with 1 and it is useful not here

    size_t max = 0;
    for (size_t i = 0; i < cycle.size(); i++) {
        for (size_t j = 0; j < cycle[i].size(); j++) {
            if ((cycle[i][j] == (Step - 1)) && (cycle[i].size() > max)) {
                max = i;
            }
        }
    }
    typedef QVector<int>  sending_type;             //crutch, wtf
    qRegisterMetaType<sending_type>("sending_type");
    if (max != 0) {
        sending_type sended;
        sended.resize((int)cycle[max].size());
        sended = sending_type::fromStdVector(cycle[max]);

        emit newChain(sended);
    }
    //qDebug() << cycle << endl;
    //qDebug() << "but true cycle is " << cycle[max];

}



int Chaining::add_cycle(int cycle_end, int cycle_st)
{
    cycle.resize(ncycle + 1); // if you're here, 1 cycle already found, so, you need 1 vector to write it
    //qDebug() << cycle << endl;
    cycle[ncycle].clear();
    cycle[ncycle].push_back(cycle_st);
    for(int v = cycle_end; v != cycle_st; v = path[v])
        cycle[ncycle].push_back(v);
    cycle[ncycle].push_back(cycle_st);

    reverse(cycle[ncycle].begin(), cycle[ncycle].end());

    return cycle[ncycle].size();
}

void Chaining::dfs(int dfsed_dot)
{

    color[dfsed_dot] = 1;
    //qDebug() << "DFS is coming! " << Edges[dfsed_dot].size() << endl;// << Edges.size();        if (i & 1)

    for(size_t i = 0; i < Edges[dfsed_dot].size(); i++) // roaming through all neighbours
    {
        //qDebug() << "DFS is coming!";

        int next_dot = Edges[dfsed_dot][i];   //i-th neighbour of dfsed dot with given key
        if(color[next_dot] == 0)
        {
            path[next_dot] = dfsed_dot;
            dfs(next_dot);
        }
        else if(color[next_dot] == 1)
        {
            if(add_cycle(dfsed_dot, next_dot) > 4) {  // Исключение вырожденных случаев, н: 1 2 1
                //qDebug() << "cycle found: " << cycle[ncycle] << ". ";
                ncycle++;
                path.clear();
            }
            else cycle.pop_back();
        }
    }
    color[dfsed_dot] = 0; // Исправлено. Было: color[dfsed_dot] = 2;
}

/*int Chaining::true_cycle(vector<vector<int>> & cycles, int point)
{
    int max = 0;
    for (auto i = 0; i < (int)cycles.size(); i++) {
        for (auto j = 0; j < (int)cycles[i].size(); j++) {
            if ((cycles[i][j] == (point - 1)) && ((cycles[i].end() - cycles[i].begin()) > max)) {
                max = i;
            }
        }
    }
    return max;
}*/
