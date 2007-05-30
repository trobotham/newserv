/* modules.h */

#ifndef __MODULES_H
#define __MODULES_H

#include "../lib/sstring.h"

#define MODULENAMELEN 40
#define MODULEDESCLEN 200

typedef struct {
  sstring *name;
  void    *handle;
  const char *version;
} module;

void initmodules();
int insmod(char *modulename);
int getindex(char *modulename);
int isloaded(char *modulename);
int rmmod(char *modulename);
char *lsmod(int index);
const char *lsmodver(int index);
void preparereload(char *modulename);
void reloadmarked(void);

#endif
