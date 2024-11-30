cmd_u-boot.imx := ./tools/mkimage -n board/freescale/mx6ullevk/imximage-ddr512.cfg.cfgtmp -T imximage -e 0x87800000 -d u-boot.bin u-boot.imx  >/dev/null
