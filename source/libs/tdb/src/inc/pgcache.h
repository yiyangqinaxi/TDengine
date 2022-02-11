/*
 * Copyright (c) 2019 TAOS Data, Inc. <jhtao@taosdata.com>
 *
 * This program is free software: you can use, redistribute, and/or modify
 * it under the terms of the GNU Affero General Public License, version 3
 * or later ("AGPL"), as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.
 *
 * You should have received a copy of the GNU Affero General Public License
 * along with this program. If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef _TD_PAGE_CACHE_H_
#define _TD_PAGE_CACHE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct SPgCache SPgCache;
typedef struct SPage    SPage;

// SPgCache
int pgCacheCreate(SPgCache **ppPgCache, pgsz_t pgSize, int32_t npage);
int pgCacheDestroy(SPgCache *pPgCache);
int pgCacheOpen(SPgCache **ppPgCache);
int pgCacheClose(SPgCache *pPgCache);

SPage *pgCacheFetch(SPgCache *pPgCache, pgid_t pgid);
int    pgCacheRelease(SPage *pPage);

// SPage

typedef TD_DLIST_NODE(SPage) SPgListNode;
struct SPage {
  pgid_t      pgid;      // page id
  frame_id_t  frameid;   // frame id
  SPgListNode freeNode;  // for SPgCache.freeList
  SPgListNode pghtNode;  // for pght
  SPgListNode lruNode;   // for LRU
  uint8_t *   pData;     // real data
};

typedef TD_DLIST(SPage) SPgList;
struct SPgCache {
  TENV *   pEnv;  // TENV containing this page cache
  SRWLatch mutex;
  pgsz_t   pgsize;
  int32_t  npage;
  SPage *  pages;
  SPgList  freeList;
  SPgList  lru;
  struct {
    int32_t  nbucket;
    SPgList *buckets;
  } pght;  // page hash table
};

#ifdef __cplusplus
}
#endif

#endif /*_TD_PAGE_CACHE_H_*/