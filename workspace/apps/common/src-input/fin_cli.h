#if !defined(FIN_API_CLI_H_INCLUDED)
#define FIN_API_CLI_H_INCLUDED

#define STDIO_IN_MAX 256
#define STDIO_CMD_MAX 25
#define STDIO_CMD_ARGS_MAX 6

typedef int (*cli_callback_t)(int args, char *argv[]);

typedef	struct cli_cmd{
	char *name;
	cli_callback_t callback;
	struct cli_cmd *next;
} cli_cmd_t;

typedef struct cli{
	char *in;
	int head;
	int tail;
	int count;
	cli_cmd_t *cmds;
} cli_t;


void cli_free(cli_t *ctx);
cli_t *cli_new(void);
void cli_input(cli_t *ctx, char c);
int cli_register(cli_t *ctx, const char *name, cli_callback_t func);

#endif  /* FIN_API_CLI_H_INCLUDED */