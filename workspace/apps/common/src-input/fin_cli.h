#if !defined(FIN_API_CLI_H_INCLUDED)
#define FIN_API_CLI_H_INCLUDED

#define STDIO_IN_MAX 256
#define STDIO_CMD_MAX 25
#define STDIO_CMD_ARGS_MAX 6

typedef int (*fin_cli_callback_t)(int args, char *argv[]);

typedef	struct fin_cli_cmd{
	char *name;
	fin_cli_callback_t callback;
	struct fin_cli_cmd *next;
} fin_cli_cmd_t;

typedef struct fin_cli{
	char *in;
	int head;
	int tail;
	int count;
	cli_cmd_t *cmds;
} fin_cli_t;


void fin_cli_free(fin_cli_t *ctx);
fin_cli_t *fin_cli_new(void);
void fin_cli_input(fin_cli_t *ctx, char c);
int fin_cli_register(fin_cli_t *ctx, const char *name, fin_cli_callback_t func);

#endif  /* FIN_API_CLI_H_INCLUDED */
