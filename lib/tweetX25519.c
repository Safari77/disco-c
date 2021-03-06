// Original code is tweetnacl
// modified to remove what I don't need
// should I add a license here?
#include "tweetX25519.h"
#define FOR(i, n) for (i = 0; i < n; ++i)
#define sv static void
typedef int64_t gf[16];

extern void randombytes(uint8_t *, uint64_t);

static const uint8_t _9[32] = {9};
static const gf _121665 = {0xDB41, 1};
sv car25519(gf o) {
  int i;
  int64_t c;
  FOR(i, 16) {
    o[i] += (1LL << 16);
    c = o[i] >> 16;
    o[(i + 1) * (i < 15)] += c - 1 + 37 * (c - 1) * (i == 15);
    o[i] -= c << 16;
  }
}

sv sel25519(gf p, gf q, int b) {
  int64_t t, i, c = ~(b - 1);
  FOR(i, 16) {
    t = c & (p[i] ^ q[i]);
    p[i] ^= t;
    q[i] ^= t;
  }
}

sv pack25519(uint8_t *o, const gf n) {
  int i, j, b;
  gf m, t;
  FOR(i, 16) t[i] = n[i];
  car25519(t);
  car25519(t);
  car25519(t);
  FOR(j, 2) {
    m[0] = t[0] - 0xffed;
    for (i = 1; i < 15; i++) {
      m[i] = t[i] - 0xffff - ((m[i - 1] >> 16) & 1);
      m[i - 1] &= 0xffff;
    }
    m[15] = t[15] - 0x7fff - ((m[14] >> 16) & 1);
    b = (m[15] >> 16) & 1;
    m[14] &= 0xffff;
    sel25519(t, m, 1 - b);
  }
  FOR(i, 16) {
    o[2 * i] = t[i] & 0xff;
    o[2 * i + 1] = t[i] >> 8;
  }
}

sv unpack25519(gf o, const uint8_t *n) {
  int i;
  FOR(i, 16) o[i] = n[2 * i] + ((int64_t)n[2 * i + 1] << 8);
  o[15] &= 0x7fff;
}

sv A(gf o, const gf a, const gf b) {
  int i;
  FOR(i, 16) o[i] = a[i] + b[i];
}

sv Z(gf o, const gf a, const gf b) {
  int i;
  FOR(i, 16) o[i] = a[i] - b[i];
}

sv M(gf o, const gf a, const gf b) {
  int64_t i, j, t[31];
  FOR(i, 31) t[i] = 0;
  FOR(i, 16) FOR(j, 16) t[i + j] += a[i] * b[j];
  FOR(i, 15) t[i] += 38 * t[i + 16];
  FOR(i, 16) o[i] = t[i];
  car25519(o);
  car25519(o);
}

sv S(gf o, const gf a) { M(o, a, a); }

sv inv25519(gf o, const gf i) {
  gf c;
  int a;
  FOR(a, 16) c[a] = i[a];
  for (a = 253; a >= 0; a--) {
    S(c, c);
    if (a != 2 && a != 4) M(c, c, i);
  }
  FOR(a, 16) o[a] = c[a];
}

int crypto_scalarmult(uint8_t *q, const uint8_t *n, const uint8_t *p) {
  uint8_t z[32];
  int64_t x[80], r, i;
  gf a, b, c, d, e, f;
  FOR(i, 31) z[i] = n[i];
  z[31] = (n[31] & 127) | 64;
  z[0] &= 248;
  unpack25519(x, p);
  FOR(i, 16) {
    b[i] = x[i];
    d[i] = a[i] = c[i] = 0;
  }
  a[0] = d[0] = 1;
  for (i = 254; i >= 0; --i) {
    r = (z[i >> 3] >> (i & 7)) & 1;
    sel25519(a, b, r);
    sel25519(c, d, r);
    A(e, a, c);
    Z(a, a, c);
    A(c, b, d);
    Z(b, b, d);
    S(d, e);
    S(f, a);
    M(a, c, a);
    M(c, b, e);
    A(e, a, c);
    Z(a, a, c);
    S(b, a);
    Z(c, d, f);
    M(a, c, _121665);
    A(a, a, d);
    M(c, c, a);
    M(a, d, f);
    M(d, b, x);
    S(b, e);
    sel25519(a, b, r);
    sel25519(c, d, r);
  }
  FOR(i, 16) {
    x[i + 16] = a[i];
    x[i + 32] = c[i];
    x[i + 48] = b[i];
    x[i + 64] = d[i];
  }
  inv25519(x + 32, x + 32);
  M(x + 16, x + 16, x + 32);
  pack25519(q, x + 16);
  return 0;
}

int crypto_scalarmult_base(uint8_t *q, const uint8_t *n) {
  return crypto_scalarmult(q, n, _9);
}

int crypto_box_keypair(uint8_t *y, uint8_t *x) {
  randombytes(x, 32);
  return crypto_scalarmult_base(y, x);
}
