/* Copyright 2011 Nick Mathewson

   You may do anything with this work that copyright law would normally
   restrict, so long as you retain the above notice(s) and this license
   in all redistributed copies and derived works.  There is no warranty.
*/

#ifndef OBFS2_H
#define OBFS2_H

#include <sys/types.h>

typedef struct obfs2_state_t obfs2_state_t;
struct evbuffer;
struct protocol_t;

#define SHARED_SECRET_LENGTH 16

obfs2_state_t *obfs2_state_new(int *initiator);
void obfs2_state_set_shared_secret(obfs2_state_t *state,
                                      const char *secret, size_t secretlen);
void obfs2_state_free(obfs2_state_t *state);
int obfs2_send_initial_message(obfs2_state_t *state, struct evbuffer *buf);
int obfs2_send(obfs2_state_t *state,
               struct evbuffer *source, struct evbuffer *dest);
int obfs2_recv(obfs2_state_t *state, struct evbuffer *source,
               struct evbuffer *dest);

void *obfs2_new(struct protocol_t *proto_struct);


#ifdef CRYPT_PROTOCOL_PRIVATE
/* ==========
   These definitions are not part of the crypt_protocol interface.
   They're exposed here so that the unit tests can use them.
   ==========
*/
/* from brl's obfuscated-ssh standard. */
//#define OBFUSCATE_MAGIC_VALUE        0x0BF5CA7E

/* our own, since we break brl's spec */
#define OBFUSCATE_MAGIC_VALUE        0x2BF5CA7E
#define OBFUSCATE_SEED_LENGTH        16
#define OBFUSCATE_MAX_PADDING        8192
#define OBFUSCATE_ZERO_SEED "\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0\0"

#define INITIATOR_PAD_TYPE "Initiator obfuscation padding"
#define RESPONDER_PAD_TYPE "Responder obfuscation padding"
#define INITIATOR_SEND_TYPE "Initiator obfuscated data"
#define RESPONDER_SEND_TYPE "Responder obfuscated data"

struct obfs2_state_t {
  /** Current protocol state.  We start out waiting for key information.  Then
      we have a key and wait for padding to arrive.  Finally, we are sending
      and receiving bytes on the connection.
  */
  enum {
    ST_WAIT_FOR_KEY,
    ST_WAIT_FOR_PADDING,
    ST_OPEN,
  } state;
  /** Random seed we generated for this stream */
  uchar initiator_seed[OBFUSCATE_SEED_LENGTH];
  /** Random seed the other side generated for this stream */
  uchar responder_seed[OBFUSCATE_SEED_LENGTH];
  /** Shared secret seed value. */
  uchar secret_seed[SHARED_SECRET_LENGTH];
  /** True iff we opened this connection */
  int we_are_initiator;

  /** key used to encrypt outgoing data */
  crypt_t *send_crypto;
  /** key used to encrypt outgoing padding */
  crypt_t *send_padding_crypto;
  /** key used to decrypt incoming data */
  crypt_t *recv_crypto;
  /** key used to decrypt incoming padding */
  crypt_t *recv_padding_crypto;

  /** Buffer full of data we'll send once the handshake is done. */
  struct evbuffer *pending_data_to_send;

  /** Number of padding bytes to read before we get to real data */
  int padding_left_to_read;
};
#endif

#endif