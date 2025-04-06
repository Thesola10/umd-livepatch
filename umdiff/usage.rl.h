
#line 1 "usage.rl"
#include <stdio.h>
#include <string.h>

#define BUFLEN 1024
#define BUFSIZE 2048

typedef struct
{
    /* commands */
    int delta;
    int fromrdiff;
    int patch;

    /* flags */
    int help;

    /* options */
    char* loadmode;

    /* arguments */
    char* output_file;
    char* rdiff_file;
    char* source_file;
    char* target_file;
    char* umdiff_file;

} options_t;

struct params
{
    options_t* opt;
    char buffer[BUFLEN + 1];
    int buflen;
    int cs;
};


#line 70 "usage.rl"



#line 40 "usage.rl.h"
static const char _params_actions[] = {
	0, 1, 0, 1, 3, 1, 4, 1, 
	5, 1, 6, 2, 1, 8, 2, 1, 
	9, 2, 1, 10, 2, 1, 11, 2, 
	1, 12, 2, 2, 0, 3, 1, 7, 
	12, 3, 1, 10, 8
};

static const char _params_key_offsets[] = {
	0, 0, 4, 6, 7, 8, 9, 10, 
	11, 12, 13, 14, 15, 16, 17, 18, 
	19, 20, 21, 22, 23, 24, 25, 26, 
	27, 28, 29, 30, 31, 32, 33, 34, 
	35, 36, 37, 38, 40, 41, 42, 43, 
	45, 47, 49, 51, 53, 55, 57, 59, 
	61, 63, 64, 65, 66, 68, 69, 71, 
	73, 75, 77, 79, 81, 83, 85, 87, 
	89, 90, 90, 91
};

static const char _params_trans_keys[] = {
	45, 100, 102, 112, 45, 104, 104, 101, 
	108, 112, 0, 101, 108, 116, 97, 0, 
	0, 0, 0, 0, 0, 0, 114, 111, 
	109, 114, 100, 105, 102, 102, 0, 0, 
	0, 97, 116, 99, 104, 0, 0, 45, 
	0, 0, 0, 0, 45, 0, 108, 0, 
	111, 0, 97, 0, 100, 0, 45, 0, 
	109, 0, 111, 0, 100, 0, 101, 0, 
	0, 0, 0, 45, 0, 0, 45, 0, 
	108, 0, 111, 0, 97, 0, 100, 0, 
	45, 0, 109, 0, 111, 0, 100, 0, 
	101, 0, 0, 0, 0
};

static const char _params_single_lengths[] = {
	0, 4, 2, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 2, 1, 1, 1, 2, 
	2, 2, 2, 2, 2, 2, 2, 2, 
	2, 1, 1, 1, 2, 1, 2, 2, 
	2, 2, 2, 2, 2, 2, 2, 2, 
	1, 0, 1, 1
};

static const char _params_range_lengths[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 0, 0
};

static const unsigned char _params_index_offsets[] = {
	0, 0, 5, 8, 10, 12, 14, 16, 
	18, 20, 22, 24, 26, 28, 30, 32, 
	34, 36, 38, 40, 42, 44, 46, 48, 
	50, 52, 54, 56, 58, 60, 62, 64, 
	66, 68, 70, 72, 75, 77, 79, 81, 
	84, 87, 90, 93, 96, 99, 102, 105, 
	108, 111, 113, 115, 117, 120, 122, 125, 
	128, 131, 134, 137, 140, 143, 146, 149, 
	152, 154, 155, 157
};

static const char _params_indicies[] = {
	0, 2, 3, 4, 1, 5, 6, 1, 
	7, 1, 8, 1, 9, 1, 6, 1, 
	10, 1, 11, 1, 12, 1, 13, 1, 
	14, 1, 15, 1, 1, 16, 18, 17, 
	1, 19, 21, 20, 1, 22, 24, 23, 
	25, 1, 26, 1, 27, 1, 28, 1, 
	29, 1, 30, 1, 31, 1, 32, 1, 
	33, 1, 1, 34, 36, 35, 37, 1, 
	38, 1, 39, 1, 40, 1, 41, 1, 
	1, 43, 42, 45, 44, 1, 46, 48, 
	47, 45, 49, 44, 45, 50, 44, 45, 
	51, 44, 45, 52, 44, 45, 53, 44, 
	45, 54, 44, 45, 55, 44, 45, 56, 
	44, 45, 57, 44, 45, 58, 44, 59, 
	44, 1, 60, 62, 61, 1, 64, 63, 
	66, 65, 66, 67, 65, 66, 68, 65, 
	66, 69, 65, 66, 70, 65, 66, 71, 
	65, 66, 72, 65, 66, 73, 65, 66, 
	74, 65, 66, 75, 65, 66, 76, 65, 
	77, 65, 1, 1, 46, 1, 60, 0
};

static const char _params_trans_targs[] = {
	2, 0, 8, 19, 30, 3, 7, 4, 
	5, 6, 65, 9, 10, 11, 12, 13, 
	14, 14, 15, 16, 16, 17, 18, 18, 
	65, 20, 21, 22, 23, 24, 25, 26, 
	27, 28, 29, 29, 17, 31, 32, 33, 
	34, 35, 36, 39, 36, 37, 38, 38, 
	17, 40, 41, 42, 43, 44, 45, 46, 
	47, 48, 49, 50, 51, 51, 52, 53, 
	54, 53, 66, 55, 56, 57, 58, 59, 
	60, 61, 62, 63, 64, 67
};

static const char _params_trans_actions[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 
	0, 0, 9, 0, 0, 0, 0, 3, 
	26, 1, 17, 26, 1, 20, 26, 1, 
	11, 0, 0, 0, 0, 0, 0, 0, 
	0, 5, 26, 1, 14, 0, 0, 0, 
	0, 7, 26, 26, 1, 17, 26, 1, 
	23, 1, 1, 1, 1, 1, 1, 1, 
	1, 1, 1, 17, 26, 1, 29, 26, 
	26, 1, 33, 1, 1, 1, 1, 1, 
	1, 1, 1, 1, 1, 33
};

static const int params_start = 1;
static const int params_first_final = 65;
static const int params_error = 0;

static const int params_en_main = 1;


#line 73 "usage.rl"

static void params_init(struct params *fsm, options_t* opt)
{
    memset(opt, 0, sizeof(options_t));

    fsm->opt = opt;
    fsm->buflen = 0;
    fsm->opt->loadmode = strdup("0");

    
#line 175 "usage.rl.h"
	{
	 fsm->cs = params_start;
	}

#line 83 "usage.rl"
}

static void params_execute(struct params *fsm, const char *data, int len)
{
    const char *p = data;
    const char *pe = data + len;

    
#line 185 "usage.rl.h"
	{
	int _klen;
	unsigned int _trans;
	const char *_acts;
	unsigned int _nacts;
	const char *_keys;

	if ( p == pe )
		goto _test_eof;
	if (  fsm->cs == 0 )
		goto _out;
_resume:
	_keys = _params_trans_keys + _params_key_offsets[ fsm->cs];
	_trans = _params_index_offsets[ fsm->cs];

	_klen = _params_single_lengths[ fsm->cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + _klen - 1;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + ((_upper-_lower) >> 1);
			if ( (*p) < *_mid )
				_upper = _mid - 1;
			else if ( (*p) > *_mid )
				_lower = _mid + 1;
			else {
				_trans += (unsigned int)(_mid - _keys);
				goto _match;
			}
		}
		_keys += _klen;
		_trans += _klen;
	}

	_klen = _params_range_lengths[ fsm->cs];
	if ( _klen > 0 ) {
		const char *_lower = _keys;
		const char *_mid;
		const char *_upper = _keys + (_klen<<1) - 2;
		while (1) {
			if ( _upper < _lower )
				break;

			_mid = _lower + (((_upper-_lower) >> 1) & ~1);
			if ( (*p) < _mid[0] )
				_upper = _mid - 2;
			else if ( (*p) > _mid[1] )
				_lower = _mid + 2;
			else {
				_trans += (unsigned int)((_mid - _keys)>>1);
				goto _match;
			}
		}
		_trans += _klen;
	}

_match:
	_trans = _params_indicies[_trans];
	 fsm->cs = _params_trans_targs[_trans];

	if ( _params_trans_actions[_trans] == 0 )
		goto _again;

	_acts = _params_actions + _params_trans_actions[_trans];
	_nacts = (unsigned int) *_acts++;
	while ( _nacts-- > 0 )
	{
		switch ( *_acts++ )
		{
	case 0:
#line 41 "usage.rl"
	{
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = (*p);
    }
	break;
	case 1:
#line 46 "usage.rl"
	{
        if (fsm->buflen < BUFLEN)
            fsm->buffer[fsm->buflen++] = 0;
    }
	break;
	case 2:
#line 51 "usage.rl"
	{ fsm->buflen = 0; }
	break;
	case 3:
#line 53 "usage.rl"
	{ fsm->opt->delta = 1; }
	break;
	case 4:
#line 54 "usage.rl"
	{ fsm->opt->fromrdiff = 1; }
	break;
	case 5:
#line 55 "usage.rl"
	{ fsm->opt->patch = 1; }
	break;
	case 6:
#line 56 "usage.rl"
	{ fsm->opt->help = 1; }
	break;
	case 7:
#line 57 "usage.rl"
	{ fsm->opt->loadmode = strdup(fsm->buffer); }
	break;
	case 8:
#line 58 "usage.rl"
	{ fsm->opt->output_file = strdup(fsm->buffer); }
	break;
	case 9:
#line 59 "usage.rl"
	{ fsm->opt->rdiff_file = strdup(fsm->buffer); }
	break;
	case 10:
#line 60 "usage.rl"
	{ fsm->opt->source_file = strdup(fsm->buffer); }
	break;
	case 11:
#line 61 "usage.rl"
	{ fsm->opt->target_file = strdup(fsm->buffer); }
	break;
	case 12:
#line 62 "usage.rl"
	{ fsm->opt->umdiff_file = strdup(fsm->buffer); }
	break;
#line 303 "usage.rl.h"
		}
	}

_again:
	if (  fsm->cs == 0 )
		goto _out;
	if ( ++p != pe )
		goto _resume;
	_test_eof: {}
	_out: {}
	}

#line 91 "usage.rl"
}

static int params_finish(struct params *fsm)
{
    if (fsm->cs == params_error)
        return -1;
    if (fsm->cs >= params_first_final)
        return 1;
    return 0;
}

static void show_usage()
{
    fprintf(stdout, "umdiff - Generate patch files for use with umd-livepatch\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Usage:\n");
    fprintf(stdout, "  umdiff delta <source_file> <target_file> <output_file>\n");
    fprintf(stdout, "  umdiff patch [--load-mode MODE] <source_file> <umdiff_file> <output_file>\n");
    fprintf(stdout, "  umdiff from-rdiff <rdiff_file> <output_file>\n");
    fprintf(stdout, "  umdiff --help\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Options:\n");
    fprintf(stdout, "  --load-mode=MODE  How to load the UMDiff file into memory [default: 0]\n");
    fprintf(stdout, "                    0: Load the file header into memory, read the rest ad-hoc\n");
    fprintf(stdout, "                    1: Load all commands into memory, read patch data ad-hoc\n");
    fprintf(stdout, "                    2: Load everything into memory\n");
    fprintf(stdout, "  -h --help         Show this message and exit\n");
    fprintf(stdout, "\n");
    fprintf(stdout, "Commands:\n");
    fprintf(stdout, "  delta             Generate a UMDiff file from the delta between source_file\n");
    fprintf(stdout, "                    and target_file, and write it to output_file.\n");
    fprintf(stdout, "  patch             Patch source_file using umdiff_file, and write the result\n");
    fprintf(stdout, "                    to output_file.\n");
    fprintf(stdout, "  from-rdiff        Convert an existing rdiff delta in rdiff_file into a\n");
    fprintf(stdout, "                    UMDiff file, and write it to output_file.\n");
    fprintf(stdout, "\n");
}

static int parse_options(int argc, char **argv, options_t* options)
{
    int a;
    struct params params;

    params_init(&params, options);
    for (a = 1; a < argc; a++ )
        params_execute(&params, argv[a], strlen(argv[a]) + 1);
    if (params_finish(&params) != 1)
    {
        fprintf(stderr, "Error processing arguments\n");
        show_usage();
        return -1;
    }

    return 0;
}

