# Noise3D：基于D3D11的渲染引擎 #
----------------------------------------------

1. 项目正式始于2015.6.12（高考完的暑假）（想不到我在2014.1月的时候，也就是高二上的寒假就已经碰过下d3d11，只不过卡了很久，后来就搞3D扫描仪去了）。直到高考完才叫做正式开始这个项目。唉渲染引擎乃至游戏引擎的架构真的是太重要了，得放慢开发速度，不能整天加new feature就要重构和给以前擦屁股（虽然如果不踩这么多坑，也很难对一些书本学到的东西有深刻印象），多使用和参考一下优秀的开源引擎/商用引擎的架构和实现才行。<br/>

2. 因为项目开始的时候还在入门c++，所以其实代码风格都有点歪了，改了很久。也没有考虑跨平台，甚至没有考虑目标平台的问题，暂时就只能用着x86/Win32的目标平台了。<br/>

3. 项目一开始的时候用的是DirectX SDK(2010 June)，但好像其实在Win8/10之后就基本不太需要这玩意了，头文件和lib都装在Windows SDK上。但是Noise3D里面还有一些对旧版D3DX的依赖（例如数学库）我觉得要慢慢把这些玩意deprecate掉了<br/>

4. (2018.7.19)现在正在把旧的D3DX deprecate掉。逐渐用上Win8.1 SDK的东西<br/>

----------------------
##  依赖库(Dependencies)
* <b>FreeType</b>(字体库)</b>：https://www.freetype.org/
* <b>FBXSDK</b>(autodesk的FBX模型导入\导出\处理SDK)，自己去autodesk网站下载
* <b>DirectXTex</b>(图片/纹理加载)：https://github.com/Microsoft/DirectXTex
* <b>Effects11</b>(shader组织框架)：https://github.com/Microsoft/FX11
* <b>SimpleMath</b>(DirectXMath wrapper，在DirectXTK下有SimpleMath的封装实现)(2018.7.19)：https://github.com/Microsoft/DirectXTK

### 注意：
* 这些文件得自己去build，然后把相应的include和lib文件copy到\ExternalInclude和\ExternalLib里面。
* \ExternalInclude里面有一些其他版本的第三方库的头文件，可以根据自己需求魔改成新的版本
* \ExternalLib里面是装第三方库的.lib静态库文件的，自己build好32-bit的debug/release放到对应的路径
* 可能需要修改一下Noise3D VS项目里面的链接选项。
* DirectXMath是一个用SSE指令实现了SIMD优化的数学库，在Win8.1 SDK开始提供，当然也可以在github上找到这个开源库 https://github.com/Microsoft/DirectXMath 。SimpleMath是DirectXMath的一个轻度封装的头文件，在DirectXTK项目下，不需要编译lib。DirectXMath大部分都是内联实现，即使不内联的好像也是开源的，include header就好了)

----------------------

## Features
* 材质Material：Diffuse Map, Normal Map, Specular Map, Cube Map(reflection)

* 在内存端的纹理的逐像素修改，也有高度图-->灰度图-->法线图的接口（转法线图的卷积核大小为2x2）

* 逐顶点/像素光照。per vertex/pixel lighting

* 基于DInput8的输入引擎

* 天空盒(cube map)/天空球

* 2D元素（点、线、面、三角形、2D贴图纹理）

* 文字渲染

* 模型加载: .stl， .obj， .fbx (.3ds格式不开源，实现不稳定，不建议使用)

* 碰撞检测器(not stable)：GPU加速的ray-mesh求交。（以后应该会换成基于第三方物理引擎里面的碰撞检测）

* 模型处理器(not stable):网格简化 (很不稳定，还没实现几何坍缩类的算法，不建议使用)；

* 后处理(Renderer)：post processing
    * 转灰度
    * Qwerty Distortion (之前用来做另一个项目Qwerty 3D时候做的，应该是没什么卵用了)

* 拖尾特效(Sweeping Trail)
--------------------------------------------------

## 一些小工具类

* 计时器(Timer)

* 3D网格切层器(3D Mesh Slicer)

* 体素化器(Voxelizer)

* 基于Marching Cube算法的网格重建器
