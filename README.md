# acx\_util

一款支持 解包、罗列、打包 .acx音频包格式文件 的工具。

打包模式下加入了16 Byte对齐特性。

## 用法

模仿`GNU tar`。

```
Usage: acx [-ctvx] [-C[DIR]] [-f[ARCHIVE]] [-T[FILE]] [--create]
            [--directory[=DIR]] [--file[=ARCHIVE]] [--list]
            [--files-from[=FILE]] [--verbose] [--extract]

  -c, --create               创建一个新归档
  -C, --directory[=DIR]      改变至目录DIR
  -f, --file[=ARCHIVE]       操作目标ACX ARCHIVE文件
  -t, --list                 列出归档内容
  -T, --files-from[=FILE]    从FILE中获取文件名来解压或创建文件
  -v, --verbose              显示详细信息
  -x, --extract              从归档中解出文件
  -?, --help                 Give this help list
      --usage                Give a short usage message

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.
```

## 编译

`Linux`、`Cygwin`、`MSYS2`环境下，直接`make`即可。大概吧。

## 许可证

LGPL >= 3

----

—— wyz\_2015
