#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <malloc.h>
#include <unistd.h>
#include <dirent.h>
#include <libgen.h>
#include <sys/stat.h>
#include <onnx.h>
#include <locale.h>


#undef PATH_MAX
#define PATH_MAX 128

#define STDIO_IN_MAX 256

#define FLOAT_EPSILON		(1e-3)

static int onnx_tensor_equal(struct onnx_tensor_t * a, struct onnx_tensor_t * b)
{
	int result = 0;
	size_t i;

	if(!a || !b)
		return 0;
	if(a->type != b->type)
		return 0;
	if(a->ndim != b->ndim)
		return 0;
	if(a->ndata != b->ndata)
		return 0;
	if(a->ndim > 0)
	{
		if(memcmp(a->dims, b->dims, sizeof(int) * a->ndim) != 0)
			return 0;
	}
	switch(a->type)
	{
	case ONNX_TENSOR_TYPE_BOOL:
	case ONNX_TENSOR_TYPE_INT8:
	case ONNX_TENSOR_TYPE_INT16:
	case ONNX_TENSOR_TYPE_INT32:
	case ONNX_TENSOR_TYPE_INT64:
	case ONNX_TENSOR_TYPE_UINT8:
	case ONNX_TENSOR_TYPE_UINT16:
	case ONNX_TENSOR_TYPE_UINT32:
	case ONNX_TENSOR_TYPE_UINT64:
		if(memcmp(a->datas, b->datas, a->ndata * onnx_tensor_type_sizeof(a->type)) == 0)
			result = 1;
		break;
	case ONNX_TENSOR_TYPE_BFLOAT16:
		{
			uint16_t * p = (uint16_t *)a->datas;
			uint16_t * q = (uint16_t *)b->datas;
			for(i = 0; i < a->ndata; i++)
			{
				if(fabsf(bfloat16_to_float32(p[i]) - bfloat16_to_float32(q[i])) > FLOAT_EPSILON)
					break;
			}
			if(i == a->ndata)
				result = 1;
		}
		break;
	case ONNX_TENSOR_TYPE_FLOAT16:
		{
			uint16_t * p = (uint16_t *)a->datas;
			uint16_t * q = (uint16_t *)b->datas;
			for(i = 0; i < a->ndata; i++)
			{
				if(fabsf(float16_to_float32(p[i]) - float16_to_float32(q[i])) > FLOAT_EPSILON)
					break;
			}
			if(i == a->ndata)
				result = 1;
		}
		break;
	case ONNX_TENSOR_TYPE_FLOAT32:
		{
			float * p = (float *)a->datas;
			float * q = (float *)b->datas;
			for(i = 0; i < a->ndata; i++)
			{
				if(fabsf(p[i] - q[i]) > FLOAT_EPSILON)
					break;
			}
			if(i == a->ndata)
				result = 1;
		}
		break;
	case ONNX_TENSOR_TYPE_FLOAT64:
		{
			double * p = (double *)a->datas;
			double * q = (double *)b->datas;
			for(i = 0; i < a->ndata; i++)
			{
				if(fabs(p[i] - q[i]) > FLOAT_EPSILON)
					break;
			}
			if(i == a->ndata)
				result = 1;
		}
		break;
	case ONNX_TENSOR_TYPE_COMPLEX64:
		{
			float * p = (float *)a->datas;
			float * q = (float *)b->datas;
			for(i = 0; i < a->ndata * 2; i++)
			{
				if(fabsf(p[i] - q[i]) > FLOAT_EPSILON)
					break;
			}
			if(i == a->ndata * 2)
				result = 1;
		}
		break;
	case ONNX_TENSOR_TYPE_COMPLEX128:
		{
			double * p = (double *)a->datas;
			double * q = (double *)b->datas;
			for(i = 0; i < a->ndata * 2; i++)
			{
				if(fabs(p[i] - q[i]) > FLOAT_EPSILON)
					break;
			}
			if(i == a->ndata * 2)
				result = 1;
		}
		break;
	case ONNX_TENSOR_TYPE_STRING:
		{
			char ** p = (char **)a->datas;
			char ** q = (char **)b->datas;
			for(i = 0; i < a->ndata; i++)
			{
				if(p[i] && q[i] && (strcmp(p[i], q[i]) != 0))
					break;
			}
			if(i == a->ndata)
				result = 1;
		}
		break;
	default:
		break;
	}
	return result;
}

static void testcase(const char * path, struct onnx_resolver_t ** r, int rlen)
{
	struct onnx_context_t * ctx;
	struct onnx_tensor_t * t, * o;
	struct stat st;
	char data_set_path[PATH_MAX];
	char tmp[PATH_MAX * 2];
	int data_set_index;
	int ninput, noutput;
	int okay;
	int len;

	sprintf(tmp, "%s/%s", path, "model.onnx");
	ctx = onnx_context_alloc_from_file(tmp, r, rlen);

	if(ctx)
	{
		data_set_index = 0;
		while(1)
		{
			sprintf(data_set_path, "%s/test_data_set_%d", path, data_set_index);
			if((lstat(data_set_path, &st) != 0) || !S_ISDIR(st.st_mode))
				break;
			ninput = 0;
			noutput = 0;
			okay = 0;
			while(1)
			{
				sprintf(tmp, "%s/input_%d.pb", data_set_path, ninput);
				if((lstat(tmp, &st) != 0) || !S_ISREG(st.st_mode))
					break;
				if(ninput > ctx->model->graph->n_input)
					break;
				t = onnx_tensor_search(ctx, ctx->model->graph->input[ninput]->name);
				o = onnx_tensor_alloc_from_file(tmp);
				onnx_tensor_apply(t, o->datas, o->ndata * onnx_tensor_type_sizeof(o->type));
				onnx_tensor_free(o);
				okay++;
				ninput++;

			}
			onnx_run(ctx);
			while(1)
			{
				sprintf(tmp, "%s/output_%d.pb", data_set_path, noutput);
				if((lstat(tmp, &st) != 0) || !S_ISREG(st.st_mode))
					break;
				if(noutput > ctx->model->graph->n_output)
					break;
				t = onnx_tensor_search(ctx, ctx->model->graph->output[noutput]->name);
				o = onnx_tensor_alloc_from_file(tmp);
				if(onnx_tensor_equal(t, o))
					okay++;
				onnx_tensor_free(o);
				noutput++;
			}

			len = printf("[%s](test_data_set_%d)", path, data_set_index);
			printf("%*s\r\n", 100 + 12 - 6 - len, ((ninput + noutput == okay) && (okay > 0)) ? "\033[42;37m[OKAY]\033[0m" : "\033[41;37m[FAIL]\033[0m");
			data_set_index++;
			break;
		}
		onnx_context_free(ctx);
	}
	else
	{
		len = printf("[%s]", path);
		printf("%*s\r\n", 100 + 12 - 6 - len, "\033[41;37m[FAIL]\033[0m");
	}
}



static void cli_free(char *in){
	free(in);
}
static char *cli_new(size_t size){
	return (char *) malloc(size);
}

static char *cli_input(char *in, size_t size){

	int retval = 0;
	int rc;
	int got_eof = 0;
	size_t idx = 0;

	puts("\n$> ");
	memset((void*)in,0,size);
	
	while (!got_eof) {

		for (;;) {
			int c = getchar();
			putchar(c);

			if (c == EOF) {
				got_eof = 1;
				break;
			} else if (c == '\n' || c == '\r') {
				got_eof = 1;
				break;
			} else if (idx >= size) {
				//fprintf(stderr, "line too long\n");
				//fflush(stderr);
				//retval = -1;
				break;
			} else {
				in[idx++] = (char) c;
			}
		}

	}

	return in;

}

int main(int argc, char * argv[])
{

	struct hmap_t * m;
	struct hmap_entry_t * e;
	struct dirent * d;
	struct stat st;
	char path[PATH_MAX];
	DIR * dir;

	setlocale(LC_ALL, "C");	

	for(;;){

		if(cli_input(path,STDIO_IN_MAX)){
			if((lstat(path, &st) == 0) && S_ISDIR(st.st_mode))
			{
				m = hmap_alloc(0);
				if((dir = opendir(path)) != NULL)
				{
					printf("opendir=%s\r\n",path );
					while((d = readdir(dir)) != NULL)
					{
						printf("readdir=%s\r\n",d->d_name );
						if((lstat(d->d_name, &st) == 0) && S_ISDIR(st.st_mode))
						{
							if(strcmp(".", d->d_name) == 0)
								continue;
							if(strcmp("..", d->d_name) == 0)
								continue;
							hmap_add(m, d->d_name, NULL);
						}
					}
					closedir(dir);
				}
				hmap_sort(m);
				hmap_for_each_entry(e, m)
				{
					//testcase(e->key, NULL, 0);
				}
				hmap_free(m, NULL);
			}
		}

	}

	return 0;
}
