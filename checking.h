#ifndef CHECKING_H
#define CHECKING_H

#include <QThread>
#include <QVector>


using std::vector;

typedef QVector<QPair<int, int>>  sending_twice_type;
typedef QVector<int>  sending_type;

class Checking : public QObject
{
    Q_OBJECT

public:
    Checking(sending_type chain, const std::vector<std::vector<int>> &, const QVector<QVector<QPair<int, int>>> &,
             const QVector<QPair<int, int>> &, const QVector<QPair<int, int>> &);
    ~Checking();

signals:
    void checked(sending_twice_type);
    void blue_chained_dot(int, int);
    void red_chained_dot(int, int);
public slots:
     void check_new_chain();

private:
    sending_type _chain;
    const std::vector<std::vector<int>> & u;
    const QVector<QVector<QPair<int, int>>> & chains;
    const QVector<QPair<int, int>> & blueChaind;
    const QVector<QPair<int, int>> & redChaind;

    void rotate(int angle, double &x, double &y);
    bool dot_in_other_colored_chain(const QVector<QPair<int, int>> & chain, double p_x, double p_y, size_t dot_number);
    bool check_chained_dots(QVector<QPair<int, int>> & dots, size_t dot_num);


};

#endif // CHECKING_H
