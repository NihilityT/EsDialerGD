//
// Created by ChileungL on 24/05/2018.
//

#pragma once
extern int dbgout_enable;

#define FAILED_STR  "FAILED"
#define dbgout(fmt, arg...) \
do {\
    if(dbgout_enable) printf("(%s) "fmt"\n",__FUNCTION__, ##arg); \
}while(0);
