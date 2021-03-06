# include "std_lib_facilities.h"

const char number = '8';
const char quit = 'x';
const char print = '=';
const string prompt = "> ";
const string result = "= ";

class Token
{
public:
    char kind;
    double value;

    Token(char ch) : kind { ch }, value { 0 } { }
    Token(char ch, double val) : kind { ch }, value { val } { }
};

class Token_stream
{
public:
    Token_stream() : full { false }, buffer { ' ' } { }
    Token get();
    void putback(Token t);
    void ignore(char c);
private:
    bool full;
    Token buffer;
};

void Token_stream::putback(Token t)
{
    if (full)
    {
        error("putback() into a full buffer");
    }

    buffer = t;
    full = true;
}

Token Token_stream::get()
{
    if (full)
    {
        full = false;
        return buffer;
    }

    char ch;
    std::cin >> ch;

    switch (ch)
    {
        case print:
        case quit:
        case '(':
        case ')':
        case '{':
        case '}':
        case '+':
        case '-':
        case '*':
        case '/':
        case '%':
        case '!':
            return Token(ch);
        case '.':
        case '0':
        case '1':
        case '2':
        case '3':
        case '4':
        case '5':
        case '6':
        case '7':
        case '8':
        case '9':
            {
                std::cin.putback(ch);
                double val;
                std::cin >> val;
                return Token(number, val);
            }
        default:
            error("Bad token");
            return Token { ' ' };
    }
}

void Token_stream::ignore(char c)
{
    if (full && c == buffer.kind)
    {
        full = false;
        return;
    }

    full = false;

    char ch;
    while (std::cin >> ch)
    {
        if (ch == c)
        {
            return;
        }
    }
}

double expression();
double term();
double factorial();
double primary();
double get_factorial(double val);
void calculate();
void clean_up_mess();

Token_stream ts;

int main()
{
    try
    {
        calculate();
        keep_window_open();
    }
    catch (exception& e)
    {
        std::cerr << e.what() << std::endl;
        keep_window_open();

        return 1;
    }
    catch (...)
    {
        std::cerr << "exception" << std::endl;
        keep_window_open();

        return 2;
    }
}

double expression()
{
    double left = term();
    Token t = ts.get();

    while (true)
    {
        switch (t.kind)
        {
            case '+':
                {
                    left += term();
                    t = ts.get();
                    break;
                }
            case '-':
                {
                    left -= term();
                    t = ts.get();
                    break;
                }
            default:
                ts.putback(t);
                return left;
        }
    }
}

double term()
{
    double left = factorial();
    Token t = ts.get();

    while (true)
    {
        switch (t.kind)
        {
            case '*':
                {
                    left *= factorial();
                    t = ts.get();
                    break;
                }
            case '/':
                {
                    double d = factorial();

                    if (!d)
                    {
                        error("divide by zero");
                    }

                    left /= d;
                    t = ts.get();
                    break;
                }
            case '%':
                {
                    double d = factorial();

                    if (!d)
                    {
                        error("divide by zero");
                    }

                    left = fmod(left, d);
                    t = ts.get();
                    break;
                }
            default:
                ts.putback(t);
                return left;
        }
    }
}

double factorial()
{
    double left = primary();
    Token t = ts.get();

    while (true)
    {
        switch (t.kind)
        {
            case '!':
                {
                    left = get_factorial(left);
                    t = ts.get();
                    break;
                }
            default:
                ts.putback(t);
                return left;
        }
    }
}

double primary()
{
    Token t = ts.get();

    switch (t.kind)
    {
        case '(':
            {
                double d = expression();
                t = ts.get();

                if (t.kind != ')')
                {
                    error("')' expected");
                }
                return d;
            }
        case '{':
            {
                double d = expression();
                t = ts.get();

                if (t.kind != '}')
                {
                    error("'}' expected");
                }
                return d;
            }
        case number:
            return t.value;
        case '-':
            return -primary();
        case '+':
            return primary();
        default:
            error("primary expected");
            return 0;
    }
}

double get_factorial(double val)
{
    int result = 1;

    for (int i = 1; i <= val; ++i)
    {
        result *= i;
    }

    return result;
}

void calculate()
{
    while (std::cin)
    try 
    {
        std::cout << prompt;

        Token t = ts.get();

        while (t.kind == print)
        {
            t = ts.get();
        }

        if (t.kind == quit)
        {
            return;
        }
        
        ts.putback(t);
        std::cout << result << expression() << std::endl;
    }
    catch (exception& e)
    {
        std::cerr << e.what() << std::endl;
        clean_up_mess();
    }
}

void clean_up_mess()
{
    ts.ignore(print);
}