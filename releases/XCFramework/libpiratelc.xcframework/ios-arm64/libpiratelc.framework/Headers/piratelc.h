#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>

/**
 * A struct that contains an account identifier along with a pointer to the binary encoding
 * of an associated key.
 *
 * # Safety
 *
 * - `encoding` must be non-null and must point to an array of `encoding_len` bytes.
 */
typedef struct FFIBinaryKey {
  uint32_t account_id;
  uint8_t *encoding;
  uintptr_t encoding_len;
} FFIBinaryKey;

/**
 * A struct that contains an account identifier along with a pointer to the string encoding
 * of an associated key.
 *
 * # Safety
 *
 * - `encoding` must be non-null and must point to a null-terminated UTF-8 string.
 */
typedef struct FFIEncodedKey {
  uint32_t account_id;
  char *encoding;
} FFIEncodedKey;

/**
 * A struct that contains a pointer to, and length information for, a heap-allocated
 * slice of [`FFIEncodedKey`] values.
 *
 * # Safety
 *
 * - `ptr` must be non-null and must be valid for reads for `len * mem::size_of::<FFIEncodedKey>()`
 *   many bytes, and it must be properly aligned. This means in particular:
 *   - The entire memory range pointed to by `ptr` must be contained within a single allocated
 *     object. Slices can never span across multiple allocated objects.
 *   - `ptr` must be non-null and aligned even for zero-length slices.
 *   - `ptr` must point to `len` consecutive properly initialized values of type
 *     [`FFIEncodedKey`].
 * - The total size `len * mem::size_of::<FFIEncodedKey>()` of the slice pointed to
 *   by `ptr` must be no larger than isize::MAX. See the safety documentation of pointer::offset.
 * - See the safety documentation of [`FFIEncodedKey`]
 */
typedef struct FFIEncodedKeys {
  struct FFIEncodedKey *ptr;
  uintptr_t len;
} FFIEncodedKeys;

typedef struct FFIBlockMeta {
  uint32_t height;
  uint8_t *block_hash_ptr;
  uintptr_t block_hash_ptr_len;
  uint32_t block_time;
  uint32_t sapling_outputs_count;
  uint32_t orchard_actions_count;
} FFIBlockMeta;

typedef struct FFIBlocksMeta {
  struct FFIBlockMeta *ptr;
  uintptr_t len;
} FFIBlocksMeta;

/**
 * Initializes global Rust state, such as the logging infrastructure and threadpools.
 *
 * # Panics
 *
 * This method panics if called more than once.
 */
void piratelc_init_on_load(void);

/**
 * Returns the length of the last error message to be logged.
 */
int32_t piratelc_last_error_length(void);

/**
 * Copies the last error message into the provided allocated buffer.
 *
 * # Safety
 *
 * - `buf` must be non-null and valid for reads for `length` bytes, and it must have an alignment
 *   of `1`.
 * - The memory referenced by `buf` must not be mutated for the duration of the function call.
 * - The total size `length` must be no larger than `isize::MAX`. See the safety documentation of
 *   pointer::offset.
 */
int32_t piratelc_error_message_utf8(char *buf, int32_t length);

/**
 * Clears the record of the last error message.
 */
void piratelc_clear_last_error(void);

/**
 * Sets up the internal structure of the data database.  The value for `seed` may be provided as a
 * null pointer if the caller wishes to attempt migrations without providing the wallet's seed
 * value.
 *
 * Returns 0 if successful, 1 if the seed must be provided in order to execute the requested
 * migrations, or -1 otherwise.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `seed` must be non-null and valid for reads for `seed_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of pointer::offset.
 */
int32_t piratelc_init_data_database(const uint8_t *db_data,
                                    uintptr_t db_data_len,
                                    const uint8_t *seed,
                                    uintptr_t seed_len,
                                    uint32_t network_id);

/**
 * Frees a FFIBinaryKey value
 *
 * # Safety
 *
 * - `ptr` must be non-null and must point to a struct having the layout of [`FFIBinaryKey`].
 *   See the safety documentation of [`FFIBinaryKey`].
 */
void piratelc_free_binary_key(struct FFIBinaryKey *ptr);

/**
 * Adds the next available account-level spend authority, given the current set of [ZIP 316]
 * account identifiers known, to the wallet database.
 *
 * Returns the newly created [ZIP 316] account identifier, along with the binary encoding of the
 * [`UnifiedSpendingKey`] for the newly created account.  The caller should manage the memory of
 * (and store) the returned spending keys in a secure fashion.
 *
 * If `seed` was imported from a backup and this method is being used to restore a
 * previous wallet state, you should use this method to add all of the desired
 * accounts before scanning the chain from the seed's birthday height.
 *
 * By convention, wallets should only allow a new account to be generated after funds
 * have been received by the currently available account (in order to enable
 * automated account recovery).
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `seed` must be non-null and valid for reads for `seed_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of pointer::offset.
 * - Call [`piratelc_free_binary_key`] to free the memory associated with the returned pointer when
 *   you are finished using it.
 *
 * [ZIP 316]: https://zips.z.cash/zip-0316
 */
struct FFIBinaryKey *piratelc_create_account(const uint8_t *db_data,
                                             uintptr_t db_data_len,
                                             const uint8_t *seed,
                                             uintptr_t seed_len,
                                             uint32_t network_id);

/**
 * Frees an array of FFIEncodedKeys values as allocated by `piratelc_derive_unified_viewing_keys_from_seed`
 *
 * # Safety
 *
 * - `ptr` must be non-null and must point to a struct having the layout of [`FFIEncodedKeys`].
 *   See the safety documentation of [`FFIEncodedKeys`].
 */
void piratelc_free_keys(struct FFIEncodedKeys *ptr);

/**
 * Initialises the data database with the given set of unified full viewing keys. This
 * should only be used in special cases for implementing wallet recovery; prefer
 * `piratelc_create_account` for normal account creation purposes.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `ufvks` must be non-null and valid for reads for `ufvks_len * sizeof(FFIEncodedKey)` bytes.
 *   It must point to an array of `FFIEncodedKey` values.
 * - The memory referenced by `ufvks` must not be mutated for the duration of the function call.
 * - The total size `ufvks_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
bool piratelc_init_accounts_table_with_keys(const uint8_t *db_data,
                                            uintptr_t db_data_len,
                                            struct FFIEncodedKey *ufvks_ptr,
                                            uintptr_t ufvks_len,
                                            uint32_t network_id);

/**
 * Derives and returns a unified spending key from the given seed for the given account ID.
 *
 * Returns the binary encoding of the spending key. The caller should manage the memory of (and
 * store, if necessary) the returned spending key in a secure fashion.
 *
 * # Safety
 *
 * - `seed` must be non-null and valid for reads for `seed_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of pointer::offset.
 * - Call `piratelc_free_binary_key` to free the memory associated with the returned pointer when
 *   you are finished using it.
 */
struct FFIBinaryKey *piratelc_derive_spending_key(const uint8_t *seed,
                                                  uintptr_t seed_len,
                                                  int32_t account,
                                                  uint32_t network_id);

/**
 * Obtains the unified full viewing key for the given binary-encoded unified spending key
 * and returns the resulting encoded UFVK string. `usk_ptr` should point to an array of `usk_len`
 * bytes containing a unified spending key encoded as returned from the `piratelc_create_account`
 * or `piratelc_derive_spending_key` functions.
 *
 * # Safety
 *
 * - `usk_ptr` must be non-null and must point to an array of `usk_len` bytes.
 * - The memory referenced by `usk_ptr` must not be mutated for the duration of the function call.
 * - The total size `usk_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of pointer::offset.
 * - Call [`piratelc_string_free`] to free the memory associated with the returned pointer
 *   when you are done using it.
 */
char *piratelc_spending_key_to_full_viewing_key(const uint8_t *usk_ptr,
                                                uintptr_t usk_len,
                                                uint32_t network_id);

/**
 * Initialises the data database with the given block metadata.
 *
 * This enables a newly-created database to be immediately-usable, without needing to
 * synchronise historic blocks.
 *
 * The string represented by `sapling_tree_hex` should contain the encoded byte representation
 * of a Sapling commitment tree.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `hash_hex` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `hash_hex` must not be mutated for the duration of the function call.
 * - `sapling_tree_hex` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `sapling_tree_hex` must not be mutated for the duration of the
 *   function call.
 */
int32_t piratelc_init_blocks_table(const uint8_t *db_data,
                                   uintptr_t db_data_len,
                                   int32_t height,
                                   const char *hash_hex,
                                   uint32_t time,
                                   const char *sapling_tree_hex,
                                   uint32_t network_id);

/**
 * Returns the most-recently-generated unified payment address for the specified account.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`piratelc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *piratelc_get_current_address(const uint8_t *db_data,
                                   uintptr_t db_data_len,
                                   int32_t account,
                                   uint32_t network_id);

/**
 * Returns a newly-generated unified payment address for the specified account, with the next
 * available diversifier.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`piratelc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *piratelc_get_next_available_address(const uint8_t *db_data,
                                          uintptr_t db_data_len,
                                          int32_t account,
                                          uint32_t network_id);

/**
 * Returns a list of the transparent receivers for the diversified unified addresses that have
 * been allocated for the provided account.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`piratelc_free_keys`] to free the memory associated with the returned pointer
 *   when done using it.
 */
struct FFIEncodedKeys *piratelc_list_transparent_receivers(const uint8_t *db_data,
                                                           uintptr_t db_data_len,
                                                           int32_t account_id,
                                                           uint32_t network_id);

/**
 * Extracts the typecodes of the receivers within the given Unified Address.
 *
 * Returns a pointer to a slice of typecodes. `len_ret` is set to the length of the
 * slice.
 *
 * See the following sections of ZIP 316 for details on how to interpret typecodes:
 * - [List of known typecodes](https://zips.z.cash/zip-0316#encoding-of-unified-addresses)
 * - [Adding new types](https://zips.z.cash/zip-0316#adding-new-types)
 * - [Metadata Items](https://zips.z.cash/zip-0316#metadata-items)
 *
 * # Safety
 *
 * - `ua` must be non-null and must point to a null-terminated UTF-8 string containing an
 *   encoded Unified Address.
 * - Call [`piratelc_free_typecodes`] to free the memory associated with the returned
 *   pointer when done using it.
 */
uint32_t *piratelc_get_typecodes_for_unified_address_receivers(const char *ua, uintptr_t *len_ret);

/**
 * Frees a list of typecodes previously obtained from the FFI.
 *
 * # Safety
 *
 * - `data` and `len` must have been obtained from
 *   [`piratelc_get_typecodes_for_unified_address_receivers`].
 */
void piratelc_free_typecodes(uint32_t *data, uintptr_t len);

/**
 * Returns the transparent receiver within the given Unified Address, if any.
 *
 * # Safety
 *
 * - `ua` must be non-null and must point to a null-terminated UTF-8 string.
 * - Call [`piratelc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *piratelc_get_transparent_receiver_for_unified_address(const char *ua);

/**
 * Returns the Sapling receiver within the given Unified Address, if any.
 *
 * # Safety
 *
 * - `ua` must be non-null and must point to a null-terminated UTF-8 string.
 * - Call [`piratelc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *piratelc_get_sapling_receiver_for_unified_address(const char *ua);

/**
 * Returns true when the provided address decodes to a valid Sapling payment address for the
 * specified network, false in any other case.
 *
 * # Safety
 *
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
bool piratelc_is_valid_shielded_address(const char *address, uint32_t network_id);

/**
 * Returns the network type and address kind for the given address string,
 * if the address is a valid Zcash address.
 *
 * Address kind codes are as follows:
 * * p2pkh: 0
 * * p2sh: 1
 * * sapling: 2
 * * unified: 3
 *
 * # Safety
 *
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
bool piratelc_get_address_metadata(const char *address,
                                   uint32_t *network_id_ret,
                                   uint32_t *addr_kind_ret);

/**
 * Returns true when the address is a valid transparent payment address for the specified network,
 * false in any other case.
 *
 * # Safety
 *
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
bool piratelc_is_valid_transparent_address(const char *address, uint32_t network_id);

/**
 * Returns true when the provided key decodes to a valid Sapling extended spending key for the
 * specified network, false in any other case.
 *
 * # Safety
 *
 * - `extsk` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `extsk` must not be mutated for the duration of the function call.
 */
bool piratelc_is_valid_sapling_extended_spending_key(const char *extsk, uint32_t network_id);

/**
 * Returns true when the provided key decodes to a valid Sapling extended full viewing key for the
 * specified network, false in any other case.
 *
 * # Safety
 *
 * - `key` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `key` must not be mutated for the duration of the function call.
 */
bool piratelc_is_valid_viewing_key(const char *key, uint32_t network_id);

/**
 * Returns true when the provided key decodes to a valid unified full viewing key for the
 * specified network, false in any other case.
 *
 * # Safety
 *
 * - `ufvk` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `ufvk` must not be mutated for the duration of the
 *   function call.
 */
bool piratelc_is_valid_unified_full_viewing_key(const char *ufvk, uint32_t network_id);

/**
 * Returns true when the provided key decodes to a valid unified address for the
 * specified network, false in any other case.
 *
 * # Safety
 *
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the
 *   function call.
 */
bool piratelc_is_valid_unified_address(const char *address, uint32_t network_id);

/**
 * Returns the balance for the specified account, including all unspent notes that we know about.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int64_t piratelc_get_balance(const uint8_t *db_data,
                             uintptr_t db_data_len,
                             int32_t account,
                             uint32_t network_id);

/**
 * Returns the verified balance for the account, which ignores notes that have been
 * received too recently and are not yet deemed spendable according to `min_confirmations`.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int64_t piratelc_get_verified_balance(const uint8_t *db_data,
                                      uintptr_t db_data_len,
                                      int32_t account,
                                      uint32_t network_id,
                                      uint32_t min_confirmations);

/**
 * Returns the verified transparent balance for `address`, which ignores utxos that have been
 * received too recently and are not yet deemed spendable according to `min_confirmations`.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
int64_t piratelc_get_verified_transparent_balance(const uint8_t *db_data,
                                                  uintptr_t db_data_len,
                                                  const char *address,
                                                  uint32_t network_id,
                                                  uint32_t min_confirmations);

/**
 * Returns the verified transparent balance for `account`, which ignores utxos that have been
 * received too recently and are not yet deemed spendable according to `min_confirmations`.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
int64_t piratelc_get_verified_transparent_balance_for_account(const uint8_t *db_data,
                                                              uintptr_t db_data_len,
                                                              uint32_t network_id,
                                                              int32_t account,
                                                              uint32_t min_confirmations);

/**
 * Returns the balance for `address`, including all UTXOs that we know about.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
int64_t piratelc_get_total_transparent_balance(const uint8_t *db_data,
                                               uintptr_t db_data_len,
                                               const char *address,
                                               uint32_t network_id);

/**
 * Returns the balance for `account`, including all UTXOs that we know about.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `address` must be non-null and must point to a null-terminated UTF-8 string.
 * - The memory referenced by `address` must not be mutated for the duration of the function call.
 */
int64_t piratelc_get_total_transparent_balance_for_account(const uint8_t *db_data,
                                                           uintptr_t db_data_len,
                                                           uint32_t network_id,
                                                           int32_t account);

/**
 * Returns the memo for a received note, if it is known and a valid UTF-8 string.
 *
 * The note is identified by its row index in the `received_notes` table within the data
 * database.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`piratelc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *piratelc_get_received_memo_as_utf8(const uint8_t *db_data,
                                         uintptr_t db_data_len,
                                         int64_t id_note,
                                         uint32_t network_id);

/**
 * Returns the memo for a received note by copying the corresponding bytes to the received
 * pointer in `memo_bytes_ret`.
 *
 * The note is identified by its row index in the `received_notes` table within the data
 * database.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `memo_bytes_ret` must be non-null and must point to an allocated 512-byte region of memory.
 */
bool piratelc_get_received_memo(const uint8_t *db_data,
                                uintptr_t db_data_len,
                                int64_t id_note,
                                uint8_t *memo_bytes_ret,
                                uint32_t network_id);

/**
 * Returns the memo for a sent note, if it is known and a valid UTF-8 string.
 *
 * The note is identified by its row index in the `sent_notes` table within the data
 * database.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Call [`piratelc_string_free`] to free the memory associated with the returned pointer
 *   when done using it.
 */
char *piratelc_get_sent_memo_as_utf8(const uint8_t *db_data,
                                     uintptr_t db_data_len,
                                     int64_t id_note,
                                     uint32_t network_id);

/**
 * Returns the memo for a sent note, by copying the corresponding bytes to the received
 * pointer in `memo_bytes_ret`.
 *
 * The note is identified by its row index in the `sent_notes` table within the data
 * database.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `memo_bytes_ret` must be non-null and must point to an allocated 512-byte region of memory.
 */
bool piratelc_get_sent_memo(const uint8_t *db_data,
                            uintptr_t db_data_len,
                            int64_t id_note,
                            uint8_t *memo_bytes_ret,
                            uint32_t network_id);

/**
 * Returns a ZIP-32 signature of the given seed bytes.
 *
 * # Safety
 * - `seed` must be non-null and valid for reads for `seed_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `seed` must not be mutated for the duration of the function call.
 * - The total size `seed_len` must be at least 32 no larger than `252`. See the safety documentation
 *   of pointer::offset.
 */
bool piratelc_seed_fingerprint(const uint8_t *seed,
                               uintptr_t seed_len,
                               uint8_t *signature_bytes_ret);

/**
 * Checks that the scanned blocks in the data database, when combined with the recent
 * `CompactBlock`s in the block cache, form a valid chain.
 *
 * This function is built on the core assumption that the information provided in the
 * block cache is more likely to be accurate than the previously-scanned information.
 * This follows from the design (and trust) assumption that the `lightwalletd` server
 * provides accurate block information as of the time it was requested.
 *
 * Returns:
 * - `-1` if the combined chain is valid.
 * - `upper_bound` if the combined chain is invalid.
 *   `upper_bound` is the height of the highest invalid block (on the assumption that the
 *   highest block in the block cache is correct).
 * - `0` if there was an error during validation unrelated to chain validity.
 *
 * This function does not mutate either of the databases.
 *
 * # Safety
 *
 * - `fs_block_db_root` must be non-null and valid for reads for `fs_block_db_root_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `fs_block_db_root` must not be mutated for the duration of the function call.
 * - The total size `fs_block_db_root_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int32_t piratelc_validate_combined_chain(const uint8_t *fs_block_db_root,
                                         uintptr_t fs_block_db_root_len,
                                         const uint8_t *db_data,
                                         uintptr_t db_data_len,
                                         uint32_t validate_limit,
                                         uint32_t network_id);

/**
 * Returns the most recent block height to which it is possible to reset the state
 * of the data database.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int32_t piratelc_get_nearest_rewind_height(const uint8_t *db_data,
                                           uintptr_t db_data_len,
                                           int32_t height,
                                           uint32_t network_id);

/**
 * Rewinds the data database to the given height.
 *
 * If the requested height is greater than or equal to the height of the last scanned
 * block, this function does nothing.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
bool piratelc_rewind_to_height(const uint8_t *db_data,
                               uintptr_t db_data_len,
                               int32_t height,
                               uint32_t network_id);

/**
 * Scans new blocks added to the cache for any transactions received by the tracked
 * accounts.
 *
 * This function pays attention only to cached blocks with heights greater than the
 * highest scanned block in `db_data`. Cached blocks with lower heights are not verified
 * against previously-scanned blocks. In particular, this function **assumes** that the
 * caller is handling rollbacks.
 *
 * For brand-new light client databases, this function starts scanning from the Sapling
 * activation height. This height can be fast-forwarded to a more recent block by calling
 * [`piratelc_init_blocks_table`] before this function.
 *
 * Scanned blocks are required to be height-sequential. If a block is missing from the
 * cache, an error will be signalled.
 *
 * # Safety
 *
 * - `fs_block_db_root` must be non-null and valid for reads for `fs_block_db_root_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `fs_block_db_root` must not be mutated for the duration of the function call.
 * - The total size `fs_block_db_root_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int32_t piratelc_scan_blocks(const uint8_t *fs_block_cache_root,
                             uintptr_t fs_block_cache_root_len,
                             const uint8_t *db_data,
                             uintptr_t db_data_len,
                             uint32_t scan_limit,
                             uint32_t network_id);

/**
 * Inserts a UTXO into the wallet database.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `txid_bytes` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `txid_bytes_len` must not be mutated for the duration of the function call.
 * - The total size `txid_bytes_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `script_bytes` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `script_bytes_len` must not be mutated for the duration of the function call.
 * - The total size `script_bytes_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
bool piratelc_put_utxo(const uint8_t *db_data,
                       uintptr_t db_data_len,
                       const uint8_t *txid_bytes,
                       uintptr_t txid_bytes_len,
                       int32_t index,
                       const uint8_t *script_bytes,
                       uintptr_t script_bytes_len,
                       int64_t value,
                       int32_t height,
                       uint32_t network_id);

/**
 * # Safety
 * Initializes the `FsBlockDb` sqlite database. Does nothing if already created
 *
 * Returns true when successful, false otherwise. When false is returned caller
 * should check for errors.
 * - `fs_block_db_root` must be non-null and valid for reads for `fs_block_db_root_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `fs_block_db_root` must not be mutated for the duration of the function call.
 * - The total size `fs_block_db_root_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
bool piratelc_init_block_metadata_db(const uint8_t *fs_block_db_root,
                                     uintptr_t fs_block_db_root_len);

/**
 * Writes the blocks provided in `blocks_meta` into the `BlockMeta` database
 *
 * Returns true if the `blocks_meta` could be stored into the `FsBlockDb`. False
 * otherwise.
 *
 * When false is returned caller should check for errors.
 *
 * # Safety
 *
 * - `fs_block_db_root` must be non-null and valid for reads for `fs_block_db_root_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `fs_block_db_root` must not be mutated for the duration of the function call.
 * - The total size `fs_block_db_root_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - Block metadata represented in `blocks_meta` must be non-null. Caller must guarantee that the
 * memory reference by this pointer is not freed up, dereferenced or invalidated while this function
 * is invoked.
 */
bool piratelc_write_block_metadata(const uint8_t *fs_block_db_root,
                                   uintptr_t fs_block_db_root_len,
                                   struct FFIBlocksMeta *blocks_meta);

/**
 * Rewinds the data database to the given height.
 *
 * If the requested height is greater than or equal to the height of the last scanned
 * block, this function does nothing.
 *
 * # Safety
 *
 * - `fs_block_db_root` must be non-null and valid for reads for `fs_block_db_root_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `fs_block_db_root` must not be mutated for the duration of the function call.
 * - The total size `fs_block_db_root_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
bool piratelc_rewind_fs_block_cache_to_height(const uint8_t *fs_block_db_root,
                                              uintptr_t fs_block_db_root_len,
                                              int32_t height);

/**
 * Get the latest cached block height in the filesystem block cache
 *
 * Returns a positive blockheight or -1 if empty or an error occurred.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `tx` must be non-null and valid for reads for `tx_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `tx` must not be mutated for the duration of the function call.
 * - The total size `tx_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int32_t piratelc_latest_cached_block_height(const uint8_t *fs_block_db_root,
                                            uintptr_t fs_block_db_root_len);

/**
 * Decrypts whatever parts of the specified transaction it can and stores them in db_data.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `tx` must be non-null and valid for reads for `tx_len` bytes, and it must have an
 *   alignment of `1`.
 * - The memory referenced by `tx` must not be mutated for the duration of the function call.
 * - The total size `tx_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int32_t piratelc_decrypt_and_store_transaction(const uint8_t *db_data,
                                               uintptr_t db_data_len,
                                               const uint8_t *tx,
                                               uintptr_t tx_len,
                                               uint32_t _mined_height,
                                               uint32_t network_id);

/**
 * Creates a transaction paying the specified address from the given account.
 *
 * Returns the row index of the newly-created transaction in the `transactions` table
 * within the data database. The caller can read the raw transaction bytes from the `raw`
 * column in order to broadcast the transaction to the network.
 *
 * Do not call this multiple times in parallel, or you will generate transactions that
 * double-spend the same notes.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `usk_ptr` must be non-null and must point to an array of `usk_len` bytes containing a unified
 *   spending key encoded as returned from the `piratelc_create_account` or
 *   `piratelc_derive_spending_key` functions.
 * - The memory referenced by `usk_ptr` must not be mutated for the duration of the function call.
 * - The total size `usk_len` must be no larger than `isize::MAX`. See the safety documentation
 *   of pointer::offset.
 * - `to` must be non-null and must point to a null-terminated UTF-8 string.
 * - `memo` must either be null (indicating an empty memo or a transparent recipient) or point to a
 *    512-byte array.
 * - `spend_params` must be non-null and valid for reads for `spend_params_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be the Sapling spend proving parameters.
 * - The memory referenced by `spend_params` must not be mutated for the duration of the function call.
 * - The total size `spend_params_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `output_params` must be non-null and valid for reads for `output_params_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be the Sapling output proving parameters.
 * - The memory referenced by `output_params` must not be mutated for the duration of the function call.
 * - The total size `output_params_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int64_t piratelc_create_to_address(const uint8_t *db_data,
                                   uintptr_t db_data_len,
                                   const uint8_t *usk_ptr,
                                   uintptr_t usk_len,
                                   const char *to,
                                   int64_t value,
                                   const uint8_t *memo,
                                   const uint8_t *spend_params,
                                   uintptr_t spend_params_len,
                                   const uint8_t *output_params,
                                   uintptr_t output_params_len,
                                   uint32_t network_id,
                                   uint32_t min_confirmations,
                                   bool use_zip317_fees);

int32_t piratelc_branch_id_for_height(int32_t height, uint32_t network_id);

/**
 * Frees strings returned by other piratelc functions.
 *
 * # Safety
 *
 * - `s` should be a non-null pointer returned as a string by another piratelc function.
 */
void piratelc_string_free(char *s);

/**
 * Shield transparent UTXOs by sending them to an address associated with the specified Sapling
 * spending key.
 *
 * # Safety
 *
 * - `db_data` must be non-null and valid for reads for `db_data_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be a string representing a valid system path in the
 *   operating system's preferred representation.
 * - The memory referenced by `db_data` must not be mutated for the duration of the function call.
 * - The total size `db_data_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `usk_ptr` must be non-null and must point to an array of `usk_len` bytes containing a unified
 *   spending key encoded as returned from the `piratelc_create_account` or
 *   `piratelc_derive_spending_key` functions.
 * - The memory referenced by `usk_ptr` must not be mutated for the duration of the function call.
 * - The total size `usk_len` must be no larger than `isize::MAX`. See the safety documentation
 * - `memo` must either be null (indicating an empty memo) or point to a 512-byte array.
 * - `shielding_threshold` a non-negative shielding threshold amount in zatoshi
 * - `spend_params` must be non-null and valid for reads for `spend_params_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be the Sapling spend proving parameters.
 * - The memory referenced by `spend_params` must not be mutated for the duration of the function call.
 * - The total size `spend_params_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 * - `output_params` must be non-null and valid for reads for `output_params_len` bytes, and it must have an
 *   alignment of `1`. Its contents must be the Sapling output proving parameters.
 * - The memory referenced by `output_params` must not be mutated for the duration of the function call.
 * - The total size `output_params_len` must be no larger than `isize::MAX`. See the safety
 *   documentation of pointer::offset.
 */
int64_t piratelc_shield_funds(const uint8_t *db_data,
                              uintptr_t db_data_len,
                              const uint8_t *usk_ptr,
                              uintptr_t usk_len,
                              const uint8_t *memo,
                              uint64_t shielding_threshold,
                              const uint8_t *spend_params,
                              uintptr_t spend_params_len,
                              const uint8_t *output_params,
                              uintptr_t output_params_len,
                              uint32_t network_id,
                              uint32_t min_confirmations,
                              bool use_zip317_fees);
