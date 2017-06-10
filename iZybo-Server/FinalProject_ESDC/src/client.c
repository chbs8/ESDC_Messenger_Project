/*
 * Copyright (c) 2007-2009 Xilinx, Inc.  All rights reserved.
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

#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwipopts.h"

#include "config_apps.h"
#ifdef __arm__
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#endif

#define SEND_BUFSIZE (1440)

static char send_buf[SEND_BUFSIZE];
void read_application_thread(void *);

u16_t write_port = 5002;

void print_write_app_header()
{
 //   xil_printf("%20s %6d %s\r\n", "echo server",
	printf("%20s %6d %s\r\n", "echo server",
                        write_port,
                        "$ telnet <board_ip> 5001");

}

/* thread spawned for each connection */
void process_write_request(void *p)
{
	int sd = (int)p;
	int n;

	bzero(send_buf, SEND_BUFSIZE);

	xil_printf("Now you can send your messages\r\n");

	while (1) {

		// Sending message
		xil_printf("Me: ");
		if(fgets(send_buf, SEND_BUFSIZE, stdin) != NULL) {
			if ((n = lwip_write(sd, send_buf, SEND_BUFSIZE)) < 0) {
				xil_printf("Error sending to socket %d\r\n", p);
			}
		}
		else {
			printf("Error fgets!\r\n");
		}

		memset(send_buf, '\0', SEND_BUFSIZE);
		//sleep(2);
	}

	/* close connection */
	close(sd);
	vTaskDelete(NULL);
}

void write_application_thread()
{
	int sock2, new_sd2;
	struct sockaddr_in addressW, remoteW;
	int size;
	struct sockaddr_in serv_addr;

	if ((sock2 = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_addr.s_addr = inet_addr("147.83.81.225");
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_port = htons(write_port);

	xil_printf("\r\nClient socket initialized -> Connecting to 147.83.81.225\r\n");

	/*Connect to the server*/
	while(lwip_connect(sock2, (struct sockaddr*)&serv_addr, sizeof(serv_addr)) < 0);


	xil_printf("Connected to server\r\n");
	lwip_write(sock2, send_buf, SEND_BUFSIZE);
	sys_thread_new("servers", process_write_request,
		(void*)sock2,
		THREAD_STACKSIZE,
		DEFAULT_THREAD_PRIO);
	    sys_thread_new("readd", read_application_thread, 0,
	            THREAD_STACKSIZE,
	            DEFAULT_THREAD_PRIO);

//	if ((sock2 = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
//		return;
//
//	addressW.sin_family = AF_INET;
//	addressW.sin_port = htons(write_port);
//	addressW.sin_addr.s_addr = INADDR_ANY;
//
//	if (lwip_bind(sock2, (struct sockaddr *)&addressW, sizeof (addressW)) < 0)
//		return;
//
//	lwip_listen(sock2, 0);
//
//	xil_printf("\r\nWaiting User 2 to connect... writing side \r\n");
//
//	size = sizeof(remoteW);
//
//	while (1) {
//		if ((new_sd2 = lwip_accept(sock2, (struct sockaddr *)&remoteW, (socklen_t *)&size)) > 0) {
//			xil_printf("\r\nWrite socket accepted: %d\r\n", new_sd2);
//			sys_thread_new("servers", process_write_request,
//				(void*)new_sd2,
//				THREAD_STACKSIZE,
//				DEFAULT_THREAD_PRIO);
//		} else {
//			xil_printf("Error on accept! Number %d\r\n", new_sd2);
//		}
//	}
}
