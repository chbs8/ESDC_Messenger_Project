/*
 * iZybo.c

 *
 *  Created on: 09/05/2017
 *      Author: adria.gil
 */

#include <FreeRTOS.h>

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>

#include "lwip/inet.h"
#include "lwip/sockets.h"
#include "lwip/sys.h"
#include "lwipopts.h"
#include "lwip/opt.h"

#include "config_apps.h"
#ifdef __arm__
#include "xil_printf.h"
#include "FreeRTOS.h"
#include "task.h"
#endif

#define BUFSIZE (100)

static char recv_buf[BUFSIZE];
static char send_buf[BUFSIZE];
static char names[12][BUFSIZE];

int sd_write[12];
int sd_read[12];

int n_users = 0;

u32_t read_port22 = 5001;
u32_t write_port22 = 2222;
u32_t opt;

void application_thread(int i)
{

	int nr, nw;
	int j;
	char buffer_local[BUFSIZE];
	char buffer_name[2*BUFSIZE];

	memset(buffer_local, '\0', BUFSIZE);



	while (1) {
		/* read a max of RECV_BUF_SIZE bytes from socket */
		if ((nr = lwip_read(sd_read[i], buffer_local, BUFSIZE)) > 0) {
			/* Print messages */
			sprintf(buffer_name,"%s: %s", names[i], buffer_local);
			//xil_printf("\r\nUser %d to Group: %s\r\n", i, buffer_local);
			xil_printf("\r\n%s", buffer_name);
			for(j = 0; j < n_users; j++) {
				//MUTEX in order to control writing
				if(j != i) {
					if ((nw = lwip_write(sd_write[j], buffer_name, BUFSIZE)) < 0) {
						xil_printf("Error sending to socket %d\r\n", sd_write[i]);
					}
				}
			}
		} else {
			//  xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd_read[i]);
			//  break;
		}
		memset(buffer_local, '\0', BUFSIZE);
	}

	close(sd_read[i]);
	close(sd_write[i]);
	n_users--;

}

void iZybo_server_thread()
{
	int sock1 = 0, sock2 = 0;
	struct sockaddr_in addressR, remoteR, addressW, remoteW;
	int size, i = 0, j;

	char buffer_name[2*BUFSIZE];

	memset(recv_buf, '\0', BUFSIZE);
	memset(send_buf, '\0', BUFSIZE);

	if ((sock1 = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	addressR.sin_family = AF_INET;
	addressR.sin_port = htons(read_port22);
	addressR.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock1, (struct sockaddr *)&addressR, sizeof (addressR)) < 0)
		return;

	lwip_listen(sock1, 0);

	xil_printf("Waiting Users to connect... reading side\r\n");

	size = sizeof(remoteR);

	if ((sock2 = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
						return;

	addressW.sin_family = AF_INET;
	addressW.sin_port = htons(write_port22);
	addressW.sin_addr.s_addr = INADDR_ANY;

	if (lwip_bind(sock2, (struct sockaddr *)&addressW, sizeof (addressW)) < 0)
		return;

	lwip_listen(sock2, 0);

	xil_printf("Waiting Users to connect... writing side\r\n");

	size = sizeof(remoteW);

	while(1) {
		if ((sd_read[i] = lwip_accept(sock1, (struct sockaddr *)&remoteR, (socklen_t *)&size)) > 0) {
			xil_printf("Read socket accepted: %d\r\n", i);
			lwip_read(sd_read[i], names[i], BUFSIZE); xil_printf("User name: %s\r\n",names[i]);
			//	Code to Use sock2 as server
			if ((sd_write[i] = lwip_accept(sock2, (struct sockaddr *)&remoteW, (socklen_t *)&size)) > 0) {
				xil_printf("Write socket accepted: %d\r\n", i);
				lwip_read(sd_write[i], recv_buf, BUFSIZE); xil_printf("ACK Read: %s\r\n",recv_buf);
				n_users++;

				sprintf(buffer_name,"New user: %s\r\n", names[i]);
				for(j = 0; j < n_users; j++) {
					if(j != i) {
						if (lwip_write(sd_write[j], buffer_name, BUFSIZE) < 0) {
						}
					}
				}


				sys_thread_new("servers", application_thread,
					i,
					THREAD_STACKSIZE,
					DEFAULT_THREAD_PRIO);
				i++;

			} else {
				xil_printf("Error on accept! Number %d\r\n", i);
				close(sd_read[i]);
			}

		} else {
			xil_printf("Error on accept! Number %d\r\n", i);
		}
	}
	close(sock1);
	close(sock2);
}
