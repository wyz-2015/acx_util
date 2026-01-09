# acx\_util

一款支持 解包、罗列、打包 .acx音频包格式文件 的工具。

打包模式下加入了16 Byte对齐特性。

## 用法

模仿`GNU tar`。

```
Usage: acx [-ctvx] [-C DIR] [-f ARCHIVE] [-T FILE] [--create]
            [--directory=DIR] [--file=ARCHIVE] [--list] [--files-from=FILE]
            [--verbose] [--extract]

  -c, --create               创建一个新归档(此模式下-C参数失效)
  -C, --directory=DIR        改变至目录DIR
  -f, --file=ARCHIVE         操作目标ACX ARCHIVE文件(必要)
  -t, --list                 列出归档内容
  -T, --files-from=FILE      从FILE中获取文件名来解压或创建文件
  -v, --verbose              显示详细信息
  -x, --extract              从归档中解出文件

  -?, --help                 Give this help list
      --usage                Give a short usage message

Mandatory or optional arguments to long options are also mandatory or optional
for any corresponding short options.
```

例子：(假设调用程序的指令是`./acx`)

```
# 在当前目录下创建a.pak，内容为./dir_a/下的所有文件、文件列表./b.txt中列文件，并显示过程。(文件包中各文件有为对其到16 Byte而补0。)
$ ./acx -cvf ./a.acx ./dir_a/*.adx --files-from=./b.txt

# 解包./0.acx，并将解出的文件放到目录./0/下
$ ./acx -xf ./0.acx -C ./0

# 不解包，查看./pl01.pak中的各文件的序号、所在偏移、文件长
$ ./acx --list -f ./1.acx
```

## 编译

`Linux`、`Cygwin`、`MSYS2`环境下，直接`make`即可。大概吧。

## 许可证

LGPL >= 3

----

—— wyz\_2015
