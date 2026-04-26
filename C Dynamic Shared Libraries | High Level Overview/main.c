#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <time.h>

#include <dlfcn.h>

int is_loaded(void* handle) {
  int _loaded = handle != NULL;
  if(handle != NULL) {
    // NOTE: RTLD_NOLOAD, TOO, INCREMENTS THE REF COUNT SO MAKE SURE IT IS      CLEANED UP
    dlclose(handle);
  }
  return _loaded;
}



void unload_all_dl_ref(const char* dl_name, void* dl_handle) {

  // https://man7.org/linux/man-pages/man3/dlerror.3.html
  // TODO: Error Handling dlerror
  while(is_loaded(dlopen(dl_name, RTLD_NOLOAD | RTLD_LAZY)) ) {
    dlclose(dl_handle);
  }

}

#define start_time clock_t s_t_a_r_t = clock();

#define end_time printf("[Cpu_time_used: %f]\n", ((double) (clock() - s_t_a_r_t)) / CLOCKS_PER_SEC);


// gcc -O0 -fPIC -rdynamic -shared -L./ -I./ -o lib_shared.so libshared.c && gcc -O0 main.c -o out.out && ./out.out

int main() {
  const char* lib_shared = "./lib_shared.so";

  void* shared_handle = dlopen(lib_shared, RTLD_LOCAL | RTLD_LAZY);
  printf("shared_handle: %p\n", shared_handle);


  void* shared_handle0 = dlopen(lib_shared, RTLD_LOCAL | RTLD_LAZY);
  assert(shared_handle == shared_handle0);

  void* a = dlsym(shared_handle, "a");
  void* b = dlsym(shared_handle, "b");
  void* c = dlsym(shared_handle, "c");

  printf("shared_handle: %p\n", shared_handle);



  void* _printf = dlsym(shared_handle, "printf");

  start_time;
  for(int i = 0; i < 1000000; i++)
  {

    // printf("printf() %p\n", _printf);

    ((int (*)(const char*, ...))_printf)("AAA %d\n", 1);
  }
  end_time;

  unload_all_dl_ref(lib_shared, shared_handle);



  return 0;
}
