//
//  player.c
//  ShmupEngine
//

#include "player.h"

void
player_update(shmup_game *g, player *p, float dt)
{

	if (p->keyflags & KF_MOV_L) {
		if (p->keyflags & KF_MOV_U) {
			p->vel = v2(-PLAYER_D_SPEED, PLAYER_D_SPEED);
		} else if (p->keyflags & KF_MOV_D) {
			p->vel = v2(-PLAYER_D_SPEED, -PLAYER_D_SPEED);
		} else {
			p->vel = v2(-PLAYER_SPEED, 0);
		}

	} else if (p->keyflags & KF_MOV_R) {
		if (p->keyflags & KF_MOV_U) {
			p->vel = v2(PLAYER_D_SPEED, PLAYER_D_SPEED);
		} else if (p->keyflags & KF_MOV_D) {
			p->vel = v2(PLAYER_D_SPEED, -PLAYER_D_SPEED);
		} else {
			p->vel = v2(PLAYER_SPEED, 0);
		}

	} else {
		if (p->keyflags & KF_MOV_U) {
			p->vel = v2(0, PLAYER_SPEED);
		} else if (p->keyflags & KF_MOV_D) {
			p->vel = v2(0, -PLAYER_SPEED);
		} else {
			p->acc = v2zero;
			p->vel = v2mul(p->vel, 0.8);
		}
	}

	if (p->keyflags & KF_FIR_1) {
		shmup_game_fire(g, 1, 1, v2add(p->pos, v2(20, -10)),
				v2(BULLET_SPEED,0), v2zero);
		shmup_game_fire(g, 1, 1, v2add(p->pos, v2(20, 10)),
				v2(BULLET_SPEED,0), v2zero);
	}

	p->pos = v2add(p->pos, v2mul(p->vel, dt));
	p->vel = v2add(p->vel, v2mul(p->acc, dt));
	if (v2lensq(p->vel) > PLAYER_SPEED_SQ) {
		p->vel = v2mul(v2normal(p->vel), PLAYER_SPEED);
	}
}