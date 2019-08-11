#include "Game.h"

#include <exception>
#include <iostream>

#ifdef DEBUG
#include <fstream>
class Logger : public std::streambuf
{
private:
    std::basic_ios<char>* zeiger_auf_ausgabe; //brauchen wir für Nutzung von rdbuf im Destruktor
    std::streambuf* alter_buf;
    std::ofstream logdatei;
    int overflow(int c)
    {
        logdatei << static_cast<char>(c);
        putchar(c);
    }
public:
    Logger(std::basic_ios<char>& a, std::string logdateiname)
    : zeiger_auf_ausgabe(&a),
     alter_buf(a.rdbuf(this)),
     logdatei(logdateiname.c_str())
    {}
    ~Logger()
    { zeiger_auf_ausgabe->rdbuf(alter_buf); }
};
#endif // DEBUG

int main()
{
	#ifdef DEBUG
	Logger l(std::cout, "log.log");
	#endif // DEBUG
    try
    {
    	Game game;
    	game.run();
    }

    catch(std::exception& e)
    {
    	std::cout << e.what();
    }
}
