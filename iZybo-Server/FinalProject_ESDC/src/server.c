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

#define RECV_BUF_SIZE (2048)

static char recv_buf[RECV_BUF_SIZE];
void write_application_thread(void *);

u16_t read_port = 5001;

void print_read_app_header()
{
 //   xil_printf("%20s %6d %s\r\n", "echo server",
	printf("%20s %6d %s\r\n", "echo server",
                        read_port,
                        "$ telnet <board_ip> 5001");

}

/* thread spawned for each connection */
void process_read_request(void *p)
{
	int sd = (int)p;
	int n, i = 1;

	bzero(recv_buf, RECV_BUF_SIZE);

	xil_printf("Connected\r\n");

	while (1) {

		xil_printf("Waiting message %d", i);
		/* read a max of RECV_BUF_SIZE bytes from socket */
		if ((n = lwip_read(sd, recv_buf, RECV_BUF_SIZE)) <= 0) {
			xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
			break;
		}

		/* break if the recved message = "quit" */
		if (!strncmp(recv_buf, "quit", 4))
			break;

		/* break if client closed connection */
		if (n <= 0)
			break;

		/* Print messages */
		if (n > 0) {
			xil_printf("\r\nUser 2: %s\r\n", recv_buf);
		}

		memset(recv_buf, '\0', RECV_BUF_SIZE);

		//sleep(2);

		i++;
	}

	/* close connection */
	close(sd);
	vTaskDelete(NULL);
}

void read_application_thread()
{
	int sock1, new_sd1;
	struct sockaddr_in addressR, remoteR;
	int size;

	if ((sock1 = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	addressR.sin_family = AF_INET;
	addressR.sin_port = htons(read_port);
	addressR.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock1, (struct sockaddr *)&addressR, sizeof (addressR)) < 0)
		return;

	lwip_listen(sock1, 0);

	xil_printf("\r\nWaiting User 2 to connect... reading side\r\n");

	size = sizeof(remoteR);

	while (1) {
		if ((new_sd1 = lwip_accept(sock1, (struct sockaddr *)&remoteR, (socklen_t *)&size)) > 0) {
			xil_printf("\r\nRead socket accepted: %d\r\n", new_sd1);
			sys_thread_new("servers", process_read_request,
				(void*)new_sd1,
				THREAD_STACKSIZE,
				DEFAULT_THREAD_PRIO);
//			sys_thread_new("writed", write_application_thread, 0,
//				THREAD_STACKSIZE,
//				DEFAULT_THREAD_PRIO);
		} else {
			xil_printf("Error on accept! Number %d\r\n", new_sd1);
		}
	}

}
