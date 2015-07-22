

/***********************************************************************

                           h：宏

************************************************************************/

//若COM已经不存在了就不用释放了
//宏名及一个空格后就是要替换的文本
#pragma once

#define MATH_PI 3.1415926f

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
				MessageBoxA(0,MsgText,0,0);\
				return FALSE;\
				};\


//调试：弹出消息
#define DEBUG_MSG(msg1,msg2,msg3)\
				g_Debug_MsgString<<msg1<<msg2<<msg3;\
				MessageBoxA(0,g_Debug_MsgString.str().c_str(),0,0);\
				g_Debug_MsgString.clear()\


;//文件尾



