#pragma once
#include <mimalloc.h>
#include <sat/binary/alignment.h>
#include <sat/memory/space.h>
#include <sat/memory/context.h>
#include <sat/memory/gc.h>
#include "./utils.h"

struct no_malloc_handler {
   static const char* name() {
      return "no-malloc";
   }
   __forceinline  static void* malloc(size_t) {
      __nop();
      return (void*)-1;
   }
   static void free(void*) {
      __nop();
   }
   static bool check(void* p) {
      return true;
   }
};

struct default_malloc_handler {
   static const char* name() {
      return "default-malloc";
   }
   static void* malloc(size_t s) {
      return ::malloc(s);
   }
   static void free(void* p) {
      return ::free(p);
   }
   static bool check(void* p) {
      return true;
   }
};

struct mi_malloc_handler {
   static const char* name() {
      return "mi-malloc";
   }
   static void* malloc(size_t s) {
      return mi_malloc(s);
   }
   static void free(void* p) {
      return mi_free(p);
   }
   static bool check(void* p) {
      return true;
   }
};

struct sat_malloc_handler {
   static sat::MemoryContext* context;
   static void init() {
      if (!context) {
         context = new sat::MemoryContext(new sat::MemorySpace());
      }
   }
   static const char* name() {
      return "sat-malloc";
   }
   static void* malloc(size_t s) {
      return context->allocateBlock(s);
   }
   static void free(void* p) {
      return context->disposeBlock(uintptr_t(p));
   }
   static bool check(void* p) {
      sat::BlockLocation block(context->space, uintptr_t(p));
      return block.descriptor || block.index;
   }
};