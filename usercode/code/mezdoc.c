#include "mezdoc.h"

#define NULL 0
#define WAIT 1
#define LINK1 2
#define LINK2 3
#define COLOR1 4
#define COLOR2 5
#define BUTTON1 6
#define BUTTON2 7
#define EXEC 8

int mzd_parse(mzd_ctx *ctx, mzd_object *obj, char c) {
    switch (ctx->mod) {
        case NULL: {
            if (c == '$') {
                memset(ctx->name, 0, sizeof(ctx->name));
                memset(ctx->link, 0, sizeof(ctx->link));
                ctx->mod = WAIT;
                ctx->type = 0;
                return 0;
            }
            return 2;
        } break;
        case WAIT: {
            switch (c) {
                case 'L': {
                    ctx->mod = LINK1;
                } break;
                case 'B': {
                    ctx->mod = BUTTON1;
                } break;
                case 'E': {
                    ctx->mod = EXEC;
                } break;
                case '!': {
                    ctx->mod = COLOR1;
                } break;
                case '*': {
                    ctx->mod = COLOR2;
                } break;
                case 'C': {
                    ctx->mod = NULL;
                    obj->type = MZD_CLS;
                    return 1;
                } break;
                default: {
                    ctx->mod = NULL;
                    return 2;
                } break;
            }
            return 0;
        } break;
        case COLOR1: {
            ctx->mod = NULL;
            obj->type = MZD_FG;
            obj->a1[0] = c;
            return 1;
        } break;
        case COLOR2: {
            ctx->mod = NULL;
            obj->type = MZD_BG;
            obj->a1[0] = c;
            return 1;
        } break;
        case EXEC: {
            if (c == '$') {
                ctx->mod = NULL;
                obj->type = MZD_EXEC;
                for (int i = 0; ctx->name[i]; ++i) {
                    obj->a1[i] = ctx->name[i];
                }
                return 1;
            }
            size_t len = strlen(ctx->name);
            if (len < sizeof(ctx->name) - 1) {
                ctx->name[len] = c;
                ctx->name[len + 1] = 0;
            }
            return 0;
        } break;
        default: {
            ctx->mod = NULL;
            return 0;
        } break;
    }
}