project('KittehWatchOS', 'c',  default_options:['c_std=c11', 'cpp_std=c++11'])

cc = meson.get_compiler('c')
pkg = import('pkgconfig')

c_args = [
  '-Og', '-g3',
  '-fvisibility=hidden',
  '-DLV_CONF_INCLUDE_SIMPLE',
]
my_c_args = [
  c_args,
  '-DANOTHER_BRICK_IN_THE', '-Wall',
  '-Wextra',
  '-Wno-unused-parameter',
]

cpp_args = c_args + [
  '-Wno-non-virtual-dtor',
]

link_args = [
'-Wl,-Map=KittehWatchOS.map'
]


deps = [
  #cc.find_library('nosys', required : false),
]
link_with = []


sources = [
	'src/main.c',
  'src/apps/appmenu/appmenu.c',
  'src/apps/clock/clock.c',
  'src/apps/test/test.c',
  'src/displaymanager/displaymanager.c',
]

includes = [
  include_directories('.'),
  include_directories('src'),
  include_directories('deps'),
]


sources_platform_pinetime = [
  'src/platform/PineTime/FreeRTOS/port.c',
  'src/platform/PineTime/FreeRTOS/port_cmsis.c',
  'src/platform/PineTime/FreeRTOS/port_cmsis_systick.c',
  'src/platform/PineTime/backlight/backlight.c',
  'src/platform/PineTime/thread/thread.c',
  'src/platform/PineTime/time/time.c',
  'src/platform/PineTime/display/display.c',
  'src/platform/PineTime/display/St7789.c',
  'src/platform/PineTime/display/spi.c',
  'src/platform/PineTime/display/i2c.c',

    'src/platform/PineTime/misc/misc.c',
]

includes_platform_pinetime = [
  includes,
  include_directories('src/platform/PineTime/FreeRTOS'),
  include_directories('src/platform/PineTime'),
]


sources_platform_linux = [
  'src/platform/Linux/backlight/backlight.c',
  'src/platform/Linux/display/display.c',
  'src/platform/Linux/thread/thread.c',
  'src/platform/Linux/time/time.c',
    'src/platform/Linux/misc/misc.c',

  'deps/lv_drivers/display/monitor.c',
  'deps/lv_drivers/indev/mouse.c',
]

includes_platform_linux = [
  includes,
  include_directories('src/platform/Linux'),
]

lvgl_sources = [
    'deps/lvgl/src/lv_misc/lv_bidi.c',
    'deps/lvgl/src/lv_misc/lv_area.c',
    'deps/lvgl/src/lv_misc/lv_ll.c',
    'deps/lvgl/src/lv_misc/lv_async.c',
    'deps/lvgl/src/lv_misc/lv_txt.c',
    'deps/lvgl/src/lv_misc/lv_utils.c',
    'deps/lvgl/src/lv_misc/lv_templ.c',
    'deps/lvgl/src/lv_misc/lv_txt_ap.c',
    'deps/lvgl/src/lv_misc/lv_gc.c',
    'deps/lvgl/src/lv_misc/lv_math.c',
    'deps/lvgl/src/lv_misc/lv_mem.c',
    'deps/lvgl/src/lv_misc/lv_task.c',
    'deps/lvgl/src/lv_misc/lv_fs.c',
    'deps/lvgl/src/lv_misc/lv_printf.c',
    'deps/lvgl/src/lv_misc/lv_log.c',
    'deps/lvgl/src/lv_misc/lv_debug.c',
    'deps/lvgl/src/lv_misc/lv_color.c',
    'deps/lvgl/src/lv_misc/lv_anim.c',
    'deps/lvgl/src/lv_gpu/lv_gpu_stm32_dma2d.c',
    'deps/lvgl/src/lv_draw/lv_draw_arc.c',
    'deps/lvgl/src/lv_draw/lv_draw_triangle.c',
    'deps/lvgl/src/lv_draw/lv_img_decoder.c',
    'deps/lvgl/src/lv_draw/lv_draw_line.c',
    'deps/lvgl/src/lv_draw/lv_draw_blend.c',
    'deps/lvgl/src/lv_draw/lv_draw_rect.c',
    'deps/lvgl/src/lv_draw/lv_img_buf.c',
    'deps/lvgl/src/lv_draw/lv_img_cache.c',
    'deps/lvgl/src/lv_draw/lv_draw_img.c',
    'deps/lvgl/src/lv_draw/lv_draw_label.c',
    'deps/lvgl/src/lv_draw/lv_draw_mask.c',
    'deps/lvgl/src/lv_core/lv_group.c',
    'deps/lvgl/src/lv_core/lv_obj.c',
    'deps/lvgl/src/lv_core/lv_disp.c',
    'deps/lvgl/src/lv_core/lv_style.c',
    'deps/lvgl/src/lv_core/lv_refr.c',
    'deps/lvgl/src/lv_core/lv_indev.c',
    'deps/lvgl/src/lv_themes/lv_theme_mono.c',
    'deps/lvgl/src/lv_themes/lv_theme_template.c',
    'deps/lvgl/src/lv_themes/lv_theme_empty.c',
    'deps/lvgl/src/lv_themes/lv_theme_material.c',
    'deps/lvgl/src/lv_themes/lv_theme.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_40.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_12_subpx.c',
    'deps/lvgl/src/lv_font/lv_font_simsun_16_cjk.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_16.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_26.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_24.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_18.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_30.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_28.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_46.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_48.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_36.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_34.c',
    'deps/lvgl/src/lv_font/lv_font_fmt_txt.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_32.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_14.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_22.c',
    'deps/lvgl/src/lv_font/lv_font_unscii_8.c',
    'deps/lvgl/src/lv_font/lv_font_loader.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_38.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_28_compressed.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_44.c',
    'deps/lvgl/src/lv_font/lv_font_dejavu_16_persian_hebrew.c',
    'deps/lvgl/src/lv_font/lv_font.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_20.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_42.c',
    'deps/lvgl/src/lv_font/lv_font_montserrat_12.c',
    'deps/lvgl/src/lv_widgets/lv_keyboard.c',
    'deps/lvgl/src/lv_widgets/lv_objmask.c',
    'deps/lvgl/src/lv_widgets/lv_tabview.c',
    'deps/lvgl/src/lv_widgets/lv_objx_templ.c',
    'deps/lvgl/src/lv_widgets/lv_dropdown.c',
    'deps/lvgl/src/lv_widgets/lv_spinbox.c',
    'deps/lvgl/src/lv_widgets/lv_checkbox.c',
    'deps/lvgl/src/lv_widgets/lv_table.c',
    'deps/lvgl/src/lv_widgets/lv_roller.c',
    'deps/lvgl/src/lv_widgets/lv_imgbtn.c',
    'deps/lvgl/src/lv_widgets/lv_linemeter.c',
    'deps/lvgl/src/lv_widgets/lv_canvas.c',
    'deps/lvgl/src/lv_widgets/lv_line.c',
    'deps/lvgl/src/lv_widgets/lv_list.c',
    'deps/lvgl/src/lv_widgets/lv_chart.c',
    'deps/lvgl/src/lv_widgets/lv_led.c',
    'deps/lvgl/src/lv_widgets/lv_arc.c',
    'deps/lvgl/src/lv_widgets/lv_cpicker.c',
    'deps/lvgl/src/lv_widgets/lv_page.c',
    'deps/lvgl/src/lv_widgets/lv_label.c',
    'deps/lvgl/src/lv_widgets/lv_cont.c',
    'deps/lvgl/src/lv_widgets/lv_btnmatrix.c',
    'deps/lvgl/src/lv_widgets/lv_switch.c',
    'deps/lvgl/src/lv_widgets/lv_gauge.c',
    'deps/lvgl/src/lv_widgets/lv_slider.c',
    'deps/lvgl/src/lv_widgets/lv_img.c',
    'deps/lvgl/src/lv_widgets/lv_textarea.c',
    'deps/lvgl/src/lv_widgets/lv_msgbox.c',
    'deps/lvgl/src/lv_widgets/lv_calendar.c',
    'deps/lvgl/src/lv_widgets/lv_bar.c',
    'deps/lvgl/src/lv_widgets/lv_spinner.c',
    'deps/lvgl/src/lv_widgets/lv_btn.c',
    'deps/lvgl/src/lv_widgets/lv_win.c',
    'deps/lvgl/src/lv_widgets/lv_tileview.c',
    'deps/lvgl/src/lv_hal/lv_hal_tick.c',
    'deps/lvgl/src/lv_hal/lv_hal_indev.c',
    'deps/lvgl/src/lv_hal/lv_hal_disp.c',
]

lvgl_includes = [
  'deps',
]

sdk_sources = [
    'deps/nRF5_SDK/modules/nrfx/mdk/system_nrf52.c',
    'deps/nRF5_SDK/modules/nrfx/mdk/gcc_startup_nrf52.S',
    'deps/nRF5_SDK/components/boards/boards.c',
    'deps/nRF5_SDK/integration/nrfx/legacy/nrf_drv_clock.c',
    'deps/nRF5_SDK/modules/nrfx/drivers/src/nrfx_clock.c',
    'deps/nRF5_SDK/modules/nrfx/drivers/src/nrfx_gpiote.c',
    'deps/nRF5_SDK/modules/nrfx/soc/nrfx_atomic.c',
    'deps/nRF5_SDK/modules/nrfx/drivers/src/nrfx_saadc.c',
    'deps/nRF5_SDK/external/freertos/source/croutine.c',
    'deps/nRF5_SDK/external/freertos/source/event_groups.c',
    'deps/nRF5_SDK/external/freertos/source/portable/MemMang/heap_1.c',
    'deps/nRF5_SDK/external/freertos/source/list.c',
    'deps/nRF5_SDK/external/freertos/source/queue.c',
    'deps/nRF5_SDK/external/freertos/source/stream_buffer.c',
    'deps/nRF5_SDK/external/freertos/source/tasks.c',
    'deps/nRF5_SDK/external/freertos/source/timers.c',
    'deps/nRF5_SDK/components/libraries/timer/app_timer_freertos.c',
    'deps/nRF5_SDK/components/libraries/atomic/nrf_atomic.c',
    'deps/nRF5_SDK/components/libraries/balloc/nrf_balloc.c',
    'deps/nRF5_SDK/components/libraries/util/nrf_assert.c',
    'deps/nRF5_SDK/components/libraries/util/app_error.c',
    'deps/nRF5_SDK/components/libraries/util/app_error_weak.c',
    'deps/nRF5_SDK/components/libraries/util/app_error_handler_gcc.c',
    'deps/nRF5_SDK/components/libraries/util/app_util_platform.c',
    'deps/nRF5_SDK/components/libraries/log/src/nrf_log_backend_rtt.c',
    'deps/nRF5_SDK/components/libraries/log/src/nrf_log_backend_serial.c',
    'deps/nRF5_SDK/components/libraries/log/src/nrf_log_default_backends.c',
    'deps/nRF5_SDK/components/libraries/log/src/nrf_log_frontend.c',
    'deps/nRF5_SDK/components/libraries/log/src/nrf_log_str_formatter.c',
    'deps/nRF5_SDK/components/libraries/memobj/nrf_memobj.c',
    'deps/nRF5_SDK/components/libraries/ringbuf/nrf_ringbuf.c',
    'deps/nRF5_SDK/components/libraries/strerror/nrf_strerror.c',
    'deps/nRF5_SDK/external/segger_rtt/SEGGER_RTT_Syscalls_GCC.c',
    'deps/nRF5_SDK/external/segger_rtt/SEGGER_RTT.c',
    'deps/nRF5_SDK/external/segger_rtt/SEGGER_RTT_printf.c',
    'deps/nRF5_SDK/external/utf_converter/utf.c',
    'deps/nRF5_SDK/external/fprintf/nrf_fprintf.c',
    'deps/nRF5_SDK/external/fprintf/nrf_fprintf_format.c',
    'deps/nRF5_SDK/modules/nrfx/drivers/src/nrfx_twi.c',
    'deps/nRF5_SDK/components/libraries/scheduler/app_scheduler.c',
    'deps/nRF5_SDK/components/libraries/fifo/app_fifo.c',
    #'deps/nRF5_SDK/components/libraries/uart/app_uart_fifo.c',
    'deps/nRF5_SDK/components/libraries/button/app_button.c',
    'deps/nRF5_SDK/components/libraries/gpiote/app_gpiote.c',
    'deps/nRF5_SDK/components/libraries/bsp/bsp.c',
    'deps/nRF5_SDK/components/libraries/bsp/bsp_btn_ant.c',
    'deps/nRF5_SDK/components/libraries/bsp/bsp_nfc.c',
    'deps/nRF5_SDK/components/libraries/fds/fds.c',
    'deps/nRF5_SDK/components/libraries/fstorage/nrf_fstorage.c',
    'deps/nRF5_SDK/components/libraries/fstorage/nrf_fstorage_sd.c',
    'deps/nRF5_SDK/components/libraries/fstorage/nrf_fstorage_nvmc.c',
    'deps/nRF5_SDK/components/softdevice/common/nrf_sdh.c',
    'deps/nRF5_SDK/components/softdevice/common/nrf_sdh_ble.c',
    'deps/nRF5_SDK/components/softdevice/common/nrf_sdh_soc.c',
    'deps/nRF5_SDK/components/libraries/experimental_section_vars/nrf_section_iter.c',
    'deps/nRF5_SDK/components/libraries/atomic_fifo/nrf_atfifo.c',



]

sdk_includes = [ 
    'deps/nRF5_SDK/components/toolchain/cmsis/include',
    'deps/nRF5_SDK/external/freertos/source/include',
    'deps/nRF5_SDK/components/softdevice/s132/headers',
    'deps/nRF5_SDK/components/softdevice/s132/headers/nrf52',
    'deps/nRF5_SDK/components/drivers_nrf/nrf_soc_nosd',
    'deps/nRF5_SDK/components',
    'deps/nRF5_SDK/components/boards',
    'deps/nRF5_SDK/components/softdevice/common',
    'deps/nRF5_SDK/integration/nrfx',
    'deps/nRF5_SDK/integration/nrfx/legacy',
    'deps/nRF5_SDK/modules/nrfx',
    'deps/nRF5_SDK/modules/nrfx/drivers/include',
    'deps/nRF5_SDK/modules/nrfx/hal',
    'deps/nRF5_SDK/modules/nrfx/mdk',
    'deps/nRF5_SDK/components/toolchain/cmsis/include',
    'deps/nRF5_SDK/components/libraries/atomic',
    'deps/nRF5_SDK/components/libraries/atomic_fifo',
    'deps/nRF5_SDK/components/libraries/atomic_flags',
    'deps/nRF5_SDK/components/libraries/balloc',
    'deps/nRF5_SDK/components/libraries/bootloader/ble_dfu',
    'deps/nRF5_SDK/components/libraries/cli',
    'deps/nRF5_SDK/components/libraries/crc16',
    'deps/nRF5_SDK/components/libraries/crc32',
    'deps/nRF5_SDK/components/libraries/crypto',
    'deps/nRF5_SDK/components/libraries/csense',
    'deps/nRF5_SDK/components/libraries/csense_drv',
    'deps/nRF5_SDK/components/libraries/delay',
    'deps/nRF5_SDK/components/libraries/ecc',
    'deps/nRF5_SDK/components/libraries/experimental_section_vars',
    'deps/nRF5_SDK/components/libraries/experimental_task_manager',
    'deps/nRF5_SDK/components/libraries/fds',
    'deps/nRF5_SDK/components/libraries/fstorage',
    'deps/nRF5_SDK/components/libraries/gfx',
    'deps/nRF5_SDK/components/libraries/gpiote',
    'deps/nRF5_SDK/components/libraries/hardfault',
    'deps/nRF5_SDK/components/libraries/hci',
    'deps/nRF5_SDK/components/libraries/led_softblink',
    'deps/nRF5_SDK/components/libraries/log',
    'deps/nRF5_SDK/components/libraries/log/src',
    'deps/nRF5_SDK/components/libraries/low_power_pwm',
    'deps/nRF5_SDK/components/libraries/mem_manager',
    'deps/nRF5_SDK/components/libraries/memobj',
    'deps/nRF5_SDK/components/libraries/mpu',
    'deps/nRF5_SDK/components/libraries/mutex',
    'deps/nRF5_SDK/components/libraries/pwm',
    'deps/nRF5_SDK/components/libraries/pwr_mgmt',
    'deps/nRF5_SDK/components/libraries/queue',
    'deps/nRF5_SDK/components/libraries/ringbuf',
    'deps/nRF5_SDK/components/libraries/scheduler',
    'deps/nRF5_SDK/components/libraries/sdcard',
    'deps/nRF5_SDK/components/libraries/slip',
    'deps/nRF5_SDK/components/libraries/sortlist',
    'deps/nRF5_SDK/components/libraries/spi_mngr',
    'deps/nRF5_SDK/components/libraries/stack_guard',
    'deps/nRF5_SDK/components/libraries/strerror',
    'deps/nRF5_SDK/components/libraries/svc',
    'deps/nRF5_SDK/components/libraries/timer',
    'deps/nRF5_SDK/components/libraries/usbd',
    'deps/nRF5_SDK/components/libraries/usbd/class/audio',
    'deps/nRF5_SDK/components/libraries/usbd/class/cdc',
    'deps/nRF5_SDK/components/libraries/usbd/class/cdc/acm',
    'deps/nRF5_SDK/components/libraries/usbd/class/hid',
    'deps/nRF5_SDK/components/libraries/usbd/class/hid/generic',
    'deps/nRF5_SDK/components/libraries/usbd/class/hid/kbd',
    'deps/nRF5_SDK/components/libraries/usbd/class/hid/mouse',
    'deps/nRF5_SDK/components/libraries/usbd/class/msc',
    'deps/nRF5_SDK/components/libraries/util',
    'deps/nRF5_SDK/external/segger_rtt/',
    'deps/nRF5_SDK/external/fprintf/',
    'deps/nRF5_SDK/external/thedotfactory_fonts',
    'deps/nRF5_SDK/components/libraries/scheduler',
    'deps/nRF5_SDK/components/libraries/fifo',
    'deps/nRF5_SDK/components/libraries/uart',
    'deps/nRF5_SDK/components/libraries/button',
    'deps/nRF5_SDK/components/libraries/gpiote',
    'deps/nRF5_SDK/components/libraries/bsp',
    'deps/nRF5_SDK/components/ble/nrf_ble_gatt',
    'deps/nRF5_SDK/components/ble/ble_advertising',
    'deps/nRF5_SDK/components/ble/peer_manager',
    'deps/nRF5_SDK/components/libraries/fds',
    'deps/nRF5_SDK/components/libraries/fstorage',
    'deps/nRF5_SDK/components/libraries/experimental_section_vars',




]

sdk_c_args = [
    '-DFLOAT_ABI_HARD',
    '-DNRF52832_XXAA',
    '-DSOFTDEVICE_PRESENT',
    '-DS132',
    '-DBOARD_PCA10040',
    '-DFREERTOS',
    '-D__HEAP_SIZE=8192',
    '-D__STACK_SIZE=8192',

]



tinygl_sources = [
    'deps/lv_lib_tinygl/src/oscontext.c',
  'deps/lv_lib_tinygl/src/texture.c',
  'deps/lv_lib_tinygl/src/clip.c',
  'deps/lv_lib_tinygl/src/zdither.c',
  'deps/lv_lib_tinygl/src/zline.c',
  'deps/lv_lib_tinygl/src/list.c',
  'deps/lv_lib_tinygl/src/gears.c',
  'deps/lv_lib_tinygl/src/zbuffer.c',
  'deps/lv_lib_tinygl/src/api.c',
  'deps/lv_lib_tinygl/src/memory.c',
  'deps/lv_lib_tinygl/src/error.c',
  'deps/lv_lib_tinygl/src/get.c',
  'deps/lv_lib_tinygl/src/select.c',
  'deps/lv_lib_tinygl/src/arrays.c',
  'deps/lv_lib_tinygl/src/glu.c',
  'deps/lv_lib_tinygl/src/specbuf.c',
  'deps/lv_lib_tinygl/src/light.c',
  'deps/lv_lib_tinygl/src/matrix.c',
  'deps/lv_lib_tinygl/src/image_util.c',
  'deps/lv_lib_tinygl/src/msghandling.c',
  'deps/lv_lib_tinygl/src/vertex.c',
  'deps/lv_lib_tinygl/src/triangle.c',
  'deps/lv_lib_tinygl/src/zmath.c',
  'deps/lv_lib_tinygl/src/ztriangle.c',
  'deps/lv_lib_tinygl/src/clear.c',
  'deps/lv_lib_tinygl/src/init.c',
  'deps/lv_lib_tinygl/src/misc.c',
]

tinygl_includes = [
    'deps/lv_lib_tinygl/src',
]




if not get_option('linux')
#sources += sdk_sources
#includes += sdk_includes
c_args += sdk_c_args
cpp_args += sdk_c_args

sdk_lib = static_library(
  'sdk',
  sources: files(sdk_sources),
  include_directories: sdk_includes + includes_platform_pinetime,
  c_args: c_args
)
sdk = declare_dependency(link_with : sdk_lib)

platform_lib = static_library(
  'platform',
  sources: files(sources_platform_pinetime), 
  include_directories: sdk_includes + includes_platform_pinetime,
  c_args: c_args + my_c_args,
  cpp_args: cpp_args + my_c_args,
  dependencies: sdk
)
platform = declare_dependency(link_with: platform_lib, include_directories: sdk_includes)


deps += sdk
lvgl_includes += includes_platform_pinetime
endif

if get_option('linux')
platform_deps = [
  cc.find_library('pthread', required : true),
  cc.find_library('rt', required : true),
  cc.find_library('SDL2', required : true),
  cc.find_library('m', required : true),
]


platform_lib = static_library(
  'platform',
  sources: files(sources_platform_linux), 
  include_directories: includes + includes_platform_linux,
  c_args: c_args + my_c_args,
  cpp_args: cpp_args + my_c_args,
  dependencies: platform_deps,
)
platform = declare_dependency(link_with: platform_lib, include_directories: includes_platform_linux)
lvgl_includes += includes_platform_linux



endif

deps += platform



lvgl_lib = static_library('lvgl', sources: files(lvgl_sources), include_directories: lvgl_includes, c_args: c_args)
lvgl = declare_dependency(link_with : lvgl_lib, include_directories : lvgl_includes)
deps += lvgl

#tinygl_lib = static_library('tinygl', sources: files(tinygl_sources), include_directories: tinygl_includes, c_args: ['-w'] + c_args)
#tinygl = declare_dependency(link_with : tinygl_lib, include_directories : tinygl_includes)
#deps += tinygl


if not get_option('linux')
link_args += ['-Tgcc_nrf52.ld', '-L..', '-L../deps/nRF5_SDK/modules/nrfx/mdk']
endif

executable('KittehWatchOS',
	name_prefix: '',
	install: false,
	c_args: c_args + my_c_args,
	cpp_args: cpp_args + my_c_args,
  link_args: link_args,
	sources: sources,
  link_with: link_with,
	include_directories: includes,
	dependencies: deps)
