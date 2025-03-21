## 概述
该项目实现了基于 Arm64 体系结构的一个简单的裸机 OS 。

## 硬件平台
树梅派 5（4G内存），其硬件参数如下：
* 处理器：Broadcom BCM2712 (Arm Cortex A76) 64 位 2.4GHz 四核，512KB 二级缓存和 2MB 共享三级缓存
* 内存：PI5-1GB：1GB PI5-2GB：2GB PI5-4GB：4GB PI5-8GB：8GB

......

## 构建

```shell
$ make
```

## qemu 仿真
```shell
$ make qemu
Hello World from Arm64OS!
printk init done.
> 
```
