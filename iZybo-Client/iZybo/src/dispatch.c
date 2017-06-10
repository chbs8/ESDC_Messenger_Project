/*
 * Copyright (c) 2009 Xilinx, Inc.  All rights reserved.
 *
 * Xilinx, Inc.
 * XILINX IS PROVIDING THIS DESIGN, CODE, OR INFORMATION "AS IS" AS A
 * COURTESY TO YOU.  BY PROVIDING THIS DESIGN, CODE, OR INFORMATION AS
 * ONE POSSIBLE   IMPLEMENTATION OF THIS FEATURE, APPLICATION OR
 * STANDARD, XILINX IS MAKING NO REPRESENTATION THAT THIS IMPLEMENTATION
 * IS FREE FROM ANY CLAIMS OF INFRINGEMENT, AND YOU ARE RESPONSIBLE
 * FOR OBTAINING ANY RIGHTS YOU MAY REQUIRE FOR YOUR IMPLEMENTATION.
 * XILINX EXPRESSLY DISCLAIMS ANY WARRANTY WHATSOEVER WITH RESPECT TO
 * THE ADEQUACY OF THE IMPLEMENTATION, INCLUDING BUT NOT LIMITED TO
 * ANY WARRANTIES OR REPRESENTATIONS THAT THIS IMPLEMENTATION IS FREE
 * FROM CLAIMS OF INFRINGEMENT, IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS FOR A PARTICULAR PURPOSE.
 *
 */

#include "lwip/inet.h"
#include "lwip/ip_addr.h"
#include "lwip/sys.h"

#include "config_apps.h"
#ifdef __arm__
#include "xil_printf.h"
#endif

void print_utxperf_app_header();
void print_read_app_header();
void print_write_app_header();
void print_rxperf_app_header();
void print_txperf_app_header();
void print_tftp_app_header();
void print_web_app_header();
void print_urxperf_app_header();

void
print_headers()
{
    xil_printf("\r\n");
    xil_printf("%20s %6s %s\r\n", "Server", "Port", "Connect With..");
    xil_printf("%20s %6s %s\r\n", "--------------------", "------", "--------------------");

    /*if (INCLUDE_ECHO_SERVER)
        print_read_app_header();
    	print_write_app_header();*/


    xil_printf("\r\n");
}

void web_application_thread(void *);
void write_application_thread(void *);
void oneSocket_application_thread(void *);
void iZybo_application_thread(void *);
void iZybo_client_thread(void *);
void UDP_application_thread(void *);
void rx_application_thread(void *);
void read_application_thread(void *);
void tftpserver_application_thread(void *);
void utxperf_application_thread(void *);
void urxperf_application_thread(void *);

void
launch_app_threads()
{
    /* start server thread */
	/*
    sys_thread_new("readd", read_application_thread, 0,
            THREAD_STACKSIZE,
            DEFAULT_THREAD_PRIO);

    sys_thread_new("writed", write_application_thread, 0,
			THREAD_STACKSIZE,
			DEFAULT_THREAD_PRIO);

	sys_thread_new("onesocketd", oneSocket_application_thread, 0,
			THREAD_STACKSIZE,
			DEFAULT_THREAD_PRIO);

	sys_thread_new("sequentialUDP", UDP_application_thread, 0,
			THREAD_STACKSIZE,
			DEFAULT_THREAD_PRIO); */

	sys_thread_new("multi_thread_client", iZybo_client_thread, 0,
			THREAD_STACKSIZE,
			DEFAULT_THREAD_PRIO);


}

