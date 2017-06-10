/*
 * iZybo.c

 *
 *  Created on: 09/05/2017
 *      Author: adria.gil
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <xparameters.h>
#include <xgpio.h>

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
#define MAX_MESSAGES (1000)

static char recv_buf[BUFSIZE];
static char send_buf[BUFSIZE];
static char database[MAX_MESSAGES][BUFSIZE];
// static char ack_buf[BUFSIZE] = {"ACK"};

u32_t write_port1 = 5001;
u32_t read_port1 = 2222;
u32_t opt;

XGpio btns, leds;
int currentPositionDatabase = 1, new_msg = 0, lastReadPos = 0;
int leds_blink = 0;

void blinking_leds_thread() {

	int led_value = 0;

	while(1) {
		while(leds_blink == 0);

		while(leds_blink == 1) {
			XGpio_DiscreteWrite(&leds, 1, led_value);
			sleep(0.7);
			if(led_value == 0)
				led_value = 15;
			else
				led_value = 0;
		}
		XGpio_DiscreteWrite(&leds, 1, 0);
	}
}

void print_thread(){

	int btns_check, i;

	while(1) {

		btns_check = XGpio_DiscreteRead(&btns, 1);
//		if((btns_check == 1 || btns_check == 2 || btns_check == 4 || btns_check == 8) && new_msg == 1) {
		if(btns_check > 0 && new_msg == 1) {
			xil_printf("\r\nDisplaying all unread messages:\r\n");
			leds_blink = 0;
			for(i = lastReadPos; i < currentPositionDatabase; i++) {
				xil_printf("%s", database[i]);
				memset(database[i], '\0', BUFSIZE);
			}
			lastReadPos = i;
			xil_printf("Me: ");
			XGpio_DiscreteWrite(&leds, 1, 0);
			new_msg = 0;
			//currentPositionDatabase = 0;
		}
	}

}

void read_thread(void *p) {

	int nr;
	int sd = (int)p;

	while(1) {

		/* read a max of RECV_BUF_SIZE bytes from socket */
		if ((nr = lwip_read(sd, database[currentPositionDatabase], BUFSIZE)) <= 0) {
			xil_printf("%s: error reading from socket %d, closing socket\r\n", __FUNCTION__, sd);
			break;
		} else {
			/* Print messages */
			currentPositionDatabase++;
			new_msg = 1;
			if(leds_blink == 0) {
				leds_blink = 1;
			}
		}
	}

}

void iZybo_client_thread() {
	int sock1 = 0, sock2 = 0, sd1 = 0;
	struct sockaddr_in addressR, addressW;
	struct ip_addr servaddr;
	IP4_ADDR(&servaddr, 147, 83, 81, 223);

	int nw;

//	XGpio_Initialize(&sw, XPAR_SWITCHES_DEVICE_ID);
//	XGpio_SetDataDirection(&sw, 1, 0xffffffff);

	XGpio_Initialize(&btns, XPAR_BUTTONS_DEVICE_ID);
	XGpio_SetDataDirection(&btns, 1, 0xffffffff);

	XGpio_Initialize(&leds, XPAR_LEDS_DEVICE_ID);
	XGpio_SetDataDirection(&leds, 1, 0x00000000);

//	memset(recv_buf, '\0', BUFSIZE);
//	memset(send_buf, '\0', BUFSIZE);

	if ((sock1 = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
		return;

	addressW.sin_family = AF_INET;
	addressW.sin_port = htons(write_port1);
	addressW.sin_addr.s_addr = servaddr.addr;

	if ((sock2 = lwip_socket(AF_INET, SOCK_STREAM, 0)) < 0)
			return;

	addressR.sin_family = AF_INET;
	addressR.sin_port = htons(read_port1);
	addressR.sin_addr.s_addr = servaddr.addr;

	/*Connect to the server*/
	xil_printf("Waiting user to connect... writing side\r\n");
	if (lwip_connect(sock1, (struct sockaddr*)&addressW, sizeof(addressW)) < 0) {
		xil_printf("Error connecting to writing server\r\n");
		return;
	}
	xil_printf("Connected to write\r\n");
//	send_buf[0] = 'B';
//	send_buf[1] = '\0';
	lwip_write(sock1, name, sizeof(name));

	xil_printf("Waiting user to connect... reading side\r\n");
	if (lwip_connect(sock2, (struct sockaddr*)&addressR, sizeof(addressR)) < 0) {
		xil_printf("Error connecting to reading server\r\n");
		return;
	}
	xil_printf("Connected to read\r\n");
	lwip_write(sock2, name, sizeof(name));

	xil_printf("Connection Established\r\n");

	memset(recv_buf, '\0', BUFSIZE);
	memset(send_buf, '\0', BUFSIZE);

	sys_thread_new("read_app", read_thread,
		(void*)sock2,
		THREAD_STACKSIZE,
		DEFAULT_THREAD_PRIO);
	sys_thread_new("print_app", print_thread,
		NULL,
		THREAD_STACKSIZE,
		DEFAULT_THREAD_PRIO);
	sys_thread_new("blink_app", blinking_leds_thread,
		NULL,
		THREAD_STACKSIZE,
		DEFAULT_THREAD_PRIO);

	while (1) {

		xil_printf("Me: ");
		/*Sending message */
		if(fgets(send_buf, BUFSIZE, stdin) != NULL) {
			if ((nw = lwip_write(sock1, send_buf, BUFSIZE)) < 0) {
				xil_printf("Error sending to socket %d\r\n", sd1);
			}
		}
		else {
			printf("Error fgets!\r\n");
		}

		// memset(send_buf, '\0', BUFSIZE);

	}

	close(sock1);
	close(sock2);
}
