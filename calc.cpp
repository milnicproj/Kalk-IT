#include "calc.h"
#include "ui_calc.h"
#include <QRegularExpression>
//////////////////////////////////////////////////////////////
///
/// Implementacja potrzebnych zmiennych globalnych
/// oraz klasy kalkulatora - calc
///
//////////////////////////////////////////////////////////////
double calcVal = 0.0;
int ConvertVal = 0;
bool divTrigger = false;
bool multTrigger = false;
bool addTrigger = false;
bool subTrigger = false;
bool hexmode = false;
bool decmode = true;
bool octmode = false;
bool binmode = false;
bool floatmode = false;

Calc::Calc(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::Calc)
{
    //Inicjalizacja GUI
    ui->setupUi(this);
    ui->Display->setText(QString::number(calcVal));
    ui->Mode->setText("Mode: Dec");
    ui->Display->setReadOnly(true);
    ui->Mode->setReadOnly(true);
    QPushButton *numButtons[16];

    //Pętla która łączy wszystkie guziki numeryczne wraz z funkcją NumPress()
    for(int i = 0; i < 16; ++i)
    {
        QString butName = "Button" + QString::number(i);
        numButtons[i] = Calc::findChild<QPushButton *>(butName);
        connect(numButtons[i], SIGNAL(released()), this,SLOT(NumPressed()));
    }

    //Łączenie guzików matematycznych z funkcją MathButtonPressed()
    connect(ui->Add, SIGNAL(released()), this,SLOT(MathButtonPressed()));
    connect(ui->Substract, SIGNAL(released()), this,SLOT(MathButtonPressed()));
    connect(ui->Multiply, SIGNAL(released()), this,SLOT(MathButtonPressed()));
    connect(ui->Divide, SIGNAL(released()), this,SLOT(MathButtonPressed()));

    //Łączenie guzików funkcjonalnych z ich respektywnymi funkcjami
    connect(ui->Equals, SIGNAL(released()), this,SLOT(EqualButtonPressed()));
    connect(ui->ChangeSign, SIGNAL(released()), this,SLOT(ChangeNumberSign()));
    connect(ui->Clear, SIGNAL(released()), this,SLOT(Clearall()));

    //Łączenie guzików systemów liczbowych z funkcją ChangeMode()
    connect(ui->Hex,SIGNAL(released()),this,SLOT(ChangeMode()));
    connect(ui->Dec,SIGNAL(released()),this,SLOT(ChangeMode()));
    connect(ui->Oct,SIGNAL(released()),this,SLOT(ChangeMode()));
    connect(ui->Bin,SIGNAL(released()),this,SLOT(ChangeMode()));
    connect(ui->Float,SIGNAL(released()),this,SLOT(ChangeMode()));

    //Włączenie i wyłączenie guzików dopowiednich dla początkowych ustawień kalkulatora
    UpdateButton();
}
Calc::~Calc()
{
    delete ui;
}

//////////////////////////////////////////////////////////////
///
///  Funkcja NumPessed - otrzymuje sygnał że guzik numeryczny
///  został wciśnięty, wczytuje jaki był to guzik i wstawia
///  jego wartość na display
///
//////////////////////////////////////////////////////////////
void Calc::NumPressed()
{
    //Czytanie i zapisywanie informacji jaki guzik został wciśnięty
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    QString butVal = button->text();

    //Zczytanie informacji jaki jest stan Display po czym ustalenie co zrobić z wartością zczytaną z guzika
    QString displayVal = ui->Display->text();
    if (displayVal == "0")
        ui->Display->setText(butVal);
    else
    {
        if(!binmode)
            ui->Display->setText(displayVal + butVal);
        else
            ui->Display->setText(butVal + displayVal);
    }
}

//////////////////////////////////////////////////////////////
///
/// Funkcja MathButtonPressed - otrzymuje sygnał że został
/// wciśnięty guzik operacji matematycznej. Jego zadaniem jest
/// przygotowanie kalkulatora do dalszego postępowania aż
/// zostanie wciśnięty przycisk równa się.
///
//////////////////////////////////////////////////////////////

void Calc::MathButtonPressed()
{
    //wyłączenie triggerów matematycznych
    divTrigger = false;
    multTrigger = false;
    addTrigger = false;
    subTrigger = false;
    //zczytanie wartości z kalkulatora
    QString displayVal = ui->Display->text();
    // zapisanie wartości z Display. W zależności jaki jest włączony tryb kalkulatora, liczba zostaje przekonwertowana
    // na system decymalny aby przeprowadzić na tej wartości następne operacje
    if(decmode || floatmode)
        calcVal = displayVal.toDouble();
    if(hexmode)
    {
        bool ok;
        ConvertVal = displayVal.toInt(&ok, 16);
    }
    if(octmode)
    {
        bool ok;
        ConvertVal = displayVal.toInt(&ok, 8);
    }
    if(binmode)
    {
        bool ok;
        ConvertVal = displayVal.toInt(&ok, 2);
    }

    // zczytanie jaki guzik matematyczny został wciśnięty do wywołania funkcji
    QPushButton *button = (QPushButton *)sender();
    QString butVal = button->text();

    // ustawienie odpowiedniego trigger'a do dalszych obliczeń - ustalenie działania jakie zostało wybrane
    if(QString::compare(butVal, "/", Qt::CaseInsensitive) == 0){
        divTrigger = true;
    } else if(QString::compare(butVal, "*", Qt::CaseInsensitive) == 0){
        multTrigger = true;
    } else if(QString::compare(butVal, "+", Qt::CaseInsensitive) == 0){
        addTrigger = true;
    } else {
        subTrigger = true;
    }
    ui->Display->setText("");
}

//////////////////////////////////////////////////////////////
///
/// funkcja EqualButtonPressed - funkcja wykonująca działanie
/// matematyczne jakie zostało ustalone przez użytkownkia
///
//////////////////////////////////////////////////////////////


void Calc::EqualButtonPressed()
{
    // stworzenie zmiennej do przechowania i wyświetlenia wyniku oraz zmiennej Boolean
    // do potwierdzania konwersji między systemami liczbowymi
    double solution = 0.0;
    bool ok;

    QString displayVal = ui->Display->text();

    // w zależności od tego jaki trigger został wcześniej ustawiony oraz jaki system liczbowy jest wybrany
    // będą wykonywane odpowiednie funkcje aby wyliczyć wynik działania podanego przez użytkownika
    // wraz z wypisanie odpowidniego wyniku przy konwersji
    if(addTrigger || subTrigger || multTrigger || divTrigger )
    {
        // Dodawanie we wszystkich systemach
        if(addTrigger)
        {
            if(decmode || floatmode)
            {
                double dblDisplayVal = displayVal.toDouble();
                solution = calcVal + dblDisplayVal;
                ui->Display->setText(QString::number(solution));
            }
            if(hexmode)
            {
                int decimalValue = displayVal.toInt(&ok, 16);
                solution = ConvertVal + decimalValue;
                ui->Display->setText(QString::number(static_cast<int>(solution), 16).toUpper());
            }
            if(octmode)
            {
                int decimalValue = displayVal.toInt(&ok, 8);
                solution = ConvertVal + decimalValue;
                ui->Display->setText(QString::number(static_cast<int>(solution), 8));
            }
            if(binmode)
            {
                int decimalValue = displayVal.toInt(&ok, 2);
                solution = ConvertVal + decimalValue;
                ui->Display->setText(QString::number(static_cast<int>(solution), 2));
            }
        }
        // Odejmowanie we wszystkich systemach
        else if(subTrigger)
        {
            if(decmode || floatmode)
            {
                double dblDisplayVal = displayVal.toDouble();
                solution = calcVal - dblDisplayVal;
                ui->Display->setText(QString::number(solution));
            }
            if(hexmode)
            {
                int decimalValue = displayVal.toInt(&ok, 16);
                solution = ConvertVal - decimalValue;
                ui->Display->setText(QString::number(static_cast<int>(solution), 16).toUpper());
            }
            if(octmode)
            {
                int decimalValue = displayVal.toInt(&ok, 8);
                solution = ConvertVal - decimalValue;
                ui->Display->setText(QString::number(static_cast<int>(solution), 8));
            }
            if(binmode)
            {
                int decimalValue = displayVal.toInt(&ok, 2);
                solution = ConvertVal - decimalValue;
                ui->Display->setText(QString::number(static_cast<int>(solution), 2));
            }
        }
        // Mnożenie we wszystkich systemach
        else if(multTrigger)
        {
            if(decmode || floatmode)
            {
                double dblDisplayVal = displayVal.toDouble();
                solution = calcVal * dblDisplayVal;
                ui->Display->setText(QString::number(solution));
            }
            if(hexmode)
            {
                int decimalValue = displayVal.toInt(&ok, 16);
                solution = ConvertVal * decimalValue;
                ui->Display->setText(QString::number(static_cast<int>(solution), 16).toUpper());
            }
            if(octmode)
            {
                int decimalValue = displayVal.toInt(&ok, 8);
                solution = ConvertVal * decimalValue;
                ui->Display->setText(QString::number(static_cast<int>(solution), 8));
            }
            if(binmode)
            {
                int decimalValue = displayVal.toInt(&ok, 2);
                solution = ConvertVal * decimalValue;
                ui->Display->setText(QString::number(static_cast<int>(solution), 2));
            }
        }
        // Dzielenie we wszystkich systemach
        else
        {
            if(displayVal == '0' || displayVal == "0.0")
            {
                ui->Display->setText("0");
            }
            else
            {
                if(decmode)
                {
                    double dblDisplayVal = displayVal.toDouble();
                    solution = floor(calcVal / dblDisplayVal);
                    ui->Display->setText(QString::number(solution));
                }
                if(hexmode)
                {
                    int decimalValue = displayVal.toInt(&ok, 16);
                    solution = floor(ConvertVal / decimalValue);
                    ui->Display->setText(QString::number(static_cast<int>(solution), 16).toUpper());
                }
                if(octmode)
                {
                    int decimalValue = displayVal.toInt(&ok, 8);
                    solution = floor(ConvertVal / decimalValue);
                    ui->Display->setText(QString::number(static_cast<int>(solution), 8));
                }
                if(binmode)
                {
                    int decimalValue = displayVal.toInt(&ok, 2);
                    solution = floor(ConvertVal / decimalValue);
                    ui->Display->setText(QString::number(static_cast<int>(solution), 2));
                }
                if(floatmode)
                {
                    double dblDisplayVal = displayVal.toDouble();
                    solution = calcVal / dblDisplayVal;
                    ui->Display->setText(QString::number(solution));
                }
            }

        }

        // reset wszystkich triggerów - zakończenie obliczeń
        divTrigger = false;
        multTrigger = false;
        addTrigger = false;
        subTrigger = false;
    }
}

//////////////////////////////////////////////////////////////
///
/// funkcja ChangeNumberSign() - funkcja służąca do zmiany
/// znaku w przypadku systemów decymalnego i float.
///
//////////////////////////////////////////////////////////////

void Calc::ChangeNumberSign()
{
    if(decmode || floatmode)
    {
        // zczytanie wartości jaka jest na Display
        QString displayVal = ui->Display->text();
        // RegularExpression który sprawdza czy podana jest odpowiednia liczba oraz z jakim znakiem
        QRegularExpression reg("[-+]?[0-9]*\\.?[0-9]*");
        // W przypadku dobrego podania wartości zostaje wynik display zamieniony na przeciwny oraz podany na display
        if (reg.match(displayVal).hasMatch())
        {
            double dblDisplayVal = displayVal.toDouble();
            double dblDisplayValSign = -1 * dblDisplayVal;
            ui->Display->setText(QString::number(dblDisplayValSign));
        }
    }
}

//////////////////////////////////////////////////////////////
///
/// Funkcja ChangeMode() - funkcja odpowiadająca za zmianę
/// między typami oraz konwersją wyników wpisanych na kalkulator
///
//////////////////////////////////////////////////////////////


void Calc::ChangeMode()
{
    // zczytanie jaki guzik trybu został wciśnięty
    QPushButton *button = qobject_cast<QPushButton*>(sender());
    QString mode = button->text();
    // konwersja wartości z display na system na jaki użytkownik chce zmienić oraz zmiana trybu kalkulatora
    // na tryb DEC
    if(mode == "Dec")
    {
        QString displayVal = ui->Display->text();
        bool ok;
        int decimalValue = 0;
        if(hexmode)
            decimalValue = displayVal.toInt(&ok, 16);
        if(octmode)
            decimalValue = displayVal.toInt(&ok, 8);
        if(binmode)
            decimalValue = displayVal.toInt(&ok, 2);
        if(floatmode)
        {
            decimalValue = displayVal.toDouble(&ok);
            decimalValue = floor(decimalValue);
        }
        if (ok)
        {
            QString displayVal = QString::number(decimalValue);
            ui->Display->setText(displayVal);
        }
        decmode = true;
        hexmode = false;
        binmode = false;
        octmode = false;
        floatmode = false;
        ui->Mode->setText("Mode: Dec");
        UpdateButton();
    }
    // na tryb HEX
    if(mode == "Hex")
    {
        QString displayVal = ui->Display->text();
        bool ok;
        int Value = 0;
        if(decmode)
            Value = displayVal.toInt(&ok);
        if(octmode)
            Value = displayVal.toInt(&ok, 8);
        if(binmode)
            Value = displayVal.toInt(&ok, 2);
        if(floatmode)
        {
            Value = displayVal.toDouble(&ok);
            Value = floor(Value);
        }
        if (ok)
        {
            ui->Display->setText(QString::number(Value, 16).toUpper());
        hexmode = true;
        decmode = false;
        binmode = false;
        octmode = false;
        floatmode = false;
        ui->Mode->setText("Mode: Hex");
        UpdateButton();
        }
    }
    // na tryb OCT
    if(mode == "Oct")
    {
            QString displayVal = ui->Display->text();
            bool ok;
            int Value = 0;
            if(decmode)
                Value = displayVal.toInt(&ok);
            if(hexmode)
                Value = displayVal.toInt(&ok, 16);
            if(binmode)
                Value = displayVal.toInt(&ok, 2);
            if(floatmode)
            {
                Value = displayVal.toDouble(&ok);
                Value = floor(Value);
            }
            if (ok)
            {
                ui->Display->setText(QString::number(Value, 8));
            }
            hexmode = false;
            decmode = false;
            binmode = false;
            octmode = true;
            floatmode = false;
            ui->Mode->setText("Mode: Oct");
            UpdateButton();
    }
    // na tryb BIN
    if(mode == "Bin")
    {
        QString displayVal = ui->Display->text();
        bool ok;
        int Value = 0;
        if(decmode)
            Value = displayVal.toInt(&ok);
        if(hexmode)
            Value = displayVal.toInt(&ok, 16);
        if(octmode)
            Value = displayVal.toInt(&ok, 8);
        if(floatmode)
        {
            Value = displayVal.toDouble(&ok);
            Value = floor(Value);
        }
        if (ok)
        {
            ui->Display->setText(QString::number(Value, 2));
        }
        hexmode = false;
        decmode = false;
        binmode = true;
        octmode = false;
        floatmode = false;
        ui->Mode->setText("Mode: Bin");
        UpdateButton();
    }
    // na tryb FLOAT
    if(mode == "Float")
    {
        QString displayVal = ui->Display->text();
        bool ok;
        double dblValue = 0.0;
        if(decmode)
            dblValue = displayVal.toInt(&ok);
        if(hexmode)
            dblValue = displayVal.toInt(&ok, 16);
        if(octmode)
            dblValue = displayVal.toInt(&ok, 8);
        if(binmode)
            dblValue = displayVal.toInt(&ok, 2);
        if (ok)
        {
            ui->Display->setText(QString::number(dblValue));
        }
        hexmode = false;
        decmode = false;
        binmode = false;
        octmode = false;
        floatmode = true;
        ui->Mode->setText("Mode: Float");
        UpdateButton();
    }
}

//////////////////////////////////////////////////////////////
///
/// Funkcja Clearall() - Funkcja resetująca triggery matematyczne
/// oraz ustawiająca 0 na display
///
//////////////////////////////////////////////////////////////


void Calc::Clearall()
{
    ui->Display->setText("0");
    divTrigger = false;
    multTrigger = false;
    addTrigger = false;
    subTrigger = false;
}


//////////////////////////////////////////////////////////////
///
/// Funkcja UpdateButton() - funkcja której zadaniem jest
/// włączanie i wyłączanie guzików w zależności od wybranego
/// systemu liczbowego
///
//////////////////////////////////////////////////////////////

void Calc::UpdateButton()
{
    if(octmode)
    {
        ui->Button2->setEnabled(true);
        ui->Button3->setEnabled(true);
        ui->Button4->setEnabled(true);
        ui->Button5->setEnabled(true);
        ui->Button6->setEnabled(true);
        ui->Button7->setEnabled(true);
        ui->Button8->setEnabled(false);
        ui->Button9->setEnabled(false);
        ui->Button10->setEnabled(false);
        ui->Button11->setEnabled(false);
        ui->Button12->setEnabled(false);
        ui->Button13->setEnabled(false);
        ui->Button14->setEnabled(false);
        ui->Button15->setEnabled(false);
        ui->Dec->setEnabled(true);
        ui->Hex->setEnabled(true);
        ui->Oct->setEnabled(false);
        ui->Bin->setEnabled(true);
        ui->Float->setEnabled(true);
    }
    if(decmode)
    {
        ui->Button2->setEnabled(true);
        ui->Button3->setEnabled(true);
        ui->Button4->setEnabled(true);
        ui->Button5->setEnabled(true);
        ui->Button6->setEnabled(true);
        ui->Button7->setEnabled(true);
        ui->Button8->setEnabled(true);
        ui->Button9->setEnabled(true);
        ui->Button10->setEnabled(false);
        ui->Button11->setEnabled(false);
        ui->Button12->setEnabled(false);
        ui->Button13->setEnabled(false);
        ui->Button14->setEnabled(false);
        ui->Button15->setEnabled(false);
        ui->Dec->setEnabled(false);
        ui->Hex->setEnabled(true);
        ui->Oct->setEnabled(true);
        ui->Bin->setEnabled(true);
        ui->Float->setEnabled(true);
    }
    if(hexmode)
    {
        ui->Button2->setEnabled(true);
        ui->Button3->setEnabled(true);
        ui->Button4->setEnabled(true);
        ui->Button5->setEnabled(true);
        ui->Button6->setEnabled(true);
        ui->Button7->setEnabled(true);
        ui->Button8->setEnabled(true);
        ui->Button9->setEnabled(true);
        ui->Button10->setEnabled(true);
        ui->Button11->setEnabled(true);
        ui->Button12->setEnabled(true);
        ui->Button13->setEnabled(true);
        ui->Button14->setEnabled(true);
        ui->Button15->setEnabled(true);
        ui->Dec->setEnabled(true);
        ui->Hex->setEnabled(false);
        ui->Oct->setEnabled(true);
        ui->Bin->setEnabled(true);
        ui->Float->setEnabled(true);
    }
    if(binmode)
    {
        ui->Button2->setEnabled(false);
        ui->Button3->setEnabled(false);
        ui->Button4->setEnabled(false);
        ui->Button5->setEnabled(false);
        ui->Button6->setEnabled(false);
        ui->Button7->setEnabled(false);
        ui->Button8->setEnabled(false);
        ui->Button9->setEnabled(false);
        ui->Button10->setEnabled(false);
        ui->Button11->setEnabled(false);
        ui->Button12->setEnabled(false);
        ui->Button13->setEnabled(false);
        ui->Button14->setEnabled(false);
        ui->Button15->setEnabled(false);
        ui->Dec->setEnabled(true);
        ui->Hex->setEnabled(true);
        ui->Oct->setEnabled(true);
        ui->Bin->setEnabled(false);
        ui->Float->setEnabled(true);
    }
    if(floatmode)
    {
        ui->Button2->setEnabled(true);
        ui->Button3->setEnabled(true);
        ui->Button4->setEnabled(true);
        ui->Button5->setEnabled(true);
        ui->Button6->setEnabled(true);
        ui->Button7->setEnabled(true);
        ui->Button8->setEnabled(true);
        ui->Button9->setEnabled(true);
        ui->Button10->setEnabled(false);
        ui->Button11->setEnabled(false);
        ui->Button12->setEnabled(false);
        ui->Button13->setEnabled(false);
        ui->Button14->setEnabled(false);
        ui->Button15->setEnabled(false);
        ui->Dec->setEnabled(true);
        ui->Hex->setEnabled(true);
        ui->Oct->setEnabled(true);
        ui->Bin->setEnabled(true);
        ui->Float->setEnabled(false);
    }

}
