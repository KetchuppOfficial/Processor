#ifndef HASH_PROTECTION_H_INCLUDED
#define HASH_PROTECTION_H_INCLUDED

#define CHECK_HASH(condition)               \
{                                           \
    if (Check_Hash ((condition)) == ERROR)  \
        return ERROR;                       \
}

#define HASH_VAR(hash, var)                 \
{                                           \
    hash += (hash_t)(var);                  \
    hash += (hash << 10);                   \
    hash ^= (hash >> 6);                    \
}

typedef unsigned long hash_t;

#endif // HASH_PROTECTION_H_INCLUDED
