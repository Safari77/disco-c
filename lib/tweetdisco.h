#pragma once

#include "tweetstrobe.h"

#include <stdlib.h>
#include <stdbool.h>

// clarity shortcuts
typedef unsigned char u8;

// asymmetric
typedef struct keyPair {
  u8 priv[32];
  u8 pub[32];
  bool isSet;
} keyPair;

//
// handshake pattern
//

// tokens
typedef enum token {
  token_e = 1,
  token_s = 2,
  token_ee = 3,
  token_es = 4,
  token_se = 5,
  token_ss = 6,
  token_end_turn = 7,
  token_end_handshake = 8
} token;

typedef struct handshakePattern {
  char *name;
  token *pre_message_patterns;
  token *message_patterns;
} handshakePattern;

#define HANDSHAKE_N                                                           \
  (const handshakePattern) {                                                  \
    .name = "N", .pre_message_patterns =                                      \
                     (token[]){token_end_turn, token_s, token_end_handshake}, \
    .message_patterns = (token[]) {                                           \
      token_e, token_es, token_end_handshake                                  \
    }                                                                         \
  }
#define HANDSHAKE_K                                                       \
  (const handshakePattern) {                                              \
    .name = "K",                                                          \
    .pre_message_patterns =                                               \
        (token[]){token_s, token_end_turn, token_s, token_end_handshake}, \
    .message_patterns = (token[]) {                                       \
      token_e, token_es, token_ss, token_end_handshake                    \
    }                                                                     \
  }
#define HANDSHAKE_X                                                           \
  (const handshakePattern) {                                                  \
    .name = "X", .pre_message_patterns =                                      \
                     (token[]){token_end_turn, token_s, token_end_handshake}, \
    .message_patterns = (token[]) {                                           \
      token_e, token_es, token_s, token_ss, token_end_handshake               \
    }                                                                         \
  }
#define HANDSHAKE_NK                                                           \
  (const handshakePattern) {                                                   \
    .name = "NK", .pre_message_patterns =                                      \
                      (token[]){token_end_turn, token_s, token_end_handshake}, \
    .message_patterns = (token[]) {                                            \
      token_e, token_es, token_end_turn, token_e, token_ee,                    \
          token_end_handshake                                                  \
    }                                                                          \
  }
#define HANDSHAKE_KK                                                      \
  (const handshakePattern) {                                              \
    .name = "KK",                                                         \
    .pre_message_patterns =                                               \
        (token[]){token_s, token_end_turn, token_s, token_end_handshake}, \
    .message_patterns = (token[]) {                                       \
      token_e, token_es, token_ss, token_end_turn, token_e, token_ee,     \
          token_se, token_end_handshake                                   \
    }                                                                     \
  }
#define HANDSHAKE_NX                                                      \
  (const handshakePattern) {                                              \
    .name = "NX", .pre_message_patterns = (token[]){token_end_handshake}, \
    .message_patterns = (token[]) {                                       \
      token_e, token_end_turn, token_e, token_ee, token_s, token_es,      \
          token_end_handshake                                             \
    }                                                                     \
  }
#define HANDSHAKE_KX                                                           \
  (const handshakePattern) {                                                   \
    .name = "KX",                                                              \
    .pre_message_patterns = (token[]){token_s, token_end_handshake},           \
    .message_patterns = (token[]) {                                            \
      token_e, token_end_turn, token_e, token_ee, token_se, token_s, token_es, \
          token_end_handshake                                                  \
    }                                                                          \
  }
#define HANDSHAKE_XK                                                           \
  (const handshakePattern) {                                                   \
    .name = "XK", .pre_message_patterns =                                      \
                      (token[]){token_end_turn, token_s, token_end_handshake}, \
    .message_patterns = (token[]) {                                            \
      token_e, token_es, token_end_turn, token_e, token_ee, token_end_turn,    \
          token_s, token_se, token_end_handshake                               \
    }                                                                          \
  }
#define HANDSHAKE_IK                                                           \
  (const handshakePattern) {                                                   \
    .name = "IK", .pre_message_patterns =                                      \
                      (token[]){token_end_turn, token_s, token_end_handshake}, \
    .message_patterns = (token[]) {                                            \
      token_e, token_es, token_s, token_ss, token_end_turn, token_e, token_ee, \
          token_se, token_end_handshake                                        \
    }                                                                          \
  }

#define HANDSHAKE_XX                                                      \
  (const handshakePattern) {                                              \
    .name = "XX", .pre_message_patterns = (token[]){token_end_handshake}, \
    .message_patterns = (token[]) {                                       \
      token_e, token_end_turn, token_e, token_ee, token_s, token_es,      \
          token_end_turn, token_s, token_se, token_end_handshake          \
    }                                                                     \
  }

#define HANDSHAKE_IX                                                          \
  (const handshakePattern) {                                                  \
    .name = "IX", .pre_message_patterns = (token[]){token_end_handshake},     \
    .message_patterns = (token[]) {                                           \
      token_e, token_s, token_end_turn, token_e, token_ee, token_se, token_s, \
          token_es, token_end_handshake                                       \
    }                                                                         \
  }
//
// states
//

typedef struct symmetricState {
  strobe_s strobe;
  bool isKeyed;
} symmetricState;

typedef struct handshakeState {
  symmetricState symmetric_state;

  keyPair s;
  keyPair e;
  keyPair rs;
  keyPair re;

  bool initiator;
  token *message_patterns;
  bool sending;
  bool handshake_done;
} handshakeState;

// handshake
void disco_Initialize(handshakeState *hs, handshakePattern hp, bool initiator,
                      u8 *prologue, size_t prologue_len, keyPair *s, keyPair *e,
                      keyPair *rs, keyPair *re);
int disco_WriteMessage(handshakeState *hs, u8 *payload, size_t payload_len,
                       u8 *message_buffer, strobe_s *client_s,
                       strobe_s *server_s);
int disco_ReadMessage(handshakeState *hs, u8 *message, size_t message_len,
                      u8 *payload_buffer, strobe_s *client_s,
                      strobe_s *server_s);
void disco_generateKeyPair(keyPair *kp);

// post handshake
void disco_EncryptInPlace(strobe_s *strobe, u8 *plaintext, size_t plaintext_len,
                          size_t plaintext_capacity);
bool disco_DecryptInPlace(strobe_s *strobe, u8 *ciphertext,
                          size_t ciphertext_len);
