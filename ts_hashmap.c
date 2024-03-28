#include <limits.h>
#include <pthread.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ts_hashmap.h"

/**
 * Creates a new thread-safe hashmap. 
 *
 * @param capacity initial capacity of the hashmap.
 * @return a pointer to a new thread-safe hashmap.
 */
ts_hashmap_t *initmap(int capacity) {
  ts_hashmap_t *map =  malloc(sizeof(struct ts_hashmap_t));
  map->table =  malloc(sizeof(struct ts_entry_t*) * capacity);//mempry allocation
  map->size = 0;
  map->capacity = capacity;
  map->numOps = 0;
  for(int i = 0; i < capacity; i++){//initialize table
    map->table[i] = NULL;
  }
  pthread_mutex_init(&map->lock, NULL);//initialize lock
  return map;
}

/**
 * Obtains the value associated with the given key.
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int get(ts_hashmap_t *map, int key) {
  // TODO
  pthread_mutex_lock(&map->lock);
  int hash = ((unsigned int)key % map->capacity);
  ts_entry_t* curr = map->table[hash];//head of bucket
 
    while(curr != NULL){//step through bucket
      if(curr->key == key){
        map->numOps++;
        int val = curr->value;//grab val and return
        pthread_mutex_unlock(&map->lock);
        return val;
      }
      curr = curr->next;
    }
    map->numOps++;
  pthread_mutex_unlock(&map->lock);//no val
  return INT_MAX;
}

/**
 * Associates a value associated with a given key.
 * @param map a pointer to the map
 * @param key a key
 * @param value a value
 * @return old associated value, or INT_MAX if the key was new
 */
int put(ts_hashmap_t *map, int key, int value) {
  // TODO
  pthread_mutex_lock(&map->lock);
  int hash = ((unsigned int)key % map->capacity);
  ts_entry_t* newEntry =  malloc(sizeof(struct ts_entry_t));//initialize new item
  newEntry->key = key;
  newEntry->value = value;
  newEntry->next = NULL;
  ts_entry_t* curr = map->table[hash];//bucket head

  if(curr == NULL){//no items in bucket 
    map->table[hash] = newEntry;
    map->numOps++;
    map->size++;
    pthread_mutex_unlock(&map->lock);
    return INT_MAX;
  }
  else{
    if(curr->key == key){//key already exists in head
      free(newEntry);
      map->numOps++;
      map->size++;
      int val = curr->value;
      pthread_mutex_unlock(&map->lock);
      return val;
    }
  while(curr != NULL && curr->next != NULL){//step through bucket
    if(curr-> key == key){//key exists
      free(newEntry);
      map->numOps++;
      map->size++;
      int val = curr->value;
      pthread_mutex_unlock(&map->lock);
      return val;
    }
    curr = curr->next;
  }
  curr->next = newEntry;//stepped to end and place new entry
  }
  map->numOps++;
  map->size++;
  pthread_mutex_unlock(&map->lock);
  return INT_MAX;
}

/**
 * Removes an entry in the map
 * @param map a pointer to the map
 * @param key a key to search
 * @return the value associated with the given key, or INT_MAX if key not found
 */
int del(ts_hashmap_t *map, int key) {
  // TODO
  pthread_mutex_lock(&map->lock);
  int hash = ((unsigned int)key % map->capacity);
  ts_entry_t* curr = map->table[hash];//bucket head
  ts_entry_t* prev = NULL;//previous
  
  while(curr != NULL){//step through
    if(key == curr->key){
      int val = curr->value;
      if(prev == NULL){//if head
        map->table[hash] = curr->next;
      }
      else{//if middle
        prev->next = curr->next;
      }
      
      free(curr);
      map->numOps++;
      map->size--;
      pthread_mutex_unlock(&map->lock);
      return val;
    }
    prev = curr;
    curr = curr->next;
  }
  map->numOps++;
  pthread_mutex_unlock(&map->lock);
  return INT_MAX;
}


/**
 * Prints the contents of the map (given)
 */
void printmap(ts_hashmap_t *map) {
  for (int i = 0; i < map->capacity; i++) {
    printf("[%d] -> ", i);
    ts_entry_t *entry = map->table[i];
    while (entry != NULL) {
      printf("(%d,%d)", entry->key, entry->value);
      if (entry->next != NULL)
        printf(" -> ");
      entry = entry->next;
    }
    printf("\n");
  }
}

/**
 * Free up the space allocated for hashmap
 * @param map a pointer to the map
 */
void freeMap(ts_hashmap_t *map) {
  // TODO: iterate through each list, free up all nodes
  // TODO: free the hash table
  // TODO: destroy locks
  for(int i = 0; i < map->capacity; i++){
    ts_entry_t* curr = map->table[i];
    while(curr != NULL){
      ts_entry_t* next = curr->next;//give all memory back
      free(curr);
      curr = next;
    }
  }
  free(map->table);
  pthread_mutex_destroy(&map->lock);
  free(map);
}