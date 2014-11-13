#include <iostream>
#include <string>
#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>

extern "C"{
	#include "crypto.h"
}

using namespace std;

int main(){

	  
/* A 256 bit key */
  string k = "01234567890123456789012345678901";
  unsigned char *key = (unsigned char *)k.c_str();

  /* A 128 bit IV */
  string i = "01234567890123456";
  unsigned char *iv = (unsigned char *)i.c_str();
  
  /* Message to be encrypted */
  string p = "The quick brown fox jumps over the lazy dog";
  int slen = p.length();
  unsigned char *plaintext = (unsigned char *)p.c_str();

  /* Buffer for ciphertext. Ensure the buffer is long enough for the
   * ciphertext which may be longer than the plaintext, dependant on the
   * algorithm and mode
   */
  unsigned char ciphertext[128];

  /* Buffer for the decrypted text */
  unsigned char decryptedtext[128];

  int decryptedtext_len, ciphertext_len;

  /* Initialise the library */
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
  OPENSSL_config(NULL);

  /* Encrypt the plaintext */
  ciphertext_len = encrypt(plaintext, slen, key, iv,
    ciphertext);

  /* Do something useful with the ciphertext here */
  printf("Ciphertext is:\n");
  BIO_dump_fp(stdout, (const char*) ciphertext, ciphertext_len);

  /* Decrypt the ciphertext */
  decryptedtext_len = decrypt(ciphertext, ciphertext_len, key, iv,
    decryptedtext);

  /* Add a NULL terminator. We are expecting printable text */
  decryptedtext[decryptedtext_len] = '\0';

  /* Show the decrypted text */
  printf("Decrypted text is:\n");
  printf("%s\n", decryptedtext);

  /* Clean up */
  EVP_cleanup();
  ERR_free_strings();

  return 0;


}
