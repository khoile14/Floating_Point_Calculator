

#ifndef HASHMAP_H
#define HASHMAP_H

// Default Values
#define HASHMAP_DEFAULT_INITIAL_SIZE 11
#define MAX_KEY_SIZE 255    // Max characters per Key

// Error Codes
#define HASHMAP_NUM_ERROR_CODES 11

enum error_codes {
  Hashmap_General_Error = -1,
  Hashmap_Success = 0,
  Hashmap_Uninitialized,
  Hashmap_Unknown_Error,
  Hashmap_Invalid_Key,
  Hashmap_Invalid_Capacity,
  Hashmap_Invalid_Index,
  Hashmap_No_Value,
  Hashmap_Insufficient_Memory,
  Hashmap_FreeValue_Missing,
  Hashmap_Exists,
};

static const char *error_str[HASHMAP_NUM_ERROR_CODES] = {
  "General Error",
  "Success",
  "The HashMap needs to be Initialized First",
  "Unknown Error",
  "Invalid Key",
  "Invalid Capacity",
  "Invalid Index",
  "No Value Provided",
  "Insufficient Memory",
  "Need a Function to Free the Value, Got NULL",
  "Hashmap Already Exists",
};

// Shared Prototypes
int hashmap_create(int initial_capacity, void (*free_value)(void *value));
void hashmap_print_keys();
int hashmap_isEmpty(); // 1 - True, 0 - False, ERRCODE - Failure
int hashmap_containsKey(const char *key); // 1 - True, 0 - False, ERRCODE - Failure
void *hashmap_get(const char *key);
int hashmap_put(const char *key, void *value);
void hashmap_destroy();
int hashmap_clear();
int hashmap_size();
int hashmap_capacity();
int hashmap_remove(const char *key);
int hashmap_remove_free(const char *key);
void hashmap_print_error(int error);

#endif
