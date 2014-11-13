#include <openssl/conf.h>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <string.h>
#include <string>
#include <cstring>

void handleErrors(void)
{
  ERR_print_errors_fp(stderr);
  abort();
}



int decrypt( char *ciphertext, int ciphertext_len, char *key,
   char *iv, char *plaintext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int plaintext_len;

  char * pt = plaintext + len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the decryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_256_cbc(), NULL,reinterpret_cast<const unsigned char*>(&key), reinterpret_cast<const unsigned char *>(&iv)))
    handleErrors();

  /* Provide the message to be decrypted, and obtain the plaintext output.
   * EVP_DecryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_DecryptUpdate(ctx, reinterpret_cast<unsigned char *>(&plaintext), &len, reinterpret_cast<const unsigned char*> (&ciphertext), ciphertext_len))
    handleErrors();
  plaintext_len = len;

  /* Finalise the decryption. Further plaintext bytes may be written at
   * this stage.
   */
  if(1 != EVP_DecryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(&pt), &len)) handleErrors();
  plaintext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return plaintext_len;
}



int encrypt(char *plaintext, int plaintext_len,char *key,
  char *iv, char *ciphertext)
{
  EVP_CIPHER_CTX *ctx;

  int len;

  int ciphertext_len;


  char * pt = plaintext + len;

  /* Create and initialise the context */
  if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();

  /* Initialise the encryption operation. IMPORTANT - ensure you use a key
   * and IV size appropriate for your cipher
   * In this example we are using 256 bit AES (i.e. a 256 bit key). The
   * IV size for *most* modes is the same as the block size. For AES this
   * is 128 bits */
  if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_256_cbc(), NULL, reinterpret_cast<const unsigned char *>(&key), reinterpret_cast<const unsigned char *>(&iv)))
    handleErrors();

  /* Provide the message to be encrypted, and obtain the encrypted output.
   * EVP_EncryptUpdate can be called multiple times if necessary
   */
  if(1 != EVP_EncryptUpdate(ctx,reinterpret_cast<unsigned char *>(&ciphertext), &len, reinterpret_cast<const unsigned char *>(&plaintext), plaintext_len))
    handleErrors();
  ciphertext_len = len;

  /* Finalise the encryption. Further ciphertext bytes may be written at
   * this stage.
   */
  if(1 != EVP_EncryptFinal_ex(ctx, reinterpret_cast<unsigned char *>(&pt), &len)) handleErrors();
  ciphertext_len += len;

  /* Clean up */
  EVP_CIPHER_CTX_free(ctx);

  return ciphertext_len;
}


int main(int arc, char *argv[])
{
  /* Set up the key and iv. Do I need to say to not hard code these in a
   * real application? :-)
   */

  /* A 256 bit key */
  string k = "01234567890123456789012345678901";
  char * key = k.c_str();

  /* A 128 bit IV */
  string i = "01234567890123456";
  char * iv = i.c_str();
  /* Message to be encrypted */
  string p =
    "The quick brown fox jumps over the lazy dog";
    char * plaintext = p.c_str();

  /* Buffer for ciphertext. Ensure the buffer is long enough for the
   * ciphertext which may be longer than the plaintext, dependant on the
   * algorithm and mode
   */
  char ciphertext[128];

  /* Buffer for the decrypted text */
  char decryptedtext[128];

  int decryptedtext_len, ciphertext_len;

  /* Initialise the library */
  ERR_load_crypto_strings();
  OpenSSL_add_all_algorithms();
  OPENSSL_config(NULL);

  /* Encrypt the plaintext */
  ciphertext_len = encrypt(plaintext, strlen(plaintext), key, iv,
    ciphertext);

  /* Do something useful with the ciphertext here */
  printf("Ciphertext is:\n");
  BIO_dump_fp(stdout, ciphertext, ciphertext_len);

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
