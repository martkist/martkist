Shared Libraries
================

## martkistconsensus

The purpose of this library is to make the verification functionality that is critical to Martkist's consensus available to other applications, e.g. to language bindings.

### API

The interface is defined in the C header `martkistconsensus.h` located in  `src/script/martkistconsensus.h`.

#### Version

`martkistconsensus_version` returns an `unsigned int` with the API version *(currently at an experimental `0`)*.

#### Script Validation

`martkistconsensus_verify_script` returns an `int` with the status of the verification. It will be `1` if the input script correctly spends the previous output `scriptPubKey`.

##### Parameters
- `const unsigned char *scriptPubKey` - The previous output script that encumbers spending.
- `unsigned int scriptPubKeyLen` - The number of bytes for the `scriptPubKey`.
- `const unsigned char *txTo` - The transaction with the input that is spending the previous output.
- `unsigned int txToLen` - The number of bytes for the `txTo`.
- `unsigned int nIn` - The index of the input in `txTo` that spends the `scriptPubKey`.
- `unsigned int flags` - The script validation flags *(see below)*.
- `martkistconsensus_error* err` - Will have the error/success code for the operation *(see below)*.

##### Script Flags
- `martkistconsensus_SCRIPT_FLAGS_VERIFY_NONE`
- `martkistconsensus_SCRIPT_FLAGS_VERIFY_P2SH` - Evaluate P2SH ([BIP16](https://github.com/martkist/bips/blob/master/bip-0016.mediawiki)) subscripts
- `martkistconsensus_SCRIPT_FLAGS_VERIFY_DERSIG` - Enforce strict DER ([BIP66](https://github.com/martkist/bips/blob/master/bip-0066.mediawiki)) compliance
- `martkistconsensus_SCRIPT_FLAGS_VERIFY_NULLDUMMY` - Enforce NULLDUMMY ([BIP147](https://github.com/martkist/bips/blob/master/bip-0147.mediawiki))
- `martkistconsensus_SCRIPT_FLAGS_VERIFY_CHECKLOCKTIMEVERIFY` - Enable CHECKLOCKTIMEVERIFY ([BIP65](https://github.com/martkist/bips/blob/master/bip-0065.mediawiki))
- `martkistconsensus_SCRIPT_FLAGS_VERIFY_CHECKSEQUENCEVERIFY` - Enable CHECKSEQUENCEVERIFY ([BIP112](https://github.com/martkist/bips/blob/master/bip-0112.mediawiki))
- `martkistconsensus_SCRIPT_FLAGS_VERIFY_WITNESS` - Enable WITNESS ([BIP141](https://github.com/martkist/bips/blob/master/bip-0141.mediawiki))

##### Errors
- `martkistconsensus_ERR_OK` - No errors with input parameters *(see the return value of `martkistconsensus_verify_script` for the verification status)*
- `martkistconsensus_ERR_TX_INDEX` - An invalid index for `txTo`
- `martkistconsensus_ERR_TX_SIZE_MISMATCH` - `txToLen` did not match with the size of `txTo`
- `martkistconsensus_ERR_DESERIALIZE` - An error deserializing `txTo`
- `martkistconsensus_ERR_AMOUNT_REQUIRED` - Input amount is required if WITNESS is used

### Example Implementations
- [NMartkist](https://github.com/NicolasDorier/NMartkist/blob/master/NMartkist/Script.cs#L814) (.NET Bindings)
- [node-libmartkistconsensus](https://github.com/bitpay/node-libmartkistconsensus) (Node.js Bindings)
- [java-libmartkistconsensus](https://github.com/dexX7/java-libmartkistconsensus) (Java Bindings)
- [martkistconsensus-php](https://github.com/Bit-Wasp/martkistconsensus-php) (PHP Bindings)
