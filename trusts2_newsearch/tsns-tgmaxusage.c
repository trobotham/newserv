#include "trusts_newsearch.h"

#include <stdio.h>
#include <stdlib.h>

void *tsns_tgmaxusage_exe(searchCtx *ctx, struct searchNode *thenode, void *theinput);
void tsns_tgmaxusage_free(searchCtx *ctx, struct searchNode *thenode);

struct searchNode *tsns_tgmaxusage_parse(searchCtx *ctx, int argc, char **argv) {
  struct searchNode *thenode;

  if (!(thenode=(struct searchNode *)malloc(sizeof (struct searchNode)))) {
    parseError = "malloc: could not allocate memory for this search.";
    return NULL;
  }

  thenode->returntype = RETURNTYPE_INT;
  thenode->localdata = NULL;
  thenode->exe = tsns_tgmaxusage_exe;
  thenode->free = tsns_tgmaxusage_free;

  return thenode;
}

void *tsns_tgmaxusage_exe(searchCtx *ctx, struct searchNode *thenode, void *theinput) {
  trustgroup_t *tg;
  patricia_node_t *node = (patricia_node_t *)theinput;

  if (ctx->searchcmd == reg_nodesearch) {
    if (node->exts[tgh_ext] != NULL) 
      return (void *)(((trusthost_t *)node->exts[tgh_ext])->trustgroup->maxusage);
    else
      return (void *)0; /* will cast to a FALSE */
  } else if (ctx->searchcmd == reg_tgsearch) {
    tg = (trustgroup_t *)theinput;
    return (void *)(tg->maxusage);
  } else {
    return NULL;
  }
}

void tsns_tgmaxusage_free(searchCtx *ctx, struct searchNode *thenode) {
  free(thenode);
}