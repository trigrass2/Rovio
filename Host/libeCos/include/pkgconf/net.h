#ifndef CYGONCE_PKGCONF_NET_H
#define CYGONCE_PKGCONF_NET_H
/*
 * File <pkgconf/net.h>
 *
 * This file is generated automatically by the configuration
 * system. It should not be edited. Any changes to this file
 * may be overwritten.
 */

#include "pkgconf/system.h"
#include CYGDAT_NET_STACK_CFG
#define CYGPKG_NET_DRIVER_FRAMEWORK 1
#define CYGPKG_NET_DRIVER_FRAMEWORK_1
#define CYGPKG_NET_STACK 1
#define CYGPKG_NET_STACK_1
#define CYGPKG_NET_STACK_INET 1
#define CYGPKG_NET_STACK_INET_1
#define CYGPKG_NET_STACK_INET6 0
#define CYGPKG_NET_STACK_INET6_0
#define INET 1
#define CYGPKG_NET_TFTP 1
#define CYGPKG_NET_TFTPD_THREAD_PRIORITY 10
#define CYGPKG_NET_TFTPD_THREAD_PRIORITY_10
#define CYGPKG_NET_TFTP_FILE_ACCESS 1
#define CYGPKG_NET_DHCP 1
#define CYGOPT_NET_DHCP_DHCP_THREAD 1
#define CYGOPT_NET_DHCP_DHCP_THREAD_PARAM 1
#define CYGOPT_NET_DHCP_DHCP_THREAD_PARAM_1
#define CYGPKG_NET_DHCP_THREAD_PRIORITY 8
#define CYGPKG_NET_DHCP_THREAD_PRIORITY_8
#define CYGHWR_NET_DRIVERS 1
#define CYGHWR_NET_DRIVERS_1
#ifndef RW99702
#define CYGHWR_NET_DRIVER_ETH0 1
#define CYGHWR_NET_DRIVER_ETH0_BOOTP 1
#define CYGHWR_NET_DRIVER_ETH0_DHCP 1
#define CYGHWR_NET_DRIVER_ETH0_BOOTP_SHOW 1
#else
#define CYGHWR_NET_DRIVER_WLAN0 1
#define CYGHWR_NET_DRIVER_WLAN0_BOOTP 1
#define CYGHWR_NET_DRIVER_WLAN0_DHCP 1
#define CYGHWR_NET_DRIVER_WLAN0_BOOTP_SHOW 1
#endif

#endif
