#ifndef IDBHANDLER_H
#define IDBHANDLER_H

#include <QString>
#include "structs.h"

class IDbHandler
{
public:
    virtual RegisterStatus registerUser(const QString &login, const QString &password) = 0;
    virtual AuthStatus authUser(const QString &login, const QString &password) = 0;

    virtual ~IDbHandler(){};
};

#endif // IDBHANDLER_H
