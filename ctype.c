// daintree

#include <ctype.h>

int toupper(int c) {
    if (c >= 'a' && c <= 'z') {
        return c - 'a' + 'A';
    }
    return c;
}

int tolower(int c) {
    if (c >= 'A' && c <= 'Z') {
        return c - 'A' + 'a';
    }
    return c;
}

int islower(int c) {
    return c >= 'a' && c <= 'z';
}

int isupper(int c) {
    return c >= 'A' && c <= 'Z';
}

int isblank(int c) {
    return c == ' ' || c == '\t';
}

int iscntrl(int c) {
    return (c >= 000 && c <= 037) || c == 0177;
}

int isalpha(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

int isdigit(int c) {
    return c >= '0' && c <= '9';
}

int isalnum(int c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9');
}

int isgraph(int c) {
    return c >= 041 && c <= 0176;
}

int isprint(int c) {
    return c >= 040 && c <= 0176;
}

int ispunct(int c) {
    return (c >= 041 && c <= 057) || (c >= 072 && c <= 0100) || (c >= 0133 && c <= 0140) || (c >= 0173 && c <= 0176);
}

int isspace(int c) {
    return c == '\t' || c == '\n' || c == '\v' || c == '\f' || c == '\r' || c == ' ';
}

int isxdigit(int c) {
    return (c >= '0' && c <= '9') || (c >= 'a' && c <= 'f') || (c >= 'A' && c <= 'F');
}

/* vim: set sw=4 et: */
