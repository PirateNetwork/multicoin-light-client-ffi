# 0.3.1
- [#88] unmined transaction shows note value spent instead of tx value

Fixes an issue where a sent transaction would show the whole note spent value
instead of the value of that the user meant to transfer until it was mined.

# 0.3.0
- [#87] Outbound transactions show the wrong amount on v_transactions

removes `v_tx_received` and `v_tx_sent`. 

`v_transactions` now shows the `account_balance_delta` column where the clients can 
query the effect of a given transaction in the account balance. If fee was paid from
the account that's being queried, the delta will include it. Transactions where funds
are received into the queried account, will show the amount that the acount is receiving
and won't include the transaction fee since it does not change the balance of the account.

Creates `v_tx_outputs` that allows clients to know the outputs involved in a transaction.


# 0.2.0
- [#34] Fix SwiftPackageManager deprecation Warning
We had to change the name of the package to make it match the name 
of the github repository due to Swift Package Manager conventions.

please see README.md for more information on how to import this package
going forward.

# FsBlock Db implementation and removal of BlockBb cache.

Implement `piratelc_init_block_metadata_db`, `piratelc_write_block_metadata`, `piratelc_free_block_meta, `piratelc_free_blocks_meta`

Declare `repr(C)` structs for FFI:
 - `FFIBlockMeta`: a block metadata row
 - `FFIBlocksMeta`: a structure that holds an array of `FFIBlockMeta`


expose shielding threshold for `shield_funds`

- [#81] Adopt latest crate versions
Bumped dependencies to `zcash_primitives 0.10`, `zcash_client_backend 0.7`,
`zcash_proofs 0.10`, `zcash_client_sqlite 0.5.0`

this adds support for `min_confirmations` on `shield_funds` and `shielding_threshold`.
- [#78] removing cocoapods support
# 0.1.1

Updating:
````
 - zcash_client_backend v0.6.0 -> v0.6.1
 - zcash_client_sqlite v0.4.0 -> v0.4.2
 - zcash_primitives v0.9.0 -> v0.9.1
````
This fixes the following issue
- [#72] fixes get_transparent_balance() fails when no UTXOs

# 0.1.0

Unified spending keys are now used in all places where spending authority
is required, both for performing spends of shielded funds and for shielding
transparent funds. Unified spending keys are represented as opaque arrays
of bytes, and FFI methods are provided to permit derivation of viewing keys
from the binary unified spending key representation.

IMPORTANT NOTE: the binary representation of a unified spending key may be
cached, but may become invalid and require re-derivation from seed to use as
input to any of the relevant APIs in the future, in the case that the
representation of the spending key changes or new types of spending authority
are recognized.  Spending keys give irrevocable spend authority over
a specific account.  Clients that choose to store the binary representation
of unified spending keys locally on device, should handle them with the 
same level of care and secure storage policies as the wallet seed itself. 

## Added
- `piratelc_create_account` provides new account creation functionality.
  This is now the preferred API for the creation of new spend authorities
  within the wallet; `piratelc_init_accounts_table_with_keys` remains available
  but should only be used if it is necessary to add multiple accounts at once,
  such as when restoring a wallet from seed where multiple accounts had been
  previously derived.

Key derivation API:
- `piratelc_derive_spending_key`
- `piratelc_spending_key_to_full_viewing_key`

Address retrieval, derivation, and verification API:
- `piratelc_get_current_address`
- `piratelc_get_next_available_address`
- `piratelc_get_sapling_receiver_for_unified_address`
- `piratelc_get_transparent_receiver_for_unified_address`
- `piratelc_is_valid_unified_address`
- `piratelc_is_valid_unified_full_viewing_key`
- `piratelc_list_transparent_receivers`
- `piratelc_get_typecodes_for_unified_address_receivers`
- `piratelc_free_typecodes`
- `piratelc_get_address_metadata`
Balance API:
- `piratelc_get_verified_transparent_balance_for_account`
- `piratelc_get_total_transparent_balance_for_account`

New memo access API:
- `piratelc_get_received_memo`
- `piratelc_get_sent_memo`

## Changed
- `piratelc_create_to_address` now has been changed as follows:
  - it no longer takes the string encoding of a Sapling extended spending key
    as spend authority; instead, it takes the binary encoded form of a unified
    spending key as returned by `piratelc_create_account` or 
    `piratelc_derive_spending_key`. See the note above.
  - it now takes the minimum number of confirmations used to filter notes to
    spend as an argument.
  - the memo argument is now passed as a potentially-null pointer to an
    `[u8; 512]` instead of a C string.
- `piratelc_shield_funds` has been changed as follows:
  - it no longer takes the transparent spending key for a single P2PKH address
    as spend authority; instead, it takes the binary encoded form of a unified
    spending key as returned by `piratelc_create_account`
    or `piratelc_derive_spending_key`. See the note above.
  - the memo argument is now passed as a potentially-null pointer to an
    `[u8; 512]` instead of a C string.
  - it no longer takes a destination address; instead, the internal shielding
    address is automatically derived from the account ID.
- Various changes have been made to correctly implement ZIP 316:
  - `FFIUnifiedViewingKey` now stores an account ID and the encoding of a
    ZIP 316 Unified Full Viewing Key.
  - `piratelc_init_accounts_table_with_keys` now takes a slice of ZIP 316 UFVKs.
- `piratelc_put_utxo` no longer has an `address_str` argument (the address is
  instead inferred from the script).
- `piratelc_get_verified_balance` now takes the minimum number of confirmations
  used to filter received notes as an argument.
- `piratelc_get_verified_transparent_balance` now takes the minimum number of
  confirmations used to filter received notes as an argument.
- `piratelc_get_total_transparent_balance` now returns a balance that includes
  all UTXOs including those only in the mempool (i.e. those with 0
  confirmations).

## Removed

The following spending key derivation APIs have been removed and replaced by
`piratelc_derive_spending_key`:
- `piratelc_derive_extended_spending_key`
- `piratelc_derive_transparent_private_key_from_seed`
- `piratelc_derive_transparent_account_private_key_from_seed`

The following viewing key APIs have been removed and replaced by
`piratelc_spending_key_to_full_viewing_key`:
- `piratelc_derive_extended_full_viewing_key`
- `piratelc_derive_shielded_address_from_viewing_key`
- `piratelc_derive_unified_viewing_keys_from_seed`

The following address derivation APIs have been removed in favor of
`piratelc_get_current_address` and `piratelc_get_next_available_address`:
- `piratelc_get_address`
- `piratelc_derive_shielded_address_from_seed`
- `piratelc_derive_transparent_address_from_secret_key`
- `piratelc_derive_transparent_address_from_seed`
- `piratelc_derive_transparent_address_from_public_key`

- `piratelc_init_accounts_table` has been removed in favor of
  `piratelc_create_account`

# 0.0.3
- [#13] Migrate to `zcash/librustzcash` revision with NU5 awareness (#20)
  This enables mobile wallets to send transactions after NU5 activation.
