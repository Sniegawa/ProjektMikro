/*
 * ringbuffer.c
 *
 *  Created on: Nov 10, 2026
 *      Author: sniegawa
 */


#include "ringbuffer.h"

void RxRB_Init(RxRingBuffer* b)
{
	b->head = 0;
	b->tail = 0;
	b->overflowCount = 0;
}
bool RxRB_Push(RxRingBuffer* b, uint8_t v)
{
	// Volatile zeby kompilator nie optymalizował przypisania
	volatile uint16_t next = (b->head + 1) & RX_RINGBUFFER_MASK; // Jeżeli przeskoczymy poza maksymalny indeks operacja AND z maską wyzeruje nam indeks

	if(next == b->tail)
	{
		b->overflowCount++;
		return false;
	}

	b->data[b->head] = v;
	b->head = next;
	return true;
}

bool RxRB_Pop(RxRingBuffer* b, uint8_t* v)
{
	if(b->head == b->tail)
		return false;

	*v = b->data[b->tail];
	b->tail = (b->tail + 1) & RX_RINGBUFFER_MASK;

	return true;
}

void TxRB_Init(TxRingBuffer* b)
{
	b->head = 0;
	b->tail = 0;
	b->overflowCount = 0;
	b->active = false;
}
bool TxRB_Push(TxRingBuffer* b, uint8_t v)
{
	// Volatile zeby kompilator nie optymalizował przypisania
	volatile uint16_t next = (b->head + 1) & TX_RINGBUFFER_MASK; // Jeżeli przeskoczymy poza maksymalny indeks operacja AND z maską wyzeruje nam indeks

	if(next == b->tail)
	{
		b->overflowCount++;
		return false;
	}

	b->data[b->head] = v;
	b->head = next;
	return true;
}
bool TxRB_Pop(TxRingBuffer* b, uint8_t* v)
{
	if(b->head == b->tail)
		return false;

	*v = b->data[b->tail];
	b->tail = (b->tail + 1) & TX_RINGBUFFER_MASK;

	return true;
}
