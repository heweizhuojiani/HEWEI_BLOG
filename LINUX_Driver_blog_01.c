1. 用于指定模块的初始化和清除函数的宏：
  __init;
  __exit;
  __initdata;
  __exitdata;

2. 常用头文件：
  #include <linux/sched.h>:  最重要的头文件之一，包含驱动程序使用的大部分内核API的定义，包括睡眠函数以及各种变量声明。
  #include <linux/init.h>: 包含了指定模块的初始化和清除函数的宏的头文件，必须的头文件之一。
  #include <linux/module.h>: 包含在模块的源代码中，必须的头文件之一。
  #include <linux/version>: 包含所构造的内核版本信息的头文件。
  #include <linux/moduleparam.h>: 包含了用于创建模块参数的宏的头文件（module_param）。
  #include <linux/kernel.h>: 包含printk等函数的头文件。

3. 常用宏定义：
  module_init()：指定初始化函数。
  module_exit()：指定清除函数。
   __init;
  __exit;
  __initdata;
  __exitdata;
  LINUX_VERSION_CODE：整数宏，在处理版本依赖的预处理条件语句中非常有用。
  EXPORT_SYMBOL(): 用于导出单个符号到讷河的宏。
  EXPORT_SYMBOL_GPL(): 功能同上，但是导出的符号只能使用于GPL许可证下的模块。
  module_param(): 创建模块参数的宏，用户可在装载模块时调整这些参数的值。


