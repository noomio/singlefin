#include <onnx.h>

static int Sub_init(struct onnx_node_t * n)
{
	if((n->ninput == 2) && (n->noutput == 1))
		return 1;
	return 0;
}

static int Sub_exit(struct onnx_node_t * n)
{
	return 1;
}

static int Sub_reshape(struct onnx_node_t * n)
{
	struct onnx_tensor_t * y = n->outputs[0];
	struct onnx_tensor_t * a = n->inputs[0];
	struct onnx_tensor_t * b = n->inputs[1];

	return onnx_tensor_reshape_multi_broadcast(y, a, b, a->type);
}

static void Sub_int32(struct onnx_node_t * n)
{
	struct onnx_tensor_t * y = n->outputs[0];
	struct onnx_tensor_t * a = n->inputs[0];
	struct onnx_tensor_t * b = n->inputs[1];
	int32_t * py = (int32_t *)y->datas;
	int32_t * pa;
	int32_t * pb;

	for(size_t i = 0, l = y->ndata; i < l; i++)
	{
		pa = onnx_tensor_broadcast_map_address(a, y, i);
		pb = onnx_tensor_broadcast_map_address(b, y, i);
		py[i] = *pa - *pb;
	}
}

static void Sub_int64(struct onnx_node_t * n)
{
	struct onnx_tensor_t * y = n->outputs[0];
	struct onnx_tensor_t * a = n->inputs[0];
	struct onnx_tensor_t * b = n->inputs[1];
	int64_t * py = (int64_t *)y->datas;
	int64_t * pa;
	int64_t * pb;

	for(size_t i = 0, l = y->ndata; i < l; i++)
	{
		pa = onnx_tensor_broadcast_map_address(a, y, i);
		pb = onnx_tensor_broadcast_map_address(b, y, i);
		py[i] = *pa - *pb;
	}
}

static void Sub_uint32(struct onnx_node_t * n)
{
	struct onnx_tensor_t * y = n->outputs[0];
	struct onnx_tensor_t * a = n->inputs[0];
	struct onnx_tensor_t * b = n->inputs[1];
	uint32_t * py = (uint32_t *)y->datas;
	uint32_t * pa;
	uint32_t * pb;

	for(size_t i = 0, l = y->ndata; i < l; i++)
	{
		pa = onnx_tensor_broadcast_map_address(a, y, i);
		pb = onnx_tensor_broadcast_map_address(b, y, i);
		py[i] = *pa - *pb;
	}
}

static void Sub_uint64(struct onnx_node_t * n)
{
	struct onnx_tensor_t * y = n->outputs[0];
	struct onnx_tensor_t * a = n->inputs[0];
	struct onnx_tensor_t * b = n->inputs[1];
	uint64_t * py = (uint64_t *)y->datas;
	uint64_t * pa;
	uint64_t * pb;

	for(size_t i = 0, l = y->ndata; i < l; i++)
	{
		pa = onnx_tensor_broadcast_map_address(a, y, i);
		pb = onnx_tensor_broadcast_map_address(b, y, i);
		py[i] = *pa - *pb;
	}
}

static void Sub_bfloat16(struct onnx_node_t * n)
{
	struct onnx_tensor_t * y = n->outputs[0];
	struct onnx_tensor_t * a = n->inputs[0];
	struct onnx_tensor_t * b = n->inputs[1];
	uint16_t * py = (uint16_t *)y->datas;
	uint16_t * pa;
	uint16_t * pb;

	for(size_t i = 0, l = y->ndata; i < l; i++)
	{
		pa = onnx_tensor_broadcast_map_address(a, y, i);
		pb = onnx_tensor_broadcast_map_address(b, y, i);
		py[i] = float32_to_bfloat16(bfloat16_to_float32(*pa) - bfloat16_to_float32(*pb));
	}
}

static void Sub_float16(struct onnx_node_t * n)
{
	struct onnx_tensor_t * y = n->outputs[0];
	struct onnx_tensor_t * a = n->inputs[0];
	struct onnx_tensor_t * b = n->inputs[1];
	uint16_t * py = (uint16_t *)y->datas;
	uint16_t * pa;
	uint16_t * pb;

	for(size_t i = 0, l = y->ndata; i < l; i++)
	{
		pa = onnx_tensor_broadcast_map_address(a, y, i);
		pb = onnx_tensor_broadcast_map_address(b, y, i);
		py[i] = float32_to_float16(float16_to_float32(*pa) - float16_to_float32(*pb));
	}
}

static void Sub_float32(struct onnx_node_t * n)
{
	struct onnx_tensor_t * y = n->outputs[0];
	struct onnx_tensor_t * a = n->inputs[0];
	struct onnx_tensor_t * b = n->inputs[1];
	float * py = (float *)y->datas;
	float * pa;
	float * pb;

	for(size_t i = 0, l = y->ndata; i < l; i++)
	{
		pa = onnx_tensor_broadcast_map_address(a, y, i);
		pb = onnx_tensor_broadcast_map_address(b, y, i);
		py[i] = *pa - *pb;
	}
}

static void Sub_float64(struct onnx_node_t * n)
{
	struct onnx_tensor_t * y = n->outputs[0];
	struct onnx_tensor_t * a = n->inputs[0];
	struct onnx_tensor_t * b = n->inputs[1];
	double * py = (double *)y->datas;
	double * pa;
	double * pb;

	for(size_t i = 0, l = y->ndata; i < l; i++)
	{
		pa = onnx_tensor_broadcast_map_address(a, y, i);
		pb = onnx_tensor_broadcast_map_address(b, y, i);
		py[i] = *pa - *pb;
	}
}

void resolver_default_op_Sub(struct onnx_node_t * n)
{
	if(n->opset >= 13)
	{
		switch(n->inputs[0]->type)
		{
		case ONNX_TENSOR_TYPE_INT32:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_int32;
			break;
		case ONNX_TENSOR_TYPE_INT64:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_int64;
			break;
		case ONNX_TENSOR_TYPE_UINT32:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_uint32;
			break;
		case ONNX_TENSOR_TYPE_UINT64:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_uint64;
			break;
		case ONNX_TENSOR_TYPE_BFLOAT16:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_bfloat16;
			break;
		case ONNX_TENSOR_TYPE_FLOAT16:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_float16;
			break;
		case ONNX_TENSOR_TYPE_FLOAT32:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_float32;
			break;
		case ONNX_TENSOR_TYPE_FLOAT64:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_float64;
			break;
		default:
			break;
		}
	}
	else if(n->opset >= 7)
	{
		switch(n->inputs[0]->type)
		{
		case ONNX_TENSOR_TYPE_INT32:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_int32;
			break;
		case ONNX_TENSOR_TYPE_INT64:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_int64;
			break;
		case ONNX_TENSOR_TYPE_UINT32:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_uint32;
			break;
		case ONNX_TENSOR_TYPE_UINT64:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_uint64;
			break;
		case ONNX_TENSOR_TYPE_FLOAT16:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_float16;
			break;
		case ONNX_TENSOR_TYPE_FLOAT32:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_float32;
			break;
		case ONNX_TENSOR_TYPE_FLOAT64:
			n->init = Sub_init;
			n->exit = Sub_exit;
			n->reshape = Sub_reshape;
			n->operator = Sub_float64;
			break;
		default:
			break;
		}
	}
	else if(n->opset >= 6)
	{
	}
	else if(n->opset >= 1)
	{
	}
}