#
# Copyright 2014 The Android Open-Source Project
#
# Licensed under the Apache License, Version 2.0 (the "License");
# you may not use this file except in compliance with the License.
# You may obtain a copy of the License at
#
#      http://www.apache.org/licenses/LICENSE-2.0
#
# Unless required by applicable law or agreed to in writing, software
# distributed under the License is distributed on an "AS IS" BASIS,
# WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
# See the License for the specific language governing permissions and
# limitations under the License.
#

# First lunching is U, api_level is 34
PRODUCT_SHIPPING_API_LEVEL := 34
PRODUCT_DTBO_TEMPLATE := $(LOCAL_PATH)/dt-overlay.in
PRODUCT_SDMMC_DEVICE := 00000000.dwmmc

BOARD_SUPPORT_MULTIAUDIO := true

PRODUCT_BOOT_DEVICE := fe310000.sdhci,fe2b0000.dwmmc

include device/rockchip/common/build/rockchip/DynamicPartitions.mk
include device/rockchip/rk356x/rk3566_panther_x2/BoardConfig.mk
include device/rockchip/common/BoardConfig.mk
$(call inherit-product, device/rockchip/rk356x/device.mk)
$(call inherit-product, device/rockchip/common/device.mk)
$(call inherit-product, frameworks/native/build/tablet-10in-xhdpi-2048-dalvik-heap.mk)

DEVICE_PACKAGE_OVERLAYS += $(LOCAL_PATH)/../overlay

PRODUCT_CHARACTERISTICS := tablet

PRODUCT_NAME := rk3566_panther_x2
PRODUCT_DEVICE := rk3566_panther_x2
PRODUCT_BRAND := Panther
PRODUCT_MODEL := rk3566_panther_x2
PRODUCT_MANUFACTURER := Panther
PRODUCT_AAPT_PREF_CONFIG := mdpi
#
## add Rockchip properties
#
PRODUCT_PROPERTY_OVERRIDES += ro.sf.lcd_density=320
PRODUCT_PROPERTY_OVERRIDES += ro.wifi.sleep.power.down=false
PRODUCT_PROPERTY_OVERRIDES += persist.wifi.sleep.delay.ms=0
PRODUCT_PROPERTY_OVERRIDES += persist.bt.power.down=true
PRODUCT_PROPERTY_OVERRIDES += ro.vendor.hdmirotationlock=false
PRODUCT_PROPERTY_OVERRIDES += vendor.hwc.device.primary=Virtual

USE_PRODUCT_RESOLUTION_WHITE := false
PRODUCT_PROPERTY_OVERRIDES += persist.sys.hdmi_dp_audio_output=1
# Force GPU composition for the virtual scanout path.
PRODUCT_VENDOR_PROPERTIES += vendor.hwc.compose_policy=0

# Only the owner PC and NAS public keys are included.
PRODUCT_ADB_KEYS := device/rockchip/rk356x/rk3566_panther_x2/adb-authorized/adb_keys
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += ro.adb.secure=1
PRODUCT_SYSTEM_DEFAULT_PROPERTIES += persist.adb.tcp.port=5555
PRODUCT_COPY_FILES += device/rockchip/rk356x/rk3566_panther_x2/init.panther.rc:system/etc/init/init.panther.rc

# This headless tablet has no automotive surround-view camera backend.
$(call soong_config_set,rockchip,disable_automotive_evs,true)

PRODUCT_VENDOR_PROPERTIES += vendor.gralloc.no_afbc_for_fb_target_layer=1

# No physical VOP IOMMU: only the final client target uses CMA.
PRODUCT_VENDOR_PROPERTIES += vendor.gralloc.contiguous_client_target=1

# Physical board: the SDK omits the optional PC gfxstream renderer.
$(call soong_config_set,rockchip,disable_host_gfxstream,true)

# Cuttlefish router images are outside this physical-board product.
$(call soong_config_set,rockchip,disable_openwrt_images,true)

# Kernel 6.1 uses CPU cgroups/uclamp. Keep the AOSP profiles instead of
# overriding them with Rockchip's legacy schedtune-only configuration.
$(call soong_config_set,rockchip,panther_use_uclamp_profiles,true)
