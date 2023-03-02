#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAX_RUN_SECONDS  30 * 60 
extern int NNodes;
// #define NNodes 1000000


/// parameters for HyRec
#define MAX_EPOCH 10
#define MIN_CHANGES 150000

/// LOG
#define LOGGAP 1000000

/// Early Stop
#define UpdThresh 0.00001
#define STOPTIMES 4

/// OnLine Test
#define ONLINE_JUDGE

#endif
