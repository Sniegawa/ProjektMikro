/*
 * ringbuffer.h
 *
 *  Created on: Nov 10, 2026
 *      Author: sniegawa
 */

#ifndef INC_RINGBUFFER_H_
#define INC_RINGBUFFER_H_

#include <stdint.h>
#include <stdbool.h>


#define RX_RINGBUFFER_SIZE 1024
#define RX_RINGBUFFER_MASK (RX_RINGBUFFER_SIZE - 1)

#define TX_RINGBUFFER_SIZE 4096
#define TX_RINGBUFFER_MASK (TX_RINGBUFFER_SIZE - 1)


typedef struct
{
	uint8_t data[RX_RINGBUFFER_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
	volatile uint16_t overflowCount;
}RxRingBuffer;

void RxRB_Init(RxRingBuffer* b);
bool RxRB_Push(RxRingBuffer* b, uint8_t v);
bool RxRB_Pop(RxRingBuffer* b, uint8_t* v);


typedef struct
{
	uint8_t data[TX_RINGBUFFER_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
	volatile uint16_t overflowCount;
	volatile bool active;
}TxRingBuffer;

void TxRB_Init(TxRingBuffer* b);
bool TxRB_Push(TxRingBuffer* b, uint8_t v);
bool TxRB_Pop(TxRingBuffer* b, uint8_t* v);



#endif /* INC_RINGBUFFER_H_ */
