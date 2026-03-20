/*
 * Copyright (c) 2019, NVIDIA CORPORATION. All rights reserved.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
 * DEALINGS IN THE SOFTWARE.
 */

#include "PyTensorNet.h"
#include "PyImageNet.h"

#include "imageNet.h"
#include "logging.h"

#include "../../utils/python/bindings/PyCUDA.h"


// 这是 Python 侧 imageNet 对象在 C/C++ 中对应的实际内存结构。
// `typedef struct { ... } PyImageNet_Object;` 是 C 风格写法，
// 作用等价于“定义一个结构体类型，并把这个类型命名为 PyImageNet_Object”。
typedef struct {
    // `base` 放在第一个成员，用来复用 PyTensorNet_Object 的公共字段。
    // 这是一种 C 风格的“继承”写法：子对象的起始内存布局与父对象一致，
    // 因此 Python 绑定层可以把它当作更通用的 TensorNet 对象来处理。
    PyTensorNet_Object base;

    // `net` 指向真正执行图像分类的 C++ imageNet 实例。
    // Python 中创建的 imageNet 对象，本质上就是持有这个底层指针。
    imageNet* net;
} PyImageNet_Object;


#define DOC_IMAGENET "Image Recognition DNN - classifies an image\n\n" \
				 "Examples (jetson-inference/python/examples)\n" \
				 "     imagenet.py\n" \
				 "     my-recognition.py\n" \
				 "__init__(...)\n" \
				 "     Loads an image recognition model.\n\n" \
				 "     Parameters:\n" \
				 "       network (string) -- name of a built-in network to use,\n" \
				 "                           see below for available options.\n\n" \
				 "       argv (strings) -- command line arguments passed to imageNet,\n" \
				 "                         see below for available options.\n\n" \
				 "     Extended parameters for loading custom models:\n" \
				 "       model (string) -- path to self-trained ONNX model to load.\n\n" \
				 "       labels (string) -- path to labels.txt file (optional)\n\n" \
				 "       input_blob (string) -- name of the input layer of the model.\n\n" \
				 "       output_blob (string) -- name of the output layer of the model.\n\n" \
 				 IMAGENET_USAGE_STRING

// 初始化 Python 侧的 imageNet 对象，并加载对应的分类网络
static int PyImageNet_Init( PyImageNet_Object* self, PyObject *args, PyObject *kwds )
{
	LogDebug(LOG_PY_INFERENCE "PyImageNet_Init()\n");
	
	// 解析 Python 传入的位置参数和关键字参数
	PyObject* argList = NULL;
	
	const char* network     = "googlenet";
	const char* model       = NULL;
	const char* labels      = NULL;
	const char* input_blob  = IMAGENET_DEFAULT_INPUT;
	const char* output_blob = IMAGENET_DEFAULT_OUTPUT;
	
	static char* kwlist[] = {"network", "argv", "model", "labels", "input_blob", "output_blob", NULL};

	if( !PyArg_ParseTupleAndKeywords(args, kwds, "|sOssss", kwlist, &network, &argList, &model, &labels, &input_blob, &output_blob))
		return -1;

	// 如果提供了 argv，则按命令行参数方式创建设备网络；
	// 否则使用内置网络名或自定义模型路径进行加载。
	if( argList != NULL && PyList_Check(argList) && PyList_Size(argList) > 0 )
	{
		LogDebug(LOG_PY_INFERENCE "imageNet loading network using argv command line params\n");

		// 将 Python 的字符串列表转换成 C 风格 argv
		const size_t argc = PyList_Size(argList);

		if( argc == 0 )
		{
			PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet.__init()__ argv list was empty");
			return -1;
		}

		char** argv = (char**)malloc(sizeof(char*) * argc);

		if( !argv )
		{
			PyErr_SetString(PyExc_MemoryError, LOG_PY_INFERENCE "imageNet.__init()__ failed to malloc memory for argv list");
			return -1;
		}

		for( size_t n=0; n < argc; n++ )
		{
			PyObject* item = PyList_GetItem(argList, n);
			
			if( !PyArg_Parse(item, "s", &argv[n]) )
			{
				PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet.__init()__ failed to parse argv list");
				return -1;
			}

			LogDebug(LOG_PY_INFERENCE "imageNet.__init__() argv[%zu] = '%s'\n", n, argv[n]);
		}

		// 释放 GIL，避免加载网络时阻塞其它 Python 线程
		Py_BEGIN_ALLOW_THREADS
		self->net = imageNet::Create(argc, argv);
		Py_END_ALLOW_THREADS
		
		// argv 仅保存指针数组本身，这里释放数组容器即可
		free(argv);
	}
	else
	{
		// 按内置网络名或自定义 ONNX 模型参数加载分类网络
		Py_BEGIN_ALLOW_THREADS
		self->net = imageNet::Create(NULL, model != NULL ? model : network, NULL, labels, input_blob, output_blob);
		Py_END_ALLOW_THREADS
	}

	// 确认网络已成功创建
	if( !self->net )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet failed to load network");
		return -1;
	}

	// 同步设置基类中的 net 指针，供 PyTensorNet 的共用逻辑访问。
	// 由于 `base` 是第一个成员，这里相当于把“父类部分”也一并初始化好。
	self->base.net = self->net;
	return 0;
}


// 释放 Python 对象以及对应的底层网络资源
static void PyImageNet_Dealloc( PyImageNet_Object* self )
{
	LogDebug(LOG_PY_INFERENCE "PyImageNet_Dealloc()\n");

	// 先销毁底层推理对象
	SAFE_DELETE(self->net);
	
	// 再释放 Python 对象本身
	Py_TYPE(self)->tp_free((PyObject*)self);
}

// 不是“功能代码”，而是给 Python 绑定函数写文档字符串（docstring），通常用于 pybind11 / Python C++ 扩展接口。
// C/C++ 预处理宏，作用是：把一大段字符串定义成一个“常量别名”等价于：const char* DOC_CLASSIFY = "...";但宏是在编译前替换，不是变量。
// 字符串拼接机制：这段字符串就是给 Python 用户看的说明，
#define DOC_CLASSIFY "Classify an image and return the object's class and confidence.\n\n" \
				 "Parameters:\n" \
				 "  image  (capsule) -- CUDA memory capsule\n" \
				 "  width  (int) -- width of the image (in pixels)\n" \
				 "  height (int) -- height of the image (in pixels)\n\n" \
				 "  topK   (int) -- the number of predictions to return (sorted by confidence)\n" \
				 "                  if topK is 0, then all valid predictions will be returned\n\n" \
				 "Returns:\n" \
				 "  (int, float) -- tuple containing the object's class index and confidence\n" \
				 "                  if topK is set, then a list of these tuples will be returned"

// 对输入图像执行分类，返回单个结果或 Top-K 结果列表
static PyObject* PyImageNet_Classify( PyImageNet_Object* self, PyObject* args, PyObject *kwds )
{
	if( !self || !self->net )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet invalid object instance");
		return NULL;
	}
	
	// 解析图像 capsule、尺寸、像素格式和 topK
	PyObject* capsule = NULL;

	int width = 0;
	int height = 0;
	int topK = -1;
	
	const char* format_str = "rgba32f";
	static char* kwlist[] = {"image", "width", "height", "format", "topK", NULL};

	if( !PyArg_ParseTupleAndKeywords(args, kwds, "O|iisi", kwlist, &capsule, &width, &height, &format_str, &topK))
		return NULL;

	// 将字符串格式转换为内部 imageFormat 枚举
	imageFormat format = imageFormatFromStr(format_str);

	// 从 CUDA capsule 中取出图像指针，并在需要时回填宽高/格式
	void* ptr = PyCUDA_GetImage(capsule, &width, &height, &format);

	if( !ptr )
		return NULL;

	// topK < 0 时走单标签分类接口，返回 (classID, confidence)
	if( topK < 0 )
	{
		float confidence = 0.0f;
		int img_class = -1;
		
		// 推理期间释放 GIL，减少对 Python 运行时的阻塞
		Py_BEGIN_ALLOW_THREADS
		img_class = self->net->Classify(ptr, width, height, format, &confidence);
		Py_END_ALLOW_THREADS
		
		if( img_class < -1 )
		{
			PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet.Classify() encountered an error classifying the image");
			return NULL;
		}

		// 构造 Python 返回值
		PyObject* pyClass = PYLONG_FROM_LONG(img_class);
		PyObject* pyConf  = PyFloat_FromDouble(confidence);

		// 返回 (类别索引, 置信度) 二元组
		PyObject* tuple = PyTuple_Pack(2, pyClass, pyConf);

		Py_DECREF(pyClass);
		Py_DECREF(pyConf);

		return tuple;
	}
	else
	{
		// topK >= 0 时收集多个预测结果，并按置信度排序返回
		std::vector<std::pair<uint32_t, float>> preds;
		int result = 0;
		
		// 使用批量输出版本的 Classify 获取 Top-K 结果
		Py_BEGIN_ALLOW_THREADS
		result = self->net->Classify(ptr, width, height, format, preds, topK);
		Py_END_ALLOW_THREADS
	
		if( result < -1 )
		{
			PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet.Classify() encountered an error classifying the image");
			return NULL;
		}
		
		// 构造 [(classID, confidence), ...] 形式的 Python 列表
		PyObject* list = PyList_New(preds.size());
		
		for( uint32_t n=0; n < preds.size(); n++ )
		{
			PyObject* pyClass = PYLONG_FROM_LONG(preds[n].first);
			PyObject* pyConf  = PyFloat_FromDouble(preds[n].second);
			PyObject* pyTuple = PyTuple_Pack(2, pyClass, pyConf);

			Py_DECREF(pyClass);
			Py_DECREF(pyConf);
			
			PyList_SET_ITEM(list, n, pyTuple);
		}
		
		return list;
	}
}


#define DOC_GET_NETWORK_NAME "Return the name of the built-in network used by the model.\n\n" \
					    "Parameters:  (none)\n\n" \
					    "Returns:\n" \
					    "  (string) -- name of the network (e.g. 'googlenet', 'alexnet')\n" \
					    "              or 'custom' if using a custom-loaded model"

// 返回当前模型对应的内置网络名称；自定义模型通常返回 custom
static PyObject* PyImageNet_GetNetworkName( PyImageNet_Object* self )
{
	if( !self || !self->net )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet invalid object instance");
		return NULL;
	}
	
	return Py_BuildValue("s", self->net->GetNetworkName());
}


#define DOC_GET_NUM_CLASSES "Return the number of object classes that this network model is able to classify.\n\n" \
				 	   "Parameters:  (none)\n\n" \
					   "Returns:\n" \
					   "  (int) -- number of object classes that the model supports"

// 返回模型可识别的类别总数
static PyObject* PyImageNet_GetNumClasses( PyImageNet_Object* self )
{
	if( !self || !self->net )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet invalid object instance");
		return NULL;
	}

	return PYLONG_FROM_UNSIGNED_LONG(self->net->GetNumClasses());
}


#define DOC_GET_CLASS_DESC "Return the class description for the given object class.\n\n" \
				 	  "Parameters:\n" \
					  "  (int) -- index of the class, between [0, GetNumClasses()]\n\n" \
					  "Returns:\n" \
					  "  (string) -- the text description of the object class"

// 根据类别索引获取该类别的人类可读描述
PyObject* PyImageNet_GetClassDesc( PyImageNet_Object* self, PyObject* args )
{
	if( !self || !self->net )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet invalid object instance");
		return NULL;
	}
	
	int classIdx = 0;

	if( !PyArg_ParseTuple(args, "i", &classIdx) )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet failed to parse arguments");
		return NULL;
	}
		
	if( /*classIdx < 0 ||*/ classIdx >= (int)self->net->GetNumClasses() )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet requested class index is out of bounds");
		return NULL;
	}

	return Py_BuildValue("s", self->net->GetClassDesc(classIdx));
}


#define DOC_GET_CLASS_SYNSET "Return the synset data category string for the given class.\n" \
					    "The synset generally maps to the class training data folder.\n\n" \
				 	    "Parameters:\n" \
					    "  (int) -- index of the class, between [0, GetNumClasses()]\n\n" \
					    "Returns:\n" \
					    "  (string) -- the synset of the class, typically 9 characters long" 

// 根据类别索引获取 synset 标识，通常对应训练数据目录名
PyObject* PyImageNet_GetClassSynset( PyImageNet_Object* self, PyObject* args )
{
	if( !self || !self->net )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet invalid object instance");
		return NULL;
	}
	
	int classIdx = 0;

	if( !PyArg_ParseTuple(args, "i", &classIdx) )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet failed to parse arguments");
		return NULL;
	}
		
	if( /*classIdx < 0 ||*/ classIdx >= (int)self->net->GetNumClasses() )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet requested class index is out of bounds");
		return NULL;
	}

	return Py_BuildValue("s", self->net->GetClassSynset(classIdx));
}


#define DOC_GET_THRESHOLD  "Return the minimum confidence threshold for classification.\n\n" \
					  "Parameters:  (none)\n\n" \
					  "Returns:\n" \
					  "  (float) -- the confidence threshold for classification"

// 获取当前分类最小置信度阈值
static PyObject* PyImageNet_GetThreshold( PyImageNet_Object* self )
{
	if( !self || !self->net )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet invalid object instance");
		return NULL;
	}

	return PyFloat_FromDouble(self->net->GetThreshold());
}


#define DOC_SET_THRESHOLD  "Set the minimum confidence threshold for classification.\n\n" \
					  "Parameters:\n" \
					  "  (float) -- confidence threshold\n\n" \
					  "Returns:  (none)"

// 设置分类最小置信度阈值
PyObject* PyImageNet_SetThreshold( PyImageNet_Object* self, PyObject* args )
{
	if( !self || !self->net )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet invalid object instance");
		return NULL;
	}
	
	float threshold = 0.0f;

	if( !PyArg_ParseTuple(args, "f", &threshold) )
		return NULL;

	self->net->SetThreshold(threshold);
	Py_RETURN_NONE;
}


#define DOC_GET_SMOOTHING  "Return the temporal smoothing factor applied to the results.\n\n" \
					  "Parameters:  (none)\n\n" \
					  "Returns:\n" \
					  "  (float) -- the weight between [0,1] or the number of frames in the smoothing window"

// 获取分类结果的时间平滑参数
static PyObject* PyImageNet_GetSmoothing( PyImageNet_Object* self )
{
	if( !self || !self->net )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet invalid object instance");
		return NULL;
	}

	return PyFloat_FromDouble(self->net->GetSmoothing());
}


#define DOC_SET_SMOOTHING  "Set the temporal smoothing factor applied to the results.\n\n" \
					  "Parameters:\n" \
					  "  (float) -- A weight between [0,1] that's placed on the latest confidence values,\n" \
					  "             or the smoothing window as a number of frames (where the weight will be 1/N)\n" \
					  "             Setting this to 0 or 1 will disable smoothing and use the unfiltered outputs\n\n" \
					  "Returns:  (none)"

// 设置分类结果的时间平滑参数
PyObject* PyImageNet_SetSmoothing( PyImageNet_Object* self, PyObject* args )
{
	if( !self || !self->net )
	{
		PyErr_SetString(PyExc_Exception, LOG_PY_INFERENCE "imageNet invalid object instance");
		return NULL;
	}
	
	float factor = 0.0f;

	if( !PyArg_ParseTuple(args, "f", &factor) )
		return NULL;

	self->net->SetSmoothing(factor);
	Py_RETURN_NONE;
}


#define DOC_USAGE_STRING     "Return the command line parameters accepted by __init__()\n\n" \
					    "Parameters:  (none)\n\n" \
					    "Returns:\n" \
					    "  (string) -- usage string documenting command-line options\n"

// 返回 imageNet 支持的命令行参数说明
static PyObject* PyImageNet_Usage( PyImageNet_Object* self )
{
	return Py_BuildValue("s", imageNet::Usage());
}

//-------------------------------------------------------------------------------
// Python 类型对象，具体字段在注册阶段填充。
// CPython 会按这个类型描述去分配 `sizeof(PyImageNet_Object)` 大小的对象内存。
static PyTypeObject pyImageNet_Type = 
{
    PyVarObject_HEAD_INIT(NULL, 0)
};

// 暴露给 Python 的成员函数表
static PyMethodDef pyImageNet_Methods[] = 
{
	{ "Classify", (PyCFunction)PyImageNet_Classify, METH_VARARGS|METH_KEYWORDS, DOC_CLASSIFY},
	{ "GetNetworkName", (PyCFunction)PyImageNet_GetNetworkName, METH_NOARGS, DOC_GET_NETWORK_NAME},
     { "GetNumClasses", (PyCFunction)PyImageNet_GetNumClasses, METH_NOARGS, DOC_GET_NUM_CLASSES},
	{ "GetClassLabel", (PyCFunction)PyImageNet_GetClassDesc, METH_VARARGS, DOC_GET_CLASS_DESC},
	{ "GetClassDesc", (PyCFunction)PyImageNet_GetClassDesc, METH_VARARGS, DOC_GET_CLASS_DESC},
	{ "GetClassSynset", (PyCFunction)PyImageNet_GetClassSynset, METH_VARARGS, DOC_GET_CLASS_SYNSET},
	{ "GetThreshold", (PyCFunction)PyImageNet_GetThreshold, METH_NOARGS, DOC_GET_THRESHOLD},
	{ "SetThreshold", (PyCFunction)PyImageNet_SetThreshold, METH_VARARGS, DOC_SET_THRESHOLD},
	{ "GetSmoothing", (PyCFunction)PyImageNet_GetSmoothing, METH_NOARGS, DOC_GET_SMOOTHING},
	{ "SetSmoothing", (PyCFunction)PyImageNet_SetSmoothing, METH_VARARGS, DOC_SET_SMOOTHING},
	{ "Usage", (PyCFunction)PyImageNet_Usage, METH_NOARGS|METH_STATIC, DOC_USAGE_STRING},
	{NULL}  /* Sentinel */
};

// 将 imageNet 类型注册到当前 Python 模块
bool PyImageNet_Register( PyObject* module )
{
	if( !module )
		return false;
	
	// 绑定类型名称、大小、继承关系和方法表
	pyImageNet_Type.tp_name		= PY_INFERENCE_MODULE_NAME ".imageNet";
	pyImageNet_Type.tp_basicsize	= sizeof(PyImageNet_Object);
	pyImageNet_Type.tp_flags		= Py_TPFLAGS_DEFAULT | Py_TPFLAGS_BASETYPE;
	pyImageNet_Type.tp_base		= PyTensorNet_Type();
	pyImageNet_Type.tp_methods	= pyImageNet_Methods;
	pyImageNet_Type.tp_new		= NULL; /*PyImageNet_New;*/
	pyImageNet_Type.tp_init		= (initproc)PyImageNet_Init;
	pyImageNet_Type.tp_dealloc	= (destructor)PyImageNet_Dealloc;
	pyImageNet_Type.tp_doc		= DOC_IMAGENET;
	 
	// 让 CPython 完成类型对象的最终初始化
	if( PyType_Ready(&pyImageNet_Type) < 0 )
	{
		LogError(LOG_PY_INFERENCE "imageNet PyType_Ready() failed\n");
		return false;
	}
	
	Py_INCREF(&pyImageNet_Type);
    
	// 把类型对象挂到模块上，供 Python 代码通过 jetson.inference.imageNet 访问
	if( PyModule_AddObject(module, "imageNet", (PyObject*)&pyImageNet_Type) < 0 )
	{
		LogError(LOG_PY_INFERENCE "imageNet PyModule_AddObject('imageNet') failed\n");
		return false;
	}
	
	return true;
}
