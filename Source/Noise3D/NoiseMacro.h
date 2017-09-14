

/***********************************************************************

                           h：宏

************************************************************************/

//若COM已经不存在了就不用释放了
//宏名及一个空格后就是要替换的文本
#pragma once

#define NOISE_MACRO_INVALID_ID			UINT_MAX

#define NOISE_MACRO_INVALID_TEXTURE_ID			UINT_MAX

#define NOISE_MACRO_INVALID_MATERIAL_ID		UINT_MAX

#define NOISE_MACRO_DEFAULT_MATERIAL_NAME "NoIsE_3d_DeFaUlT_MaTeRiAl"//default material name

#define NOISE_MACRO_DEFAULT_COLOR_BYTESIZE 16U //DXGI_FORMAT_R32G32B32A32_FLOAT 128bit 16bytes

#define NOISE_MACRO_FUNCTION_EXTERN_CALL   //A mark that indicates this function is called by external function or friend class

#define NOISE_MACRO_FUNCTION_WINAPI //A mark that indicates this function is windows api,might help when migrate to other platform 

#define NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_ROW_COUNT 8U	//used in font mgr

#define NOISE_MACRO_FONT_ASCII_BITMAP_TABLE_COLUMN_COUNT 16U

#define NOISE_MACRO_DLL_EXPORT _declspec(dllexport)


#define MATH_PI 3.1415926f

//释放一个COM对象
#define ReleaseCOM(ComPointer)\
				if(ComPointer!=nullptr)\
				{\
				ComPointer->Release();\
				ComPointer=nullptr;\
				}\


//调试：调试弹框
#define HR_DEBUG(hr,MsgText)\
				if(FAILED(hr)) \
				{\
				ERROR_MSG("d3d returned error code : "+std::to_string(hr)+"\n"+MsgText);\
				return false;\
				};\

//调试：调试弹框
#define HR_DEBUG_CREATETEX(hr,MsgText)\
				if(FAILED(hr)) \
				{\
				ERROR_MSG(MsgText);\
				return nullptr;\
				};\



//implementing these 2 op with functions is not acceptable because pre-processed
//data will be wiped out.
#define ERROR_MSG(msg)\
	{\
		std::ostringstream debugMsg;\
		debugMsg << "ERROR:" << std::endl;\
		debugMsg << msg << std::endl;\
		debugMsg << "file: " << __FILE__ << std::endl;\
		debugMsg << "line: " << __LINE__ << std::endl;\
		debugMsg << "function:" << __func__ << std::endl;\
		throw std::exception(debugMsg.str().c_str());\
		debugMsg.clear();\
	}\
	//MessageBoxA(0, debugMsg.str().c_str(), 0, 0);

#define WARNING_MSG(msg)\
	{\
		std::ostringstream debugMsg;\
		debugMsg << "WARNING:" << std::endl;\
		debugMsg << msg << std::endl;\
		debugMsg << "file: " << __FILE__ << std::endl;\
		debugMsg << "line: " << __LINE__ << std::endl;\
		debugMsg << "function:" << __func__ << std::endl;\
		throw std::exception(debugMsg.str().c_str());\
		debugMsg.clear();\
	}\
;