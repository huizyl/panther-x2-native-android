# Panther X2 native Android 14 adaptation

黑豹 X2 / RK3566 无显示器原生 Android 14 移植的源码快照、补丁与实测记录。欢迎复用、审查和继续维护。

这是 **源码项目，不附通用刷机固件**。个人设备镜像、出厂参数、MAC、ADB 授权、公私钥、应用和登录数据均不在本仓库。快照保留 19 个实际修改的 SDK 项目及固定基线；不是把整套 AOSP 重新上传一次。

## 已验证的功能

| 项目 | 本机实测范围 |
| --- | --- |
| 启动 | TF Android；eMMC 独立 Android；TF Linux / eMMC Android 切换 |
| 网络 | 千兆以太网、Wi-Fi 开关/扫描/连接、纯无线重启自动重连 |
| 图形 | Mali GPU、720×1280 虚拟显示、RGA 像素对照 |
| 投屏 | 带认证的局域网 ADB + scrcpy；H.264 硬件编码 |
| 解码 | 强制 Rockchip H.264 / HEVC / VP9 解码器，720p 8-bit 合成视频，48 帧、EOS 和像素参考对照 |
| 蓝牙 | 开关、BLE 扫描；LE 扫描 BUSY 重试修复 |
| USB | USB-A 主机读卡器枚举、无 UDC 时的 USB gadget HAL 保护 |
| 存储与启动日志 | eMMC 冷启动；关闭 UART 控制台及 earlycon 后重启通过 |

范围与限制见 [docs/VALIDATION.md](docs/VALIDATION.md)。2026-11-08 只读复核了当前设备启动介质与系统分区；没有在该日重新执行全部硬件压力测试。

## 源码基线和目录

- Android 14：Radxa `Android14-rkr6-rock3c` manifest，起始提交 `424341c143fc3482fd494c64d0e49524aaff2f21`。
- Linux 6.1.99：起始提交 `b2d11ea3307ca36c86c8db3c8cd61a2a66d232b4`。
- `manifests/`：固定清单、不可得厂商提交的替代记录、严格依赖补充。
- `projects/<SDK路径>/changes.patch`：相对于该项目记录基线的真实 tracked 差分。
- `projects/<SDK路径>/untracked/`：实际新增的板型、DTS、软件 Gatekeeper 等源文件。
- `source-snapshot.json`：19 个项目的基线、差分文件和新增源文件摘要。
- `docs/FINAL-BOOT.md`：后续镜像组装阶段的存储选择、解码节点和串口关闭设置。SDK 原始板型文件保留了历史 bring-up 配置，不能直接等同最终运行 DTB。

## 使用

先从固定清单和替代记录准备 Linux 下的 SDK。厂商公开仓库曾存在缺失提交和空仓库，必须读 `manifests/vendor-source-overrides.json`，不能假定未经修正的原始 manifest 可以完整同步。

```sh
python3 tools/apply-overlay.py --sdk /path/to/android14 --check
python3 tools/apply-overlay.py --sdk /path/to/android14
python3 tools/prepare-adb-keys.py --sdk /path/to/android14 --public-key "$HOME/.android/adbkey.pub"
```

应用工具在修改前检查所有项目 HEAD、补丁和新增文件；不会运行刷盘命令。已应用的同一补丁可通过检查，不会重复叠加。

内核需使用板型 headless 配置、匹配的 DTS 和 Broadcom 模块构建，将 `Image`、DTB、`resource.img`、模块放入板型配置引用的位置。随后在 SDK 中：

```sh
source build/envsetup.sh
lunch rk3566_panther_x2-userdebug
export BUILD_BROKEN_DISABLE_BAZEL=true
m -j8
```

保持 `ALLOW_MISSING_DEPENDENCIES=false`；不要跳过 VINTF、ELF 或属性检查。完整映像还需要匹配的引导器和介质选择组装，见 [docs/FINAL-BOOT.md](docs/FINAL-BOOT.md)。本仓库不提供直接写物理磁盘的安装器，也不附带开发签名私钥。

## 维护注意

这是 ARM64、userdebug、软件 KeyMint/Gatekeeper、无物理 HDMI 的实验适配。系统/vendor 安全补丁仍为 **2025-01-05**，没有官方 OTA；更新 WebView 不会更新整个 Android。不要将它标注为官方安全维护版本。受保护内容、真实摄像头/人脸认证、银行或 DRM 应用可能无法使用。

源码和原作者信息按各组件许可保留，见 [LICENSES.md](LICENSES.md)。项目没有普适性能承诺，首次网页/购物搜索的间歇长等待也未证明已根治。
