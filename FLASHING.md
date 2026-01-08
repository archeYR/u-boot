# lisa

```
make CROSS_COMPILE=aarch64-linux-gnu- O=.output qcom_defconfig
make CROSS_COMPILE=aarch64-linux-gnu- O=.output -j$(nproc) DEVICE_TREE=qcom/sm7325-xiaomi-lisa
gzip .output/u-boot-nodtb.bin -c > .output/u-boot-nodtb.bin.gz
mkbootimg \
    --kernel .output/u-boot-nodtb.bin.gz \
    --dtb .output/dts/upstream/src/arm64/qcom/sm7325-xiaomi-lisa.dtb \
    --base 0x0 \
    --kernel_offset 0x8000 \
    --ramdisk_offset 0x1000000 \
    --tags_offset 0x100 \
    --pagesize 4096 \
    --header_version 2 \
    -o .output/u-boot.img
fastboot flash boot .output/u-boot.img
```

# mondrian

```
make CROSS_COMPILE=aarch64-linux-gnu- O=.output qcom_defconfig qcom-phone.config
make menuconfig O=.output
make CROSS_COMPILE=aarch64-linux-gnu- O=.output -j$(nproc) DEVICE_TREE=qcom/sm8475-xiaomi-mondrian
gzip .output/u-boot-dtb.bin -c > .output/u-boot-dtb.bin.gz
mkbootimg --kernel .output/u-boot-dtb.bin.gz --ramdisk ramdisk --base 0x0 --kernel_offset 0x0 --ramdisk_offset 0x0 --tags_offset 0x0 --os_version 15.0.0 --os_patch_level 2025-05 --pagesize 4096 --header_version 4 -o .output/u-boot.img
fastboot flash boot .output/u-boot.img
```
