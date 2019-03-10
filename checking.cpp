#include <cmath>
#include <QDebug>

#include "checking.h"

Checking::Checking(sending_type chain, const std::vector<std::vector<int>> & u, const QVector<QVector<QPair<int, int>>> & chains,
                   const QVector<QPair<int, int>> & blueChaind, const QVector<QPair<int, int>> & redChaind)
    : _chain(chain), u(u), chains(chains), blueChaind(blueChaind), redChaind(redChaind)
{
}

Checking::~Checking()
{

}

void Checking::check_new_chain()
{


    if (_chain[0] & 1) {
        //qDebug() << "received red chain: " << _chain;
    }
    else {
        //qDebug() << "received blue chain: " << _chain;
    }
    bool circuit_broken = false;

    auto & chained((_chain[0] & 1) ? redChaind : blueChaind);

    for (auto & chain_point_num : _chain) {
        for (auto & chained_point : chained) {
            if (u[chain_point_num+1][0] == chained_point.first && u[chain_point_num+1][1] == chained_point.second) {
                circuit_broken = true;
                break;
            }
        }
    }

    if (!circuit_broken) {
        QVector<QPair<int, int>> newOldChain;

        for (auto & chain_pt : _chain){
            newOldChain.push_back(qMakePair(u[static_cast<size_t>(chain_pt) + 1][0],
                                            u[static_cast<size_t>(chain_pt) + 1][1]));
        }
        newOldChain.push_back(qMakePair((_chain[0] & 1), 0));

        qDebug() << "cycle successfully checked: " << newOldChain << ((_chain[0] & 1) ? "red" : "blue");

        QVector<QPair<int, int>> new_chained_dots;
        for (size_t point_num = 0; point_num < (u.size() - 1); ++point_num) {

            if (dot_in_other_colored_chain(newOldChain, u[point_num+1][0], u[point_num+1][1], point_num)) {
                new_chained_dots.push_back(qMakePair(u[point_num+1][0], u[point_num+1][1]));

                if (newOldChain.last().first & 1) {
                    emit blue_chained_dot(u[point_num+1][0], u[point_num+1][1]);
                } else {
                    emit red_chained_dot(u[point_num+1][0], u[point_num+1][1]);
                }
            }
        }

        if (check_chained_dots(new_chained_dots, ((_chain[0] & 1) + 1))) {

            qDebug() << "succesfully added : " << newOldChain;
            emit checked(newOldChain);

        }
    }

}


void Checking::rotate(int angle, double &x, double &y)
{
    double x1 = x * cos(angle) - y * sin(angle);
    y = x * sin(angle) + y * cos(angle);
    x = x1;
}

bool Checking::dot_in_other_colored_chain(const QVector<QPair<int, int>> & chain,
                                          double p_x, double p_y, size_t dot_number)
{
    if (chain.last().first == (dot_number & 1)) {
        return false;
    }

    bool chained = false;
    int next_chaining_dot = chain.size() - 2;
    for (int chaining_dot = 0; chaining_dot < (chain.size() - 1); ++chaining_dot) {
        double pt_x = chain[chaining_dot].first;
        double pt_y = chain[chaining_dot].second;
        double next_pt_x = chain[next_chaining_dot].first;
        double next_pt_y = chain[next_chaining_dot].second;
        double point_x(p_x);
        double point_y(p_y);


        int beam_angle = 25; // angle for scanning beam
        rotate(beam_angle, pt_x, pt_y);
        rotate(beam_angle, next_pt_x, next_pt_y);
        rotate(beam_angle, point_x, point_y);

        if ( ( (pt_y < point_y && next_pt_y >= point_y) || (next_pt_y < point_y && pt_y >= point_y) )
             &&
             (pt_x + (point_y - pt_y) / (next_pt_y - pt_y) * (next_pt_x - pt_x) < point_x) ) {
            chained = !chained;
        }
        next_chaining_dot = chaining_dot;
    }
    return chained;
}

bool Checking::check_chained_dots(QVector<QPair<int, int>> & dots, size_t dot_num)
{
    if (dots.isEmpty()) {
        return false;
    }
    for (auto & old_chain : chains) {
        for (auto & dot : dots) {
            if (!dot_in_other_colored_chain(old_chain, dot.first, dot.second, dot_num)) {
                qDebug() << "no dot" << dot << "chained in" << old_chain;
                break;
            }

            if (dot == dots[dots.size() - 1]) {
                qDebug() << "not added";
                return false;
            }
        }
    }
    return true;
}
