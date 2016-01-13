/**
 * @file
 */
/******************************************************************************
 * Copyright (c) 2012-2014, AllSeen Alliance. All rights reserved.
 *
 *    Permission to use, copy, modify, and/or distribute this software for any
 *    purpose with or without fee is hereby granted, provided that the above
 *    copyright notice and this permission notice appear in all copies.
 *
 *    THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 *    WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 *    MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 *    ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 *    WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 *    ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 *    OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 ******************************************************************************/

/**
 * Per-module definition of the current module for debug logging.  Must be defined
 * prior to first inclusion of aj_debug.h
 */
#define AJ_MODULE NET

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <socket.h>
#include <assert.h>
#include <errno.h>
#include <time.h>
#include <unistd.h>	

#include "aj_target.h"
#include "aj_bufio.h"
#include "aj_net.h"
#include "aj_util.h"
#include "aj_debug.h"

/**
 * Turn on per-module debug printing by setting this variable to non-zero value
 * (usually in debugger).
 */
#ifndef NDEBUG
uint8_t dbgNET = 0;
#endif

#define SHUT_RDWR 2

#define INVALID_SOCKET (-1)

/*
 * IANA assigned IPv4 multicast group for AllJoyn.
 */
static const char AJ_IPV4_MULTICAST_GROUP[] = "224.0.0.113";

/*
 * IANA assigned UDP multicast port for AllJoyn
 */
#define AJ_UDP_PORT 9956

/**
 * Target-specific context for network I/O
 */
typedef struct {
    int tcpSock;
    int udpSock;	 
} NetContext;

static NetContext netContext = { INVALID_SOCKET, INVALID_SOCKET };

static const char* AddrStr(uint32_t addr) {
    static char txt[17];
    sprintf((char*)&txt, "%u.%u.%u.%u\0",
            (addr & 0xFF000000) >> 24,
            (addr & 0x00FF0000) >> 16,
            (addr & 0x0000FF00) >>  8,
            (addr & 0x000000FF)
            );

    return txt;
}

static AJ_Status CloseNetSock(AJ_NetSocket* netSock)
{
    NetContext* context = (NetContext*)netSock->rx.context;
    if (context) {
        if (context->tcpSock != INVALID_SOCKET) {
            esp_ip_close(context->tcpSock);
        }
        if (context->udpSock != INVALID_SOCKET) {
            esp_ip_close(context->udpSock);
        }				 

        context->tcpSock = context->udpSock = INVALID_SOCKET;
        memset(netSock, 0, sizeof(AJ_NetSocket));
    }
    return AJ_OK;
}

AJ_Status AJ_Net_Send(AJ_IOBuffer* buf)
{
    NetContext* context = (NetContext*) buf->context;
    ssize_t ret;
    size_t tx = AJ_IO_BUF_AVAIL(buf);

    AJ_InfoPrintf(("AJ_Net_Send(buf=0x%p)\n", buf));

    assert(buf->direction == AJ_IO_BUF_TX);

    if (tx > 0) {
        ret = esp_ip_send(context->tcpSock, (char*)buf->readPtr, tx);
        if (ret == -1) {
            AJ_ErrPrintf(("AJ_Net_Send(): esp_ip_send() failed. status=AJ_ERR_WRITE\n"));
            return AJ_ERR_WRITE;
        }
        buf->readPtr += ret;
    }
    if (AJ_IO_BUF_AVAIL(buf) == 0) {
        AJ_IO_BUF_RESET(buf);
    }

    AJ_InfoPrintf(("AJ_Net_Send(): status=AJ_OK\n"));

    return AJ_OK;
}

AJ_Status AJ_Net_Recv(AJ_IOBuffer* buf, uint32_t len, uint32_t timeout)
{																											 
    AJ_Status status = AJ_OK;
	int ret;					
    NetContext* context = (NetContext*) buf->context;
    size_t rx = AJ_IO_BUF_SPACE(buf);

    AJ_InfoPrintf(("AJ_Net_Recv(buf=0x%p, len=%d., timeout=%d.)\n", buf, len, timeout));

    assert(buf->direction == AJ_IO_BUF_RX);
	ret = esp_ip_poll(context->tcpSock, timeout);
	if (ret == 0) {
		return AJ_ERR_TIMEOUT;			
	}

    rx = min(rx, len);
    if (rx) {
        ret = esp_ip_recv(context->tcpSock, (char*)buf->writePtr);
        if ((ret == -1) || (ret == 0)) {
            AJ_ErrPrintf(("AJ_Net_Recv(): esp_ip_recv() failed. status=AJ_ERR_READ\n"));
            status = AJ_ERR_READ;
        } else {
            buf->writePtr += ret;
        }
    }
    AJ_InfoPrintf(("AJ_Net_Recv(): status=%s\n", AJ_StatusText(status)));

    return status;
}

static uint8_t rxData[1024];
static uint8_t txData[1500];

AJ_Status AJ_Net_Connect(AJ_NetSocket* netSock, uint16_t port, uint8_t addrType, const uint32_t* addr)
{			   
    AJ_InfoPrintf(("AJ_Net_Connect(netSock=0x%p, port=%d., addrType=%d., addr=0x%p)\n", netSock, port, addrType, addr));
	AJ_InfoPrintf(("AJ_Net_Connect(): Connect to \"%s:%u\"\n", AddrStr(*addr), port));							
    int tcpSock = esp_ip_connect("TCP", AddrStr(*addr), port);
    if (tcpSock == INVALID_SOCKET) {
		AJ_ErrPrintf(("AJ_Net_Connect(): esp_ip_connect() failed. status=AJ_ERR_CONNECT\n"));
        esp_ip_close(tcpSock);
		return AJ_ERR_CONNECT;
    }

    netContext.tcpSock = tcpSock;
    AJ_IOBufInit(&netSock->rx, rxData, sizeof(rxData), AJ_IO_BUF_RX, &netContext);
    netSock->rx.x1.recv = AJ_Net_Recv;
    AJ_IOBufInit(&netSock->tx, txData, sizeof(txData), AJ_IO_BUF_TX, &netContext);
    netSock->tx.x1.send = AJ_Net_Send;
    AJ_InfoPrintf(("AJ_Net_Connect(): status=AJ_OK\n"));

    return AJ_OK; 
}

void AJ_Net_Disconnect(AJ_NetSocket* netSock)
{
    CloseNetSock(netSock);
}

AJ_Status AJ_Net_SendTo(AJ_IOBuffer* buf)
{
    int ret;
    size_t tx = AJ_IO_BUF_AVAIL(buf);
    NetContext* context = (NetContext*) buf->context;
    AJ_InfoPrintf(("AJ_Net_SendTo(buf=0x%p)\n", buf));
    assert(buf->direction == AJ_IO_BUF_TX);

    if (tx > 0) {
        if (context->udpSock != INVALID_SOCKET) {
			ret = esp_ip_send(context->udpSock, (char*)buf->readPtr, tx);
	        if (ret == -1) {
	            AJ_ErrPrintf(("AJ_Net_SendTo(): esp_ip_send() failed. status=AJ_ERR_WRITE\n"));
	            return AJ_ERR_WRITE;
	        }
	        buf->readPtr += ret;			
        }
    }
    AJ_IO_BUF_RESET(buf);
    AJ_InfoPrintf(("AJ_Net_SendTo(): status=AJ_OK\n"));

    return AJ_OK;
}

AJ_Status AJ_Net_RecvFrom(AJ_IOBuffer* buf, uint32_t len, uint32_t timeout)
{
    AJ_Status status = AJ_OK;

    NetContext* context = (NetContext*) buf->context;
    int sock = context->udpSock;
    
    AJ_InfoPrintf(("AJ_Net_RecvFrom(buf=0x%p, len=%d., timeout=%d.)\n", buf, len, timeout));

    assert(buf->direction == AJ_IO_BUF_RX);

    int rc = esp_ip_poll(sock, timeout);
    if (rc == 0) {				
        AJ_InfoPrintf(("AJ_Net_RecvFrom(): esp_ip_poll() timed out. status=AJ_ERR_TIMEOUT\n"));
        return AJ_ERR_TIMEOUT;
    }

    size_t rx = AJ_IO_BUF_SPACE(buf);
    rx = min(rx, len);
    if (rx) {
        int ret = esp_ip_recv(sock, (char*)buf->writePtr);
        if (ret == -1) {
            AJ_ErrPrintf(("AJ_Net_RecvFrom(): esp_ip_recv() failed. status=AJ_ERR_READ\n"));
            status = AJ_ERR_READ;
        } else {
            buf->writePtr += ret;
            status = AJ_OK;
        }
    }

    AJ_InfoPrintf(("AJ_Net_RecvFrom(): status=%s\n", AJ_StatusText(status)));
    return status;
}

/*
 * Need enough space to receive a complete name service packet when used in UDP
 * mode.  NS expects MTU of 1500 subtracts UDP, IP and ethertype overhead.
 * 1500 - 8 -20 - 18 = 1454.  txData buffer size needs to be big enough to hold
 * a NS WHO-HAS for one name (4 + 2 + 256 = 262)
 */
static uint8_t rxDataMCast[1454];
static uint8_t txDataMCast[262];
			 
AJ_Status AJ_Net_MCastUp(AJ_NetSocket* netSock)
{
    AJ_InfoPrintf(("AJ_Net_MCastUp(netSock=0x%p)\n", netSock));
    netContext.udpSock = esp_ip_connect("UDP", AJ_IPV4_MULTICAST_GROUP, AJ_UDP_PORT); 

    if (netContext.udpSock != INVALID_SOCKET) {
		esp_ip_group_join(AJ_IPV4_MULTICAST_GROUP);
        AJ_IOBufInit(&netSock->rx, rxDataMCast, sizeof(rxDataMCast), AJ_IO_BUF_RX, &netContext);
        netSock->rx.x1.recv = AJ_Net_RecvFrom;
        AJ_IOBufInit(&netSock->tx, txDataMCast, sizeof(txDataMCast), AJ_IO_BUF_TX, &netContext);
        netSock->tx.x1.send = AJ_Net_SendTo;
        return AJ_OK;
    }

    return AJ_ERR_READ;
}

void AJ_Net_MCastDown(AJ_NetSocket* netSock)
{
    AJ_InfoPrintf(("AJ_Net_MCastDown(netSock=0x%p)\n", netSock));
	esp_ip_group_leave(AJ_IPV4_MULTICAST_GROUP);
    CloseNetSock(netSock);
}


