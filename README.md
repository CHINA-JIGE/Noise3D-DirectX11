# GitHub-Noise3D-DIRECTX11

-Noise3D

-基于D3D11的渲染引擎

-作者：鸡哥

-项目正式始于2015.6.12（想不到我在2014.1月的时候，也就是高二上的寒假就已经碰过下d3d11，只不过卡了很久，后来就搞3D扫描仪去了）。直到高考完才叫做正式开始这个项目。到现在(2016.5.2)差不多也有一年了，然后中间也是有各种事，进度还是有点缓慢的。设计不好，重构用了我很久，2D图形（包括GUI）也用了我三四个月，这个也是略坑啊。现在该把架构和代码风格改进一下了。

This is a simple 3D Render Engine based on Direct3D 11.

For the time being, this project is developed for learning purpose... 


//////////////////////////////////////////////////////////////////////////////
Some simple features are implemented:

--Normal Mapping

--Specular Mapping

--Cube Reflection Map

--Texture Modification by pixel (including HeightMap -> Normal Map Conversion

--per pixel lighting (local illumination)

--Input Engine using DIRECT INPUT

--skybox(cubemap) / SkySphere

--2D Element Rendering(points,lines,triangle,textured rectangles)

--Text Rendering

--Mesh File Loading : STL , OBJ，FBX

--Collision Test

//////////////////////////////////////////////////////////////////////////////////////////////

Utility:

--Timer

--3D Mesh Slicer

--Voxelizer

--Marching  Cube Mesh Reconstructor

In the future my own 3d Scanner (data processing pipeline) will be combined into this engine as an Utility.

