# Noise3D：基于D3D11的渲染引擎 #
----------------------------------------------

1. 项目正式始于2015.6.12（高考完的暑假）（想不到我在2014.1月的时候，也就是高二上的寒假就已经碰过下d3d11，只不过卡了很久，后来就搞3D扫描仪去了）。直到高考完才叫做正式开始这个项目。唉渲染引擎乃至游戏引擎的架构真的是太重要了，得放慢开发速度，不能整天加new feature就要重构和给以前擦屁股（虽然如果不踩这么多坑，也很难对一些书本学到的东西有深刻印象），多使用和参考一下优秀的开源引擎/商用引擎的架构和实现才行。

2. 因为项目开始的时候还在入门c++，所以其实代码风格都有点歪了，改了很久。也没有考虑跨平台，甚至没有考虑目标平台的问题，暂时就只能用着x86/Win32的目标平台了。
<br/>
<br/>

----------------------
##  依赖库(Dependencies)
* FreeType（字体库）：https://www.freetype.org/
* FBXSDK(autodesk的FBX模型导入\导出\处理SDK)
* DirectXTex（图片/纹理加载）：https://github.com/Microsoft/DirectXTex
* Effects11（shader组织框架）：https://github.com/Microsoft/FX11

----------------------

## Features
* Material:Diffuse, Normal Mapping, Specular Mapping, Cube Map(reflection)

* 在内存端的纹理的逐像素修改，也有高度图-->灰度图-->法线图的接口（转法线图的卷积核大小为2x2）

* 逐顶点/像素光照。per vertex/pixel lighting

* 基于DInput8的输入引擎

* 天空盒(cube map)/天空球

* 2D元素（点、线、面、三角形、2D贴图纹理）

* 文字渲染

* 模型加载: .stl， .obj， .fbx (.3ds is deprecated due to unstability)

* 碰撞检测器(not stable)：GPU加速的ray-mesh求交。（以后应该会换成基于第三方物理引擎里面的碰撞检测）

* 模型处理器(not stable):网格简化（not robust)；

* 后处理(Renderer)：post processing
    * 转灰度
    * Qwerty Distortion (之前用来做另一个项目Qwerty 3D时候做的，应该是没什么卵用了)

--------------------------------------------------

一些小工具

* 计时器(Timer)

* 3D网格切层器(3D Mesh Slicer)

* 体素化器(Voxelizer)

* 基于Marching Cube算法的网格重建器
