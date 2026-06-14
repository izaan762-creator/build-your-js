#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <cstring>
#include <cstdlib>
#include <unistd.h>

extern "C" {
#include "../quickjs/quickjs.h"
}

static JSValue js_console_log(JSContext *ctx, JSValueConst /*this_val*/,
                               int argc, JSValueConst *argv)
{
    for (int i = 0; i < argc; i++) {
        if (i) std::cout << ' ';
        const char *s = JS_ToCString(ctx, argv[i]);
        if (s) { std::cout << s; JS_FreeCString(ctx, s); }
    }
    std::cout << '\n';
    std::cout.flush();
    return JS_UNDEFINED;
}

static JSValue js_console_error(JSContext *ctx, JSValueConst /*this_val*/,
                                 int argc, JSValueConst *argv)
{
    for (int i = 0; i < argc; i++) {
        if (i) std::cerr << ' ';
        const char *s = JS_ToCString(ctx, argv[i]);
        if (s) { std::cerr << s; JS_FreeCString(ctx, s); }
    }
    std::cerr << '\n';
    return JS_UNDEFINED;
}

static JSValue js_process_exit(JSContext */*ctx*/, JSValueConst,
                                int argc, JSValueConst *argv) {
    // can't use ctx here safely during exit
    exit(0);
}

static void report_error(JSContext *ctx) {
    JSValue ex = JS_GetException(ctx);
    const char *msg = JS_ToCString(ctx, ex);
    std::cerr << "[Error] " << (msg ? msg : "(unknown)") << '\n';
    if (msg) JS_FreeCString(ctx, msg);
    JSValue stk = JS_GetPropertyStr(ctx, ex, "stack");
    if (!JS_IsUndefined(stk)) {
        const char *s = JS_ToCString(ctx, stk);
        if (s) { std::cerr << s << '\n'; JS_FreeCString(ctx, s); }
    }
    JS_FreeValue(ctx, stk);
    JS_FreeValue(ctx, ex);
}

static const char BOOTSTRAP[] = R"JS(
globalThis.console = {
    log:   (...a) => __print(...a),
    error: (...a) => __printerr(...a),
    warn:  (...a) => __printerr('[WARN]', ...a),
    info:  (...a) => __print('[INFO]', ...a),
    debug: (...a) => __print('[DEBUG]', ...a),
    assert(cond, ...a) { if (!cond) __printerr('Assertion failed:', ...a); },
    table(o) { __print(JSON.stringify(o, null, 2)); },
    dir(o)   { __print(JSON.stringify(o, null, 2)); },
    time()   {},
    timeEnd(){}
};
globalThis.process = {
    argv: [],
    env: {},
    exit: __process_exit,
    version: 'v20.0.0',
    platform: 'linux',
};
let __tid = 0;
globalThis.setTimeout   = (fn, d=0, ...a) => { fn(...a); return ++__tid; };
globalThis.clearTimeout  = () => {};
globalThis.setInterval   = (fn, d=0, ...a) => { fn(...a); return ++__tid; };
globalThis.clearInterval = () => {};
globalThis.queueMicrotask = fn => fn();
globalThis.global = globalThis;
)JS";

int main(int argc, char *argv[])
{
    const char *filename = nullptr;
    bool eval_mode = false;
    std::string eval_code;

    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "-e" && i + 1 < argc) { eval_mode = true; eval_code = argv[++i]; }
        else { filename = argv[i]; }
    }

    std::string code;
    const char *src_name;
    if (eval_mode) {
        code = eval_code; src_name = "<eval>";
    } else if (filename) {
        std::ifstream f(filename, std::ios::binary);
        if (!f) { std::cerr << "Error: cannot open: " << filename << '\n'; return 1; }
        std::stringstream buf; buf << f.rdbuf();
        code = buf.str(); src_name = filename;
    } else if (!isatty(STDIN_FILENO)) {
        std::stringstream buf; buf << std::cin.rdbuf();
        code = buf.str(); src_name = "<stdin>";
    } else {
        std::cerr << "Usage: myjs <file.js>\n       myjs -e '<code>'\n       cat f.js | myjs\n";
        return 1;
    }

    JSRuntime *rt = JS_NewRuntime();
    JS_SetMemoryLimit(rt, 256ULL * 1024 * 1024);
    JS_SetMaxStackSize(rt, 4 * 1024 * 1024);

    JSContext *ctx = JS_NewContext(rt);

    JSValue g = JS_GetGlobalObject(ctx);
    JS_SetPropertyStr(ctx, g, "__print",
        JS_NewCFunction(ctx, js_console_log,   "__print",    1));
    JS_SetPropertyStr(ctx, g, "__printerr",
        JS_NewCFunction(ctx, js_console_error, "__printerr", 1));
    JS_SetPropertyStr(ctx, g, "__process_exit",
        JS_NewCFunction(ctx, js_process_exit,  "__process_exit", 1));
    JS_FreeValue(ctx, g);

    // Bootstrap
    JSValue br = JS_Eval(ctx, BOOTSTRAP, strlen(BOOTSTRAP),
                         "<bootstrap>", JS_EVAL_TYPE_GLOBAL);
    if (JS_IsException(br)) {
        std::cerr << "Bootstrap failed!\n";
        report_error(ctx);
        JS_FreeValue(ctx, br);
        JS_FreeContext(ctx);
        JS_FreeRuntime(rt);
        return 1;
    }
    JS_FreeValue(ctx, br);

    // User code
    JSValue res = JS_Eval(ctx, code.c_str(), code.size(),
                          src_name, JS_EVAL_TYPE_GLOBAL);
    int exit_code = 0;
    if (JS_IsException(res)) {
        report_error(ctx);
        exit_code = 1;
    }
    JS_FreeValue(ctx, res);

    // Drain microtask / promise queue manually
    JSContext *ctx2;
    for (;;) {
        int r = JS_ExecutePendingJob(rt, &ctx2);
        if (r <= 0) break;
    }

    JS_FreeContext(ctx);
    JS_FreeRuntime(rt);
    return exit_code;
}
