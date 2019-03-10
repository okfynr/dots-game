#include "game.h"

Game::Game()
{
    u.push_back({0, 0}); //will be removed, when step will begin with 0
    finished_chaining_threads = 0;
    currently_checking_threads = 0;
}
//other constructors needed, for replays and resumes

void Game::addPoint(std::vector<int> added)
{
    u.push_back(added);

    while (true) {
        if(finished_chaining_threads >= u.size() - 2 && currently_checking_threads == 0)
        {
            break;
        }
    }

    typedef QVector<int>  sending_type;
    qRegisterMetaType<sending_type>("sending_type");

    QThread thread;
    Chaining *chaining = new Chaining(u[u.size()-1][0], u[u.size()-1][1], static_cast<int>(u.size())-1, u_for_chains);
    chaining->moveToThread(&thread);

    connect(chaining, &Chaining::newChain, this, &Game::NEWchain);
    connect(&thread, &QThread::finished, this, &Game::chaining_finished);
    connect(&thread, &QThread::finished, chaining, &QObject::deleteLater);
    connect(&thread, &QThread::started, chaining, &Chaining::check_chains);

    thread.start();
    thread.quit();
    thread.wait();


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
    qRegisterMetaType<sending_twice_type>("sending_twice_type");

    QThread thread;
    Checking *checking = new Checking(chain, u, chains, blueChaind, redChaind);
    checking->moveToThread(&thread);

    connect(checking, &Checking::checked, this, &Game::ADDchain, Qt::QueuedConnection);
    connect(checking, &Checking::red_chained_dot, this, &Game::ADDred, Qt::QueuedConnection);
    connect(checking, &Checking::blue_chained_dot, this, &Game::ADDblue, Qt::QueuedConnection);
    connect(&thread, &QThread::finished, this, &Game::chaining_check_finished);
    connect(&thread, &QThread::finished, checking, &QObject::deleteLater);
    connect(&thread, &QThread::started, checking, &Checking::check_new_chain);

    ++currently_checking_threads;

    thread.start();
    thread.quit();
    thread.wait();

}

void Game::chaining_finished()
{
    ++finished_chaining_threads;
    qDebug() << "now threads finished:" << finished_chaining_threads;
}

void Game::chaining_check_finished()
{
    --currently_checking_threads;
}

void Game::ADDchain(sending_twice_type NewOld)
{
    chains.push_back(NewOld);
    emit chains_changed();

    //qDebug() << "old_chains:" << chains;
}

void Game::ADDblue(int first, int second)
{
    blueChaind.push_back(qMakePair(first, second));

    //to remove double counting of chaind dots:

    std::sort(blueChaind.begin(), blueChaind.end());
    blueChaind.resize(std::unique(blueChaind.begin(), blueChaind.end()) - blueChaind.begin());

    qDebug() << "blue chained:" << blueChaind;

}

void Game::ADDred(int first, int second)
{
    redChaind.push_back(qMakePair(first, second));

    //to remove double counting of chaind dots:

    std::sort(redChaind.begin(), redChaind.end());
    redChaind.resize(std::unique(redChaind.begin(), redChaind.end()) - redChaind.begin());

    qDebug() << "red chained:" << redChaind;
}


Game::~Game()
{

}
