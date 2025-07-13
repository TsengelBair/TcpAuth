#include <QDebug>
#include <QSqlError>
#include <QSqlQuery>
#include <exception>

#include "postgresdb.h"

PostgresDB::PostgresDB(const QString &dbName)
{
    m_db = QSqlDatabase::addDatabase("QPSQL");
    m_db.setHostName("localhost");
    m_db.setDatabaseName(dbName);
    m_db.setUserName("postgres");

    if (!m_db.open()) {
        qDebug() << "Database connection error:" << m_db.lastError().text();
    }
}

PostgresDB::~PostgresDB()
{
    if (m_db.isOpen()) m_db.close();
}

RegisterStatus PostgresDB::registerUser(const QString &login, const QString &password)
{
    if (!m_db.isOpen()) return RegisterDatabaseError;

    try {
        if (userExist(login)) return RegisterUserExists;

        QSqlQuery query(m_db);
        query.prepare("INSERT INTO users (login, password) VALUES (:login, :password)");
        query.bindValue(":login", login);
        query.bindValue(":password", password);

        executeQuery(query);
        return RegisterSuccess;

    } catch (const std::exception &e) {
        qDebug() << "Registration error:" << e.what();
        return RegisterDatabaseError;
    }
}

AuthStatus PostgresDB::authUser(const QString &login, const QString &password)
{
    if (!m_db.isOpen()) return AuthStatus::AuthDatabaseError;

    try {
        QSqlQuery query(m_db);
        query.prepare("SELECT password FROM users WHERE login = :login");
        query.bindValue(":login", login);
        executeQuery(query);

        if (!query.next()) {
            qDebug() << "User not found:" << login;
            return AuthStatus::AuthUserNotFound;
        }

        QString storedPassword = query.value(0).toString();

        if (!checkPasswords(password, storedPassword)) {
            qDebug() << "Invalid password for user:" << login;
            return AuthStatus::AuthInvalidPassword;
        }

        qDebug() << "User authenticated successfully:" << login;
        return AuthStatus::AuthSuccess;

    } catch (const std::exception &e) {
        qDebug() << "Authentication error:" << e.what();
        return AuthStatus::AuthDatabaseError;
    }
}
bool PostgresDB::userExist(const QString &login)
{
    if (!m_db.isOpen()) {
        throw std::runtime_error("Database connection is not open");
    }

    QSqlQuery query(m_db);
    query.prepare("SELECT 1 FROM users WHERE login = :login");
    query.bindValue(":login", login);

    executeQuery(query);  /// Будет бросать исключение при ошибке
    return query.next();
}

bool PostgresDB::executeQuery(QSqlQuery &query)
{
    if (!query.exec()) {
        throw std::runtime_error(
            "Query execution failed: " +
            query.lastError().text().toStdString()
            );
    }
    return true;
}

bool PostgresDB::checkPasswords(const QString &password, const QString &passwordFromDb)
{
    /// в реальном проекте в бд хранится захэшированный пароль
    /// в этом методе необходимо захэшировать переданный, поэтому он вынесен в отдельный метод
    return password == passwordFromDb;
}
