//----- World -----//

//#define WORLD_SIZE_LOG_X    16
//#define WORLD_SIZE_LOG_Y    10
//#define WORLD_SIZE_LOG_XY   26
//
//#define WORLD_SIZE_X    0x00010000u
//#define WORLD_SIZE_Y    0x00000400u
//#define WORLD_SIZE_XY   0x04000000u
//
//#define WORLD_SIZE_X_MONE    0x0000ffffu
//#define WORLD_SIZE_Y_MONE    0x000003ffu
//#define WORLD_SIZE_XY_MONE   0x03ffffffu
//
//#define WORLD_SIZE_X_MTWO    0x0000fffeu
//#define WORLD_SIZE_Y_MTWO    0x000003feu

#define WORLD_SIZE_LOG_X    7
#define WORLD_SIZE_LOG_Y    6
#define WORLD_SIZE_LOG_XY   13

#define WORLD_SIZE_X    0x00000080u
#define WORLD_SIZE_Y    0x00000040u
#define WORLD_SIZE_XY   0x00002c00u

#define WORLD_SIZE_X_MONE    0x0000007fu
#define WORLD_SIZE_Y_MONE    0x0000003fu
#define WORLD_SIZE_XY_MONE   0x000005ffu

#define WORLD_SIZE_X_MTWO    0x0000007eu
#define WORLD_SIZE_Y_MTWO    0x0000003eu

//----- Block -----//
#define BLOCK_PIXEL_SIZE_X    10
#define BLOCK_PIXEL_SIZE_Y    10

#define WORLD_PIXEL_SIZE_X  (WORLD_SIZE_X * BLOCK_PIXEL_SIZE_X)
#define WORLD_PIXEL_SIZE_Y  (WORLD_SIZE_Y * BLOCK_PIXEL_SIZE_Y)
