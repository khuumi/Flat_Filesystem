#ifndef EXAMPLE_H_
#define EXAMPLE_H_

void handleErrors(void);

int aes_decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
  unsigned char *iv, unsigned char *plaintext);

int aes_encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
  unsigned char *iv, unsigned char *ciphertext);

#endif
