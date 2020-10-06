# KalinoteOS
一个可以运行多任务和图形界面的微型系统

基于《30天自制操作系统》里的heboos优化

# 注意：所有源文件必须使用GB2312编码，push前请确认编码是否正确！

# 下面是更新日志
## 20100401
- 加入了一些编译需要使用的工具

## 20100402
- 更新并优化了核心文件
> 现在在CoreFiles目录使用指令 .\make.bat run 就可以一键启动系统

## 20100403
- 更新并优化了IPL和makefile
> 现在在CoreFiles目录使用指令 .\make.bat src_only 可以删除除源代码以外生成的文件

> 其余makefile还有asm img install clean等指令，具体使用方法有时间再说

## 20100501
- 向IPL中的启动部分添加了启动失败重新尝试，尝试超过5次报错

## 20100502
- 扩大了IPL中启动时读取磁盘的柱面数

# **至此，启动区制作完成**

## 20100503
- 优化了IPL，并将其更名位IPL10(因为只能读取10个柱面)
- 增加了一个启动程序kalinote.nas

## 20100504
- 更新了启动程序kalinote.nas，使其在启动时初始化显卡和键盘状态

## 20100505
- 更新了kalinote.nas，更名为asmhead.nas
- 添加了bootpack.c，这个文件和asmhead.nas是一起的，但是asmhead.nas使用的汇编语言，bootpack.c使用的c语言
- 将原本kalinote.nas中的fin的部分移动到了bootpack.c

## 20100506
- 使用bim2kal替换了bim2hrb，用于生成c语言的程序

## 20100507
- 更改了bootpack.c的结构
- 更改了目录结构

## 20100508
- 增加了naskfunc文件，以后可能会在里面写一点函数
- 向bootpack中加入了hlt函数

## 20100509
- 向naskfunc中加入了_write_mem8，用于写入内存
- 修改bootpack中的内容，让屏幕渲染显示白色

## 20100510
- 把显示的白屏改成了小彩条:)

## 20100511
- 使用指针代替了write_mem8函数

## 20100601
- bootpack增加了用于显示色彩的调色板(init_palette和set_palette函数)
- naskfunc.nas增加了多个io相关函数

## 20100602
- 修改了bootpack，尝试在启动时绘制RGB方块

## 20100603
- 去掉了RGB方块，换成了全屏纯色背景，用来当桌面

## 20100604
- 优化了bootpack代码结构

## 20100605
- 加入了半角字体并在系统中成功显示单个字符
