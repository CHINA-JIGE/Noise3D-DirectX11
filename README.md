# GitHub-Noise3D-DIRECTX11

--Noise3D：基于D3D11的渲染引擎

----------------------------------------------

-项目正式始于2015.6.12（高考完的暑假）（想不到我在2014.1月的时候，也就是高二上的寒假就已经碰过下d3d11，只不过卡了很久，后来就搞3D扫描仪去了）。直到高考完才叫做正式开始这个项目。唉渲染引擎乃至游戏引擎的架构真的是太重要了，得放慢开发速度，不能整天加new feature就要重构和给以前擦屁股（虽然如果不踩这么多坑，也很难对一些书本学到的东西有深刻印象），多使用和参考一下优秀的开源引擎/商用引擎的架构和实现才行。

-因为项目开始的时候还在入门c++，所以其实代码风格都有点歪了，改了很久。也没有考虑跨平台，甚至没有考虑目标平台的问题，暂时就只能用着x86/Win32的目标平台了。

-Noise3D依赖的外部库：
FreeType（字体）：https://www.freetype.org/
FBXSDK(autodesk的FBX模型导入\导出\处理SDK)
DirectXTex（图片/纹理加载）：https://github.com/Microsoft/DirectXTex
Effects11（shader组织框架）：https://github.com/Microsoft/FX11

------------------------------------------------------

实现了一些简单的功能：

--Normal Mapping

--Specular Mapping

--Cube Reflection Map

--material/texture systems

--Texture Modification by pixel (including HeightMap -> Normal Map Conversion

--per vertex/pixel lighting (gouraud/phong shading)(local illumination)

--DirectInput8 based input engine

--skybox(cubemap) / SkySphere

--2D Element Rendering(points,lines,triangle,textured rectangles)

--Text Rendering

--ModelLoader : .stl, .obj , .fbx(due to unstability, .3ds is deprecated)

--Collision Testor(not stable):GPU accelerated ray-mesh intersection;

--ModelProcessor(not stable):mesh simplification;

--post processing

--------------------------------------------------

一些小工具

--Timer

--3D Mesh Slicer

--Voxelizer

--Marching Cube Mesh Reconstructor
