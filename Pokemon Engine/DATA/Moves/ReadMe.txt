This will teach you the basic format of pokemon moves:

Name - the moves name
PP - The moves PP
Base Power - The moves attack damage
Accuracy - The moves accuracy
Type - The moves type as an index
Physical/Special/Status split - 1 = physical (attack v defense), 2 is special (special atk v special def ), 3 is "status" (No damage)
Particle effects -Will be explained below:

//Types:
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

//Particle effect:
Emitter type;Particle image;arguments for emmitter type

Emitter types:
1 = Burst emitter: Arguments <count;Xspeed;Yspeed;Xvariance;YVariance;startsize;endsize>

eg:
Tackle
35
50
100
1
1
1;Glow;20;0;0;0.01;0.01;0;1;