#include "game.h"

Game::Game()
{
    u.push_back({0, 0}); //will be removed, when step will begin with 0
}
//other constructors needed, for replays and resumes

void Game::addPoint(std::vector<int> added)
{
    u.push_back(added);


    //must be remade

    typedef QVector<int>  sending_type;               // crutch again
    qRegisterMetaType<sending_type>("sending_type");

    thread = new Chaining(u[u.size()-1][0], u[u.size()-1][1], static_cast<int>(u.size())-1, this);

    connect(thread, SIGNAL(newChain(sending_type)), this, SLOT(NEWchain(sending_type)));

    thread  ->  start();
}

int Game::getPointX(size_t step)
{
    return u[step][0];
}

int Game::getPointY(size_t step)
{
    return u[step][1];
}

void Game::setClear()
{

    chains.clear();
    blueChaind.clear();
    redChaind.clear();


    u.clear();
    u.push_back({0, 0});
}

size_t Game::getSize()
{
    return u.size();
}

void Game::deletePoint()
{

    std::vector<int> deleted_chains;
    // at first, let's delete old chain(s), contains deleting dot, find it:
    //for every old chain
    for (int m = 0; m < chains.size(); ++m) {

        //for every point in this old chain
        for (int j = 0; j < chains[m].size(); ++j) {
            if ((u[u.size()][0] == chains[m][j].first)
             && (u[u.size()][1] == chains[m][j].second)) {
                deleted_chains.push_back(m);
                qDebug() << "old chain" << m << "founded to delete because of dot" << chains[m][j];
            }
        }
    }
    //delete it:
    for (size_t i = 0; i < deleted_chains.size(); ++i) {
        chains[deleted_chains[i]].clear();
        qDebug() << "chain" << deleted_chains[i] << "deleted";
    }
    qDebug() << "old_chains:" << chains;

    //if (chains[0].size() < 2) chains.clear();

    // then, delete our dot:

    u.pop_back();
}

size_t Game::getBlueChainedSize()
{
    return static_cast<size_t>(blueChaind.size());
}

size_t Game::getRedChainedSize()
{
    return static_cast<size_t>(redChaind.size());
}

QPair<size_t, size_t> Game::getChainsElem(int current_chain, int current_point)
{
    return chains[current_chain][current_point];
}

int Game::getChainsSize()
{
    return chains.size();
}

int Game::getChainSize(int current_chain)
{
    return chains[current_chain].size();
}

//function for checking new chain for intersections with old chains;

void Game::NEWchain(sending_type chain)
{
    if (chain[0] & 1) {
        qDebug() << "received red chain: " << chain;
    }
    else {
        qDebug() << "received blue chain: " << chain;
    }
    bool circuit_broken = true, chained = false, chain_added = false; //chained never read?

    int broken = 0; //iterator for chained dots in new cycle: if not 0, cycle will not be added in old chains


    //meet the old_chains massive of coordinats of points of chains and color in last member for every chain
    for (int m = 0; m < chains.size(); ++m) {      //for every old chain

        chained = false;
        chain_added = true; // so chain will not be added in old chains by default
        qDebug() << u.size() << "dots will be checked";


        for (size_t k = 0; k < (u.size() - 1); ++k){        // for every dot, except "0" damned dot -> k+1 used

            qDebug() << "checking dot chaining...";
            chained = false;
            int j = chains[m].size() - 2;


            for (int i = 0; i < (chains[m].size() - 1); ++i) { // for every dot in chaining (around) dot

                //now, meet the ith and jth points of chaining edge and chained point coordinates
                double xi = chains[m][i].first;
                double yi = chains[m][i].second;
                double xj = chains[m][j].first;
                double yj = chains[m][j].second;
                double point_x = u[k+1][0], point_y = u[k+1][1];

                int beam_angle = 25; // angle for our beam, searching for chaining edges
                rotate(beam_angle, xi, yi);
                rotate(beam_angle, xj, yj);
                rotate(beam_angle, point_x, point_y);

                if ( ( (yi < point_y && yj >= point_y) || (yj < point_y && yi >= point_y) )
                     &&
                     (xi + (point_y - yi) / (yj - yi) * (xj - xi) < point_x) ) {
                    chained = !chained;
                    qDebug() << "edge at left!";
                }
                j = i;
            }
            if (chains[m].last().first == (k & 1)) {
                chained = false; // is our dot not same color as chain?

            }

            qDebug()  << k << "th dot, chained in old chain " << m << " info is: " << chained;
            qDebug() << "color is same, it is" << (chains[m].last().first == (k & 1));


            if (chained) {
                qDebug() << "chained dot" << k << "detected";

                if (chains[m].last().first & 1) {
                    blueChaind.push_back(qMakePair(u[k][0], u[k][1]));
                    qDebug() << "so, blueChaind:" << blueChaind;
                } else {
                    redChaind.push_back(qMakePair(u[k][0], u[k][1]));
                    qDebug() << "so, redChaind:" << redChaind;
                }

                for (auto checked : chain) {
                    qDebug() << "checking chain for chained dots...";
                    if (k == static_cast<size_t>(checked)) {
                        broken++;
                        qDebug() << "in new chain found chained dots:" << broken;
                    }
                }
            }

        }

    }

    if ((!chain_added) || (broken == 0)) circuit_broken = false;
    qDebug() << "circuit is" << circuit_broken << "broken!";

    if (!circuit_broken) {
        qDebug() << "accepted!" << endl;
        QVector<QPair<int, int>> NewOld;

        for (int k = 0; k < chain.size(); ++k){
            NewOld.push_back(qMakePair(u[static_cast<size_t>(chain[k])+1][0],
                                       u[static_cast<size_t>(chain[k])+1][1]));
        }
        qDebug() << "got chain, colored" << (chain[0] & 1) << "in old chains";
        NewOld.push_back(qMakePair((chain[0] & 1), 0));    //color of chain should be written also???????
        chains.push_back(NewOld);
    }
    qDebug() << "old_chains:" << chains;


    //to remove double counting of chaind dots:

    std::sort(blueChaind.begin(), blueChaind.end());
    blueChaind.resize(std::unique(blueChaind.begin(), blueChaind.end()) - blueChaind.begin());

    std::sort(redChaind.begin(), redChaind.end());
    redChaind.resize(std::unique(redChaind.begin(), redChaind.end()) - redChaind.begin());


}

void Game::rotate(int angle, double &x, double &y)
{
    double x1 = x * cos(angle) - y * sin(angle);
    y = x * sin(angle) + y * cos(angle);
    x = x1;
}

Game::~Game()
{

}
