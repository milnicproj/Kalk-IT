#ifndef CALC_H
#define CALC_H

#include <QMainWindow>

namespace Ui {
class Calc;
}

class Calc : public QMainWindow
{
    Q_OBJECT

public:

    explicit Calc(QWidget *parent = 0);
    ~Calc();

private:
    Ui::Calc *ui;

private slots :

    void NumPressed();
    void MathButtonPressed();
    void EqualButtonPressed();
    void ChangeNumberSign();
    void ChangeMode();
    void Clearall();
    void UpdateButton();
};

#endif
