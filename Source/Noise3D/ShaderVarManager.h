
/***********************************************************************

                           h£ºShader Variable Manager
				Desc: WHenever I want to create a new constant buffer
				in shader(s), i had to create a struct with identical memory 
				layout in c++, then define one of this structure in Renderer
				then fill up those cbuffer struct in memory. Moreover,
				modification of shader cbuffer requires an even more 
				complicated cascaded modification in c++.s

				Thus I want to develop a shader variable manager, making 
				full use of Effect11 framework and ignore the concept of 
				"cbuffer" in c++. then variable is the unit for updating data
				to gpu. No more "identical struct" in c++ needs to define. 

************************************************************************/

#pragma once

namespace Noise3D
{

	//inherited by IRenderer, providing shader variable update service
	class /*_declspec(dllexport)*/ IShaderVariableManager
	{
	public:
		
		//set variables
		void SetVar(const char* var, float val);

		void SetVar(const char* var, int val);

		void SetVar(const char* var, NVECTOR2 val);

		void SetVar(const char* var, NVECTOR3 val);

		void SetVar(const char* var, NVECTOR4 val);

		void SetVar(const char* var, NMATRIX val);

	private:

		friend IRenderer;

		IShaderVariableManager();

		~IShaderVariableManager();

	};
}