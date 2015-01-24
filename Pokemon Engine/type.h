#pragma once

#include "stdafx.h"

#define TYPE_NORMAL 1
#define TYPE_GROUND 2
#define TYPE_STEEL 3
#define TYPE_GRASS 4
#define TYPE_DRAGON 5
#define TYPE_FIGHT 6
#define TYPE_ROCK 7
#define TYPE_QMARKS 8
#define TYPE_ELECTRIC 9
#define TYPE_DARK 10
#define TYPE_FLYING 11
#define TYPE_BUG 12
#define TYPE_FIRE 13
#define TYPE_PHYCHIC 14
#define TYPE_POISON 15
#define TYPE_GHOST 16
#define TYPE_WATER 17
#define TYPE_ICE 18
#define TYPE_FAIRY 19

class Type
{
public:
	int type;
	float TestFor( Type target );
};