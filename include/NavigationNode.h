#ifndef NAVIGATION_NODE_H
#define NAVIGATION_NODE_H

#include "types.h"

struct NavigationNode
{
#define MAXNEIGHBORS 8
// #define MAXNEIGHBORS 4
	// Este no e um bloqueio ?
	bool8 blocked = false;
	// Numero de vizinhos que este no possui
	uint8 neighborCount;
	// Coordenadas do no
	uint16 x;
	uint16 y;
	// Item
	char item = 'G';  
	// Custo da origem ate este n�
	uint16 GCost = 0;
	// Custo deste no ate o destino
	uint16 HCost = 0;
	// Cust total do n�
	uint16 FCost = 0;
	NavigationNode* parent = nullptr;
	// const char value = {};
	NavigationNode* neighbor[MAXNEIGHBORS] = {};

	inline void addNeighbor(NavigationNode* other)
	{
		NavigationNode** nodePtr = neighbor + neighborCount++;
		*nodePtr = other;
		//assert(neighborCount <= MAXNEIGHBORS, "MAX_EIGHT_NEIGHBOR");
	}

#undef MAXNEIGHBORS
};

#endif