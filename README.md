# KalinoteOS
一个可以运行多任务和图形界面的微型系统

基于《30天自制操作系统》里的heboos优化。

# 注意：所有源文件必须使用GB2312编码，push前请确认编码是否正确！

# 下面是更新日志
## 20100401
- 加入了一些编译需要使用的工具。

## 20100402
- 更新并优化了核心文件。
> 现在在CoreFiles目录使用指令 .\make.bat run 就可以一键启动系统。

## 20100403
- 更新并优化了IPL和makefile
> 现在在CoreFiles目录使用指令 .\make.bat src_only 可以删除除源代码以外生成的文件。

> 其余makefile还有asm img install clean等指令，具体使用方法有时间再说。

## 20100501
- 向IPL中的启动部分添加了启动失败重新尝试，尝试超过5次报错。

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
