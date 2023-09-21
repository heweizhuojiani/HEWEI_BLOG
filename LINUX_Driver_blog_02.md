# 字符设备驱动程序

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
    分配设备号包括动态分配和静态分配。
    涉及到的函数按下不表，值得注意的是不能在设备使用时释放设备号。
    设备还涉及到权限问题（尤其是通过脚本的方式进行创建）：当使用脚本创建设备文件时候，脚本通常由root用
户允许，则创建的设备文件所属的用户自然是root。默认的权限只有root对其有写访问权，而其他用户只有读权限，例如脚本：
```shell
                        module="scull"
                        device="scull"
                        mode="664"
                        
                        if grep -q '^staff:' /etc/group; then
                            group="staff"
                        else
                            group="wheel"
                        fi
                        
                        /sbin/insmod ./$module.ko $* || exit 1
                        major=$(awk "\$2==\"$module\" {print \$1}" /proc/devices)
                        
                        rm -f /dev/${device}[0-3]
                        mknod /dev/${device}0 c $major 0
                        mknod /dev/${device}1 c $major 1
                        mknod /dev/${device}2 c $major 2
                        mknod /dev/${device}3 c $major 3
                        ln -sf ${device}0 /dev/${device}
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
