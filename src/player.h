//
//  player.h
//  ShmupEngine
//
#ifndef PLAYER_H
#define PLAYER_H

#include <GL/glfw.h>

#include "vector.h"
#include "common.h"
#include "game.h"

#define PLAYER_ACC 3000
#define PLAYER_SPEED 300
#define PLAYER_D_SPEED PLAYER_SPEED * HALFSQRT2
#define PLAYER_SPEED_SQ PLAYER_SPEED * PLAYER_SPEED
#define PLAYER_D_ACC PLAYER_ACC * HALFSQRT2
#define BULLET_SPEED 2000

#define KF_NONE		0
#define KF_MOV_L	1 << 0
#define KF_MOV_R	1 << 1
#define KF_MOV_U	1 << 2
#define KF_MOV_D	1 << 3
#define KF_FIR_1	1 << 4

void player_update(shmup_game *g, player *p, float dt);
void player_fire(player *p);

#endif