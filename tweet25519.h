#ifndef TWEETNACL_H
#define TWEETNACL_H
#define crypto_box_PRIMITIVE "curve25519xsalsa20poly1305"
#define crypto_box crypto_box_curve25519xsalsa20poly1305
#define crypto_box_open crypto_box_curve25519xsalsa20poly1305_open
#define crypto_box_keypair crypto_box_curve25519xsalsa20poly1305_keypair
#define crypto_box_beforenm crypto_box_curve25519xsalsa20poly1305_beforenm
#define crypto_box_afternm crypto_box_curve25519xsalsa20poly1305_afternm
#define crypto_box_open_afternm crypto_box_curve25519xsalsa20poly1305_open_afternm
#define crypto_box_PUBLICKEYBYTES crypto_box_curve25519xsalsa20poly1305_PUBLICKEYBYTES
#define crypto_box_SECRETKEYBYTES crypto_box_curve25519xsalsa20poly1305_SECRETKEYBYTES
#define crypto_box_BEFORENMBYTES crypto_box_curve25519xsalsa20poly1305_BEFORENMBYTES
#define crypto_box_NONCEBYTES crypto_box_curve25519xsalsa20poly1305_NONCEBYTES
#define crypto_box_ZEROBYTES crypto_box_curve25519xsalsa20poly1305_ZEROBYTES
#define crypto_box_BOXZEROBYTES crypto_box_curve25519xsalsa20poly1305_BOXZEROBYTES
#define crypto_box_IMPLEMENTATION crypto_box_curve25519xsalsa20poly1305_IMPLEMENTATION
#define crypto_box_VERSION crypto_box_curve25519xsalsa20poly1305_VERSION
#define crypto_scalarmult_PRIMITIVE "curve25519"
#define crypto_scalarmult crypto_scalarmult_curve25519
#define crypto_scalarmult_base crypto_scalarmult_curve25519_base
#define crypto_scalarmult_BYTES crypto_scalarmult_curve25519_BYTES
#define crypto_scalarmult_SCALARBYTES crypto_scalarmult_curve25519_SCALARBYTES
#define crypto_scalarmult_IMPLEMENTATION crypto_scalarmult_curve25519_IMPLEMENTATION
#define crypto_scalarmult_VERSION crypto_scalarmult_curve25519_VERSION
#define crypto_scalarmult_curve25519_tweet_BYTES 32
#define crypto_scalarmult_curve25519_tweet_SCALARBYTES 32
extern int crypto_scalarmult_curve25519_tweet(unsigned char *,const unsigned char *,const unsigned char *);
extern int crypto_scalarmult_curve25519_tweet_base(unsigned char *,const unsigned char *);
#define crypto_scalarmult_curve25519_tweet_VERSION "-"
#define crypto_scalarmult_curve25519 crypto_scalarmult_curve25519_tweet
#define crypto_scalarmult_curve25519_base crypto_scalarmult_curve25519_tweet_base
#define crypto_scalarmult_curve25519_BYTES crypto_scalarmult_curve25519_tweet_BYTES
#define crypto_scalarmult_curve25519_SCALARBYTES crypto_scalarmult_curve25519_tweet_SCALARBYTES
#define crypto_scalarmult_curve25519_VERSION crypto_scalarmult_curve25519_tweet_VERSION
#define crypto_scalarmult_curve25519_IMPLEMENTATION "crypto_scalarmult/curve25519/tweet"
#define crypto_sign_PRIMITIVE "ed25519"
#define crypto_sign crypto_sign_ed25519
#define crypto_sign_open crypto_sign_ed25519_open
#define crypto_sign_keypair crypto_sign_ed25519_keypair
#define crypto_sign_BYTES crypto_sign_ed25519_BYTES
#define crypto_sign_PUBLICKEYBYTES crypto_sign_ed25519_PUBLICKEYBYTES
#define crypto_sign_SECRETKEYBYTES crypto_sign_ed25519_SECRETKEYBYTES
#define crypto_sign_IMPLEMENTATION crypto_sign_ed25519_IMPLEMENTATION
#define crypto_sign_VERSION crypto_sign_ed25519_VERSION
#define crypto_sign_ed25519_tweet_BYTES 64
#define crypto_sign_ed25519_tweet_PUBLICKEYBYTES 32
#define crypto_sign_ed25519_tweet_SECRETKEYBYTES 64
extern int crypto_sign_ed25519_tweet(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_tweet_open(unsigned char *,unsigned long long *,const unsigned char *,unsigned long long,const unsigned char *);
extern int crypto_sign_ed25519_tweet_keypair(unsigned char *,unsigned char *);
#define crypto_sign_ed25519_tweet_VERSION "-"
#define crypto_sign_ed25519 crypto_sign_ed25519_tweet
#define crypto_sign_ed25519_open crypto_sign_ed25519_tweet_open
#define crypto_sign_ed25519_keypair crypto_sign_ed25519_tweet_keypair
#define crypto_sign_ed25519_BYTES crypto_sign_ed25519_tweet_BYTES
#define crypto_sign_ed25519_PUBLICKEYBYTES crypto_sign_ed25519_tweet_PUBLICKEYBYTES
#define crypto_sign_ed25519_SECRETKEYBYTES crypto_sign_ed25519_tweet_SECRETKEYBYTES
#define crypto_sign_ed25519_VERSION crypto_sign_ed25519_tweet_VERSION
#define crypto_sign_ed25519_IMPLEMENTATION "crypto_sign/ed25519/tweet"