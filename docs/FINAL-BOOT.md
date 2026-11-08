# 最终镜像组装与历史板型的区别

源码快照保留实际 SDK 文件和基线，不悄悄把后续离线镜像修补伪装成已经整包重编译过的源码状态。

1. 最终已验收系统仍为原 6.1.99 #3 内核。Wi-Fi、RGA、编码和解码等后续实验优先通过单项 DTS / vendor 修复验收，没有将曾无法启动的多项 R7 候选作为稳定基线。
2. 解码组装额外开启 `/rkvdec@fdf80200` 与 `/iommu@fdf80800` 的 `status=okay`。保持其余已验证参数；不是开启所有 VPU 节点。
3. 介质必须明确选择。TF 系统只让 `fe2b0000.dwmmc` 提供 Android 分区；eMMC 系统使用 `fe310000.sdhci`。对应控制器的 DT 状态、命令行 `androidboot.boot_devices`、分区布局和 initramfs 必须一致。不要混用两个控制器的同名 by-name 分区。
4. eMMC 最终版本关闭串口：从 bootargs 删除 `androidboot.console=ttyFIQ0`，将 `console=ttyFIQ0` 改为 `console=null`；DT `/chosen/bootargs` 为空、删除 `/chosen/stdout-path`、`/fiq-debugger/status=disabled`。重新生成 `boot.scr` 时必须同时更新 U-Boot 镜像头 CRC 和数据 CRC，不能直接字符串覆盖二进制。
5. 全新 userdata 和 metadata 是发布镜像的独立输入。不要复制某台已登录设备的这两个分区，不捆绑其 ADB 公钥、Wi-Fi、出厂 NVM、MAC 或设备编号。
6. 本项目不分发参考 idbloader/U-Boot 二进制，不声称 Android 内核源码能重建外部 bootloader。用户需从对应上游获取有合法来源且兼容自己硬件的引导链。整盘 GPT 原始镜像不是 Rockchip `update.img`。
7. 安装到 eMMC 要从另一介质启动，识别真实 MMC/SD 类型和目标容量，保留自己的厂商参数，写完完整回读，并留可靠 TF 恢复入口。不要从正在运行的同一 eMMC 覆盖自己。

eMMC 和 TF 都曾实测启动，但本仓库仅发布源快照与方法，未提供完成上述所有步骤的通用无人值守发布构建。不同批次、全新 userdata 首启、不同外部引导链仍需独立验证。

上游来源：[Radxa manifests](https://github.com/radxa/manifests)、[Rockchip kernel](https://github.com/rockchip-linux/kernel)、[ophub bootloader](https://github.com/ophub/u-boot)、[U-Boot Rockchip 文档](https://docs.u-boot.org/en/latest/board/rockchip/rockchip.html)。
