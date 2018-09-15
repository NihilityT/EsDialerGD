//
// Created by ChileungL on 24/05/2018.
//

#pragma once

#define FAILED_STR  "FAILED"
#define dbgout(fmt, arg...) \
do {\
    printf("[%s] "fmt"\n",__FUNCTION__, ##arg); \
}while(0);
