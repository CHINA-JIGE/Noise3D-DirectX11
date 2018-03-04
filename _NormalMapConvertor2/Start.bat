@echo off
echo "格式<exe名> <图片名> <heightField的缩放因子> <转灰度图时的R权重> <G权重> <B权重> <格式,有bmp,jpg,dds可选>"
echo "推荐输出.dds文件，这个蛇皮D3DX11的png导出好像有点问题，jpg又有损，导出的bmp又好像不受3dsmax2017支持。。。"
echo Noise3D Normal Map Converter(2018.3.4). 转换开始...
NormalMapConvertor.exe arc18_filtered.png 30.0 0.3 0.59 0.11 dds
echo 输出成功！
pause