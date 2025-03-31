#include <lang/Tokenizer.h>
#include <lib/IO/TTY.h>

U32 TokenNext(String str, Token *tok) {
    U32 pos = 0;
    tok->type = 0;
    tok->num = 0;
    MemSet(tok->str, 0, sizeof(tok->str));
    for (;*str && (pos < sizeof(tok->str) - 1);) {
        if (!tok->type) {
            if ((*str >= '0' && *str <= '9') || (*str == '-' && (str[1] >= '0' && str[1] <= '9'))) {
                pos = 0;
                tok->type = TOK_NUMBER;
            }
            else if (*str == '"') {
                tok->type = TOK_STR;
                pos = 0;
                ++str;
                while (*str && pos < sizeof(tok->str) - 1) {
                    if (*str == '"') {
                        ++pos;
                        break;
                    }
                    if (*str == '\\') {
                        ++str;
                        switch (*str) {
                            case '"':
                                tok->str[pos++] = '"';
                                ++str;
                            break;
                            case 'n':
                                tok->str[pos++] = '\n';
                                ++str;
                            break;
                            case 't':
                                tok->str[pos++] = '\t';
                                ++str;
                            break;
                            case 'r':
                                tok->str[pos++] = '\r';
                                ++str;
                            break;
                        }
                    }
                    else {
                        tok->str[pos] = *str;
                        ++str;
                        ++pos;
                    }
                }
                ++str;
                return pos + 2;
            }
            else if ((*str >= '!' && *str <= '/') || (*str >= ':' && *str <= '@') || (*str >= '[' && *str <= '`') || (*str >= '{' && *str <= '~')) {
                pos = 0;
                tok->type = TOK_SPEC;
                tok->str[pos++] = *str++;
                if ((*str == '!' || *str == '=' || *str == '<' || *str == '>') && str[1] == '=') {
                    tok->str[pos++] = *str++;
                }
                return pos;
            }
            else if ((*str == ' ') || (*str == '\n') || (*str == '\r') || (*str == '\t')) {
                ++pos;
                break;
            }
            else {
                pos = 0;
                tok->type = TOK_WORD;
            }
        }
        else {
            if ((*str == ' ') || (*str == '\n') || (*str == '\r') || (*str == '\t')) {
                ++pos;
                break;
            }
            if ((tok->type == TOK_NUMBER) && (*str < '0' || *str > '9') && *str != 'x') {
                break;
            }
            if ((*str >= '!' && *str <= '/') || (*str >= ':' && *str <= '@') || (*str >= '[' && *str <= '`') || (*str >= '{' && *str <= '~')) {
                break;
            }
        }
        tok->str[pos] = *str;
        ++pos;
        ++str;
    }
    return pos;
}