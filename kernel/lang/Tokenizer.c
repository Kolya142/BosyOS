#include <lang/Tokenizer.h>

U32 TokenNext(String str, Token *tok) {
    U32 pos = 0;
    tok->type = 0;
    tok->num = 0;
    MemSet(tok->str, 0, sizeof(tok->str));
    for (;*str && (pos < sizeof(tok->str) - 1);) {
        if (!tok->type) {
            if ((*str >= '0' && *str <= '9') || (*str == '-' && (str[1] >= '0' && str[1] <= '9'))) {
                tok->type = TOK_NUMBER;
            }
            else if ((*str >= '!' && *str <= '/') || (*str >= ':' && *str <= '@') || (*str >= '[' && *str <= '`') || (*str >= '{' && *str <= '~')) {
                tok->type = TOK_SPEC;
                tok->str[0] = *str;
                if ((*str == '!' || *str == '=' || *str == '<' || *str == '>') && str[1] == '=') {
                    tok->str[1] = str[1];
                    ++str;
                }
                ++pos;
                break;
            }
            else if (*str == '"') {
                tok->type = TOK_STR;
                ++str;
                continue;
            }
            else if (*str == ' ') {
                ++pos;
                break;
            }
            else {
                tok->type = TOK_WORD;
            }
        }
        else if (tok->type == TOK_STR) {
            if (*str == '"') break;
        }
        else {
            if ((*str == ' ') || (*str == '\n') || (*str == '\r') || (*str == '\t')) {
                ++pos;
                break; 
            }
            if ((tok->type == TOK_NUMBER) && (*str < '0' || *str > '9')) {
                break;
            }
            if ((*str >= '!' && *str <= '/') || (*str >= ':' && *str <= '@') || (*str >= '[' && *str <= '`') || (*str >= '{' && *str <= '~')) {
                ++pos;
                break;
            }
        }
        tok->str[pos] = *str;
        ++pos;
        ++str;
    }
    return pos;
}