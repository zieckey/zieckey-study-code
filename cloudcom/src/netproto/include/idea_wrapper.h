#ifndef QOSLIB_ENCRYPT_UTILITY_H_
#define QOSLIB_ENCRYPT_UTILITY_H_

#include "netproto/include/config.h"
#include "netproto/include/platform_micros.h"
#include "netproto/include/data_stream.h"

#if H_PROVIDE_IDEA_ENCRYPT

#include <idea/idea.h>

//#ifdef H_OS_WINDOWS
//#include <idea/idea.h>
//#elif defined( H_OS_FREEBSD )
//#include <idea.h>
//#elif defined( H_OS_LINUX )
//#include <openssl/idea.h>
//#else
//#include <idea.h>
//#endif


namespace npp
{


    class _EXPORT_OSLIB IDEA
    {
    public:
        //! \brief IDEA encrypt
        //! \param const unsigned char*  szSource - the input source data
        //! \param const unsigned int nSourceLen - the source data length
        //! \param const std::string & strEncryptKey - the 16 bytes long encrypting key
        //! \param npp::MemoryDataStream & dataEncrypted - the encrypted data is stored here
        //! \return bool - true if encrypts successfully
        static bool encrypt( const unsigned char* szSource, const unsigned int nSourceLen, const std::string& strEncryptKey, npp::MemoryDataStream& dataEncrypted );
        static bool encrypt( const unsigned char* szSource, const unsigned int nSourceLen, const ::IDEA_KEY_SCHEDULE& ideaEncryptKey, npp::MemoryDataStream& dataEncrypted );


        //! \brief IDEA decrypt
        //! \param const unsigned char*  szSource - the input source data
        //! \param const unsigned int nSourceLen - the source data length
        //! \param const std::string & strDecryptKey - the 16 bytes long decrypting key
        //! \param npp::MemoryDataStream & dataDecrypted - the decrypted data is stored here
        //! \return bool - true if decrypts successfully
        static bool decrypt( const unsigned char* szSource, const unsigned int nSourceLen, const std::string& strDecryptKey, npp::MemoryDataStream& dataDecrypted );
        static bool decrypt( const unsigned char* szSource, const unsigned int nSourceLen, const ::IDEA_KEY_SCHEDULE& ideaDecryptKey, npp::MemoryDataStream& dataDecrypted );


        typedef struct {
            /*{{{*/
            u_char* data;   /** The memory data */
            size_t data_len;/** The length of data */
            /*}}}*/
        } mem_data_t;

        //for test, don't use it
        //! param[out] mem_data_t* out_data - The decrypted/encrypted data is stored here
        //!      The memory MUST BE allocated before you call this.
        static void encrypt( const unsigned char* in_buf, const unsigned int in_buf_len, const IDEA_KEY_SCHEDULE* key, mem_data_t* out_data );
        static void decrypt( const unsigned char* in_buf, const unsigned int in_buf_len, const IDEA_KEY_SCHEDULE* key, mem_data_t* out_data );
    };
} // end of namespace ext

#endif // end of #if H_PROVIDE_IDEA_ENCRYPT



#endif //end of #ifndef Q_APP_UTILITY_ENCRYPT_UTILITY_H_




