/*
 * 
 * Author information summary for YARP:
 *
 * Copyright (C) 2007 iXs Research Corp
 * CopyPolicy: Released under the terms of the GNU GPL version 2
 *
 */


#ifndef __URBTC_H__
#define __URBTC_H__

/* IOCTL��̿����� */
#define URBTC_GET_VENDOR 1
#define URBTC_GET_PRODUCT 2
#define URBTC_REQUEST_READ 3
#define URBTC_CONTINUOUS_READ 4
#define URBTC_COUNTER_SET 5
#define URBTC_DESIRE_SET 6
#define URBTC_GET_READ_STATUS 7
#define URBTC_GET_WRITE_STATUS 8
#define URBTC_BUFREAD 9
#define URBTC_WAITREAD 10

#define URBTC_STATUS_READ_REQUEST 0
#define URBTC_STATUS_READ_CONTINUOUS 1
#define URBTC_STATUS_WRITE_COUNTER 2
#define URBTC_STATUS_WRITE_DESIRE 3

#endif
