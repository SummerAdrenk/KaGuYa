# Xiph源码依赖

- libogg 1.3.6：官方libogg-1.3.6.zip，SHA-256 e347b59c29e8f938bb48b2fda5da02f2d5d97316c004abaa76e171cf4a2fbdd7。
- libvorbis 1.3.7：官方libvorbis-1.3.7.zip，SHA-256 57c8bc92d2741934b8dc939af49c2639edc44b8879cba2ec14ad3189e2814582。

来源：[Xiph官方发布包](https://www.xiph.org/downloads/)。源码由用户提供的离线包解出，首次导入核对官方校验值；两个版本目录内的库源码（`include/`、`src/`、`lib/`）未修改，各自COPYING、AUTHORS、CHANGES完整保留；autotools、文档、示例、测试及其它平台的构建脚本未随仓库提供，完整内容见上游发布包。

上层KaGuYaVorbis.vcxproj按上游核心/Vorbisfile源码列表构建一个Win32/v143静态库。所有对象和库输出在工程build目录；Sound静态链接它，不新增运行时DLL，也不从原游戏模块导出解码入口。构建会将两份许可证复制到build/bin配置目录的licenses下。

用途是恢复原Sound VorbisInput采用的ov_open_callbacks、ov_pcm_total(-1)、ov_read、ov_pcm_seek流程，尤其是多个逻辑流串接；不是手工识别文件名后拼接PCM，也没有失败后改用另一解码库的兜底。
