#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "./list.h"
#include "./new_utils.h"
#include "./tokenizer.h"

#define MAX_COMMAND_BUFFER 100
#define MAX_NAME_LEN 20

struct list_item
{
  struct list_elem list_sequence;
  int item;
};

struct list_named
{
  struct list inner_list;
  struct list_named *next;
  char name[MAX_NAME_LEN];
};

typedef struct _Environment
{
  struct list_named* all_list;
  char ** argv;
  int argc;
  int command_num;
} Environment;

int
get_command_num (const char* command_name);
int
command_handle (Environment* env);
struct list_named*
find_list_named(struct list_named* all_list
                , const char* name);

int
main ()
{
  Environment env;
  env.all_list = calloc (1, sizeof(struct list_named));
  env.argv = NULL;
  env.argc = 0;
  env.command_num = 0;

  while (true)
    {
      char command_line_buffer[MAX_COMMAND_BUFFER];
      int argc = 0;

      fgets(command_line_buffer, MAX_COMMAND_BUFFER, stdin);
      tokenizer(command_line_buffer, &env.argc, &env.argv); // argv[0] : command

      int res = 0;
      if (env.argv[0] != NULL)
        {
          env.command_num = get_command_num(env.argv[0]);

          res = command_handle(&env);
        }
      else
        {
          fprintf(stderr, "Enter argv[0]\n");
        }
      tokenizer_deallocate_argvs(&env.argv, argc);

      if (res == -1 || res == 0) { return res; } // if res == -1 : error occurs.
    }
}

struct list_named*
find_list_named(struct list_named* all_list
                , const char* name)
{
  struct list_named* temp_list_named = all_list;
  while (temp_list_named)
    {
      if (!strcmp(temp_list_named->name, name)) { break; }
      temp_list_named = temp_list_named->next;
    }

  return temp_list_named;
}


int
get_command_num(const char* command_name)
{
  char * collect_command[ ] = 
    {
      // Create, Dump : start : 0~
      "create", "dumpdata"
       // List . count : 18, start : 2~
       , "list_insert", "list_splice", "list_push_back"
       , "list_remove", "list_pop_front", "list_pop_back"
       , "list_front", "list_back"
       , "list_size", "list_empty"
       , "list_reverse", "list_sort"
       , "list_insert_ordered", "list_unique"
       , "list_max", "list_min", "list_swap"
       , "list_shuffle"
       // Hashtable . count : 9, start : 20
       , "hash_insert", "hash_replace", "hash_find"
       , "hash_delete", "hash_clear", "hash_size"
       , "hash_empty", "hash_apply", "hash_int_2"
       // Bitmap . count : 17 , start : 29
       , "bitmap_size", "bitmap_set", "bitmap_mark"
       , "bitmap_reset", "bitmap_flip", "bitmap_test"
       , "bitmap_set_all", "bitmap_set_multiple"
       , "bitmap_count", "bitmap_contains"
       , "bitmap_any", "bitmap_none", "bitmap_all"
       , "bitmap_scan", "bitmap_scan_and_flip"
       , "bitmap_dump", "bitmap_expand"
       // Quit , start : 46
       , "quit"
       , "list_push_front" // TODO : list 라인에 같이 넣어서 case 처리 수정. 혹은 다른 방법으로 해결.
       , "delete"
    };

  int i = 0, cnt_command = 2 + 18 + 9 + 17 + 3;

  for (i = 0; i < cnt_command; i++)
    {
      if (!strcmp(collect_command[i], command_name)) return i;
    }

  return -1; // command not found
}

int
command_handle (Environment *env)
{
  int ret = 1;  // result : 1 - normal, 0 - exit, -1 - error
  // TODO : argc에 따른 에러처리 해주면 매우 안정적인 프로그램이 될 것이다.! 겨울 방학 때의 나에게 맡긴다 후후.
  switch (env->command_num)
    {
    case 0 : // Create
        {
          if (env->argc != 3)
            {
              fprintf(stderr, "create <list|hashtable|bitmap> <list_name>\n");
              return -1;
            }
          if (!strcmp(env->argv[1], "list"))
            {
              // TODO : list에 넣기 전에 name이 겹치는지 체크하면 더 좋을 듯!
              // create new list
              struct list_named* new_list_named = calloc(1, sizeof(struct list_named));
              strcpy(new_list_named->name, env->argv[2]);
              list_init(&new_list_named->inner_list);

              struct list_named* temp_list = env->all_list;
              if (temp_list->next == NULL)
                {
                  temp_list->next = new_list_named;
                }
              else
                {
                  while (temp_list->next)
                    {
                      temp_list = temp_list->next;
                    }
                  temp_list->next = new_list_named;
                }
            }
          else if (!strcmp(env->argv[1], "hashtable"))
            {

            }
          else if (!strcmp(env->argv[1], "bitmap"))
            {

            }
          else // error
            {
              fprintf(stderr, "create <list|hashtable|bitmap> <list_name>\n");
              return -1;
            }
        }
      break;
    case 1 : // dumpdata
        {
          if (env->argc < 2)
            {
              fprintf(stderr, "dumpdata <list_name|hashtable_name|bitmap_name>\n");
              return -1;
            }
          // List
          struct list_named* temp_list_named = 
           find_list_named(env->all_list, env->argv[1]);

          if (temp_list_named)
            {
              struct list_elem* temp_list_elem = 
               list_begin(&temp_list_named->inner_list);
              for (temp_list_elem = list_begin(&temp_list_named->inner_list);
                   temp_list_elem != list_end(&temp_list_named->inner_list);
                   temp_list_elem = list_next(temp_list_elem))
                {
                  printf("%d ", list_entry(temp_list_elem
                                           , struct list_item
                                           , list_sequence)->item
                        );
                }
              printf("\n");
            }
        }
      break;
    case 48: // delete
        {
          // TODO : Delete 할 때 지우기 전 있는지 체크!
          struct list_named* temp_list_named = env->all_list;
          while (temp_list_named->next)
            {
              if (!strcmp(temp_list_named->next->name, env->argv[1])) { break; }
              temp_list_named = temp_list_named->next;
            }

          if (temp_list_named->next)
            {
              struct list *temp_list =
               &temp_list_named->next->inner_list;
              struct list_elem* temp_remove = NULL;

              while (!list_empty(temp_list))
                {
                  temp_remove = list_pop_front(temp_list);
                  struct list_item* temp_remove_item  = 
                   list_entry(temp_remove, struct list_item, list_sequence);
                  free(temp_remove_item);

                }
              struct list_named* temp_remove_named = 
               temp_list_named->next;
              temp_list_named->next = temp_list_named->next->next;

              free(temp_remove_named);
            }
        }
      break;
      // List
    case 2: // list_insert
        {
          struct list_named* temp_list_named =
           find_list_named(env->all_list, env->argv[1]);
          struct list_item* temp_item =
           calloc(1, sizeof(struct list_item));
          temp_item->item = atoi(env->argv[3]);

          int i = 0,
              n = atoi(env->argv[2]);
          struct list inner_list = temp_list_named->inner_list;
          struct list_elem* before;
          for (i = 0, before = list_begin(&inner_list)
               ; i < n && before != list_end (&inner_list)
               ; ++i, before = list_next(before));
          list_insert(before, &temp_item->list_sequence);
        }
      break;
    case 3: // list_splice
      break;
    case 47: // list_push_front
        {
          struct list_named* temp_list_named = 
           find_list_named(env->all_list, env->argv[1]);
          struct list_item* temp_item = 
           calloc(1, sizeof(struct list_item));
          temp_item->item = atoi(env->argv[2]);
          list_push_front(&temp_list_named->inner_list
                          , &temp_item->list_sequence
                         );
        }
      break;
    case 4: // list_push_back
        {
          struct list_named* temp_list_named = 
           find_list_named(env->all_list, env->argv[1]);
          struct list_item* temp_item = 
           calloc(1, sizeof(struct list_item));
          temp_item->item = atoi(env->argv[2]);
          list_push_back(&temp_list_named->inner_list
                         , &temp_item->list_sequence
                        );
        }
      break;
    case 5: // list_remove
        {
          struct list_named* temp_list_named =
           find_list_named(env->all_list, env->argv[1]);

          int i = 0, n = atoi(env->argv[2]);
          struct list inner_list = temp_list_named->inner_list;
          struct list_elem* before;
          for (before = list_begin(&inner_list)
               ; i < n && before != list_end (&inner_list)
               ; i++, before = list_next(before));
          list_remove(before);
        }
      break;
    case 6: // list_pop_front
        {
          struct list_named* temp_list_named = 
           find_list_named(env->all_list, env->argv[1]);
          struct list_elem* temp_remove = 
           list_pop_front(&temp_list_named->inner_list);
          struct list_item* temp_remove_item  = 
           list_entry(temp_remove, struct list_item, list_sequence);
          free(temp_remove_item);
        }
      break;
    case 7: // list_pop_back
        {
          struct list_named* temp_list_named = 
           find_list_named(env->all_list, env->argv[1]);
          struct list_elem* temp_remove = 
           list_pop_back(&temp_list_named->inner_list);
          struct list_item* temp_remove_item = 
           list_entry(temp_remove, struct list_item, list_sequence);
          free(temp_remove_item);
        }
      break;
    case 8: // list_front
        {
          struct list_named* temp_list_named = 
           find_list_named(env->all_list, env->argv[1]);
          struct list_elem* temp_item = 
           list_front(&temp_list_named->inner_list);
          printf("%d\n", list_entry(temp_item
                                    , struct list_item
                                    , list_sequence)->item
                );
        }

      break;
    case 9: // list_back
        {
          struct list_named* temp_list_named = 
           find_list_named(env->all_list, env->argv[1]);
          struct list_elem* temp_item = 
           list_back(&temp_list_named->inner_list);
          printf("%d\n", list_entry(temp_item
                                    , struct list_item
                                    , list_sequence)->item
                );
        }
      break;
    case 10: // list_size
        {
          struct list_named* temp_named =
           find_list_named(env->all_list, env->argv[1]);

          printf("%d\n", list_size(&temp_named->inner_list));
        }

      break;
    case 11: // list_empty
        {
          struct list_named* temp_list_named =
           find_list_named(env->all_list, env->argv[1]);
          if (list_empty(&temp_list_named->inner_list))
            {
              printf("true\n");
            }
          else
            {
              printf("false\n");
            }
        }
      break;
    case 12: // list_reverse
      break;
    case 13: // list_sort
      break;
    case 14: // list_insert_ordered
      break;
    case 15: // list unique
      break;
    case 16: // list_max
      break;
    case 17: // list_min
      break;
    case 18: // list_swap
      break;
    case 19: // list_shuffle
      break;




      // Hashtable
    case 20: // hash_insert
      break;
    case 21: // hash_replace
      break;
    case 22: // hash_find
      break;
    case 23: // hash_delete
      break;
    case 24: // hash_clear
      break;
    case 25: // hash_size
      break;
    case 26: // hash_empty
      break;
    case 27: // hash_apply
      break;
    case 28: // hash_int_2
      break;

      // Bitmap
    case 29: // bitmap_size
      break;
    case 30: // bitmap_set
      break;
    case 31: // bitmap_mark
      break;
    case 32: // bitmap_reset
      break;
    case 33: // bitmap_flip
      break;
    case 34: // bitmap_test
      break;
    case 35: // bitmap_set_all
      break;
    case 36: // bitmap_set_multiple
      break;
    case 37: // bitmap_count
      break;
    case 38: // bitmap_contains
      break;
    case 39: // bitmap_any
      break;
    case 40: // bitmap_none
      break;
    case 41: // bitmap_all
      break;
    case 42: // bitmap_scan
      break;
    case 43: // bitmap_scan_and_flip
      break;
    case 44: // bitmap_dump
      break;
    case 45: // bitmap_expand
      break;
    case 46: // Quit
      ret = 0;
      break;
    }


  return ret;
}
