# Panther X2 / RK3566, first SD-card-only bring-up.
include device/rockchip/rk356x/BoardConfig.mk
# Override the reference board's permissive missing-dependency build default.
ALLOW_MISSING_DEPENDENCIES := false
BUILD_WITH_GO_OPT := false
TARGET_BOARD_CPU := RK3566
BOARD_BUILD_GKI := false
BOARD_SELINUX_ENFORCING := true
PRODUCT_UBOOT_CONFIG += rk3566.config
PRODUCT_KERNEL_CONFIG += rk356x.config panther-headless.config
PRODUCT_KERNEL_DTS := rk3566-panther-x2-android

# Only the TF controller may supply Android partitions in the first image.
PRODUCT_BOOT_DEVICE := fe2b0000.dwmmc
TARGET_PREBUILT_KERNEL := device/rockchip/rk356x/rk3566_panther_x2/kernel/Image
BOARD_PREBUILT_DTBIMAGE_DIR := device/rockchip/rk356x/rk3566_panther_x2/kernel/dtb
TARGET_PREBUILT_RESOURCE := device/rockchip/rk356x/rk3566_panther_x2/kernel/resource.img

BOARD_HS_ETHERNET := true
BOARD_SUPPORT_HDMI_CEC := false
BOARD_SENSOR_ST := false
BOARD_GRAVITY_SENSOR_SUPPORT := false
BUILD_WITH_GOOGLE_FRP := false
# The known-working SD boot chain does not include an OP-TEE payload.
PRODUCT_HAVE_OPTEE := false
PRODUCT_USE_SOFTWARE_KEYMINT := true
DEVICE_FRAMEWORK_COMPATIBILITY_MATRIX_FILE += device/rockchip/rk356x/rk3566_panther_x2/software-security-matrix.xml
BOARD_VENDOR_SEPOLICY_DIRS += device/rockchip/rk356x/rk3566_panther_x2/sepolicy
BOARD_GSENSOR_MXC6655XA_SUPPORT := false
BOARD_CAMERA_SUPPORT := false
BOARD_CAMERA_SUPPORT_EXT := false
BOARD_BLUETOOTH_SUPPORT := true
BOARD_BLUETOOTH_LE_SUPPORT := true
# The SDIO/UART combination on this unit is Broadcom; omit unrelated HALs.
override BOARD_HAVE_BLUETOOTH_AIC := false
override BOARD_HAVE_BLUETOOTH_SEEKWAVE := false
override BOARD_HAVE_BLUETOOTH_RTK := false

# Standard Android module packaging supplies dependency metadata and the
# /vendor/lib/modules compatibility path when vendor_dlkm is present.
BOARD_VENDOR_KERNEL_MODULES := \
    device/rockchip/rk356x/rk3566_panther_x2/kernel/modules/dhd_static_buf.ko \
    device/rockchip/rk356x/rk3566_panther_x2/kernel/modules/bcmdhd.ko

# First Hema bring-up uses the reference SDK's supported arm64-only mode.
DEVICE_IS_64BIT_ONLY := true
TARGET_2ND_ARCH :=
TARGET_2ND_ARCH_VARIANT :=
TARGET_2ND_CPU_ABI :=
TARGET_2ND_CPU_ABI2 :=
TARGET_2ND_CPU_VARIANT :=
