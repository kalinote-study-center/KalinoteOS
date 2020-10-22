# KalinoteOS
一个可以运行多任务和图形界面的微型系统

基于《30天自制操作系统》里的haribos优化

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

## 20100606
- 加入了字符串显示功能

## 20100607
- 增加了鼠标指针

## 20100608
- 增加了GDT和IDT的初始化函数

## 20100609
- 分割了文件，现在画面和中断等操作由单独的文件组成，便于维护

## 20100610
- 优化了makefile

## 20100611
- 优化了程序结构，增加了bootpack.h用来保存函数声明、常量和结构体

## 20100612
- 增加了PIC初始化

## 20100613
- 增加中断，用于鼠标和键盘监听

## 20100614
- 调整了中断程序，能够获取键盘码

## 20100615
- 优化中断程序，减少中断程序的处理步骤
- 优化键盘码捕获

## 20100616
- 改善了FIFO缓冲区
- 修复了编译时的一大堆Warning

## 20100617
- 添加鼠标初始化程序

## 20100618
- 添加了fifo.c，用于管理FIFO缓冲区
- 整理优化了FIFO缓冲区逻辑

## 20100619
- 优化了鼠标数据接收
- 现在一次可以接收鼠标的三个数据

## 20100620
- 优化了bootpack.c的代码结构

## 20100621
- 优化鼠标数据传输

## 20100622
- 解决了鼠标移动问题，现在鼠标可以随意移动了

## 20100701
- 新增了mouse.c和keyboard.c用于管理鼠标和键盘
- 优化了代码结构

## 20100702
- 加入了内存检查功能

## 20100703
- 修复了编译器导致的功能bug

## 20100704
- 完善了内存管理程序

## 20100705
- 优化了程序结构，增加了memory.c，用于管理内存
- 增加了对4K内存的处理，减少出现片段内存的情况

## 20100706
- 增加了叠加处理，现在系统显示有层(sheet)的概念了

## 20100707
- 提高了叠加处理速度

## 20100708
- 进一步提高了叠加处理速度

## 20100709
- 优化了鼠标指针显示
- 实现画面外渲染支持

## 20100710
- 优化了程序结构，减少了shtctl的使用

## 20100711
- 实现了生成窗口的功能

## 20100801
- 增加了一个计数器
- 优化了图层渲染程序(graphic.c)

## 20101301
- 优化了编译器

## 20101302
- 添加了20号中断器
- 添加了定时器(Timer)

## 20101303
- 优化了定时器
- 更改了counter的计算方式，现在变成每秒增加100

## 20101304
- 设置了多个定时器
- 实现了一个鼠标闪烁的小功能

## 20101305
- 优化了20号中断，减少了中断时的处理动作，加快了中断速度

## 20101306
- 优化了字符串显示

## 20101307
- 优化了定时器和FIFO缓冲区

## 20101401
- 重新调整并优化了FIFO缓冲区

## 20101402
- 进一笔优化了第20号中断处理程序

## 20101403
- 增加了定时器的哨兵程序

## 20101501
- 将画面模式切换为VEB模式
- 提升了画面分辨率

## 20101502
- 增加了键盘输入字符的判定

## 20101601
- 增加了textbox
- 加入了测试使用的文本输入框

## 20101602
- 实现了简单的窗口移动

## 20101603
- 实现了简单的多任务(10秒任务切换)

## 20101604
- 实现了多个任务快速切换达到的多任务

## 20101701
- 优化了多任务速度

## 20101901
- 优化了多任务处理逻辑

## 20101902
- 优化了多任务管理方式

## 20101903
- 增加了空闲任务休眠，提升了多任务性能以及对性能利用的效率

## 20101904
- 尝试同时运行四个程序

## 20102001
- 增加了程序优先级的设定

## 20102002
- 改进了优先级判定程序

## 20102003
- 增加了命令行窗口

## 20102004
- 实现了tab键切换窗口的标题栏部分

## 20102005
- 实现了tab键切换窗口后键盘输入的切换

## 20102006
- 实现了输入时通过shift的大小写切换

## 20102107
- 修复了CapsLock没办法大写的问题
- 增加了对键盘指示灯的判定

## 20102108
- 优化了输入框光标显示逻辑

## 20102109
- 进一步优化了光标显示逻辑

## 20102110
- 添加了对命令行回车的支持

## 20102111
- 添加了对命令行滚动的支持

## 20102112
- 增加了用于查询内存的mem指令，详细内容见《系统支持文档》

## 20102113
- 增加了用于清屏的cls和clear指令，详细内容见《系统支持文档》

## 20102114
- 增加了用于查询文件的dir和ls指令，详细内容见《系统支持文档》

## 20102201
- 增加了用于输出文件内容的type指令，详细内容见《系统支持文档》
