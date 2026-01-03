#ifndef _WRITER_H
#define _WRITER_H

#include "args.h"
#include "common.h"
#include "acx.h"
#include "reader.h"

typedef ACXReader ACXWriter;

/*
extern void PAKReader_init(PAKReader* __restrict preader, const char* inFilePath);
extern void PAKReader_clear(PAKReader* __restrict preader);
extern void PAKReader_read_content(PAKReader* __restrict preader, const uint32_t fileIndex); // 读取文件内容。只有要进行提取操作时才读取。建立目录还是必要的。
extern void PAKReader_copy_content(PAKReader* __restrict preader, const uint32_t fileIndex, const char* outFilePath);

extern void extract(Args* __restrict args);
extern void list(Args* __restrict args);
*/

extern void ACXWriter_init(ACXWriter* __restrict awriter, const Args* __restrict args); // 类的初始化
extern void ACXWriter_read(ACXWriter* __restrict awriter);				// 读取待写入文件的内容
extern void ACXWriter_build_acxFile(ACXWriter* __restrict awriter);			// 构建ACX文件
// 清理类，释放内存
#define ACXWriter_clear(awriter) ACXReader_clear((awriter))

extern void archive(const Args* __restrict args);

#define BLOCK_SIZE (4 * sizeof(uint32_t))

#endif
