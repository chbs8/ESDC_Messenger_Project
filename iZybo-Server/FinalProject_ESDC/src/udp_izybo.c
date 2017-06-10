/*
 * iZybo.c

 *
 *  Created on: 09/05/2017
 *      Author: adria.gil
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

#define BUFSIZE (1440)

// static char recv_buf[BUFSIZE];
static char send_buf[BUFSIZE];
// static char ack_buf[BUFSIZE] = {"ACK"};


void UDP_application_thread()
{
	int sock1 = 0, sock2 = 0;
	struct sockaddr_in addressR, remoteW;
	struct ip_addr servaddr;

	int nr, nw;

//	memset(send_buf, '\0', BUFSIZE);
//	memset(recv_buf, '\0', BUFSIZE);

	/* Create sock1 as reading server */
	if ((sock1 = lwip_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		xil_printf("Error creating first socket\r\n");
		vTaskDelete(NULL);
		return;
	}

	memset((void*)&addressR, 0, sizeof addressR);
	addressR.sin_family = AF_INET;
	addressR.sin_port = htons(5001);
	addressR.sin_addr.s_addr = INADDR_ANY;

	/* Create sock2 as writing client */
	IP4_ADDR(&servaddr, 147, 83, 81, 226);  // Address 6
	if ((sock2 = lwip_socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		xil_printf("Error creating second socket\r\n");

		return;
	}

	memset((void*)&remoteW, 0, sizeof remoteW);
	remoteW.sin_addr.s_addr = servaddr.addr;
	remoteW.sin_family = AF_INET;
	remoteW.sin_port = htons(2222);

	/*Connect to the server*/
	if (lwip_connect(sock2, (struct sockaddr*)&remoteW, sizeof(remoteW)) < 0) {
		xil_printf("Error connecting to server\r\n");
		vTaskDelete(NULL);
		return;
	}

	/* Binding client */
	if (lwip_bind(sock1, (struct sockaddr *)&addressR, sizeof (addressR)) < 0) {
		xil_printf("Error binding the socket\r\n");
		vTaskDelete(NULL);
		return;
	}

	xil_printf("Connection established\r\n");


	while (1) {
		char recv_buf[BUFSIZE];

		xil_printf("\r\nWaiting message");
		/* read a max of RECV_BUF_SIZE bytes from socket */
		if ((nr = read(sock1, recv_buf, BUFSIZE)) < 0) {
			xil_printf("\r\nError Reading\r\n");
			break;
		}

		/* Print messages */
		if (nr > 0) {
			xil_printf("\r\nUser 2: %s\r\n", recv_buf);
		}

		xil_printf("Now you can send your message\r\n");

		// Sending message
		xil_printf("Me: ");
		if(fgets(send_buf, BUFSIZE, stdin) == NULL) {
			xil_printf("Error sending message\r\n");
			return;
		}
		if ((nw = write(sock2, send_buf, BUFSIZE)) < 0) {
			xil_printf("Error sending to socket\r\n");
		}
		xil_printf("Message sent\r\n");

//		memset(recv_buf, '\0', BUFSIZE);
//		memset(send_buf, '\0', BUFSIZE);

	}

	close(sock1);
	close(sock2);
}