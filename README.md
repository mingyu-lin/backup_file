# backup_file
## 简介：
文件备份系统
## 环境配置：
windows系统、VS2022、C++ 17及以上、QT 6以上，openssl win64
1. **安装依赖**

   * 下载并安装 **Qt 6**（安装时务必勾选 *MSVC* 支持）。
   * 下载并安装 **OpenSSL**（Win64 版本）。
2. **获取项目代码**

   * 通过 Visual Studio 图形化界面或命令行，将本项目仓库 `clone` 到本地。

3. **准备环境**

   * 将 `OpenSSL-Win64` 和 `msvc2022_64`（位于 Qt 6.9.2 安装目录下）两个文件夹复制到本项目目录。
   * 在系统环境变量 `Path` 中添加以下路径：

     * `D:\openssl\OpenSSL-Win64\bin`
     * `C:\Users\30281\source\repos\back_up\msvc2022_64\bin`
       
4. **编译与运行**

   * 使用 Visual Studio 打开项目目录中的 `CMakeLists.txt` 文件。
   * 配置并生成解决方案后即可进行编译与运行。
