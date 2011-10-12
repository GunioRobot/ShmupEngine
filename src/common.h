#ifndef COMMON_H
#define COMMON_H

#include <enet/enet.h>
#include "vector.h"

typedef struct {
	vec2d pos;
	vec2d vel;
	vec2d acc;
	short keyflags;
} player;

typedef struct {
	vec2d pos;
	vec2d vel;
	vec2d acc;
	unsigned int color;
	unsigned int btype;
	float padding[2];
} bullet;

typedef struct {
	int size;
	int n_active;	
	GLuint tex[2];
	GLuint prog;
	bullet *bdata;
} bpool;

typedef struct {
	int quit;
	int render_type;
	int network_type;
	int window_width;
	int window_height;
	int num_players;
	vec2d emitter;
	vec2d gravity;
	player player[4];
	bpool *bpool;
	ENetHost *host;
	ENetPeer *peer;
} shmup_game;

#endif