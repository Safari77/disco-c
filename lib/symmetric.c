#include <assert.h>
#include <stdio.h>

#include "tweetstrobe.h"
#include "symmetric.h"

//
// Hash Functions
//

// disco_Hash is a simple hash function that can produce a digest of any length
// from an `input`. The minimum output length `out_len` accepted is 32 bytes.
// The `out` buffer must have at least `out_len` bytes of capacity to receive
// the digest.
void disco_Hash(uint8_t* input, size_t input_len, uint8_t* out,
                size_t out_len) {
  assert(out_len >= 32);
  strobe_s strobe;
  strobe_init(&strobe, (uint8_t*)"DiscoHash", 9);
  strobe_operate(&strobe, TYPE_AD, input, input_len, false);
  strobe_operate(&strobe, TYPE_PRF, out, out_len, false);
}

// disco_HashNew is a way to hold on to a hash context in order to continuously
// write to it. This is usefull when we don't know in advance what we're going
// to hash, or if we going to have to produce a digest at several point in time.
// It is to be used once to initialize a discoHashCtx context.
// Then the context can be used with disco_HashWrite or disco_HashWriteTuple to
// absorb a message to hash.
// Then the context can be used with disco_HashSum to produce a digest of any
// length.
void disco_HashNew(discoHashCtx* ctx) {
  assert(ctx != NULL);
  // can't check if it's already initialized coz memory is uninitialized in C
  strobe_init(&(ctx->strobe), (uint8_t*)"DiscoHash", 9);
  strobe_operate(&(ctx->strobe), TYPE_AD, NULL, 0,
                 false);  // to start streaming
  ctx->initialized = INITIALIZED;
}

// disco_HashWrite absorbs data to hash. Several call to this function on
// fragmented data are equivalent to a single call to this function on the full
// data (or a single call to disco_Hash on the full data).
void disco_HashWrite(discoHashCtx* ctx, uint8_t* input, size_t input_len) {
  assert(ctx != NULL && ctx->initialized == INITIALIZED);
  assert((input != NULL && input_len > 0) || input_len == 0);
  strobe_operate(&(ctx->strobe), TYPE_AD, input, input_len, true);
}

// disco_HashWriteTuple absorbs data to hash, and place delimiters around it.
// Several calls to this function on fragmented data are not equivalent to a
// single call to that function on the full data. To reproduce the same digest
// you must call disco_HashWriteTuple in-order with the same fragments of data.
void disco_HashWriteTuple(discoHashCtx* ctx, uint8_t* input, size_t input_len) {
  assert(ctx != NULL && ctx->initialized == INITIALIZED);
  assert((input != NULL && input_len > 0) || input_len == 0);
  strobe_operate(&(ctx->strobe), TYPE_AD, input, input_len, false);
}

// disco_HashSum produces a digest. It does not mutate the context, and thus can
// be re-used many times to produce the same digest. The context can also be
// used to absorb more data to hash after.
void disco_HashSum(discoHashCtx* ctx, uint8_t* out, size_t out_len) {
  assert(ctx != NULL && ctx->initialized == INITIALIZED);
  assert(out != NULL && out_len > 0);
  strobe_s copy = ctx->strobe;
  strobe_operate(&copy, TYPE_PRF, out, out_len, false);
}

// reset the context for re-use. Must be re-initialized after that.
void disco_HashResetCtx(discoHashCtx* ctx) {
  assert(ctx != NULL);
  ctx->initialized = 0;
  strobe_destroy(&(ctx->strobe));
}

//
// Key Derivation Function
//

// disco_DeriveKeys can be used to write random data of size `out_len` to an
// `out` buffer. It requires an `inputKey` of size `key_len` at least 16 bytes.
// This key must be of high entropy: the output of a key exchange, randomness
// generated by /dev/urandom, etc...
void disco_DeriveKeys(uint8_t* inputKey, size_t key_len, uint8_t* out,
                      size_t out_len) {
  assert(inputKey != NULL && out != NULL);
  assert(key_len >= 16 && out_len > 0);
  strobe_s strobe;
  strobe_init(&strobe, (uint8_t*)"DiscoKDF", 8);
  strobe_operate(&strobe, TYPE_AD, inputKey, key_len, false);
  strobe_operate(&strobe, TYPE_PRF, out, out_len, false);
}

//
// Message Authentication Code
//

// disco_ProtectIntegrity takes a key of at least 16 bytes, and creates an
// authentication tag of 16 bytes (or higher) in the `out` buffer (so the `out`
// buffer must have a capacity of at least 16 bytes and `out_len` must be set to
// 16 or higher)
void disco_ProtectIntegrity(uint8_t* key, size_t key_len, uint8_t* data,
                            size_t data_len, uint8_t* out, size_t out_len) {
  assert(key != NULL && key_len >= 16);
  assert(data != NULL && data_len > 0);
  assert(out != NULL && out_len >= 16);

  strobe_s strobe;
  strobe_init(&strobe, (uint8_t*)"DiscoMAC", 8);
  strobe_operate(&strobe, TYPE_AD, key, key_len, false);
  strobe_operate(&strobe, TYPE_AD, data, data_len, false);
  strobe_operate(&strobe, TYPE_MAC, out, out_len, false);
}

// disco_VerifyIntegrity can be used to verify a `tag` of size `tag_len`
// produced by disco_ProtectIntegrity with a key of size `key_len` over the
// `data` of size `data_len`. It returns true if the data has not been modified.
// False otherwise.
bool disco_VerifyIntegrity(uint8_t* key, size_t key_len, uint8_t* data,
                           size_t data_len, uint8_t* tag, size_t tag_len) {
  assert(key != NULL && key_len >= 16);
  assert(data != NULL && data_len > 0);
  assert(tag != NULL && tag_len >= 16);

  strobe_s strobe;
  strobe_init(&strobe, (uint8_t*)"DiscoMAC", 8);
  strobe_operate(&strobe, TYPE_AD, key, key_len, false);
  strobe_operate(&strobe, TYPE_AD, data, data_len, false);
  if (strobe_operate(&strobe, TYPE_MAC | FLAG_I, tag, tag_len, false) < 0) {
    return false;
  }
  return true;
}

//
// Pseudo-Random Number Generator
//

// This is a sensible PRNG: if your process forks, the two threads will produce
// the same random numbers. If you're inside a VM that gets cloned, the VMs will
// then produce the same random numbers.
void disco_RandomSeed(discoRandomCtx* ctx, uint8_t* seed, size_t seed_len) {
  assert(ctx != NULL);
  assert(seed != NULL && seed_len > 16);
  strobe_init(&(ctx->strobe), (uint8_t*)"DiscoPRNG", 9);
  strobe_operate(&(ctx->strobe), TYPE_AD, seed, seed_len, false);
  ctx->initialized = INITIALIZED;
}

// You can re-inject entropy into your PRNG.
void disco_InjectEntropy(discoRandomCtx* ctx, uint8_t* entropy,
                         size_t entropy_len) {
  assert(ctx != NULL && ctx->initialized == INITIALIZED);
  strobe_operate(&(ctx->strobe), TYPE_KEY, entropy, entropy_len, false);
}

// to obtain a random number of size `out_len`.
void disco_RandomGet(discoRandomCtx* ctx, uint8_t* out, size_t out_len) {
  assert(ctx != NULL && ctx->initialized == INITIALIZED);
  assert(out != NULL && out_len > 0);
  strobe_operate(&(ctx->strobe), TYPE_PRF, out, out_len, false);
}
