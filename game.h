#ifndef GAME_H
#define GAME_H


#include <QMainWindow>
#include <vector>
#include <QDebug>

#include "chaining.h"
#include "checking.h"


class Game : public QObject
{
    Q_OBJECT

private:
    std::vector<std::vector<int>> u;
    QVector<QVector<QPair<int, int>>> chains;
    QVector<QPair<int, int>> u_for_chains, blueChaind, redChaind;
    size_t finished_chaining_threads;
    size_t currently_checking_threads;
public:
    Game();
    //other constructors needed, for replays and resumes
    void addPoint(std::vector<int>); //for new point
    int getPointX(size_t);
    int getPointY(size_t);
    void setClear();
    size_t getSize();
    void deletePoint(); //for stepback()
    size_t getBlueChainedSize(); // acces for this counter
    size_t getRedChainedSize();  // access for this counter
    QPair<size_t, size_t> getChainsElem(int, int); //for access in oldchains()
    int getChainsSize();
    int getChainSize(int);
    void showU(); // for resuming games/replays?
    void rotate(int, double &, double &);
    bool chainsChecking();
    ~Game();

signals:
    void chains_changed();

private slots:
    void NEWchain(sending_type);
    void chaining_finished();
    void chaining_check_finished();
    void ADDchain(sending_twice_type NewOld);
    void ADDblue(int, int);
    void ADDred(int, int);
};

#endif // GAME_H
