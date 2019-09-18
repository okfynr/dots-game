#include "qmath.h"
#include <iostream>
#include <fstream>
#include <QDebug>
#include <vector>
#include <stack>

#include "chaining.h"

Chaining::Chaining(int xP, int yP, int step, QVector<QPair<int, int> > & dots)
    : Dots(dots)
{
    this -> xDot = xP;
    this -> yDot = yP;
    this -> Step = step;
}

Chaining::~Chaining()
{

}

void Chaining::check_chains()
{
    typedef QVector<int>  sending_type;
    qRegisterMetaType<sending_type>("sending_type");
    std::stack<int> dfs_stack;

    //even dots are blue, odds are red

    bool is_red = (Step - 1) & 1;
    //only build edges for current color (maybe could be saved and updated)
    int curr_dots_num(is_red ? (Step + 1) / 2 : (Step + 2) / 2);

    dots_edges.resize(Step);
    dots_colors.resize(Step);
    previous_dot.resize(Step);
    std::fill(dots_colors.begin(), dots_colors.end(), 0);
    std::fill(previous_dot.begin(), previous_dot.end(), -1);

    Dots.push_back(qMakePair(xDot, yDot));

    for (int i = 0; i < curr_dots_num; ++i) {
        for (int j = 0; j < curr_dots_num; ++j) {


            int curr_dot_i(is_red ? i * 2 + 1 : i * 2);
            int curr_dot_j(is_red ? j * 2 + 1 : j * 2);
            //dots are close enough
            if ((    (Dots[curr_dot_i].first == Dots[curr_dot_j].first)
                  || (Dots[curr_dot_i].first == (Dots[curr_dot_j].first - 1))
                  || (Dots[curr_dot_i].first == (Dots[curr_dot_j].first + 1)) )
             && (    (Dots[curr_dot_i].second == Dots[curr_dot_j].second)
                  || (Dots[curr_dot_i].second == (Dots[curr_dot_j].second - 1))
                  || (Dots[curr_dot_i].second == (Dots[curr_dot_j].second + 1)) )
             //not the same dot
             && (i != j))
            {
                dots_edges[curr_dot_i].push_back(curr_dot_j);
            }

        }
    }

    dfs_stack.push(Step - 1);
    dots_colors[Step - 1] = 1;
    while (!dfs_stack.empty()) {
        int curr_dot = dfs_stack.top();
        bool dot_has_unseen_dots = false;
        for (size_t curr_dot_neighbour = 0; curr_dot_neighbour < dots_edges[curr_dot].size(); ++curr_dot_neighbour) {
            int neighbour = dots_edges[curr_dot][curr_dot_neighbour];

            //if that neighbour not the dot we came from, mark that we came to neighbour from current
            //else skip
            if (previous_dot[curr_dot] != neighbour) {
                if (dots_colors[neighbour] == 0) {
                    previous_dot[neighbour] = curr_dot;
                }
            } else {
                continue;
            }
            if (dots_colors[neighbour] == 0) {
                dots_colors[neighbour] = 1;
                dfs_stack.push(neighbour);
                dot_has_unseen_dots = true;
            } else {
                if (dfs_stack.size() > 3) {
                    add_cycle(curr_dot, neighbour);
                }
            }
        }
        if (!dot_has_unseen_dots) {
            dfs_stack.pop();
            dots_colors[curr_dot] = 2;
        }
    }

}

void Chaining::add_cycle(int first_dot, int last_dot)
{
    sending_type sended;
    int temp_dot;

    //seeking to the root from the beginning
    int viewed_dot = first_dot;
    //also exclude case if last dot is next to first
    while (viewed_dot != -1 && viewed_dot != last_dot) {
        sended.push_back(viewed_dot);
        temp_dot = previous_dot[viewed_dot];
        viewed_dot = temp_dot;
    }

    sending_type sended_tail;
    //seeking to the root from the end
    viewed_dot = last_dot;
    //also exclude case if first dot is next to last
    while (viewed_dot != -1 && viewed_dot != first_dot) {
        sended_tail.push_back(viewed_dot);
        temp_dot = previous_dot[viewed_dot];
        viewed_dot = temp_dot;
        auto found_dot = std::find(sended.begin(), sended.end(), viewed_dot);
        if (found_dot != sended.end()) {
            sended.erase(++found_dot, sended.end());
            break;
        }
    }

    std::for_each(sended_tail.rbegin(), sended_tail.rend(), [&sended](int tail_dot){sended.push_back(tail_dot);});
    sended.push_back(first_dot);

    if (sended.size() < 5) {
        return;
    }
    qDebug() << "sended: " << sended;

    emit newChain(sended);
}
