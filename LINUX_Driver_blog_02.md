# 字符设备驱动程序——自动化脚本

### 1. 主次设备号

        主设备号通常标识设备对应的驱动程序，次设备号用来正确确定设备文件所指向的设备。现代的LINUX内核允许
    多个驱动程序对应一个主设备号，但是多数设备仍然按照一个主设备号对应一个驱动程序的方式，而内核对于次设备
    号，只关心它所指向的设备，其他的信息内核并不关心。

### 2. 设备编号的内部表达

        在内核中，dev_t类型（在<linux/types.h>中定义）用来保存设备编号————包括主设备号和次设备号。在2.6
    版本的内核中，它是一个32位的数据，其中12位表示主设备号，20位表示次设备号。（但我们在使用时不允许通过这
    种方式去赋值主次设备号）
    在<linux/kdev_t.h>中专门定义了两个宏从 dev_t 解析出主次设备号：
      **MAJOR(dev_t dev);
        MINOR(dev_t dev);**
    也有一个宏用于通过主次设备号创建一个 dev_t 变量：
      **MKDEV(int major, int minor);**

### 3. 分配和释放设备编号

```shell
	分配设备号包括动态分配和静态分配。
	涉及到的函数按下不表，值得注意的是不能在设备使用时释放设备号。
	设备还涉及到权限问题（尤其是通过脚本的方式进行创建）：当使用脚本创建设备文件时候，脚本通常由root用户允许
，则创建的设备文件所属的用户自然是root。默认的权限只有root对其有写访问权，而其他用户只有读权限，例如脚本：
```

```shell
### ————加载模块脚本       
        #定义内核模块名称及相关设备的名称。
        module="scull"
        device="scull"
        #定义权限定义设备节点的权限模式
        mode="664"

        #此处使用了正则表达式，匹配以staff:开头的行，其中^在正则表达式中表示行首，-q表示静默输出。
        if grep -q '^staff:' /etc/group; then
        group="staff"
        else
        group="wheel"
        fi
        
		#shell脚本语法：|| 表示如果前面的任务如果失败，则执行后面的程序。（注意，此处insmod使用了全路径）
        /sbin/insmod ./$module.ko $* || exit 1
        #正则表达式：$2在awk中表示第二个字段（默认式用空格或tab符分割），表示在/proc/devices中匹配第二个字段为$module（此处为scull）的行，如果匹配成功，则输出改行的第一个字段（print执行）。
        major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
		
		#在创建新的设备节点前，删除可能存在的同名节点是一个良好的做法。以确保你能够创建具有所需权限和配置的新设备节点。
        rm -f /dev/${device}[0-3]
        #创建新的设备节点，其中主设备号是相同的（major），
        mknod /dev/${device}0 c $major 0
        mknod /dev/${device}1 c $major 1
        mknod /dev/${device}2 c $major 2
        mknod /dev/${device}3 c $major 3
        #创建符号链接，此处仅仅创建关于第一个设备的链接，因为后面三个设备均可以通过第一个加偏移数字来找到。
        #ln -sf 命令：ln 是创建链接的命令，-s 选项表示创建符号链接，-f 选项表示如果目标文件已存在，则强制覆盖它。
        ln -sf ${device}0 /dev/${device}
        #设置用户组和权限
        chgrp $group /dev/${device}[0-3] 
        chmod $mode  /dev/${device}[0-3]

        rm -f /dev/${device}pipe[0-3]
        mknod /dev/${device}pipe0 c $major 4
        mknod /dev/${device}pipe1 c $major 5
        mknod /dev/${device}pipe2 c $major 6
        mknod /dev/${device}pipe3 c $major 7
        ln -sf ${device}pipe0 /dev/${device}pipe
        chgrp $group /dev/${device}pipe[0-3] 
        chmod $mode  /dev/${device}pipe[0-3]

        rm -f /dev/${device}single
        mknod /dev/${device}single  c $major 8
        chgrp $group /dev/${device}single
        chmod $mode  /dev/${device}single

        rm -f /dev/${device}uid
        mknod /dev/${device}uid   c $major 9
        chgrp $group /dev/${device}uid
        chmod $mode  /dev/${device}uid

        rm -f /dev/${device}wuid
        mknod /dev/${device}wuid  c $major 10
        chgrp $group /dev/${device}wuid
        chmod $mode  /dev/${device}wuid

        rm -f /dev/${device}priv
        mknod /dev/${device}priv  c $major 11
        chgrp $group /dev/${device}priv
        chmod $mode  /dev/${device}priv
```

```
	由于动态分配的设备号无法直接使用，所以先获取动态分配的设备号，其中典型的/proc/devices文件主要包含两个字段，即 主设备号 内核模块名称，以空格分隔。故可以通过awk等工具很容易查找模块对应的主设备号。
```

```shell
##————卸载模块
#!/bin/sh
module="scull"
device="scull"

# invoke rmmod with all arguments we got
/sbin/rmmod $module $* || exit 1

# Remove stale nodes
rm -f /dev/${device} /dev/${device}[0-3] 
rm -f /dev/${device}priv
rm -f /dev/${device}pipe /dev/${device}pipe[0-3]
rm -f /dev/${device}single
rm -f /dev/${device}uid
rm -f /dev/${device}wuid

```

```
	以上代码是卸载模块的脚本。我们在正式使用该模块时，可以单独写一个init脚本，将其放进发行版的init脚本目录中，较为复杂却更加规范：
```

```shell
#————init模块
#!/bin/bash
# Sample init script for the a driver module <rubini@linux.it>

DEVICE="scull"
SECTION="misc"

# The list of filenames and minor numbers: $PREFIX is prefixed to all names
PREFIX="scull"
#FILES变量包含一组设备文件名和次设备号。这些文件名和次设备号将用于创建设备节点。
FILES="     0 0         1 1         2 2        3 3    priv 16 
        pipe0 32    pipe1 33    pipe2 34   pipe3 35
       single 48      uid 64     wuid 80"

#设置加载模块的命令（可选择insmod或modprobe）
INSMOD=/sbin/insmod; # use /sbin/modprobe if you prefer

#这是两个分别用于在加载和卸载模块时执行的函数，用于在加载和卸载设备模块时执行设备特定的操作。此处的操作时true函数，即不做任何处理。
function device_specific_post_load () {
    true; # fill at will
}
function device_specific_pre_unload () {
    true; # fill at will
}

# Everything below this line should work unchanged for any char device.
# Obviously, however, no options on the command line: either in
# /etc/${DEVICE}.conf or /etc/modules.conf (if modprobe is used)

# Optional configuration file: format is
#    owner  <ownername>
#    group  <groupname>
#    mode   <modename>
#    options <insmod options>
#提供了我们自己的conf文件，变量包含配置文件的路径，用于指定设备的所有者、组、权限和其他选项。（此处只提供了conf文件的路径，文件的创建和编写均需要我们自己完成）
CFG=/etc/${DEVICE}.conf

# kernel version, used to look for modules
KERNEL=`uname -r`

#FIXME: it looks like there is no misc section. Where should it be?
##FIXME 是一种常见的代码注释格式，用于标记代码中的问题或需要解决的问题。此处是作者给读者提出的一个疑问：如果是misc（杂项设备，即不属于三种驱动中的模块），该如何处理？总的来说，这句注释的用意是提醒脚本的维护者，特别是在与 "misc" 设备相关的处理方面，可能存在一些未解决的问题或需要进一步思考和补充的地方。这有助于提高脚本的可维护性和可扩展性，以适应未来可能出现的需求变化。

#模块不仅仅会存在于这两个文件夹中。这段代码中的逻辑是为了尝试查找设备模块的默认位置，
MODDIR="/lib/modules/${KERNEL}/kernel/drivers/${SECTION}"
if [ ! -d $MODDIR ]; then MODDIR="/lib/modules/${KERNEL}/${SECTION}"; fi

# Root or die
#此处判断是否是以root身份允许脚本，如果不是则报错并退出脚本。$(id -u) 是一个命令替换，用于获取当前用户的用户ID（UID）。
if [ "$(id -u)" != "0" ]
then
  echo "You must be root to load or unload kernel modules"
  exit 1
fi

# Read configuration file
#此处读取配置文件，通过正则表达式将
if [ -r $CFG ]; then
    OWNER=`awk "\\$1==\"owner\" {print \\$2}" $CFG`
    GROUP=`awk "\\$1==\"group\" {print \\$2}" $CFG`
    MODE=`awk "\\$1==\"mode\" {print \\$2}" $CFG`
    
    # The options string may include extra blanks or only blanks
    #执行以下 sed 命令后，它会扫描配置文件 $CFG，找到所有以 "options " 开头的行，并将这些行中 "options " 后面的内容提取出来。并存放到变量OPTIONS中。
    #此处使用了sed工具 + 正则表达式，sed -n表示静默输出，/^options /表示匹配以optinos开头的行，s/options //表示将options 删掉，/p表示将结果打印出来。（s/<被替换项/<替换项>/)
    #因此，此处的sed工具的格式如下：sed -n '/<匹配项>/ s/<被替换项>/<替换项>//p' 文本路径
    OPTIONS=`sed -n '/^options / s/options //p' $CFG`
fi


# Create device files
function create_files () {
    cd /dev
    #此处声明了两个局部变量，分别是devlist（初始化为空字符串）和file（用于临时存储设备文件名称）。
    local devlist=""
    local file
    #此处是一个无限循环的开始，条件是判断参数是否小于2，如果参数个数小于2，说明没有足够的参数创建设备文件，则中断，否则就执行创建设备文件的任务。
    while true; do
	#此处 $# 为传入函数的参数个数，-lt 是一个比较运算符，表示 "小于"。
	if [ $# -lt 2 ]; then break; fi
	file="${DEVICE}$1"
	mknod $file c $MAJOR $2
	devlist="$devlist $file"
	shift 2
    done
    if [ -n "$OWNER" ]; then chown $OWNER $devlist; fi
    if [ -n "$GROUP" ]; then chgrp $GROUP $devlist; fi
    if [ -n "$MODE"  ]; then chmod $MODE  $devlist; fi
}

# Remove device files
function remove_files () {
    cd /dev
    local devlist=""
    local file
    while true; do
	if [ $# -lt 2 ]; then break; fi
	file="${DEVICE}$1"
	devlist="$devlist $file"
	shift 2
    done
    rm -f $devlist
}

# Load and create files
function load_device () {
    
    if [ -f $MODDIR/$DEVICE.o ]; then
	devpath=$MODDIR/$DEVICE.o
    else if [ -f ./$DEVICE.o ]; then
	devpath=./$DEVICE.o
    else
	devpath=$DEVICE; # let insmod/modprobe guess
    fi; fi
    if [ "$devpath" != "$DEVICE" ]; then
	echo -n " (loading file $devpath)"
    fi

    if $INSMOD $devpath $OPTIONS; then
	MAJOR=`awk "\\$2==\"$DEVICE\" {print \\$1}" /proc/devices`
	remove_files $FILES
	create_files $FILES
	device_specific_post_load
    else
	echo " FAILED!"
     fi
}

# Unload and remove files
function unload_device () {
    device_specific_pre_unload 
    /sbin/rmmod $DEVICE
    remove_files $FILES
}


case "$1" in
  start)
     echo -n "Loading $DEVICE"
     load_device
     echo "."
     ;;
  stop)
     echo -n "Unloading $DEVICE"
     unload_device
     echo "."
     ;;
  force-reload|restart)
     echo -n "Reloading $DEVICE"
     unload_device
     load_device
     echo "."
     ;;
  *)
     echo "Usage: $0 {start|stop|restart|force-reload}"
     exit 1
esac

exit 0

```

