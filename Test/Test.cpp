/**
MMath - vector math library for 3D applications.
Released under the MIT License:

Copyright 2020 Trevor van Hoof

Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated documentation files (the "Software"), to deal in the Software without restriction, including without limitation the rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
**/
#include "Messages.h"
#include <MMath/Mat44.h>
#include <MMath/Quat.h>
#include <MMath/Math.h>
#include <MMath/SIMD.h>
#include <MMath/Friends.h>
#include <MMath/Enums.h>

// TODO: Let's do a python script to produce unit test values form Maya
// and then also match against OpenGL, and maybe glm later?
#include <windows.h>
#if 0
#include <gl/gl.h>
#pragma comment(lib, "opengl32.lib")
#endif

void DebugPrintMat44(const Mat44& m)
{
	Info("%.03f %.03f %.03f %.03f\n%.03f %.03f %.03f %.03f\n%.03f %.03f %.03f %.03f\n%.03f %.03f %.03f %.03f\n\n",
		m.m00, m.m01, m.m02, m.m03,
		m.m10, m.m11, m.m12, m.m13,
		m.m20, m.m21, m.m22, m.m23,
		m.m30, m.m31, m.m32, m.m33);
}

void DebugPrintQuat(const Quat& m)
{
	Info("%.03f %.03f %.03f %.03f\n\n", m.x, m.y, m.z, m.w);
}
void DebugPrintVec4(const __m128& v)
{
	Info("%.03f %.03f %.03f %.03f\n\n", v.m128_f32[0], v.m128_f32[1], v.m128_f32[2], v.m128_f32[3]);
}
void DebugPrintEuler(const __m128& v)
{
	Info("%.03f %.03f %.03f\n\n", v.m128_f32[0] * RAD2DEG, v.m128_f32[1] * RAD2DEG, v.m128_f32[2] * RAD2DEG);
}

#define RAPIDJSON_HAS_STDSTRING 1
#define RAPIDJSON_SSE42 1
#include "rapidjson/document.h"
#include "rapidjson/reader.h"
#include "rapidjson/writer.h"
#include "rapidjson/prettywriter.h"
#include "rapidjson/filereadstream.h"
#include "rapidjson/filewritestream.h"
#include "rapidjson/ostreamwrapper.h"
#include "rapidjson/istreamwrapper.h"
#include <fstream>
#include <iostream>
#include <vector>

struct UnitTestJSonHandler : public rapidjson::BaseReaderHandler<rapidjson::UTF8<>, UnitTestJSonHandler>
{
	static void run(const char* fp, const char* dst_fp)
	{
		std::ifstream stream(fp);
		rapidjson::IStreamWrapper fs(stream);
		rapidjson::Reader reader;
		UnitTestJSonHandler handler;
		handler.Init();
		reader.Parse(fs, handler);
		handler.Save(dst_fp);
	}
	
	void Save(const char* fp)
	{
		std::ofstream stream(fp);
		rapidjson::OStreamWrapper osw(stream);
		rapidjson::PrettyWriter<rapidjson::OStreamWrapper, rapidjson::UTF8<>, rapidjson::UTF8<>, rapidjson::CrtAllocator, rapidjson::kWriteNanAndInfFlag> writer(osw);
		outData.Accept(writer);
	}

	// SAX state
	// we know that the document root is the only object
	bool isInDocumentRoot = false;
	// so we can cache the keys as we find them and knovw they point to functions that we wish to test
	std::string currentAttribute;
	// we then know every value is a list containing either more lists, or floats
	// instead of tracking all that data, we'll push floats as they arrive and flush as we close any array
	// if floats is not empty, we can use them to run a test call
	std::vector<float> queue;
	// besides that we trace the 'list depth' to know how many nested lists in we are, this way based on the function name we can react when things happen at the wrong depth
	int stackDepth = 0;

	rapidjson::Document outData;
	rapidjson::Document::AllocatorType* pAllocator;

	std::vector<std::string*> alive; // rapidjson doesn't copy our strings so our constantly changing currentAttribute really messes up the serialization, instead let's push copies for new members
	
	void Push(const std::string& key, const Mat44& m)
	{
		rapidjson::Value list(rapidjson::kArrayType);
		list.PushBack(m.m00, *pAllocator);
		list.PushBack(m.m01, *pAllocator);
		list.PushBack(m.m02, *pAllocator);
		list.PushBack(m.m03, *pAllocator);
		list.PushBack(m.m10, *pAllocator);
		list.PushBack(m.m11, *pAllocator);
		list.PushBack(m.m12, *pAllocator);
		list.PushBack(m.m13, *pAllocator);
		list.PushBack(m.m20, *pAllocator);
		list.PushBack(m.m21, *pAllocator);
		list.PushBack(m.m22, *pAllocator);
		list.PushBack(m.m23, *pAllocator);
		list.PushBack(m.m30, *pAllocator);
		list.PushBack(m.m31, *pAllocator);
		list.PushBack(m.m32, *pAllocator);
		list.PushBack(m.m33, *pAllocator);
		std::string* heapCopy = new std::string(key);
		alive.push_back(heapCopy);
		outData.AddMember(rapidjson::StringRef(*heapCopy), list, *pAllocator);
	}

	void PushA(const std::string& key, const Mat44& m)
	{
		if(*alive[alive.size() - 1] !=  key) // first occurance
		// if (!outData.HasMember(key))
		{
			rapidjson::Value _parentList(rapidjson::kArrayType);
			std::string* heapCopy = new std::string(key);
			alive.push_back(heapCopy);
			outData.AddMember(rapidjson::StringRef(*heapCopy), _parentList, *pAllocator);
		}

		rapidjson::Value& parentList = outData[*alive[alive.size() - 1]];

		rapidjson::Value list(rapidjson::kArrayType);
		list.PushBack(m.m00, *pAllocator);
		list.PushBack(m.m01, *pAllocator);
		list.PushBack(m.m02, *pAllocator);
		list.PushBack(m.m03, *pAllocator);
		list.PushBack(m.m10, *pAllocator);
		list.PushBack(m.m11, *pAllocator);
		list.PushBack(m.m12, *pAllocator);
		list.PushBack(m.m13, *pAllocator);
		list.PushBack(m.m20, *pAllocator);
		list.PushBack(m.m21, *pAllocator);
		list.PushBack(m.m22, *pAllocator);
		list.PushBack(m.m23, *pAllocator);
		list.PushBack(m.m30, *pAllocator);
		list.PushBack(m.m31, *pAllocator);
		list.PushBack(m.m32, *pAllocator);
		list.PushBack(m.m33, *pAllocator);

		parentList.PushBack(list, *pAllocator);
	}

	void PushA(const std::string& key, const float& m)
	{
		if (*alive[alive.size() - 1] != key) // first occurance
		// if (!outData.HasMember(key))
		{
			rapidjson::Value _parentList(rapidjson::kArrayType);
			std::string* heapCopy = new std::string(key);
			alive.push_back(heapCopy);
			outData.AddMember(rapidjson::StringRef(*heapCopy), _parentList, *pAllocator);
		}

		rapidjson::Value& parentList = outData[*alive[alive.size() - 1]];

		parentList.PushBack(m, *pAllocator);
	}

	void PushA(const std::string& key, const __m128& m)
	{
		if (*alive[alive.size() - 1] != key) // first occurance
		// if (!outData.HasMember(key))
		{
			rapidjson::Value _parentList(rapidjson::kArrayType);
			std::string* heapCopy = new std::string(key);
			alive.push_back(heapCopy);
			outData.AddMember(rapidjson::StringRef(*heapCopy), _parentList, *pAllocator);
		}

		rapidjson::Value& parentList = outData[*alive[alive.size() - 1]];

		rapidjson::Value list(rapidjson::kArrayType);
		list.PushBack(m.m128_f32[0], *pAllocator);
		list.PushBack(m.m128_f32[1], *pAllocator);
		list.PushBack(m.m128_f32[2], *pAllocator);

		parentList.PushBack(list, *pAllocator);
	}

	void Init()
	{
		outData.SetObject(); // root obj
		pAllocator = &outData.GetAllocator();

		// add an identity matrix to the output
		Push("Mat44Identity", Mat44Identity());
	}

	void RunUnitTest()
	{
		const ERotateOrder roLUT[] = { // Maya to MMath mapping
			ERotateOrder::XYZ,
			ERotateOrder::YZX,
			ERotateOrder::ZXY,
			ERotateOrder::XZY,
			ERotateOrder::YXZ,
			ERotateOrder::ZYX,
		};
		
		// this function gets called at the end of every list as long as the queue is not empty
		// so based on the function name we can await until the list length matches the expected argument count and run the test
		if (currentAttribute == "Mat44Translate")
		{
			PushA(currentAttribute, Mat44Translate(queue[0], queue[1], queue[2]));
		}
		else if (currentAttribute == "Mat44RotateX")
		{
			PushA(currentAttribute, Mat44RotateX(queue[0]));
		}
		else if (currentAttribute == "Mat44RotateY")
		{
			PushA(currentAttribute, Mat44RotateY(queue[0]));
		}
		else if (currentAttribute == "Mat44RotateZ")
		{
			PushA(currentAttribute, Mat44RotateZ(queue[0]));
		}
		else if (currentAttribute == "Mat44Rotate")
		{
			ERotateOrder o = roLUT[(int)queue[3]];
			PushA(currentAttribute, Mat44Rotate(queue[0], queue[1], queue[2], o));
		}
		else if (currentAttribute == "Mat44Rotate2")
		{
			if (queue.size() != 4)
				return; // wait for rotate order arg
			ERotateOrder o = roLUT[(int)queue[3]];
			PushA(currentAttribute, Mat44Rotate2(_mm_set_ps(0.0f, queue[2], queue[1], queue[0]), o));
		}
		else if (currentAttribute == "Mat44Scale")
		{
			PushA(currentAttribute, Mat44Scale(queue[0], queue[1], queue[2]));
		}
		else if (currentAttribute == "Mat44Scale2")
		{
			PushA(currentAttribute, Mat44Scale2(_mm_set_ps(0.0f, queue[2], queue[1], queue[0])));
		}
		else if (currentAttribute == "Mat44TranslateRotate")
		{
			ERotateOrder o = roLUT[(int)queue[6]];
			PushA(currentAttribute, Mat44TranslateRotate(queue[0], queue[1], queue[2],
				queue[3], queue[4], queue[5], o));
		}
		else if (currentAttribute == "Mat44TranslateRotate2")
		{
			if (queue.size() != 7)
				return; // wait for rotate order arg
			ERotateOrder o = roLUT[(int)queue[6]];
			PushA(currentAttribute, Mat44TranslateRotate2(
				_mm_set_ps(0.0f, queue[2], queue[1], queue[0]),
				_mm_set_ps(0.0f, queue[5], queue[4], queue[3]),
				o));
		}
		else if (currentAttribute == "Mat44TRS")
		{
			ERotateOrder o = roLUT[(int)queue[9]];
			PushA(currentAttribute, Mat44TRS(queue[0], queue[1], queue[2],
				queue[3], queue[4], queue[5], queue[6], queue[7], queue[8], o));
		}
		else if (currentAttribute == "Mat44TRS2")
		{
			if (queue.size() != 10)
				return; // wait for rotate order arg
			ERotateOrder o = roLUT[(int)queue[9]];
			PushA(currentAttribute, Mat44TRS2(
				_mm_set_ps(0.0f, queue[2], queue[1], queue[0]),
				_mm_set_ps(0.0f, queue[5], queue[4], queue[3]),
				_mm_set_ps(0.0f, queue[8], queue[7], queue[6]),
				o));
		}
		else if (currentAttribute == "Mat44Mul")
		{
			if (queue.size() != 32)
				return; // wait for two matrix args
			Mat44 child, parent;
			CopyMemory(child.m, &queue[0], sizeof(Mat44));
			CopyMemory(parent.m, &queue[16], sizeof(Mat44));
			PushA(currentAttribute, Mat44Mul(child, parent));
		}
		else if (currentAttribute == "Mat44Inversed")
		{
			Mat44 m;
			CopyMemory(m.m, &queue[0], sizeof(Mat44));
			PushA(currentAttribute, Mat44Inversed(m));
		}
		else if (currentAttribute == "Mat44InversedFast")
		{
			Mat44 m;
			CopyMemory(m.m, &queue[0], sizeof(Mat44));
			PushA(currentAttribute, Mat44InversedFast(m));
		}
		else if (currentAttribute == "Mat44InversedFastNoScale")
		{
			Mat44 m;
			CopyMemory(m.m, &queue[0], sizeof(Mat44));
			PushA(currentAttribute, Mat44InversedFastNoScale(m));
		}
		else if (currentAttribute == "Mat44Transposed")
		{
			Mat44 m;
			CopyMemory(m.m, &queue[0], sizeof(Mat44));
			PushA(currentAttribute, Mat44Transposed(m));
		}
		else if (currentAttribute == "Mat44Determinant")
		{
			Mat44 m;
			CopyMemory(m.m, &queue[0], sizeof(Mat44));
			PushA(currentAttribute, Mat44Determinant(m));
		}
		else if (currentAttribute == "Mat44VectorTransform")
		{
			if (queue.size() != 19)
				return; // wait for matrix and vector arg
			Mat44 m;
			CopyMemory(m.m, &queue[0], sizeof(Mat44));
			__m128 v = _mm_set_ps(1.0f, queue[18], queue[17], queue[16]);
			PushA(currentAttribute, Mat44VectorTransform(m, v));
		}
		else if (currentAttribute == "Mat44Delta")
		{
			if (queue.size() != 32)
				return; // wait for two matrix args
			Mat44 child, parent;
			CopyMemory(child.m, &queue[0], sizeof(Mat44));
			CopyMemory(parent.m, &queue[16], sizeof(Mat44));
			// Given child in world-space, get the child in parent-space
			// Mat44Mul(child, Mat44Inversed(parent.m))
			PushA(currentAttribute, Mat44Delta(child, parent));
		}
		else if (currentAttribute == "Mat44FromVectors")
		{
			if (queue.size() != 16)
				return; // wait for 4 vector args
			__m128 v0 = _mm_set_ps(queue[3], queue[2], queue[1], queue[0]);
			__m128 v1 = _mm_set_ps(queue[7], queue[6], queue[5], queue[4]);
			__m128 v2 = _mm_set_ps(queue[11], queue[10], queue[9], queue[8]);
			__m128 v3 = _mm_set_ps(queue[15], queue[14], queue[13], queue[12]);
			Push(currentAttribute, Mat44FromVectors(v0, v1, v2, v3));
		}
		else if (currentAttribute == "Mat44ToTop33")
		{
			Mat44 m;
			CopyMemory(m.m, &queue[0], sizeof(Mat44));
			PushA(currentAttribute, Mat44ToTop33(m));
		}
		else if (currentAttribute == "Mat44ToTranslate")
		{
			Mat44 m;
			CopyMemory(m.m, &queue[0], sizeof(Mat44));
			PushA(currentAttribute, Mat44ToTranslate(m));
		}
		else if (currentAttribute == "Mat44ToEuler")
		{
			Mat44 m;
			if (queue.size() != 17)
				return; // wait for rotate order arg
			CopyMemory(m.m, &queue[0], sizeof(Mat44));
			PushA(currentAttribute, Mat44Rotate2(Mat44ToEuler(m, roLUT[(int)queue[16]]), roLUT[(int)queue[16]]));
		}
		else if (currentAttribute == "Mat44ToScale")
		{
			Mat44 m;
			CopyMemory(m.m, &queue[0], sizeof(Mat44));
			PushA(currentAttribute, Mat44ToScale(m));
		}
		else if (currentAttribute == "Mat44PerspectiveX")
		{
			Push(currentAttribute, Mat44PerspectiveX(queue[0], queue[1], queue[2], queue[3]));
		}
		else
		{
			__debugbreak(); // unexpected attribute
		}

		// avoid these args ending up with us again after use
		queue.clear();
	}

	bool Null()
	{
		__debugbreak(); // unexpected null, we only have string keys and arrays of arrays or arrays of floats
		return true;
	}
	bool Bool(bool b)	
	{
		__debugbreak(); // unexpected bool, we only have string keys and arrays of arrays or arrays of floats
		return true;
	}
	bool Int(int i)
	{
		__debugbreak(); // unexpected int, we only have string keys and arrays of arrays or arrays of floats
		return true;
	}
	bool Uint(unsigned u)
	{
		// we expect rotate order args as int but we'll just throw them in as float
		return Double((double)u);
		// __debugbreak(); // unexpected int, we only have string keys and arrays of arrays or arrays of floats
		// return true;
	}
	bool Int64(int64_t i)
	{
		__debugbreak(); // unexpected int, we only have string keys and arrays of arrays or arrays of floats
		return true;
	}
	bool Uint64(uint64_t u)
	{
		__debugbreak(); // unexpected int, we only have string keys and arrays of arrays or arrays of floats
		return true;
	}
	bool Double(double d)
	{
		if (!isInDocumentRoot || stackDepth < 1) __debugbreak(); // unexpected data outside of expected root-level single object and outside of any lists
		queue.push_back((float)d);
		return true;
	}
	bool String(const char* str, rapidjson::SizeType length, bool copy)
	{
		__debugbreak(); // unexpected string, we only have string keys and arrays of arrays or arrays of floats
		return true;
	}
	bool StartObject()
	{
		if (isInDocumentRoot) __debugbreak(); // nested objects, unexpected
		isInDocumentRoot = true;
		return true;
	}
	bool Key(const char* str, rapidjson::SizeType length, bool copy)
	{
		currentAttribute = std::string(str, length);
		return true;
	}
	bool EndObject(rapidjson::SizeType memberCount)
	{
		if (!isInDocumentRoot) __debugbreak(); // json parse error
		isInDocumentRoot = false;
		return true;
	}
	bool StartArray()
	{
		if (!isInDocumentRoot) __debugbreak(); // unexpected data outside of expected root-level single object
		stackDepth += 1;
		return true;
	}
	bool EndArray(rapidjson::SizeType elementCount)
	{
		if (queue.size() != 0)
		{
			// assert(queue.size() == elementCount);
			RunUnitTest();
		}
		stackDepth -= 1;
		if (stackDepth < 0) __debugbreak(); // json parse error
		return true;
	}
};

int main()
{
#if 0
	HWND hWnd = CreateWindowA("static", 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	HDC hDC = GetDC(hWnd);
	const PIXELFORMATDESCRIPTOR pfd = { 0, 0, PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	SetPixelFormat(hDC, ChoosePixelFormat(hDC, &pfd), &pfd);
	wglMakeCurrent(hDC, wglCreateContext(hDC));
	//RECT r;
	//GetWindowRect(hWnd, &r);
	//float aspectRatio = (float)(r.right - r.left) / (float)(r.bottom - r.top);
	float buffer[16];
	glMatrixMode(GL_MODELVIEW_MATRIX);
	glPushMatrix();
	glTranslatef(0.1f, 0.2f, 0.3f);
	glRotatef(22.5f, 1.0f, 0.0f, 0.0f);
	glGetFloatv(GL_MODELVIEW_MATRIX, buffer);
	glPopMatrix();
	DebugPrintMat44(*reinterpret_cast<Mat44*>(&buffer[0]));
#endif

	//DebugPrintMat44(Mat44Identity());
	Mat44 t = Mat44Translate(0.1f, 0.2f, 0.3f);
	Mat44 rx = Mat44RotateX(22.5f * DEG2RAD);
	Mat44 ry = Mat44RotateY(-10.0f * DEG2RAD);
	Mat44 rz = Mat44RotateZ(45.0f * DEG2RAD);
	// To mimic a maya transform's attributes we multiply scale * rotateX * rotateY * rotateZ * translate in that order
	// Mostly because this rotate order matches the rotate order notation, XYZ...
	Mat44 tr = Mat44Mul(Mat44Mul(rx, rz), Mat44Mul(ry, t));

	// OutputDebugStringA("MatMul\r\n");
	// DebugPrintMat44(tr);

	// DebugPrintMat44(Mat44InverseFastNoScale(tr));
	// DebugPrintQuat(QuatRotateX(22.5f * DEG2RAD));
	// DebugPrintQuat(QuatRotateY(-10.0f * DEG2RAD));
	Quat qrx = QuatRotateX(22.5f * DEG2RAD);
	Quat qry = QuatRotateY(-10.0f * DEG2RAD);
	Quat qrz = QuatRotateZ(45.0f * DEG2RAD);
	Quat q = QuatMul(QuatMul(qrx, qry), qrz);

	//DebugPrintQuat(QuatRotateZ(45.0f * DEG2RAD));
	// OutputDebugStringA("QuatMul\r\n");
	// DebugPrintQuat(q);

	//OutputDebugStringA("Quat -> Mat\r\n");
	//DebugPrintMat44(Mat44FromQuat(q));

	//DebugPrintVec4(QuatVectorTransform(q, F32_UNIT_X));
	//DebugPrintVec4(QuatVectorTransform(q, F32_UNIT_Y));
	//DebugPrintVec4(QuatVectorTransform(q, F32_UNIT_Z));

	//DebugPrintVec4(Mat44VectorTransform(tr, F32_UNIT_X));
	//DebugPrintVec4(Mat44VectorTransform(tr, F32_UNIT_Y));
	//DebugPrintVec4(Mat44VectorTransform(tr, F32_UNIT_Z));
	//DebugPrintVec4(Mat44VectorTransform(tr, _mm_set_ps(1.0f, 0.5f, 0.25f, 0.125f)));

	// OutputDebugStringA("Mat -> Quat\r\n");
	// DebugPrintQuat(QuatFromMat44(tr));

	// OutputDebugStringA("Mat -> Angles XZY\r\n");
	// DebugPrintEuler(EulerFromMat44(tr));

	/*OutputDebugStringA("Quat -> Angles XZY\r\n");
	q = QuatMul(QuatMul(qrx, qrz), qry);
	DebugPrintEuler(EulerFromQuat(q, ERotateOrder::XZY));

	OutputDebugStringA("Quat -> Angles XYZ\r\n");
	q = QuatMul(QuatMul(qrx, qry), qrz);
	DebugPrintEuler(EulerFromQuat(q, ERotateOrder::XYZ));

	OutputDebugStringA("Quat -> Angles ZYX\r\n");
	q = QuatMul(QuatMul(qrz, qry), qrx);
	DebugPrintEuler(EulerFromQuat(q, ERotateOrder::ZYX));*/

	UnitTestJSonHandler::run("Test/in.json", "Test/mmath_out.json");

	return 0;
}
