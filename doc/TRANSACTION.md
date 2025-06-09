# Supra Transaction Serialization

## Overview

All transactions executed on the Supra blockchain must be signed. Unsigned transactions are known as RawTransactions. They contain all the information about how to execute an operation on an account within Supra.

## Structure

### Raw transaction

| Field                       | Size (bytes) | Description                                                                                                                                      |
| --------------------------- | :----------: | ------------------------------------------------------------------------------------------------------------------------------------------------ |
| `sender`                    |      32      | Account address of the sender                                                                                                                    |
| `sequence_number`           |      8       | Sequence number of this transaction. This must match the sequence number stored in the sender's account at the time the transaction executes     |
| `payload`                   |     var      | Instructions for the Supra blockchain, including publishing a module, execute a script function or execute a script payload                      |
| `max_gas_amount`            |      8       | Maximum total gas to spend for this transaction. The account must have more than this gas or the transaction will be discarded during validation |
| `gas_unit_price`            |      8       | Price to be paid per gas unit                                                                                                                    |
| `expiration_timestamp_secs` |      8       | The blockchain timestamp at which the blockchain would discard this transaction                                                                  |
| `chain_id`                  |      1       | The chain ID of the blockchain that this transaction is intended to be run on                                                                    |

### BCS (Binary Canonical Serialization)

Binary Canonical Serialization (BCS) is a serialization format applied to the raw (unsigned) transaction. BCS is not a self-descriptive format. Therefore, to deserialize a message, you need to know its type and scheme beforehand.

### Signing message

In Supra, any content that is signed or hashed is salted with a unique prefix to distinguish it from other types of messages. This is done to ensure that the content can only be used in the intended scenarios. The signing message of a `RawTransaction` is prefixed with `prefix_bytes`, which is `sha3_256("SUPRA::RawTransaction")`. Therefore: `signing_message = prefix_bytes | bcs_bytes_of_raw_transaction`.

Supra also supports signing with the MultiEd25519 signature scheme, which corresponds to the salt `sha3_256("SUPRA::RawTransactionWithData")`, but Ledger, at this stage, does not support deserialization of such a transaction.

Any other data is treated as a request to sign an arbitrary message, first verifying that it contains only ASCII characters.
