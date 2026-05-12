cmake_minimum_required(VERSION 3.20)
project(ProxyDriver C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Kernel headers for IDE indexing (not actually compiled)
execute_process(COMMAND uname -r OUTPUT_VARIABLE KERNEL_RELEASE OUTPUT_STRIP_TRAILING_WHITESPACE)
set(KERNEL_HEADERS /lib/modules/${KERNEL_RELEASE}/build/include)
include_directories(
        ${KERNEL_HEADERS}
        ${KERNEL_HEADERS}/uapi
        /lib/modules/${KERNEL_RELEASE}/build/arch/x86/include
        /lib/modules/${KERNEL_RELEASE}/build/arch/x86/include/generated
        /lib/modules/${KERNEL_RELEASE}/build/include/generated
)
add_definitions(-D__KERNEL__ -DMODULE)

# Fake target for CLion indexing only
add_library(charproxy_ide EXCLUDE_FROM_ALL
        charproxy_main.c
        charproxy_fops.c
        charproxy_fops.h
        ioctl_proxy.h
)

add_custom_target(charproxy_module ALL
        COMMAND make
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Building Linux kernel module"
        SOURCES charproxy_main.c charproxy_fops.c charproxy_fops.h
)

add_custom_target(clean_module
        COMMAND make clean
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Cleaning Linux kernel module"
)

add_custom_target(load_module
        COMMAND sudo insmod charproxy.ko
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Loading charproxy module"
)

add_custom_target(unload_module
        COMMAND sudo rmmod charproxy
        WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
        COMMENT "Unloading charproxy module"
)