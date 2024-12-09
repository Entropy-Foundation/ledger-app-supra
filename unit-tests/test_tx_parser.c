#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#include <cmocka.h>

#include "transaction/deserialize.h"
#include "transaction/types.h"

static void test_tx_deserialization(void **state) {
    (void) state;

    static transaction_t tx;
    // clang-format off
    static const uint8_t raw_tx[] = {
        136, 209, 110, 209, 171, 166, 215, 125, 82, 26, 175, 173, 166, 214, 233, 45, 122, 23, 69, 39, 173, 180, 87,
        173, 250, 102, 169, 197, 160, 241, 254, 55, 122, 61, 60, 56, 131, 16, 0, 157, 163, 130, 3, 53, 36, 27, 160,
        248, 179, 221, 122, 90, 74, 251, 196, 84, 23, 65, 157, 18, 50, 195, 72, 223, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 4, 99, 111, 105, 110,
        8, 116, 114, 97, 110, 115, 102, 101, 114, 1, 7, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 10, 115, 117, 112, 114, 97, 95, 99, 111, 105, 110, 9, 83, 117, 112, 114, 97,
        67, 111, 105, 110, 0, 2, 32, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 1, 8, 16, 39, 0, 0, 0, 0, 0, 0, 32, 161, 7, 0, 0, 0, 0, 0, 100, 0, 0, 0, 0, 0, 0, 0, 255, 15, 165,
        212, 232, 0, 0, 0, 6
    };

    buffer_t buf = {.ptr = raw_tx, .size = sizeof(raw_tx), .offset = 0};

    parser_status_e status = transaction_deserialize(&buf, &tx);

    assert_int_equal(status, PARSING_OK);
    // clang-format off
    static const uint8_t sender [] = {
        122,  61,  60, 56, 131,  16, 0, 157, 163, 130, 3, 53, 36, 27, 160, 248,
        179, 221, 122, 90, 74, 251, 196, 84, 23,  65, 157, 18,  50, 195,  72, 223
    };
    assert_memory_equal(tx.sender, sender, 32);
    assert_int_equal(tx.sequence, 0);
    assert_int_equal(tx.max_gas_amount, 500000);
    assert_int_equal(tx.gas_unit_price, 100);
    assert_int_equal(tx.expiration_timestamp_secs, 999999999999);
    assert_int_equal(tx.chain_id, 6);
    assert_int_equal(tx.payload_variant, PAYLOAD_ENTRY_FUNCTION);
    assert_int_equal(tx.payload.entry_function.known_type, FUNC_COIN_TRANSFER);
    // clang-format off
    static const uint8_t coin_module_address [] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
    };
    assert_memory_equal(tx.payload.entry_function.args.coin_transfer.ty_coin.address, coin_module_address, 32);
    assert_int_equal(tx.payload.entry_function.args.coin_transfer.ty_coin.module_name.len, 10);
    assert_memory_equal(tx.payload.entry_function.args.coin_transfer.ty_coin.module_name.bytes, "supra_coin", 10);
    assert_int_equal(tx.payload.entry_function.args.coin_transfer.ty_coin.name.len, 9);
    assert_memory_equal(tx.payload.entry_function.args.coin_transfer.ty_coin.name.bytes, "SupraCoin", 9);
    assert_int_equal(tx.payload.entry_function.args.coin_transfer.ty_coin.type_args_size, 0);
    // clang-format off
    static const uint8_t receiver [] = {
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
        0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1
    };
    assert_memory_equal(tx.payload.entry_function.args.coin_transfer.receiver, receiver, 32);
    assert_int_equal(tx.payload.entry_function.args.coin_transfer.amount, 10000);
}

int main() {
    const struct CMUnitTest tests[] = {cmocka_unit_test(test_tx_deserialization)};

    return cmocka_run_group_tests(tests, NULL, NULL);
}
