#ifndef _ACX_H
#define _ACX_H

#include "common.h"

// 数据都是大端序的。或许CRI氏的文件都爱用大端序。

typedef struct ACX_Header { // [0x00, 0x08)	文件头部
	uint32_t zero;	    // [0x00, 0x04)	0
	uint32_t fileCount; // [0x04, 0x08)	包内文件数
} ACX_Header;

typedef struct ACX_Item {	 // 文件头后即为目录，fileCount条此条目顺序排列构成目录
	uint32_t relativeOffset; // 于.acx文件内的相对偏移
	uint32_t fileLen;	 // 文件长度
} ACX_Item;

/* ********************** */

// 与上面的不同，下面的是便于程序使用而定义的结构

typedef struct ACX_File {
	uint32_t relativeOffset;
	uint32_t fileLen;
	uint32_t bufferSize;
	void* content;
} ACX_File;

#endif
