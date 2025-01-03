sdk_generate_library()
sdk_library_add_sources(FreeRTOS-Kernel/croutine.c)
sdk_library_add_sources(FreeRTOS-Kernel/event_groups.c)
sdk_library_add_sources(FreeRTOS-Kernel/list.c)
sdk_library_add_sources(FreeRTOS-Kernel/queue.c)
sdk_library_add_sources(FreeRTOS-Kernel/tasks.c)
sdk_library_add_sources(FreeRTOS-Kernel/timers.c)
sdk_library_add_sources(port/freertos_port.c)
sdk_library_add_sources(FreeRTOS-Kernel/portable/MemMang/heap_3.c)

sdk_add_include_directories(FreeRTOS-Kernel/include)

sdk_add_compile_definitions(-DCONFIG_FREERTOS)

if(("${CHIP}" STREQUAL "bl808") AND ("${CPU_ID}" STREQUAL "d0"))
sdk_library_add_sources(port/GCC/RISC-V/c906/port.c)
sdk_library_add_sources(port/GCC/RISC-V/c906/portASM.S)
sdk_add_include_directories(port/GCC/RISC-V/c906)
sdk_add_include_directories(port/GCC/RISC-V/c906/chip_specific_extensions/Thead_E906FD_RV32IMAFDC)
sdk_add_compile_definitions(-D__OS_FREERTOS__)
else()
sdk_library_add_sources(port/GCC/RISC-V/common/port.c)
sdk_library_add_sources(port/GCC/RISC-V/common/portASM.S)
sdk_add_include_directories(port/GCC/RISC-V/common)
sdk_add_include_directories(port/GCC/RISC-V/common/chip_specific_extensions/RV32I_CLINT_no_extensions)
sdk_add_compile_definitions(-DportasmHANDLE_INTERRUPT=interrupt_entry -Ddefault_interrupt_handler=freertos_risc_v_trap_handler)

endif()

if(CONFIG_POSIX)
# posix for freertos
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_clock.c)
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_mqueue.c)
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread.c)
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_barrier.c)
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_cond.c)
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_pthread_mutex.c)
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_sched.c)
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_semaphore.c)
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_timer.c)
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_unistd.c)
sdk_library_add_sources(posix/FreeRTOS-Plus-POSIX/source/FreeRTOS_POSIX_utils.c)
sdk_add_include_directories(posix/include)
sdk_add_include_directories(posix/include/private)
sdk_add_include_directories(posix/FreeRTOS-Plus-POSIX/include)
sdk_add_include_directories(posix/FreeRTOS-Plus-POSIX/include/portable)
sdk_add_include_directories(port)
sdk_add_compile_definitions(-DCONFIG_POSIX)
endif()
