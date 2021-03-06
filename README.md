# KalinoteOS
一个可以运行多任务和图形界面的微型系统

基于《30天自制操作系统》里的haribos优化

# 注意：为了保证对中文的支持，所有源文件必须使用GB2312编码，push前请确认编码是否正确！

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

## 20102202
- 优化了type指令，使其支持了制表符(0x09)、换行符(0x0a)、回车符(0x0d,忽略)

## 20102301
- 增加了对FAT文件系统的支持

## 20102302
- 整理了源代码文件，单独分出了窗口，文件系统，命令窗口的C文件

## 20102303
- 成功地执行了第一个应用程序(hlt.kal)

## 20102401
- 整理了命令窗口(console.c)相关代码

## 20102501
- 添加了第一个API，显示一个字符(\_asm_cons_putchar,详细内容见《系统支持文档》)，并在hlt.kal中调用成功

## 20102502
- 优化了程序运行逻辑，使程序在运行完成后可以结束

## 20102503
- API接口不会再随操作系统版本改变而改变
- 更改了\_asm_cons_putchar的调用方式，详细内容见《系统支持文档》

## 20102504
- 优化了cmd对程序的查找和调用逻辑

## 20102505
- 增加了两个用于打印字符串的API，详细内容见《系统支持文档》

## 20102506
- 修复了API显示字符串的一个小BUG

## 20102507
- 尝试使用C语言编写外部程序

## 20102508
- 给应用程序分配了专用内存段，提高了系统安全性

## 20102509
- 使用0d中断对异常程序监控并处理

## 20102510
- 提升了系统安全性

## 20102601
- 进一步提升了系统安全性
- 现在发生栈错误回返回EIP号码

## 20102602
- 增加了强制停止应用程序的功能

## 20102603
- 增加了对C语言API的支持

## 20102701
- 修改了《系统支持文档》中部分API的命名方式
- 添加了用于生成窗口的API(api_openwin)，详细内容见《系统支持文档》

## 20102702
- 添加了用于窗口显示字符串和绘制方块的API，详细内容见《系统支持文档》

## 20102703
- 添加了内存管理相关API，详细内容见《系统支持文档》

## 20102704
- 添加了用于绘制像素点的API，详细内容见《系统支持文档》

## 20102705
- 增加了刷新窗口的API，详细内容见《系统支持文档》

## 20102706
- 增加了窗口直线绘制的API，详细内容见《系统支持文档》

## 20102707
- 增加了关闭窗口的API，详细内容见《系统支持文档》

## 20102801
- 增加了键盘输入的API，详细内容见《系统支持文档》

## 20102802
- 优化了强制结束应用程序时的处理逻辑

## 20102803
- 实现了鼠标切换窗口层(sheet)

## 20102804
- 实现了左键移动窗口

## 20102805
- 实现了用鼠标关闭窗口

## 20102806
- 实现了用鼠标和TAB键切换窗口

## 20102807
- 添加了四个定时器相关的API，详细内容见《系统支持文档》

## 20102808
- 添加了停止定时器的相关功能

## 20102809
- 添加了控制蜂鸣器发声的API，详细内容见《系统支持文档》

## 20102810
- 实现了更多色彩显示

## 20102811
- 调整了应用程序窗口显示逻辑，所有新应用窗口打开默认在屏幕中间

## 20102901
- 实现了多个命令窗口，并且可以同时启动多个应用程序

## 20102902
- 去掉了一些没用的窗口

## 20102903
- 优化了窗口移动图形刷新逻辑，提高了窗口移动流畅度

## 20102904
- 进一步提升了窗口移动流畅度

## 20102905
- 实现了Shift+F2打开新的命令窗口

## 20102906
- 现在可以无限增加命令行窗口

## 20102907
- 新增了用于关闭命令窗口的exit指令，详细内容见《系统支持文档》

## 20102908
- 现在可以通过点击命令窗口右上角的X来关闭命令窗口，功能和exit命令一样

## 20102909
- 新增了start指令，详细内容见《系统支持文档》

## 20103101
- 提升了系统安全性，保护应用程序

## 20103102
- 分离了API文件，让应用程序体积减小
- 优化了Makefile\

## 20103103
- 将API整理成库和头文件，方便程序调用

## 20103104
- 重构了文件结构
- 更改了编译逻辑

## 20103105
- 增加了用于应对栈变量超过4K时，C编译器所需的alloca函数API

## 20103106
- 增加了读取文件的API，详细内容见《系统支持文档》

## 20103107
- 添加了获取命令行参数的API，详细内容见《系统支持文档》

## 20103108
- 增加了对日语(包括SHIFT-JIS和EUC两种模式)的支持
- 增加了查询语言模式的API，详细内容见《系统支持文档》

## 20103109
- 增加了对中文的支持

## 20110101
- 增加了几个应用程序
- 改良了ipl50.nas，优化了启动速度

## 20110102
- 删除了几个应用
- 实现了32位真彩色(0x00000000~0x00ffffff RGB)
- 因为实现了32位色彩，所有buf的类型都从char转换成了int

## 20110103
- 现在VMware和Hyper-V都无法启动KalinoteOS，估计是ipl50.nas的问题

## 20110201
- 上面无法启动的问题应该是修改画面模式为32位色造成的
- 增加了壁纸支持

## 20110202
- 增加了时间显示

## 20110203
- 增加了日期显示

## 20110204
- 调整了命令窗口
- 优化了部分命令

## 20110205
- 改变了标题栏颜色

## 20110208
- 增加了一些应用程序
- 增加了部分标准函数支持

## 20121401
- 更新了标准函数库，增加了部分标准函数

## 20121402
- 更新了kal文件头标准
- 增加了《新kal文件结构标准》描述文档

## 20121801
- 增加了关机指令，详细内容见《系统支持文档》
- 补全了《系统支持文档》
- 增加了部分标准函数API的支持

## 20121901
- 更新了dir(ls)指令，现在dir(ls)指令可以识别更多的文件属性以及可以识别目录了

## 20121902
- 增加了部分代码的注释

## 21010201
- 增加了ide.c，用于读取ide硬盘
- 增加了list.c，用于处理双链表数据
- 增加了lock.c，用于处理进程锁

## 21010202
- 完善了ide硬盘驱动

## 21010301
- 硬盘驱动程序大体完成了，现在如果要进一步编写驱动程序需要先完善部分系统基本功能

## 21010302
- 优化了部分半角字体，修正了键盘布局

## 21010302
- 完善了部分代码的注释

## 21010701
- 增加了sysmode和echo指令，详细内容见《系统支持文档》

## 21010901
- 实现了任务栏层，并将高度固定为1

## 21011001
- 实现了菜单栏的创建、增加选项、显示、隐藏功能

## 21011002
- 做了一个简单的开始菜单，但是还没有功能

## 21011301
- 给菜单加了选择变色功能

## 21011401
- 更改了window的显示方式和sheet结构体的数据结构，使窗口、菜单栏与sheet相互绑定

## 21011501
- 完善了菜单栏API，实现了鼠标点击事件功能

## 21011601
- 实现了多层菜单管理，现在可以无限创建多个子图层

## 21011801
- 增加了用于输出DEBUG信息的DEBUG console窗口

## 21012001
- 实现了IDE硬盘读写功能

## 21012002
- 整理了源代码文件结构

## 21012003
- 实现了按钮点击以及子图层寻找

## 21012301
- 实现了按钮按下和未按下图形状态切换的功能

## 21030701
- 进行了若干功能更新
- 进行了若干性能优化
