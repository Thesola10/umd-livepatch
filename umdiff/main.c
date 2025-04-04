#include "usage.rl.h"

int main(int argc, char *argv[])
{
    int res;
    options_t opts;

    res = parse_options(argc, argv, &opts);
    if (res == -1)
        return -1;

    if (opts.help) {
        show_usage();
        return 0;
    }

    return 1;
}
