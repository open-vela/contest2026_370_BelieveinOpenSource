# STM32N647 DNN647 board - src
# NuttX creates the global board library before including this directory.
target_sources(board PRIVATE board_boot.c)

if(CONFIG_DNN647_USERLEDS)
  target_sources(board PRIVATE dnn647_userled.c)
endif()

if(CONFIG_DNN647_ES8388_PROBE)
  target_sources(board PRIVATE dnn647_es8388_probe.c)
endif()

if(CONFIG_BOOT_RUNFROMISRAM)
  set_property(GLOBAL PROPERTY LD_SCRIPT
               "${NUTTX_BOARD_ABS_DIR}/scripts/ld-debug.script")
else()
  set_property(GLOBAL PROPERTY LD_SCRIPT
               "${NUTTX_BOARD_ABS_DIR}/scripts/ld.script")
endif()
