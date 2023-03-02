#ifndef __CONFIG_H__
#define __CONFIG_H__

#define MAX_RUN_SECONDS  30 * 60 
#define NNodes 10000


/// parameters for HyRec
#define MAX_EPOCH 10
#define MIN_CHANGES 1000

/// LOG
#define LOGGAP 10000

/// Early Stop
#define UpdThresh 0.01
#define STOPTIMES 2

#endif