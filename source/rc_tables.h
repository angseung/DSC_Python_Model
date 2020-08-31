/***************************************************************************
*    Copyright (c) 2017, Broadcom Ltd.
*    All rights reserved.
*    VESA CONFIDENTIAL
*
*  Statement regarding contribution of copyrighted materials to VESA:
*
*  This code is owned by Broadcom Limited and is contributed to VESA
*  for inclusion and use in its VESA Display Stream Compression specification.
*  Accordingly, VESA is hereby granted a worldwide, perpetual, non-exclusive
*  license to revise, modify and create derivative works to this code and
*  VESA shall own all right, title and interest in and to any derivative
*  works authored by VESA.
*
*  Terms and Conditions
*
*  Without limiting the foregoing, you agree that your use
*  of this software program does not convey any rights to you in any of
*  Broadcom�s patent and other intellectual property, and you
*  acknowledge that your use of this software may require that
*  you separately obtain patent or other intellectual property
*  rights from Broadcom or third parties.
*
*  Except as expressly set forth in a separate written license agreement
*  between you and Broadcom, if applicable:
*
*  1. TO THE MAXIMUM EXTENT PERMITTED BY LAW, THE SOFTWARE IS PROVIDED
*  "AS IS" AND WITH ALL FAULTS AND BROADCOM MAKES NO PROMISES,
*  REPRESENTATIONS OR WARRANTIES, EITHER EXPRESS, IMPLIED, STATUTORY, OR
*  OTHERWISE, WITH RESPECT TO THE SOFTWARE.  BROADCOM SPECIFICALLY
*  DISCLAIMS ANY AND ALL IMPLIED WARRANTIES OF TITLE, MERCHANTABILITY,
*  NONINFRINGEMENT, FITNESS FOR A PARTICULAR PURPOSE, LACK OF VIRUSES,
*  ACCURACY OR COMPLETENESS, QUIET ENJOYMENT, QUIET POSSESSION OR
*  CORRESPONDENCE TO DESCRIPTION. YOU ASSUME THE ENTIRE RISK ARISING
*  OUT OF USE OR PERFORMANCE OF THE SOFTWARE.
*
*  2. TO THE MAXIMUM EXTENT PERMITTED BY LAW, IN NO EVENT SHALL
*  BROADCOM OR ITS LICENSORS BE LIABLE FOR (i) CONSEQUENTIAL, INCIDENTAL,
*  SPECIAL, INDIRECT, OR EXEMPLARY DAMAGES WHATSOEVER ARISING OUT OF OR
*  IN ANY WAY RELATING TO YOUR USE OF OR INABILITY TO USE THE SOFTWARE EVEN
*  IF BROADCOM HAS BEEN ADVISED OF THE POSSIBILITY OF SUCH DAMAGES; OR (ii)
*  ANY AMOUNT IN EXCESS OF THE AMOUNT ACTUALLY PAID FOR THE SOFTWARE ITSELF
*  OR U.S. $1, WHICHEVER IS GREATER. THESE LIMITATIONS SHALL APPLY
*  NOTWITHSTANDING ANY FAILURE OF ESSENTIAL PURPOSE OF ANY LIMITED REMEDY.
***************************************************************************/

#include <string.h>
#include <malloc.h>

char *minqp444_8b[] = {
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"3	2	2	2	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"3	3	2	2	1	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"5	4	4	3	3	3	3	3	3	3	3	3	3	3	2	1	1	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0",
	"5	4	4	4	3	3	3	3	3	3	3	3	3	3	3	2	1	1	1	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0",
	"5	5	4	4	3	3	3	3	3	3	3	3	3	3	3	2	2	2	1	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0",
	"5	5	4	4	3	3	3	3	3	3	3	3	3	3	3	2	2	2	2	2	2	2	2	2	2	2	1	1	1	1	1	1	0	0	0	0	0",
	"5	5	4	4	3	3	3	3	3	3	3	3	3	3	3	2	2	2	2	2	2	2	2	2	2	2	2	2	1	1	1	1	1	1	0	0	0",
	"6	5	5	4	4	4	3	3	3	3	3	3	3	3	3	3	3	3	3	3	3	3	3	3	3	3	2	2	1	1	1	1	1	1	0	0	0",
	"6	6	5	5	5	5	5	5	5	5	5	5	5	4	4	4	4	4	4	4	4	4	3	3	3	3	2	2	1	1	1	1	1	1	1	1	0",
	"6	6	6	5	5	5	5	5	5	5	5	5	5	5	5	5	5	4	4	4	4	4	4	4	4	4	3	3	2	2	2	2	1	1	1	1	0",
	"6	6	6	6	5	5	5	5	5	5	5	5	5	5	5	5	5	5	5	5	5	5	4	4	4	4	3	3	2	2	2	2	1	1	1	1	0",
	"9	9	9	9	8	8	7	7	7	7	7	7	7	7	7	7	7	7	7	7	6	6	5	5	5	5	4	4	3	3	3	3	2	2	1	1	1",
	"14	14	13	13	12	12	12	12	11	11	10	10	10	10	9	9	9	8	8	8	7	7	7	7	6	6	5	5	5	5	4	4	4	3	3	3	3"
};

char *maxqp444_8b[] = {
	"4	4	4	4	4	4	3	3	3	3	2	2	2	2	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"6	6	5	5	4	4	4	4	4	4	4	4	3	3	3	2	2	1	1	1	1	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0",
	"8	7	7	6	5	5	5	5	5	5	5	5	4	4	4	3	2	2	1	1	1	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0",
	"8	8	7	7	6	6	6	6	6	6	6	6	5	5	5	4	3	3	2	2	2	2	2	2	2	2	1	1	1	1	1	1	1	1	1	1	0",
	"9	8	8	7	7	7	7	7	7	7	7	7	6	6	5	4	4	3	2	2	2	2	2	2	2	2	2	2	1	1	1	1	1	1	1	1	0",
	"9	8	8	8	7	7	7	7	7	7	7	7	6	6	6	5	4	4	3	3	3	3	3	3	3	3	2	2	2	2	1	1	1	1	1	1	1",
	"9	9	8	8	7	7	7	7	7	7	7	7	7	6	6	5	4	4	3	3	3	3	3	3	3	3	2	2	2	2	1	1	1	1	1	1	1",
	"10	10	9	9	8	8	8	8	8	8	8	8	8	7	7	6	5	5	4	4	4	4	3	3	3	3	2	2	2	2	2	2	1	1	1	1	1",
	"11	11	10	10	9	9	9	9	9	9	8	8	8	7	7	6	6	5	5	5	5	5	4	4	4	4	3	3	2	2	2	2	2	2	1	1	1",
	"12	11	11	10	10	10	9	9	9	9	9	9	9	8	8	7	6	6	5	5	5	5	4	4	4	4	3	3	2	2	2	2	2	2	1	1	1",
	"12	12	11	11	10	10	10	10	10	10	9	9	9	8	8	7	7	6	6	6	5	5	4	4	4	4	3	3	2	2	2	2	2	2	2	2	1",
	"12	12	12	11	11	11	10	10	10	10	9	9	9	9	8	8	8	7	7	7	6	6	5	5	5	5	4	4	3	3	3	3	2	2	2	2	1",
	"12	12	12	12	11	11	11	11	11	10	10	9	9	9	8	8	8	7	7	7	6	6	5	5	5	5	4	4	3	3	3	3	2	2	2	2	1",
	"13	13	13	13	12	12	11	11	11	11	10	10	10	10	9	9	8	8	8	8	7	7	6	6	6	6	5	5	4	4	4	4	3	3	2	2	2",
	"15	15	14	14	13	13	13	13	12	12	11	11	11	11	10	10	10	9	9	9	8	8	8	8	7	7	6	6	6	6	5	5	5	4	4	4	4"
};

char *minqp444_10b[] = {
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"4	4	4	4	4	4	4	4	4	4	3	3	2	2	2	2	2	2	2	2	2	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"7	6	6	6	5	5	5	5	5	5	5	5	4	4	4	3	3	3	3	3	3	2	2	2	2	2	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"7	7	6	6	5	5	5	5	5	5	5	5	4	4	4	4	4	4	4	4	4	3	3	3	3	3	2	2	2	2	2	2	2	2	2	2	1	1	0	0	0	0	0	0	0	0	0	0	0",
	"9	8	8	7	7	7	7	7	7	7	7	7	6	6	5	5	5	5	4	4	4	4	3	3	3	3	3	3	2	2	2	2	2	2	2	2	1	1	1	1	0	0	0	0	0	0	0	0	0",
	"9	8	8	8	7	7	7	7	7	7	7	7	6	6	6	6	5	5	5	5	5	5	4	4	4	4	3	3	3	3	3	2	2	2	2	2	2	2	2	2	1	1	1	0	0	0	0	0	0",
	"9	9	8	8	7	7	7	7	7	7	7	7	7	7	7	6	6	6	5	5	5	5	5	4	4	4	3	3	3	3	3	3	3	3	3	3	3	2	2	2	1	1	1	1	0	0	0	0	0",
	"9	9	8	8	7	7	7	7	7	7	7	7	7	7	7	7	6	6	6	6	6	6	5	5	5	5	4	4	4	4	4	4	3	3	3	3	3	3	2	2	2	2	1	1	1	0	0	0	0",
	"9	9	8	8	7	7	7	7	7	7	7	7	7	7	7	7	7	6	6	6	6	6	5	5	5	5	5	5	5	4	4	4	4	4	3	3	3	3	3	3	2	2	2	1	1	1	1	0	0",
	"10	9	9	8	8	8	7	7	7	7	7	7	7	7	7	7	7	7	6	6	6	6	6	6	6	6	6	6	5	5	4	4	4	4	3	3	3	3	3	3	3	2	2	2	1	1	1	1	0",
	"10	10	9	9	9	9	9	9	9	9	9	9	9	8	8	8	8	7	7	7	7	7	6	6	6	6	6	6	5	5	5	5	5	5	5	5	4	4	4	3	3	3	2	2	2	1	1	1	1",
	"10	10	10	9	9	9	9	9	9	9	9	9	9	9	9	9	9	8	8	8	8	8	7	7	7	7	7	7	6	6	6	6	5	5	5	5	4	4	4	4	3	3	3	3	2	2	1	1	1",
	"10	10	10	10	9	9	9	9	9	9	9	9	9	9	9	9	9	9	9	9	8	8	8	8	8	8	7	7	6	6	6	6	5	5	5	5	4	4	4	4	3	3	3	3	2	2	2	2	1",
	"12	12	12	12	12	12	12	12	12	12	11	11	11	11	11	11	11	11	11	11	10	10	9	9	9	9	8	8	7	7	7	7	6	6	5	5	5	5	4	4	3	3	3	3	2	2	2	2	1",
	"18	18	17	17	16	16	16	16	15	15	14	14	14	14	13	13	13	12	12	12	11	11	11	11	10	10	9	9	9	9	9	8	8	7	7	7	7	7	6	6	5	5	5	5	4	4	3	3	3"
};

char *maxqp444_10b[] = {
	"8	8	8	8	8	8	7	7	7	6	5	5	4	4	3	3	3	2	2	2	2	2	2	1	1	1	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0",
	"10	10	9	9	8	8	8	8	8	8	7	7	6	6	6	5	5	4	4	4	4	3	3	3	3	3	2	2	2	2	2	2	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0",
	"12	11	11	10	9	9	9	9	9	9	9	9	8	8	8	7	6	6	5	5	5	4	4	4	4	4	3	3	3	3	3	3	2	2	2	1	1	1	1	1	1	0	0	0	0	0	0	0	0",
	"12	12	11	11	10	10	10	10	10	10	10	10	9	9	9	8	7	7	6	6	6	5	5	5	5	5	4	4	3	3	3	3	3	3	3	3	2	2	1	1	1	1	1	0	0	0	0	0	0",
	"13	12	12	11	11	11	11	11	11	11	11	11	10	10	9	8	8	7	6	6	6	6	5	5	5	5	5	5	4	4	4	3	3	3	3	3	2	2	2	2	1	1	1	1	0	0	0	0	0",
	"13	12	12	12	11	11	11	11	11	11	11	11	10	10	10	9	8	8	7	7	7	7	6	6	6	6	5	5	5	5	4	3	3	3	3	3	3	3	3	3	2	2	2	1	1	1	1	0	0",
	"13	13	12	12	11	11	11	11	11	11	11	11	11	10	10	9	8	8	7	7	7	7	7	6	6	6	5	5	5	5	4	4	4	4	4	4	4	3	3	3	2	2	2	2	1	1	1	1	1",
	"14	14	13	13	12	12	12	12	12	12	12	12	12	11	11	10	9	9	8	8	8	8	7	7	7	7	6	6	6	5	5	5	4	4	4	4	4	4	3	3	3	3	2	2	2	1	1	1	1",
	"15	15	14	14	13	13	13	13	13	13	12	12	12	11	11	10	10	9	9	9	9	9	8	8	8	8	7	7	6	5	5	5	5	5	4	4	4	4	4	4	3	3	3	2	2	2	2	1	1",
	"16	15	15	14	14	14	13	13	13	13	13	13	13	12	12	11	10	10	9	9	9	9	8	8	8	8	7	7	6	6	5	5	5	5	4	4	4	4	4	4	4	3	3	3	2	2	2	2	1",
	"16	16	15	15	14	14	14	14	14	14	13	13	13	12	12	11	11	10	10	10	9	9	8	8	8	8	7	7	6	6	6	6	6	6	6	6	5	5	5	4	4	4	3	3	3	2	2	2	2",
	"16	16	16	15	15	15	14	14	14	14	13	13	13	13	12	12	12	11	11	11	10	10	9	9	9	9	8	8	7	7	7	7	6	6	6	6	5	5	5	5	4	4	4	4	3	3	2	2	2",
	"16	16	16	16	15	15	15	15	15	14	14	13	13	13	12	12	12	11	11	11	10	10	9	9	9	9	8	8	7	7	7	7	6	6	6	6	5	5	5	5	4	4	4	4	3	3	3	3	2",
	"17	17	17	17	16	16	15	15	15	15	14	14	14	14	13	13	12	12	12	12	11	11	10	10	10	10	9	9	8	8	8	8	7	7	6	6	6	6	5	5	4	4	4	4	3	3	3	3	2",
	"19	19	18	18	17	17	17	17	16	16	15	15	15	15	14	14	14	13	13	13	12	12	12	12	11	11	10	10	10	10	10	9	9	8	8	8	8	8	7	7	6	6	6	6	5	5	4	4	4"
};

char *minqp444_12b[] = {
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"5	5	5	5	4	4	4	4	4	4	4	4	4	4	4	3	3	2	2	2	2	2	2	2	2	2	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"11	10	10	9	8	8	8	8	8	8	8	8	7	7	7	6	5	5	4	4	4	3	3	3	3	3	2	2	2	2	2	2	1	1	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"11	11	10	10	9	9	9	9	9	9	9	9	8	8	8	7	6	6	6	6	6	5	5	5	5	5	4	4	3	3	3	3	3	3	3	3	2	2	2	2	2	2	2	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0",
	"13	12	12	11	11	11	11	11	11	11	11	11	10	10	9	9	9	8	7	7	7	7	5	5	5	5	5	5	4	4	4	3	3	3	3	3	3	3	3	3	2	2	2	2	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0",
	"13	12	12	12	11	11	11	11	11	11	11	11	11	11	11	10	9	9	8	8	8	8	6	6	6	6	5	5	5	5	5	4	4	4	4	4	4	4	4	4	3	3	3	2	2	2	2	1	1	1	1	1	1	0	0	0	0	0	0	0	0",
	"13	13	12	12	11	11	11	11	11	11	11	11	11	11	11	10	9	9	9	9	9	9	9	8	8	8	7	7	7	6	5	5	5	5	5	5	5	4	4	4	4	4	4	4	3	3	3	2	2	1	1	1	1	1	1	1	0	0	0	0	0",
	"13	13	12	12	11	11	11	11	11	11	11	11	11	11	11	11	10	10	10	10	10	10	9	9	9	9	8	8	8	7	7	7	6	6	6	6	6	6	5	5	5	5	4	4	4	3	3	2	2	2	2	2	2	1	1	1	1	0	0	0	0",
	"13	13	12	12	11	11	11	11	11	11	11	11	11	11	11	11	11	11	11	11	11	11	10	10	10	10	9	9	8	7	7	7	7	7	6	6	6	6	6	6	5	5	5	4	4	4	4	3	3	2	2	2	2	1	1	1	1	1	1	0	0",
	"14	13	13	12	12	12	11	11	11	11	11	11	11	11	11	11	11	11	11	11	11	11	10	10	10	10	9	9	8	8	8	8	8	8	7	7	7	7	7	6	6	5	5	5	4	4	4	4	3	3	3	3	3	2	2	2	2	2	1	1	0",
	"14	14	13	13	13	13	13	13	13	13	13	13	13	13	13	12	12	12	12	12	11	11	11	11	11	11	10	10	9	9	9	9	9	9	9	9	8	8	8	7	7	7	6	6	6	5	5	5	5	4	4	3	3	3	2	2	2	2	1	1	1",
	"14	14	14	13	13	13	13	13	13	13	13	13	13	13	13	13	13	13	13	13	12	12	11	11	11	11	11	11	10	10	10	10	9	9	9	9	8	8	8	8	7	7	7	7	6	6	5	5	5	4	4	4	3	3	3	3	2	2	1	1	1",
	"14	14	14	14	13	13	13	13	13	13	13	13	13	13	13	13	13	13	13	13	12	12	12	12	12	12	11	11	10	10	10	10	9	9	9	9	8	8	8	8	7	7	7	7	6	6	6	6	5	4	4	4	3	3	3	3	2	2	1	1	1",
	"17	17	17	17	16	16	15	15	15	15	15	15	15	15	15	15	15	15	15	15	14	14	13	13	13	13	12	12	11	11	11	11	10	10	9	9	9	9	8	8	7	7	7	7	7	6	6	6	5	5	5	5	4	4	3	3	3	3	2	2	1",
	"22	22	21	21	20	20	20	20	19	19	18	18	18	18	17	17	17	16	16	16	15	15	15	15	14	14	13	13	13	13	13	12	12	11	11	11	11	11	10	10	9	9	9	9	9	8	8	7	7	7	7	7	6	6	5	5	5	5	4	4	3"
};

char *maxqp444_12b[] = {
	"12	12	12	12	12	12	11	11	11	10	9	9	6	6	5	5	5	4	4	4	4	4	4	3	3	3	3	3	2	2	2	2	2	2	2	2	2	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"14	14	13	13	12	12	12	12	12	12	11	11	9	9	9	8	8	7	7	7	7	5	5	5	5	5	4	4	4	3	3	3	2	2	2	2	2	2	2	1	1	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0",
	"16	15	15	14	13	13	13	13	13	13	13	13	12	12	12	11	10	10	9	9	9	7	7	7	7	7	6	6	5	5	5	5	4	4	4	3	3	3	3	3	3	2	2	2	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0",
	"16	16	15	15	14	14	14	14	14	14	14	14	13	13	13	12	11	11	10	10	10	8	8	8	8	8	7	7	6	5	5	5	5	5	5	5	4	4	3	3	3	3	3	2	2	2	2	2	2	1	1	0	0	0	0	0	0	0	0	0	0",
	"17	16	16	15	15	15	15	15	15	15	15	15	14	14	13	12	12	11	10	10	10	10	8	8	8	8	8	8	7	7	7	6	6	5	5	5	4	4	4	4	3	3	3	3	2	2	2	2	2	1	1	1	1	1	1	1	1	1	1	0	0",
	"17	16	16	16	15	15	15	15	15	15	15	15	14	14	14	13	12	12	11	11	11	11	9	9	9	9	8	8	8	8	7	6	6	6	6	6	5	5	5	5	4	4	4	3	3	3	3	2	2	2	2	2	2	1	1	1	1	1	1	1	0",
	"17	17	16	16	15	15	15	15	15	15	15	15	15	14	14	13	12	12	11	11	11	11	11	10	10	10	9	9	9	8	7	7	7	7	7	7	7	6	6	6	5	5	5	5	4	4	4	3	3	2	2	2	2	2	2	2	1	1	1	1	0",
	"18	18	17	17	16	16	16	16	16	16	16	16	16	15	15	14	13	13	12	12	12	12	11	11	11	11	10	10	10	8	8	8	7	7	7	7	7	7	6	6	6	6	5	5	5	4	4	3	3	3	3	3	3	2	2	2	2	1	1	1	1",
	"19	19	18	18	17	17	17	17	17	17	16	16	16	15	15	14	14	13	13	13	13	13	12	12	12	12	11	11	10	9	8	8	8	8	7	7	7	7	7	7	6	6	6	5	5	5	5	4	4	3	3	3	3	2	2	2	2	2	2	1	1",
	"20	19	19	18	18	18	17	17	17	17	17	17	17	16	16	15	14	14	13	13	13	13	12	12	12	12	11	11	10	10	9	9	9	9	8	8	8	8	8	7	7	6	6	6	5	5	5	5	4	4	4	4	4	3	3	3	3	3	2	2	1",
	"20	20	19	19	18	18	18	18	18	18	17	17	17	16	16	15	15	14	14	14	13	13	12	12	12	12	11	11	10	10	10	10	10	10	10	10	9	9	9	8	8	8	7	7	7	6	6	6	6	5	5	4	4	4	3	3	3	3	2	2	2",
	"20	20	20	19	19	19	18	18	18	18	17	17	17	17	16	16	16	15	15	15	14	14	13	13	13	13	12	12	11	11	11	11	10	10	10	10	9	9	9	9	8	8	8	8	7	7	6	6	6	5	5	5	4	4	4	4	3	3	2	2	2",
	"20	20	20	20	19	19	19	19	19	18	18	17	17	17	16	16	16	15	15	15	14	14	13	13	13	13	12	12	11	11	11	11	10	10	10	10	9	9	9	9	8	8	8	8	7	7	7	7	6	5	5	5	4	4	4	4	3	3	2	2	2",
	"21	21	21	21	20	20	19	19	19	19	18	18	18	18	17	17	16	16	16	16	15	15	14	14	14	14	13	13	12	12	12	12	11	11	10	10	10	10	9	9	8	8	8	8	8	7	7	7	6	6	6	6	5	5	4	4	4	4	3	3	2",
	"23	23	22	22	21	21	21	21	20	20	19	19	19	19	18	18	18	17	17	17	16	16	16	16	15	15	14	14	14	14	14	13	13	12	12	12	12	12	11	11	10	10	10	10	10	9	9	8	8	8	8	8	7	7	6	6	6	6	5	5	4"
};

char *minqp422_8b[] = {
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"2	2	2	2	2	2	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"3	3	3	3	3	2	2	1	1	1	1	1	1	1	1	0	0	0	0	0	0",
	"3	3	3	3	3	2	2	1	1	1	1	1	1	1	1	1	1	0	0	0	0",
	"3	3	3	3	3	2	2	1	1	1	1	1	1	1	1	1	1	1	0	0	0",
	"3	3	3	3	3	3	2	2	2	2	2	2	2	2	2	1	1	1	1	0	0",
	"3	3	3	3	3	3	2	2	2	2	2	2	2	2	2	2	2	1	1	1	1",
	"3	3	3	3	3	3	3	3	3	3	3	3	3	3	3	2	2	2	1	1	1",
	"5	5	5	5	5	4	4	4	4	4	4	4	4	3	3	3	2	2	1	1	1",
	"5	5	5	5	5	5	5	4	4	4	4	4	4	4	3	3	3	2	2	1	1",
	"5	5	5	5	5	5	5	5	5	5	5	5	5	4	4	3	3	2	2	1	1",
	"8	8	7	7	7	7	7	7	7	7	6	6	5	5	4	4	3	3	2	2	2",
	"12	12	11	11	10	10	9	9	8	8	7	7	6	6	5	5	4	4	4	3	3"
};

char *maxqp422_8b[] = {
	"4	4	3	3	2	2	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"4	4	4	4	4	3	2	2	1	1	1	1	1	0	0	0	0	0	0	0	0",
	"5	5	5	5	5	4	3	2	1	1	1	1	1	1	1	1	1	0	0	0	0",
	"6	6	6	6	6	5	4	3	2	2	2	2	2	1	1	1	1	1	1	0	0",
	"7	7	7	7	7	6	5	3	2	2	2	2	2	2	2	1	1	1	1	1	1",
	"7	7	7	7	7	6	5	4	3	3	3	2	2	2	2	2	2	1	1	1	1",
	"7	7	7	7	7	6	5	4	3	3	3	3	3	2	2	2	2	2	1	1	1",
	"8	8	8	8	8	7	6	5	4	4	4	3	3	3	3	2	2	2	2	1	1",
	"9	9	9	8	8	7	6	6	5	5	5	4	4	3	3	3	3	2	2	2	2",
	"10	10	9	9	9	8	7	6	5	5	5	5	4	4	4	3	3	3	2	2	2",
	"10	10	10	9	9	8	7	7	6	6	6	5	5	4	4	4	3	3	2	2	2",
	"11	11	10	10	9	9	8	7	7	7	6	6	5	5	4	4	4	3	3	2	2",
	"11	11	11	10	9	9	8	8	7	7	7	6	6	5	5	4	4	3	3	2	2",
	"12	12	11	11	10	10	9	9	8	8	7	7	6	6	5	5	4	4	3	3	3",
	"13	13	12	12	11	11	10	10	9	9	8	8	7	7	6	6	5	5	5	4	4"
};

char *minqp422_10b[] = {
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"4	4	4	2	2	2	2	2	2	2	2	2	2	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0",
	"5	5	5	4	3	3	3	3	2	2	2	2	2	2	2	2	2	1	1	1	1	0	0	0	0	0	0	0	0",
	"6	6	6	6	5	4	4	4	3	3	3	3	3	2	2	2	2	2	2	1	1	1	0	0	0	0	0	0	0",
	"6	6	6	6	5	5	5	4	4	4	4	4	4	4	4	3	3	3	2	2	2	1	1	1	0	0	0	0	0",
	"6	6	6	6	6	5	5	5	5	4	4	4	4	4	4	4	4	3	3	3	3	2	1	1	0	0	0	0	0",
	"6	6	6	6	6	5	5	5	5	5	5	5	5	4	4	4	4	4	3	3	3	2	2	1	1	1	0	0	0",
	"7	7	7	7	7	6	6	6	6	6	6	5	5	5	5	4	4	4	4	3	3	3	2	2	1	1	1	1	1",
	"7	7	7	7	7	6	6	6	6	6	6	6	6	5	5	5	4	4	4	4	4	3	2	2	1	1	1	1	1",
	"8	8	7	7	7	7	7	7	7	7	7	7	6	6	6	6	5	5	4	4	4	3	3	2	2	1	1	1	1",
	"9	9	9	8	8	8	8	8	8	8	8	7	7	6	6	6	5	5	5	5	5	3	3	2	2	2	1	1	1",
	"9	9	9	9	8	8	8	8	8	8	8	8	7	7	6	6	6	6	6	5	5	4	3	3	2	2	1	1	1",
	"9	9	9	9	9	9	9	9	9	9	9	8	8	8	8	7	7	6	6	5	5	4	3	3	3	2	2	1	1",
	"12	12	11	11	11	11	11	11	11	11	10	10	9	9	8	8	7	7	6	6	5	5	4	4	3	3	2	2	1",
	"16	16	15	15	14	14	13	13	12	12	11	11	10	10	9	9	8	8	8	7	7	6	6	5	5	5	4	4	3"
};

char *maxqp422_10b[] = {
	"8	8	7	5	4	4	3	3	2	2	2	2	2	2	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0",
	"8	8	8	6	6	5	4	4	3	3	3	3	3	2	2	2	2	2	2	1	1	1	0	0	0	0	0	0	0",
	"9	9	9	8	7	6	5	4	3	3	3	3	3	3	3	3	3	2	2	2	2	1	1	1	0	0	0	0	0",
	"10	10	10	10	9	8	7	6	5	5	5	5	5	4	4	3	3	3	3	2	2	2	1	1	1	1	1	1	1",
	"11	11	11	11	10	9	8	6	5	5	5	5	5	5	5	4	4	4	3	3	3	2	2	2	1	1	1	1	1",
	"11	11	11	11	11	10	9	8	7	6	6	5	5	5	5	5	5	4	4	4	4	3	2	2	1	1	1	1	1",
	"11	11	11	11	11	10	9	8	7	7	7	7	7	6	6	6	5	5	4	4	4	3	3	2	2	2	1	1	1",
	"12	12	12	12	12	11	10	9	8	8	8	7	7	7	7	6	5	5	5	4	4	4	3	3	2	2	2	2	2",
	"13	13	13	12	12	11	10	10	9	9	9	8	8	7	7	7	6	5	5	5	5	4	3	3	2	2	2	2	2",
	"14	14	13	13	13	12	11	10	9	9	9	9	8	8	8	7	6	6	5	5	5	4	4	3	3	2	2	2	2",
	"14	14	14	13	13	12	11	11	10	10	10	9	9	8	8	8	7	7	6	6	6	4	4	3	3	3	2	2	2",
	"15	15	14	14	13	13	12	11	11	11	10	10	9	9	8	8	8	7	7	6	6	5	4	4	3	3	2	2	2",
	"15	15	15	14	13	13	12	12	11	11	11	10	10	9	9	8	8	7	7	6	6	5	4	4	4	3	3	2	2",
	"16	16	15	15	14	14	13	13	12	12	11	11	10	10	9	9	8	8	7	7	6	6	5	5	4	4	3	3	2",
	"17	17	16	16	15	15	14	14	13	13	12	12	11	11	10	10	9	9	9	8	8	7	7	6	6	6	5	5	4"
};

char *minqp422_12b[] = {
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"4	4	4	4	4	3	3	3	2	2	2	2	2	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"9	9	9	8	7	6	5	5	4	4	4	4	4	4	3	3	2	1	1	1	1	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"10	10	10	10	8	8	8	7	6	6	6	6	6	5	4	3	3	3	3	2	2	2	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0",
	"11	11	11	11	10	9	9	8	7	7	7	7	6	6	5	4	4	4	3	3	3	2	2	2	2	2	2	1	1	1	0	0	0	0	0	0	0",
	"11	11	11	11	11	10	10	9	9	8	8	7	6	6	5	5	5	4	4	4	4	3	2	2	2	2	2	1	1	1	1	0	0	0	0	0	0",
	"11	11	11	11	11	10	10	10	9	9	9	9	8	7	7	7	6	6	5	5	5	4	4	3	3	3	2	2	2	2	2	1	1	0	0	0	0",
	"11	11	11	11	11	11	10	10	10	10	10	9	8	8	8	7	6	6	6	5	5	5	4	4	3	3	3	3	3	2	2	2	1	1	0	0	0",
	"11	11	11	11	11	11	11	11	11	11	11	10	9	8	8	8	7	6	6	6	6	5	4	4	3	3	3	3	3	3	3	2	2	1	0	0	0",
	"11	11	11	11	11	11	11	11	11	11	11	11	9	9	9	8	7	7	6	6	6	5	5	4	4	3	3	3	3	3	3	2	2	1	1	0	0",
	"13	13	13	13	13	12	12	12	12	12	12	11	11	10	10	10	9	9	8	8	8	6	6	5	5	5	4	4	4	4	3	3	2	2	1	1	1",
	"13	13	13	13	13	13	13	13	13	13	12	12	11	11	10	10	10	9	9	8	8	7	6	6	5	5	4	4	4	4	4	3	3	2	2	1	1",
	"13	13	13	13	13	13	13	13	13	13	13	12	12	11	11	10	10	9	9	8	8	7	6	6	6	5	5	4	4	4	4	3	3	2	2	1	1",
	"16	16	15	15	15	15	15	15	15	15	14	14	13	13	12	12	11	11	10	10	9	9	8	8	7	7	6	6	5	5	4	4	4	3	3	2	2",
	"20	20	19	19	18	18	17	17	16	16	15	15	14	14	13	13	12	12	12	11	11	10	10	9	9	9	8	8	7	7	6	6	6	5	5	4	4"
};

char *maxqp422_12b[] = {
	"12	12	11	9	6	6	5	5	4	4	4	3	3	3	2	2	2	2	2	2	2	1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0",
	"12	12	12	10	9	8	7	7	6	6	5	5	5	4	4	4	3	3	3	2	2	2	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0",
	"13	13	13	12	10	9	8	7	6	6	6	6	6	6	5	5	4	3	3	3	3	2	2	2	1	1	1	1	1	0	0	0	0	0	0	0	0",
	"14	14	14	14	12	11	10	9	8	8	8	8	8	7	6	5	5	5	5	4	4	4	3	3	2	2	2	2	2	1	1	0	0	0	0	0	0",
	"15	15	15	15	14	13	12	10	9	9	9	9	8	8	7	6	6	6	5	5	5	4	4	4	3	3	3	2	2	2	1	1	1	1	0	0	0",
	"15	15	15	15	15	14	13	12	11	10	10	9	8	8	7	7	7	6	6	6	6	5	4	4	3	3	3	2	2	2	2	1	1	1	1	1	1",
	"15	15	15	15	15	14	13	12	11	11	11	11	10	9	9	9	8	8	7	7	7	6	6	5	5	5	4	4	4	3	3	2	2	1	1	1	1",
	"16	16	16	16	16	15	14	13	12	12	12	11	10	10	10	9	8	8	8	7	7	7	6	6	5	5	5	5	5	3	3	3	2	2	1	1	1",
	"17	17	17	16	16	15	14	14	13	13	13	12	11	10	10	10	9	8	8	8	8	7	6	6	5	5	5	5	5	4	4	3	3	2	1	1	1",
	"18	18	17	17	17	16	15	14	13	13	13	13	11	11	11	10	9	9	8	8	8	7	7	6	6	5	5	5	5	4	4	3	3	2	2	1	1",
	"18	18	18	17	17	16	15	15	14	14	14	13	13	12	12	12	11	11	10	10	10	8	8	7	7	7	6	6	6	5	4	4	3	3	2	2	2",
	"19	19	18	18	17	17	16	15	15	15	14	14	13	13	12	12	12	11	11	10	10	9	8	8	7	7	6	6	6	5	5	4	4	3	3	2	2",
	"19	19	19	18	17	17	16	16	15	15	15	14	14	13	13	12	12	11	11	10	10	9	8	8	8	7	7	6	6	5	5	4	4	3	3	2	2",
	"20	20	19	19	18	18	17	17	16	16	15	15	14	14	13	13	12	12	11	11	10	10	9	9	8	8	7	7	6	6	5	5	5	4	4	3	3",
	"21	21	20	20	19	19	18	18	17	17	16	16	15	15	14	14	13	13	13	12	12	11	11	10	10	10	9	9	8	8	7	7	7	6	6	5	5"
};

char *minqp420_8b[] = {
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0	0",
	"1	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0	0",
	"3	3	3	3	3	2	1	1	1	1	1	1	1	0	0	0	0",
	"3	3	3	3	3	2	2	1	1	1	1	1	1	1	0	0	0",
	"3	3	3	3	3	3	2	2	1	1	1	1	1	1	1	0	0",
	"3	3	3	3	3	3	2	2	2	2	2	2	2	1	1	1	0",
	"3	3	3	3	3	3	2	2	2	2	2	2	2	2	1	1	0",
	"3	3	3	3	3	3	3	3	3	3	3	3	3	2	2	1	1",
	"5	5	5	5	5	4	4	4	4	4	3	3	3	3	2	1	1",
	"5	5	5	5	5	5	5	4	4	4	4	4	4	3	2	2	1",
	"5	5	5	5	5	5	5	5	5	5	5	4	4	3	3	2	1",
	"9	8	8	7	7	7	7	7	7	6	5	5	4	3	3	3	2",
	"13	12	12	11	10	10	9	8	8	7	6	6	5	5	4	4	3"
};

char *maxqp420_8b[] = {
	"4	4	3	3	2	2	1	1	0	0	0	0	0	0	0	0	0",
	"4	4	4	4	4	3	2	2	1	1	1	1	0	0	0	0	0",
	"5	5	5	5	5	4	3	2	1	1	1	1	1	1	0	0	0",
	"6	6	6	6	6	5	4	3	2	2	2	1	1	1	1	0	0",
	"7	7	7	7	7	5	4	3	2	2	2	2	2	1	1	1	0",
	"7	7	7	7	7	6	5	4	3	3	3	2	2	2	1	1	0",
	"7	7	7	7	7	6	5	4	3	3	3	3	2	2	2	1	1",
	"8	8	8	8	8	7	6	5	4	4	4	3	3	2	2	2	1",
	"9	9	9	8	8	7	6	6	5	5	4	4	3	3	2	2	1",
	"10	10	9	9	9	8	7	6	5	5	5	4	4	3	3	2	2",
	"10	10	10	9	9	8	8	7	6	6	5	5	4	4	3	2	2",
	"11	11	10	10	9	9	8	7	7	6	6	5	5	4	3	3	2",
	"11	11	11	10	9	9	9	8	7	7	6	5	5	4	4	3	2",
	"13	12	12	11	10	10	9	8	8	7	6	6	5	4	4	4	3",
	"14	13	13	12	11	11	10	9	9	8	7	7	6	6	5	5	4"
};

char *minqp420_10b[] = {
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"4	4	4	3	2	2	2	2	2	2	2	2	1	0	0	0	0	0	0	0	0	0	0",
	"4	4	4	3	3	3	3	3	3	2	2	2	2	2	1	0	0	0	0	0	0	0	0",
	"5	5	5	4	4	4	4	4	4	3	3	2	2	2	2	1	0	0	0	0	0	0	0",
	"7	7	7	6	6	5	5	4	4	3	3	3	3	2	2	2	1	1	1	0	0	0	0",
	"7	7	7	7	7	6	5	5	5	5	5	4	3	3	2	2	1	1	1	1	1	0	0",
	"7	7	7	7	7	6	6	5	5	5	5	4	4	4	3	2	2	2	2	1	1	1	0",
	"7	7	7	7	7	7	6	6	6	6	6	5	4	4	4	3	2	2	2	1	1	1	0",
	"7	7	7	7	7	7	7	7	6	6	6	6	5	5	4	4	3	3	2	2	2	1	1",
	"7	7	7	7	7	7	7	7	7	7	7	6	6	5	5	4	4	3	3	2	2	1	1",
	"9	9	9	9	9	8	8	8	8	8	7	7	6	6	5	5	4	4	3	3	2	2	1",
	"9	9	9	9	9	9	8	8	8	8	8	8	8	7	6	6	5	4	4	3	3	2	1",
	"9	9	9	9	9	9	9	9	9	9	9	8	8	7	7	6	5	4	4	3	3	2	1",
	"13	12	12	11	11	11	11	11	11	10	9	9	8	7	7	6	5	5	4	3	3	2	2",
	"17	16	16	15	14	14	13	12	12	11	10	10	10	9	8	8	7	6	6	5	5	4	4"
};

char *maxqp420_10b[] = {
	"8	8	7	6	4	4	3	3	2	1	1	1	1	1	0	0	0	0	0	0	0	0	0",
	"8	8	8	7	6	5	4	4	3	3	3	3	2	1	1	1	0	0	0	0	0	0	0",
	"9	9	9	8	8	7	6	5	4	3	3	3	3	3	2	1	1	1	0	0	0	0	0",
	"10	10	10	9	9	8	7	6	5	4	4	3	3	3	3	2	1	1	1	1	1	0	0",
	"11	11	11	10	10	8	7	6	5	4	4	4	4	3	3	3	2	2	2	1	1	1	0",
	"11	11	11	10	10	9	8	7	6	6	6	5	4	4	3	3	2	2	2	2	2	1	1",
	"11	11	11	11	11	10	9	8	7	7	7	6	5	5	4	3	3	3	3	2	2	2	1",
	"12	12	12	12	12	11	10	9	8	8	8	7	6	5	5	4	3	3	3	2	2	2	1",
	"13	13	13	12	12	11	10	10	9	9	8	8	7	7	6	5	4	4	3	3	3	2	2",
	"14	14	13	13	13	12	11	10	9	9	9	8	8	7	7	6	5	4	4	3	3	2	2",
	"14	14	14	13	13	12	12	11	10	10	9	9	8	8	7	6	5	5	4	4	3	3	2",
	"15	15	14	14	13	13	12	11	11	10	10	9	9	8	7	7	6	5	5	4	4	3	2",
	"15	15	15	14	13	13	13	12	11	11	10	9	9	8	8	7	6	5	5	4	4	3	2",
	"17	16	16	15	14	14	13	12	12	11	10	10	9	8	8	7	6	6	5	4	4	3	3",
	"18	17	17	16	15	15	14	13	13	12	11	11	11	10	9	9	8	7	7	6	6	5	5"
};

char *minqp420_12b[] = {
	"0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"4	4	4	4	4	4	3	3	2	2	2	2	2	2	2	1	0	0	0	0	0	0	0	0	0	0	0	0	0",
	"9	8	8	7	7	6	5	5	4	4	4	4	3	3	3	2	2	1	0	0	0	0	0	0	0	0	0	0	0",
	"10	9	9	8	8	8	7	7	6	6	6	5	5	4	4	3	2	2	1	1	1	0	0	0	0	0	0	0	0",
	"11	10	10	10	10	9	9	8	7	6	6	6	6	5	5	4	3	3	3	2	2	1	0	0	0	0	0	0	0",
	"11	11	11	11	11	10	10	9	9	9	9	8	7	6	5	5	4	4	3	3	3	2	1	1	0	0	0	0	0",
	"11	11	11	11	11	11	10	10	9	9	9	8	8	7	6	5	5	5	5	4	3	3	2	1	1	1	1	1	0",
	"11	11	11	11	11	11	11	10	10	10	10	9	8	8	8	7	6	6	5	4	4	3	2	2	1	1	1	1	1",
	"11	11	11	11	11	11	11	11	11	11	10	10	9	9	8	8	7	7	6	5	5	4	4	2	2	1	1	1	1",
	"11	11	11	11	11	11	11	11	11	11	11	10	10	9	9	8	8	7	7	6	5	4	4	3	2	2	1	1	1",
	"13	13	13	13	13	13	13	12	12	12	11	11	10	10	9	9	8	8	7	7	6	5	4	3	3	2	2	1	1",
	"13	13	13	13	13	13	13	13	13	12	12	12	12	11	10	10	9	8	8	7	7	6	5	4	3	3	2	2	1",
	"13	13	13	13	13	13	13	13	13	13	13	12	12	11	11	10	9	8	8	7	7	6	5	4	4	3	2	2	1",
	"15	15	15	15	15	15	15	15	15	14	13	13	12	11	11	10	9	9	8	8	7	6	6	5	4	4	3	3	2",
	"21	20	20	19	18	18	17	16	16	15	14	14	14	13	12	12	11	10	10	10	9	8	8	7	6	6	5	5	4"
};

char *maxqp420_12b[] = {
	"11	10	9	8	6	6	5	5	4	3	3	2	2	2	1	1	1	1	1	1	1	0	0	0	0	0	0	0	0",
	"12	11	11	10	9	8	7	7	6	6	5	5	4	3	3	2	1	1	1	1	1	1	1	0	0	0	0	0	0",
	"13	12	12	11	11	10	9	8	7	6	6	6	5	5	4	3	3	2	1	1	1	1	1	0	0	0	0	0	0",
	"14	13	13	12	12	11	10	9	8	7	7	6	6	5	5	4	3	3	2	2	2	1	1	1	0	0	0	0	0",
	"15	14	14	13	13	11	10	9	8	7	7	7	7	6	6	5	4	4	4	3	3	2	1	1	1	0	0	0	0",
	"15	15	15	14	14	13	12	11	10	10	10	9	8	7	6	6	5	5	4	4	4	3	2	2	1	1	0	0	0",
	"15	15	15	15	15	14	13	12	11	11	11	10	9	8	7	6	6	6	6	5	4	4	3	2	2	2	1	1	0",
	"16	16	16	16	16	15	14	13	12	12	12	11	10	9	9	8	7	7	6	5	5	4	3	3	2	2	2	1	1",
	"17	17	17	16	16	15	14	14	13	13	12	12	11	11	10	9	8	8	7	6	6	5	5	3	3	2	2	1	1",
	"18	18	17	17	17	16	15	14	13	13	13	12	12	11	11	10	9	8	8	7	6	5	5	4	3	3	2	2	1",
	"18	18	18	17	17	16	16	15	14	14	13	13	12	12	11	10	9	9	8	8	7	6	5	4	4	3	3	2	2",
	"19	19	18	18	17	17	16	15	15	14	14	13	13	12	11	11	10	9	9	8	8	7	6	5	4	4	3	3	2",
	"19	19	19	18	17	17	17	16	15	15	14	13	13	12	12	11	10	9	9	8	8	7	6	5	5	4	3	3	2",
	"21	20	20	19	18	18	17	16	16	15	14	14	13	12	12	11	10	10	9	9	8	7	7	6	5	5	4	4	3",
	"22	21	21	20	19	19	18	17	17	16	15	15	15	14	13	13	12	11	11	11	10	9	9	8	7	7	6	6	5"
};

int **minqp_444[3], **maxqp_444[3];
int **minqp_422[3], **maxqp_422[3];
int **minqp_420[3], **maxqp_420[3];

void line_to_int(char *s, int **l)
{
	int i, n;

	n = 1;
	for (i = 0; s[i] != '\0'; ++i)
	{
		if (s[i] == '\t')
			n++;
	}
	*l = (int *)malloc(sizeof(int) * n);
	
	for (i = 0; i < n - 1; ++i)
	{
		(*l)[i] = atoi(s);
		while (s[0] != '\t')
			s++;
		s++;
	}
	(*l)[i] = atoi(s);
}

void make_qp_table(char *table[], int ***dest)
{
	int i;

	*dest = (int **)malloc(sizeof(int *) * 15);
	
	for (i = 0; i < 15; ++i)
		line_to_int(table[i], &((*dest)[i]));
}

void make_qp_tables(void)
{
	make_qp_table(minqp444_8b, &minqp_444[0]);
	make_qp_table(maxqp444_8b, &maxqp_444[0]);
	make_qp_table(minqp444_10b, &minqp_444[1]);
	make_qp_table(maxqp444_10b, &maxqp_444[1]);
	make_qp_table(minqp444_12b, &minqp_444[2]);
	make_qp_table(maxqp444_12b, &maxqp_444[2]);

	make_qp_table(minqp422_8b, &minqp_422[0]);
	make_qp_table(maxqp422_8b, &maxqp_422[0]);
	make_qp_table(minqp422_10b, &minqp_422[1]);
	make_qp_table(maxqp422_10b, &maxqp_422[1]);
	make_qp_table(minqp422_12b, &minqp_422[2]);
	make_qp_table(maxqp422_12b, &maxqp_422[2]);

	make_qp_table(minqp420_8b, &minqp_420[0]);
	make_qp_table(maxqp420_8b, &maxqp_420[0]);
	make_qp_table(minqp420_10b, &minqp_420[1]);
	make_qp_table(maxqp420_10b, &maxqp_420[1]);
	make_qp_table(minqp420_12b, &minqp_420[2]);
	make_qp_table(maxqp420_12b, &maxqp_420[2]);
}