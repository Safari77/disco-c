#include "tweetdisco.h"
#include <stdio.h>

void test_N() {
  // generate server keypair
  keyPair server_keypair;
  disco_generateKeyPair(&server_keypair);

  // initialize client
  handshakeState hs_client;
  disco_Initialize(&hs_client, HANDSHAKE_N, true, NULL, 0, NULL, NULL,
                   &server_keypair, NULL);

  // initialize server
  handshakeState hs_server;
  disco_Initialize(&hs_server, HANDSHAKE_N, false, NULL, 0, &server_keypair,
                   NULL, NULL, NULL);

  // write the first handshake message → e, es
  u8 out[500];
  strobe_s c_write;
  strobe_s c_read;
  u8 text[] = "hey!";
  int out_len = disco_WriteMessage(&hs_client, text, 5, out, &c_write, &c_read);
  if (out_len < 0) {
    printf("can't write handshake message\n");
    abort();
  }

  // handshake should be done for the client
  assert(c_write.initialized && c_read.initialized);

  // debug
  printf("sent %d bytes\n", out_len);

  // process the first handshake message → e, es
  u8 in[500];
  strobe_s s_read;
  strobe_s s_write;
  int in_len =
      disco_ReadMessage(&hs_server, out, out_len, in, &s_read, &s_write);
  if (in_len < 0) {
    printf("can't read handshake message\n");
    abort();
  }

  assert(s_write.initialized && s_read.initialized);

  // debug
  printf("received %d bytes:%s\n", in_len, in);

  // trying to send a post-handshake message
  u8 pt_in_place[] =
      "Us little brogammers we like to brogamme on our motorbikes yeah ";
  u8 *ct_and_mac = (u8 *)malloc(sizeof(pt_in_place) + 16);
  memcpy(ct_and_mac, pt_in_place, sizeof(pt_in_place));

  disco_EncryptInPlace(&c_write, ct_and_mac, sizeof(pt_in_place),
                       sizeof(pt_in_place) + 16);

  // decrypt
  if (disco_DecryptInPlace(&s_read, ct_and_mac, sizeof(pt_in_place) + 16) ==
      false) {
    printf("cannot decrypt in place");
    abort();
  }
  printf("final decrypt in place: %s\n", ct_and_mac);
}

void test_NX() {
  // generate server keypair
  keyPair server_keypair;
  disco_generateKeyPair(&server_keypair);

  // initialize client
  handshakeState hs_client;
  disco_Initialize(&hs_client, HANDSHAKE_NX, true, NULL, 0, NULL, NULL, NULL,
                   NULL);

  // initialize server
  handshakeState hs_server;
  disco_Initialize(&hs_server, HANDSHAKE_NX, false, NULL, 0, &server_keypair,
                   NULL, NULL, NULL);

  // write the first handshake message → e, es
  u8 out[500];
  u8 text[] = "hey!";
  int out_len = disco_WriteMessage(&hs_client, text, 5, out, NULL, NULL);
  if (out_len < 0) {
    printf("can't write handshake message\n");
    abort();
  }

  // debug
  printf("sent %d bytes\n", out_len);
  for (int i = 0; i < out_len; i++) {
    printf("%02x", out[i]);
  }
  printf("\n");

  // process the first handshake message → e, es
  u8 in[500];
  int in_len = disco_ReadMessage(&hs_server, out, out_len, in, NULL, NULL);
  if (in_len < 0) {
    printf("can't read handshake message\n");
    abort();
  }

  // debug
  printf("received %d bytes:%s\n", in_len, in);
  printf("hs_client and hs_server state after first trip\n");
  strobe_print(&(hs_client.symmetric_state.strobe));
  strobe_print(&(hs_server.symmetric_state.strobe));

  // create second handshake message
  printf("\n\npreparing second handshake message ->\n\n");
  strobe_s s_write;
  strobe_s s_read;
  out_len = disco_WriteMessage(&hs_server, (u8 *)"hello hello", 12, out,
                               &s_read, &s_write);
  if (out_len < 0) {
    printf("can't write handshake message\n");
    abort();
  }

  // should be initialized
  assert(s_write.initialized && s_read.initialized);

  // debug
  printf("sent %d bytes:\n", out_len);
  for (int i = 0; i < out_len; i++) {
    printf("%02x", out[i]);
  }
  printf("\n");

  // process second handshake message
  printf("\n\nparsing second handshake message <-\n\n");
  strobe_s c_write;
  strobe_s c_read;
  in_len = disco_ReadMessage(&hs_client, out, out_len, in, &c_write, &c_read);
  if (in_len < 0) {
    printf("can't read handshake message\n");
    abort();
  }

  // debug
  printf("received %d bytes:%s\n", in_len, in);

  // should be initialized
  assert(c_write.initialized && c_read.initialized);

  // trying to send a post-handshake message
  u8 pt_in_place[] =
      "Us little brogammers we like to brogamme on our motorbikes yeah ";
  u8 *ct_and_mac = (u8 *)malloc(sizeof(pt_in_place) + 16);
  memcpy(ct_and_mac, pt_in_place, sizeof(pt_in_place));

  disco_EncryptInPlace(&c_write, ct_and_mac, sizeof(pt_in_place),
                       sizeof(pt_in_place) + 16);

  // decrypt
  if (disco_DecryptInPlace(&s_read, ct_and_mac, sizeof(pt_in_place) + 16) ==
      false) {
    printf("cannot decrypt in place");
    abort();
  }
  printf("final decrypt in place: %s\n", ct_and_mac);

  // another one
  memcpy(ct_and_mac, pt_in_place, sizeof(pt_in_place));
  disco_EncryptInPlace(&c_write, ct_and_mac, sizeof(pt_in_place),
                       sizeof(pt_in_place) + 16);
  if (disco_DecryptInPlace(&s_read, ct_and_mac, sizeof(pt_in_place) + 16) ==
      false) {
    printf("cannot decrypt in place");
    abort();
  }
  printf("final decrypt in place: %s\n", ct_and_mac);
}

void test_IK() {
  // generate server keypair
  keyPair client_keypair;
  keyPair server_keypair;
  disco_generateKeyPair(&client_keypair);
  disco_generateKeyPair(&server_keypair);

  // initialize client
  handshakeState hs_client;
  disco_Initialize(&hs_client, HANDSHAKE_IK, true, NULL, 0, &client_keypair,
                   NULL, &server_keypair, NULL);

  // initialize server
  handshakeState hs_server;
  disco_Initialize(&hs_server, HANDSHAKE_IK, false, NULL, 0, &server_keypair,
                   NULL, NULL, NULL);

  // write the first handshake message → e, es, s, ss
  u8 out[500];
  u8 text[] = "hey!";
  int out_len = disco_WriteMessage(&hs_client, text, 5, out, NULL, NULL);
  if (out_len < 0) {
    printf("can't write handshake message\n");
    abort();
  }

  // debug
  printf("sent %d bytes\n", out_len);
  for (int i = 0; i < out_len; i++) {
    printf("%02x", out[i]);
  }
  printf("\n");

  // process the first handshake message
  u8 in[500];
  int in_len = disco_ReadMessage(&hs_server, out, out_len, in, NULL, NULL);
  if (in_len < 0) {
    printf("can't read handshake message\n");
    abort();
  }

  // debug
  printf("received %d bytes:%s\n", in_len, in);
  printf("hs_client and hs_server state after first trip\n");
  strobe_print(&(hs_client.symmetric_state.strobe));
  strobe_print(&(hs_server.symmetric_state.strobe));

  // create second handshake message <- e, ee, se
  printf("\n\npreparing second handshake message ->\n\n");
  strobe_s s_write;
  strobe_s s_read;
  out_len = disco_WriteMessage(&hs_server, (u8 *)"hello hello", 12, out,
                               &s_read, &s_write);
  if (out_len < 0) {
    printf("can't write handshake message\n");
    abort();
  }

  // should be initialized
  assert(s_write.initialized && s_read.initialized);

  // debug
  printf("sent %d bytes:\n", out_len);
  for (int i = 0; i < out_len; i++) {
    printf("%02x", out[i]);
  }
  printf("\n");

  // process second handshake message
  printf("\n\nparsing second handshake message <-\n\n");
  strobe_s c_write;
  strobe_s c_read;
  in_len = disco_ReadMessage(&hs_client, out, out_len, in, &c_write, &c_read);
  if (in_len < 0) {
    printf("can't read handshake message\n");
    abort();
  }

  // debug
  printf("received %d bytes:%s\n", in_len, in);

  // should be initialized
  assert(c_write.initialized && c_read.initialized);

  // trying to send a post-handshake message
  u8 pt_in_place[] =
      "Us little brogammers we like to brogamme on our motorbikes yeah ";
  u8 *ct_and_mac = (u8 *)malloc(sizeof(pt_in_place) + 16);
  memcpy(ct_and_mac, pt_in_place, sizeof(pt_in_place));

  disco_EncryptInPlace(&c_write, ct_and_mac, sizeof(pt_in_place),
                       sizeof(pt_in_place) + 16);

  // decrypt
  if (disco_DecryptInPlace(&s_read, ct_and_mac, sizeof(pt_in_place) + 16) ==
      false) {
    printf("cannot decrypt in place");
    abort();
  }
  printf("final decrypt in place: %s\n", ct_and_mac);

  // other direction
  u8 pt_in_place2[] = "this is hopefully the last test for today";
  u8 *ct_and_mac2 = (u8 *)malloc(sizeof(pt_in_place2) + 16);
  memcpy(ct_and_mac2, pt_in_place2, sizeof(pt_in_place2));

  disco_EncryptInPlace(&s_write, ct_and_mac2, sizeof(pt_in_place2),
                       sizeof(pt_in_place2) + 16);

  // decrypt
  if (disco_DecryptInPlace(&c_read, ct_and_mac2, sizeof(pt_in_place2) + 16) ==
      false) {
    printf("cannot decrypt in place");
    abort();
  }
  printf("final decrypt in place: %s\n", ct_and_mac2);
}

int main() {
  // doing a loop coz I have a bug SOMETIMES
  for (int j = 0; j < 100; j++) {
    printf("iteration #%d\n", j);
    printf("\n\ntesting N\n\n");
    test_N();
    printf("\n\ntesting NX\n\n");
    test_NX();
    printf("\n\ntesting IK\n\n");
    test_IK();
  }

}
