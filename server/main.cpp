#include <QCoreApplication>

#include "server.h"
#include "postgresdb.h"

int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);

    PostgresDB db("test_db");

    Server server(db);

    return a.exec();
}
