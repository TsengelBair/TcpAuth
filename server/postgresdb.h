#ifndef POSTGRESDB_H
#define POSTGRESDB_H

#include <QSqlDatabase>
#include "IDbHandler.h"
#include "structs.h"

class PostgresDB : public IDbHandler
{
public:
    explicit PostgresDB(const QString &dbName);
    ~PostgresDB();

    RegisterStatus registerUser(const QString &login, const QString &password) override;
    AuthStatus authUser(const QString &login, const QString &password) override;

private:
    bool userExist(const QString &login);
    bool executeQuery(QSqlQuery &query);
    bool checkPasswords(const QString &password, const QString &passwordFromDb);

private:
    QSqlDatabase m_db;
};

#endif // POSTGRESDB_H
