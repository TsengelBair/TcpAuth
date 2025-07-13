#ifndef STRUCTS_H
#define STRUCTS_H

enum RegisterStatus : char
{
    RegisterSuccess,
    RegisterUserExists,
    RegisterDatabaseError,
    RegisterUnknownError
};

enum AuthStatus : char
{
    AuthSuccess,
    AuthUserNotFound,
    AuthInvalidPassword,
    AuthDatabaseError,
    AuthUnknownError
};

#endif // STRUCTS_H
