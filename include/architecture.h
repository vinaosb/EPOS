// EPOS Architecture Mediator

#ifndef __architecture_h
#define __architecture_h

#include <system/config.h>

#ifdef __CPU_H
#include __CPU_H
#endif

#ifdef __MMU_H
#include __MMU_H
#endif

#ifdef __FPU_H
#include __FPU_H
#endif

#ifdef __PMU_H
#include __PMU_H
#endif

#ifdef __TSC_H
#include __TSC_H
#endif

#endif
