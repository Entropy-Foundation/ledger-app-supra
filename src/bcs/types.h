#pragma once

#include <stdint.h>
#include <stddef.h>

// Maximum length allowed for sequence (vectors, bytes, strings) and maps
#define MAX_SEQUENCE_LENGTH ((1ull << 31) - 1)
// Maximum number of nested structs and enum variants
#define MAX_CONTAINER_DEPTH 500
// Address size
#define ADDRESS_LEN 32
// default coin module
#define SUPRA_COIN "0x1::supra_coin::SupraCoin"
// prefix for RawTransaction
#define RAW_TRANSACTION_SALT "SUPRA::RawTransaction"
// prefix for MultiAgentRawTransaction
#define RAW_TRANSACTION_WITH_DATA_SALT "SUPRA::RawTransactionWithData"
// size of hashed prefix
#define TX_HASHED_PREFIX_LEN 32
// size in bytes of data (max_gas_amount + gas_unit_price + expiration_timestamp_secs + chain_id)
// at the end of the transaction
#define TX_FOOTER_LEN 25
// sha3-256 hash of the RAW_TRANSACTION_SALT
static const uint8_t PREFIX_RAW_TX_HASHED[] = {
    136, 209, 110, 209, 171, 166, 215, 125,  82, 26, 175, 173, 166, 214, 233,
    45, 122,  23,  69,  39, 173, 180,  87, 173, 250, 102, 169, 197, 160, 241, 254,  55};
// sha3-256 hash of the RAW_TRANSACTION_WITH_DATA_SALT
static const uint8_t PREFIX_RAW_TX_WITH_DATA_HASHED[] = {
    153, 182, 239, 100, 227, 135, 159, 196, 232, 207, 11, 183, 62, 196, 92, 16,
    8,  32, 169,  77,  69, 192, 103, 160, 98, 44, 2, 52, 104, 207, 174, 239};

typedef struct {
    uint64_t high;
    uint64_t low;
} uint128_t;

typedef struct {
    int64_t high;
    uint64_t low;
} int128_t;

typedef struct {
    uint8_t *bytes;
    size_t len;
} fixed_bytes_t;

typedef enum {
    TYPE_TAG_BOOL = 0,
    TYPE_TAG_U8 = 1,
    TYPE_TAG_U64 = 2,
    TYPE_TAG_U128 = 3,
    TYPE_TAG_ADDRESS = 4,
    TYPE_TAG_SIGNER = 5,
    TYPE_TAG_VECTOR = 6,
    TYPE_TAG_STRUCT = 7,
    TYPE_TAG_UNDEFINED = 1000
} type_tag_variant_t;

typedef struct {
    type_tag_variant_t type_tag;
    size_t size;
    void *value;
} type_tag_t;

typedef struct {
    uint8_t address[ADDRESS_LEN];
    fixed_bytes_t module_name;
    fixed_bytes_t name;
    size_t type_args_size;
    type_tag_t *type_args;
} type_tag_struct_t;

typedef struct {
    uint8_t address[ADDRESS_LEN];
    fixed_bytes_t name;
} module_id_t;

typedef enum {
    FUNC_UNKNOWN = 0,
    FUNC_SUPRA_ACCOUNT_TRANSFER = 1,
    FUNC_COIN_TRANSFER = 2
} entry_function_known_type_t;

typedef struct {
    type_tag_t *ty_args;
    fixed_bytes_t *args;
} agrs_raw_t;

typedef struct {
    uint8_t receiver[ADDRESS_LEN];
    uint64_t amount;
} agrs_supra_account_trasfer_t;

typedef struct {
    uint8_t receiver[ADDRESS_LEN];
    uint64_t amount;
    type_tag_struct_t ty_coin;
} agrs_coin_trasfer_t;

typedef struct {
    module_id_t module_id;
    fixed_bytes_t function_name;
    entry_function_known_type_t known_type;
    struct {
        size_t ty_size;
        size_t args_size;
        union {
            agrs_raw_t raw;
            agrs_supra_account_trasfer_t transfer;
            agrs_coin_trasfer_t coin_transfer;
        };
    } args;
} entry_function_payload_t;

typedef struct {
    fixed_bytes_t code;
    size_t ty_size;
    type_tag_t *ty_args;
    size_t args_size;
    fixed_bytes_t *args;
} script_payload_t;

typedef enum { TX_RAW = 0, TX_RAW_WITH_DATA = 1, TX_MESSAGE = 2, TX_UNDEFINED = 1000 } tx_variant_t;

typedef enum {
    PAYLOAD_SCRIPT = 0,
    PAYLOAD_ENTRY_FUNCTION = 2,
    PAYLOAD_UNDEFINED = 1000
} payload_variant_t;

typedef struct {
    tx_variant_t tx_variant;
    uint8_t sender[ADDRESS_LEN];
    uint64_t sequence;
    payload_variant_t payload_variant;
    union {
        script_payload_t script;
        entry_function_payload_t entry_function;
    } payload;
    uint64_t max_gas_amount;
    uint64_t gas_unit_price;
    uint64_t expiration_timestamp_secs;
    uint8_t chain_id;
} supra_transaction_t;
