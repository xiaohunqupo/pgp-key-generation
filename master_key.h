#pragma once

#include <sodium.h>
#include <array>


/**
 *  A master key to be used for key derivation
 */
class master_key : public std::array<uint8_t, crypto_kdf_KEYBYTES>
{
    public:
        /**
         *  Constructor
         */
        master_key()
        {
            // initialize the key with random data
            crypto_kdf_keygen(data());
        }

        /**
         *  Read a password, a random salt is used
         *
         *  @param  password    The password for the key generation
         */
        master_key &operator=(const std::string &password)
        {
            // a buffer for the salt
            std::array<uint8_t, crypto_pwhash_SALTBYTES> salt;
            randombytes_buf(salt.data(), salt.size());

            // generate the key from the password and salted
            auto result = crypto_pwhash(
                data(), size(),
                password.data(), password.size(),
                salt.data(),
                crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE, crypto_pwhash_ALG_DEFAULT
            );

            // check for a valid result
            if (result != 0) {
                // failed to generate the password
                throw std::runtime_error{ "Failed to generate derived key: out of memory" };
            }

            // allow chaining
            return *this;
        }

        /**
         *  Allow assignment
         */
        using std::array<uint8_t, crypto_kdf_KEYBYTES>::operator=;

};
