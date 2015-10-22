

/***********************************************************************

                           h：宏

************************************************************************/

//若COM已经不存在了就不用释放了
//宏名及一个空格后就是要替换的文本
#pragma once

#define MATH_PI 3.1415926f
//the first element in Mat Mgr is default value (means we dont accept invalid material)
//but texture can be invalid
#define NOISE_MACRO_INVALID_MESH_ID				UINT_MAX
#define	 NOISE_MACRO_INVALID_TEXTURE_ID			UINT_MAX
#define NOISE_MACRO_INVALID_GRAPHICOBJ_ID	UINT_MAX
#define NOISE_MACRO_DEFAULT_MATERIAL_ID 0U
#define NOISE_MACRO_DEFAULT_COLOR_BYTESIZE 16U //DXGI_FORMAT_R32G32B32A32_FLOAT 128bit 16bytes
#define NOISE_MACRO_FUNCTION_EXTERN_CALL   //A mark that indicates this function is called by external friend class


//释放一个COM对象
#define ReleaseCOM(ComPointer)\
				if(ComPointer!=0)\
				{\
				ComPointer->Release();\
				}\


//调试：调试弹框
#define HR_DEBUG(hr,MsgText)\
				if(FAILED(hr)) \
				{\
				DEBUG_MSG1(MsgText);\
				return FALSE;\
				};\


//调试：弹出消息
#define DEBUG_MSG3(msg1,msg2,msg3)\
				g_Debug_MsgString<<msg1<<msg2<<msg3<<std::endl;\
				g_Debug_MsgString<<"file: "<<__FILE__<<std::endl;\
				g_Debug_MsgString<<"line: "<<__LINE__<<std::endl;\
				MessageBoxA(0,g_Debug_MsgString.str().c_str(),0,0);\
				g_Debug_MsgString.clear()\

#define DEBUG_MSG1(msg)\
				g_Debug_MsgString<<msg<<std::endl;\
				g_Debug_MsgString<<"file: "<<__FILE__<<std::endl;\
				g_Debug_MsgString<<"line: "<<__LINE__<<std::endl;\
				MessageBoxA(0,g_Debug_MsgString.str().c_str(),0,0);\
				g_Debug_MsgString.clear()\

;//文件尾



