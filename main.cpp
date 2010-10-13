#include <QtCore/QCoreApplication>
#include <QTextStream> //Необходим для вывода в терминал.
#include <QStringList> //Необходим для получения списка аргументов.
#include <math.h> //Необходим для функции pow (возведение в степень).

//Структура для основных параметров терминала.
struct param
{
    double d;
    double ds;
    double tw;
    double ts;
    int it;
};

//Функция, возвращающая структуру param в ответ на s, m, phi с заданной точностью acc.
param getParam(double s, double m, double phi, double acc)
{
    //Начало рассчетной части.
    double lambda,lambdas,psi,mu,pn,p,q; //Переменные для рассчетов.
    param terminal; //Переменная для параметров терминала.

    //Считаем на основе формул.
    mu = 1;
    psi = phi * s;
    lambda = psi;
    lambdas = lambda / m;
    p = 0;
    q = 1;
    pn = 0;
    terminal.tw = 0; //Для первой иттерации.
    terminal.ts = 0;
    terminal.it = 0;
    terminal.d = 0;
    terminal.ds = 0;

    //Рассчитываем tw для phi1.
    while(fabs(terminal.ts - 1 - terminal.tw) > acc) //Повторяем цикл, пока не добьемся необходимой точности.
    {
        p = (1 / mu + terminal.tw) * lambdas;
        q = 1 - p;
        terminal.d = 0;
        terminal.ds = 0;
        pn = pow(q, m); //n = 0 - нулевая итеррация.
        for(int n=1; n<=m; n++)
        {
            pn = pn * ((m - (n - 1)) / n) * (p / q);
            if(n > s)
            {
                terminal.d = terminal.d + pn * (n - s);
            }
            //terminal.ds = terminal.ds + pn * n; //Можно считать вне цикла (см. ниже).
        }
        terminal.ds = m * p; //Можно считать в цикле (см. выше).
        terminal.tw = terminal.d / (lambdas * m);
        terminal.ts = terminal.ds / (lambdas * m);
        if(p > 1) //Выполнены все условия?
        {
            terminal.d = -1;
            terminal.ds = -1;
            terminal.tw = -1;
            terminal.ts = 0;
        }
        terminal.it++;
    }

    return terminal;
}

//Функция для проверки пользовательского ввода.
bool valid(QStringList args)
{
    if(args.at(1).toDouble() > 0 && args.at(2).toDouble() > 0 && args.at(3).toDouble() > 0 && args.at(3).toDouble() < 1)
    {
        if(args.count() == 4)
            return true;
        else
            if(args.count() == 5 && args.at(4).toDouble() >= 0 && args.at(4).toDouble() < 1)
                return true;
            else
                if(args.count() == 6 && args.at(5).toDouble() > 0)
                    return true;
                else
                    return false;
    }
    else
        return false;
}

int main(int argc, char *argv[])
{
    //Инициализируем приложение.
    QCoreApplication a(argc, argv);

    //Переменные.
    QTextStream out(stdout); //Переменная для вывода в терминал.
    double s,m,phi,nu,acc; //Переменные для пользовательского ввода.
    double phi1,phi2; //Переменные для рассчетов.
    param terminal,terminal1,terminal2; //Переменные для работы с функцией getParam.
    bool run; //Флаг.

    //Собираем аргументы.
    QStringList args = a.arguments();
    acc = 1e-8; //Точность по-умолчанию.

    //Проверка параметров.
    if(valid(args))
    {
        if(args.count() == 6)
        {
            s = args.at(1).toDouble();
            m = args.at(2).toDouble();
            phi = args.at(3).toDouble();
            nu = args.at(4).toDouble();
            acc = args.at(5).toDouble();
            run = true;
        }
        else if(args.count() == 5)
        {
            s = args.at(1).toDouble();
            m = args.at(2).toDouble();
            phi = args.at(3).toDouble();
            nu = args.at(4).toDouble();
            run = true;
        }
        else if (args.count() == 4)
        {
            s = args.at(1).toDouble();
            m = args.at(2).toDouble();
            phi = args.at(3).toDouble();
            nu = 0;
            run = true;
        }
        else
            run = false;
    }
    else
        run = false;

    if(run)
    {
        if (nu) //А есть ли приоритет?
        {
            //Рассчитываем значения phi по формуле.
            phi1 = phi * nu;
            phi2 = phi - phi1;

            terminal1 = getParam(s, m, phi1, acc); //Считаем основные параметры для судов с приоритетом.
            terminal = getParam(s, m, phi, acc); //Считаем основные параметры для всех судов.
            if(terminal1.tw != -1 and terminal.tw != -1) //Все ли было посчитано верно.
            {
                terminal2.d = 0;
                terminal2.ds = 0;
                terminal2.tw = (terminal.tw - nu * terminal1.tw) / (1 - nu); //Считаем время ожидания для судов без приоритета.
                terminal2.ts = 1 + terminal2.tw;
                terminal2.it = 0;
            }
            else
            {
                terminal.d = -1;
                terminal.ds = -1;
                terminal.tw = -1;
                terminal.ts = -1;
                terminal.it = -1;
                terminal1.d = -1;
                terminal1.ds = -1;
                terminal1.tw = -1;
                terminal1.ts = -1;
                terminal1.it = -1;
                terminal2.d = -1;
                terminal2.ds = -1;
                terminal2.tw = -1;
                terminal2.ts = -1;
                terminal2.it = -1;
            }

            out << terminal.tw << " " << terminal.ts << " " << terminal.d << " " << terminal.ds << " " << terminal.it << endl;
            out << terminal1.tw << " " << terminal1.ts << " " << terminal1.d << " " << terminal1.ds << " " << terminal1.it << endl;
            out << terminal2.tw << " " << terminal2.ts << " " << terminal2.d << " " << terminal2.ds << " " << terminal2.it << endl;
            out.flush(); //Вывод в терминал.
        }
        else
        {
            terminal = getParam(s, m, phi, acc); //Ищем основные параметры без приоритета.
            out << terminal.tw << " " << terminal.ts << " " << terminal.d << " " << terminal.ds << " " << terminal.it << endl;
            out.flush(); //Вывод в терминал.
        }
    }

    return 0;
}
