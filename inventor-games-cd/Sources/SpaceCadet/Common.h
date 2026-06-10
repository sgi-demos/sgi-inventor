#include <stdlib.h>

////////////////////////////////////////////////////////////////////////////
//
// Some generally useful macros.
//
////////////////////////////////////////////////////////////////////////////

// Returns a random number in the range from min to max
#define RANDOM(min, max)	((min) + drand48() * ((max) - (min)))

// Converts degrees to radians
#define RADIANS(degrees)	((degrees) * M_PI / 180.0)

////////////////////////////////////////////////////////////////////////////
//
// Scoring values.
//
////////////////////////////////////////////////////////////////////////////

#define HIGH_SCORE_FILE			"/usr/tmp/SpaceCadet.scores"

#define NUM_HIGH_SCORES			10

#define SCORE_DEACTIVATE_MINE		100
#define SCORE_ABSORBED_ENERGY(surplus)	((int) ((surplus) * 100))
#define SCORE_LEVEL_COMPLETE(level)	((level) >= 10 ? 100000 :	      \
					 (level) * (level) * 1000)
#define SCORE_TIME_BONUS(elapsed, level)((elapsed) > 120.0 ? 0 :	      \
					 (1000 * (level) *		      \
					  (int) (120.0 - (elapsed))))

////////////////////////////////////////////////////////////////////////////
//
// Changes in energy based on various occurrences.
//
////////////////////////////////////////////////////////////////////////////

#define ACCEL_ENERGY_LOSS			.001

#define BLAST_ENERGY_LOSS			.001

// Max loss is .2 (at force 1)
#define MINE_ENERGY_LOSS(force) 		(0.2 * (force))

#define CRASH_ENERGY_LOSS(speed, maxSpeed)	(.4 * (speed) / (maxSpeed))


////////////////////////////////////////////////////////////////////////////
//
// Speed constants, in units per second.
//
////////////////////////////////////////////////////////////////////////////

#define BLAST_SPEED		40.0

#define CRASH_SPEED_MULT	0.5

// Translation factors from cursor motion to turning/tilting
#define HORIZ_SPEED_MULT	0.001
#define VERT_SPEED_MULT 	0.000025

#define MAX_SPEED		2.0

#define WARP_SPEED		MAX_SPEED

// Acceleration/deceleration amounts
#define SPEED_INCREMENT		0.01
#define SPEED_DECREMENT		0.02


////////////////////////////////////////////////////////////////////////////
//
// Tunnel parameters.
//
////////////////////////////////////////////////////////////////////////////

#define TUNNEL_NUM_SIDES	20
#define TUNNEL_RADIUS		1.0
#define TUNNEL_TOTAL_LENGTH	1000.0
#define MIN_SECTION_LENGTH	30.0
#define MAX_SECTION_LENGTH	100.0
#define MAX_SUB_SECTION_LENGTH	10.0

// No tunnel joint angle can exceed this number of degrees
#define MAX_JOINT_ANGLE		45.0

// No mine or ring can appear this close to a joint
#define TUNNEL_SAFE_LENGTH	4.0

// Maximum joint angle increases this many degrees per level
#define LEVEL_ANGLE_INCREASE	1.0


////////////////////////////////////////////////////////////////////////////
//
// Mine parameters.
//
////////////////////////////////////////////////////////////////////////////

// Ship has to come this close to activate mine
#define MINE_ACTIVATE_DISTANCE	30.0

// Radius around mine that blast has to intersect for deactivation
#define MINE_DEACTIVATE_SCALE	1.3

#define MAX_MINES_PER_SECTION(level)	((level) >= 9 ? 3.0 : (level) / 3.0)
#define MIN_MINE_SIZE(level)		((level) >= 8 ? 0.1 :		      \
					 (.2 - ((level) - 1) / 70))
#define MAX_MINE_SIZE			0.2
#define MIN_DELAY_TIME(level)		(2.0 + 5.0 / (level))
#define MAX_DELAY_TIME			10.0

// Explosion force (0 - 1) based on distance from mine
#define MINE_EXPLOSION_FORCE(distance)	(1.0 / (1.0 + 0.05 * (distance)))

// Maximum amount of rotation an exploding mine can cause (degrees)
#define MAX_MINE_EXPLOSION_ROTATION	3.0


////////////////////////////////////////////////////////////////////////////
//
// Ring parameters.
//
////////////////////////////////////////////////////////////////////////////

// Odds (0-1) that an ring appears in a given tunnel section
#define RING_ODDS(level)		((level) >= 5 ? 0.4 :		      \
					 ((level) - 1) / 10.0)

#define MIN_RING_SIZE(level)		((level) >= 5 ? .2 :		      \
					 .2 + (5 - (level)) / 5.0)
#define MAX_RING_SIZE(level)		.8

#define MIN_RING_ENERGY			0.2
#define MAX_RING_ENERGY			0.9

#define MIN_RING_STEALTH_TIME		10.0
#define MAX_RING_STEALTH_TIME		30.0

#define MIN_RING_WARP_DISTANCE		100.0
#define MAX_RING_WARP_DISTANCE		400.0

#define RING_BUTTON_SIZE		.2

////////////////////////////////////////////////////////////////////////////
//
// Blaster parameters.
//
////////////////////////////////////////////////////////////////////////////

// Maximum distance a blast can go
#define MAX_BLAST_DISTANCE	100.0

// Maximum number of blasts that can be active simultaneously
#define MAX_BLASTS		10

// Minimum time between blasts, in seconds
#define MIN_BLAST_INTERVAL	0.25


////////////////////////////////////////////////////////////////////////////
//
// Various other constants.
//
////////////////////////////////////////////////////////////////////////////

// Minimum time between sensor firings
#define SENSOR_INTERVAL		(1.0 / 30.0)

// Length of time effects displays are shown
#define CRASH_DISPLAY_TIME	0.5
#define RING_DISPLAY_TIME	0.5
#define EXPLODE_DISPLAY_TIME	0.5
