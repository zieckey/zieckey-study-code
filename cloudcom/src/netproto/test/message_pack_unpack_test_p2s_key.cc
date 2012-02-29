
#include "netproto/include/test_common.h"

#include "netproto/include/npp_config.h"
#include "netproto/include/message_packer.h"
#include "netproto/include/message_unpacker.h"
#include "netproto/include/idea_wrapper.h"

#include "test_rsa_self_pair_key.h"
#include "test_client_rsa_key.h"
#include "test_server_rsa_key.h"
#include "idea_key.h"

#include <iostream>

namespace npp { namespace ext {

    //! Name: auto_delete
    template<class T>
    struct auto_delete
    {
        T*& ptr_ref_to_be_deleted_;
        auto_delete( T*& pointer )
            : ptr_ref_to_be_deleted_( pointer )
        {
        }

        ~auto_delete()
        {
            if ( ptr_ref_to_be_deleted_ )
            {
                delete ptr_ref_to_be_deleted_;
                ptr_ref_to_be_deleted_ = 0;
            }
        }

        void noop() {}
    private:
        auto_delete(const auto_delete&);
        auto_delete&operator=(const auto_delete&);
    };

    template<class T>
    struct auto_delete<T*>; //! \note Leave it be. Do not write any implementation
}
}

namespace npp { namespace ext {
    template<> inline
        auto_delete< npp::NppConfig >::~auto_delete()
    {
        if ( ptr_ref_to_be_deleted_ )
        {
            delete ptr_ref_to_be_deleted_;
            ptr_ref_to_be_deleted_ = NULL;
        }
    }
}
}

namespace
{
    npp::NppConfig* CreateNppConfig(bool support_plain, bool sign_pack, bool verify_sign)
    {
        npp::NppConfig* npp_config = new npp::NppConfig(support_plain, sign_pack, verify_sign);
#define H_ADD_KEY(no) {\
    npp_config->AddIdeaKey(no, idea_key##no);\
    npp_config->AddSimpleRSAKey(no, g_slrsa_private_key##no, g_slrsa_private_key##no##_len, g_slrsa_public_key##no, g_slrsa_public_key##no##_len);\
    npp_config->AddOpenSSLRSAKey(no, g_rsa_private_key##no, g_rsa_private_key##no##_len, g_rsa_public_key##no, g_rsa_public_key##no##_len);\
        }

        H_ADD_KEY(1);
        H_ADD_KEY(2);
        H_ADD_KEY(3);
        H_ADD_KEY(4);

#undef  H_ADD_KEY

        return npp_config;
    }

    npp::NppConfig* CreateServerNppConfig(bool support_plain, bool sign_pack, bool verify_sign)
    {
        npp::NppConfig* npp_config = new npp::NppConfig(support_plain, sign_pack, verify_sign);
#define H_ADD_KEY(no) {\
    npp_config->AddIdeaKey(no, idea_key##no);\
    npp_config->AddSimpleRSAKey(no, g_server_slrsa_private_key##no, g_server_slrsa_private_key##no##_len, g_server_slrsa_public_key##no, g_server_slrsa_public_key##no##_len);\
    npp_config->AddOpenSSLRSAKey(no, g_server_rsa_private_key##no, g_server_rsa_private_key##no##_len, g_server_rsa_public_key##no, g_server_rsa_public_key##no##_len);\
        }

        H_ADD_KEY(1);
        H_ADD_KEY(2);
        H_ADD_KEY(3);
        H_ADD_KEY(4);

#undef  H_ADD_KEY

        return npp_config;
    }

    npp::NppConfig* CreateClientNppConfig(bool support_plain, bool sign_pack, bool verify_sign)
    {
        npp::NppConfig* npp_config = new npp::NppConfig(support_plain, sign_pack, verify_sign);
#define H_ADD_KEY(no) {\
    npp_config->AddIdeaKey(no, idea_key##no);\
    npp_config->AddSimpleRSAKey(no, g_client_slrsa_private_key##no, g_client_slrsa_private_key##no##_len, g_client_slrsa_public_key##no, g_client_slrsa_public_key##no##_len);\
    npp_config->AddOpenSSLRSAKey(no, g_client_rsa_private_key##no, g_client_rsa_private_key##no##_len, g_client_rsa_public_key##no, g_client_rsa_public_key##no##_len);\
        }

        H_ADD_KEY(1);
        H_ADD_KEY(2);
        H_ADD_KEY(3);
        H_ADD_KEY(4);

#undef  H_ADD_KEY

        return npp_config;
    }
}


TEST_UNIT(test_func_MessagePackUnitTest_p2sp_key_test)
{
    /** RSA private_key */
    /*{{{*/
    //use to sign output packet sign
    const u_char private_key1[1024] = {48,130,1,58,2,1,0,2,65,0,150,174,244,171,118,131,0,41,228,219,169,104,69,56,73,121,97,219,246,66,161,196,101,134,19,47,56,169,24,249,117,173,45,183,218,17,60,15,6,245,0,174,99,86,245,106,251,223,80,238,1,158,146,57,55,1,176,76,115,244,158,155,174,107,2,3,1,0,1,2,64,83,208,193,155,214,114,11,215,65,203,197,177,144,63,60,239,93,181,162,135,113,191,242,208,223,62,246,255,104,58,234,111,45,105,81,190,85,229,126,92,242,73,238,179,2,54,124,149,162,208,123,15,61,51,13,84,246,45,181,53,199,115,202,81,2,33,0,198,207,162,62,146,96,187,246,210,100,105,38,230,39,48,234,151,251,165,234,64,38,242,166,179,32,52,170,156,105,24,111,2,33,0,194,7,56,181,205,123,164,253,164,124,129,46,191,61,72,200,253,128,1,0,203,89,70,121,228,64,139,243,106,120,175,197,2,32,4,197,149,10,85,111,22,243,220,166,230,83,53,176,249,30,152,89,68,233,51,114,251,221,254,108,166,118,245,220,27,231,2,33,0,145,29,3,141,253,67,199,79,40,81,49,240,187,120,227,81,231,62,112,1,159,20,131,0,48,245,17,118,153,31,240,29,2,32,49,195,118,188,169,28,162,148,46,172,77,85,171,127,99,81,155,190,224,52,68,52,187,13,129,35,110,118,234,135,236,219};
    const size_t private_key1_len = 318;

    const u_char private_key2[1024] = {48,130,2,93,2,1,0,2,129,129,0,186,40,28,61,169,207,140,229,184,135,92,221,191,111,122,192,65,79,157,74,40,150,197,245,157,122,186,218,18,57,200,120,15,140,121,212,227,159,40,75,240,201,53,236,157,27,224,56,12,199,21,215,71,181,228,123,229,137,95,226,174,201,250,152,23,124,19,244,203,238,203,237,61,115,231,154,21,133,187,250,155,94,59,55,184,57,31,138,152,63,49,65,128,255,16,183,13,30,135,14,193,21,58,164,67,36,234,126,235,199,180,114,113,92,212,119,226,53,102,119,198,15,220,219,227,3,69,93,2,3,1,0,1,2,129,128,30,226,114,181,4,76,75,234,252,239,112,71,213,239,239,228,159,177,78,190,51,247,183,198,37,39,108,130,171,141,108,166,6,166,94,188,36,59,142,154,242,202,223,40,111,116,101,129,199,20,236,153,183,235,4,138,77,173,185,251,200,213,247,190,37,142,36,176,27,249,10,215,125,188,55,190,172,165,35,206,44,175,144,220,2,138,169,110,64,19,113,241,93,8,151,247,219,28,157,4,154,27,138,101,7,64,190,229,4,206,250,26,252,185,165,234,248,149,223,13,43,116,39,167,117,199,238,189,2,65,0,220,248,93,216,176,32,167,155,52,111,171,235,204,131,252,34,130,164,46,151,89,223,122,214,196,163,189,168,156,201,233,49,93,127,120,159,227,8,121,100,251,45,160,31,67,5,92,237,215,171,179,214,114,96,50,233,56,0,199,73,199,102,181,107,2,65,0,215,170,232,70,253,194,65,152,234,15,242,111,171,199,215,188,5,24,193,153,148,10,28,73,142,221,66,22,104,117,92,64,94,115,193,63,186,108,109,154,128,68,46,255,52,66,193,132,52,63,58,47,120,37,211,150,152,99,40,202,202,252,90,87,2,65,0,196,6,197,98,68,165,67,107,9,114,131,198,180,112,230,13,75,110,17,51,13,116,151,9,205,250,32,119,99,88,47,186,46,98,182,4,201,40,172,96,171,255,70,204,84,237,126,10,53,72,202,74,84,191,8,139,34,182,118,20,250,131,32,47,2,65,0,188,240,158,179,235,6,111,63,142,247,79,150,163,176,221,128,124,26,151,41,123,247,107,150,201,209,111,95,128,62,41,6,191,120,187,185,160,17,243,231,162,45,153,218,188,167,43,48,46,49,162,65,10,10,97,151,18,131,244,173,205,45,77,219,2,64,111,194,245,128,231,116,168,62,184,99,16,14,36,107,164,54,216,39,84,32,101,63,23,191,22,88,243,175,148,112,60,36,35,245,175,218,16,72,109,209,189,69,86,40,104,218,104,144,219,98,156,192,149,130,132,39,247,108,68,109,230,164,93,243};
    const size_t private_key2_len = 609;


    //use to verify input packet sign
    const u_char public_key1[1024] = {48,72,2,65,0,150,174,244,171,118,131,0,41,228,219,169,104,69,56,73,121,97,219,246,66,161,196,101,134,19,47,56,169,24,249,117,173,45,183,218,17,60,15,6,245,0,174,99,86,245,106,251,223,80,238,1,158,146,57,55,1,176,76,115,244,158,155,174,107,2,3,1,0,1};
    const size_t public_key1_len = 74;

    const u_char public_key2[1024] = {48,129,137,2,129,129,0,186,40,28,61,169,207,140,229,184,135,92,221,191,111,122,192,65,79,157,74,40,150,197,245,157,122,186,218,18,57,200,120,15,140,121,212,227,159,40,75,240,201,53,236,157,27,224,56,12,199,21,215,71,181,228,123,229,137,95,226,174,201,250,152,23,124,19,244,203,238,203,237,61,115,231,154,21,133,187,250,155,94,59,55,184,57,31,138,152,63,49,65,128,255,16,183,13,30,135,14,193,21,58,164,67,36,234,126,235,199,180,114,113,92,212,119,226,53,102,119,198,15,220,219,227,3,69,93,2,3,1,0,1};
    const size_t public_key2_len = 140;

    const unsigned char g_rsa_prvFileKey1[706] = {
        0x00,0x04,0xc4,0x44,0x11,0x7f,0xd4,0x53,0x1b,0xc2,0x92,0x7e,0x4d,0xb2,0x8a,0x3a,0x9f,0xdc,0xde,0x7a,0x98,0x29,0x68,0xc5,0x24,0x7f,0x43,0x69,0x48,0x70,0x5e,0x9a,
        0x9d,0xc3,0x00,0x66,0x86,0xee,0xc7,0x52,0xd4,0xea,0xdc,0x1b,0xd5,0xaf,0xd5,0x49,0x22,0x3d,0x8a,0x9b,0x8c,0xd1,0xda,0x22,0x6b,0xb5,0x7d,0x17,0xcc,0x12,0x84,0xd2,
        0x45,0x8c,0x63,0x86,0xae,0x5b,0x23,0xd1,0x52,0xb0,0xe1,0x83,0x6e,0x2b,0xb3,0xa0,0xc1,0xdd,0xba,0xf3,0xb6,0x27,0x8f,0x49,0x8d,0xc1,0x2f,0x13,0x5f,0xb7,0x04,0x93,
        0x76,0x3a,0x6b,0xf0,0x2b,0x5b,0x0d,0x20,0x67,0x05,0x06,0x5e,0x27,0x40,0x02,0xbe,0x63,0xbe,0x4a,0x19,0xf3,0xd4,0x75,0xec,0xd4,0xeb,0xc7,0xca,0x30,0xcb,0xaf,0x00,
        0xda,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
        0x00,0x01,0x55,0x8c,0xf2,0x2f,0xcd,0x46,0x78,0xc8,0x44,0x80,0xe7,0xaf,0x56,0x8f,0x9d,0x55,0x72,0x68,0xef,0xe0,0xc4,0xf9,0x82,0x22,0x33,0x29,0x36,0x59,0x3c,0xf3,
        0x9a,0xa7,0x00,0x8a,0xb0,0x70,0x6e,0xd4,0x0d,0x59,0x5e,0x9e,0x87,0x73,0xc0,0xbd,0xc3,0x3e,0x1e,0x28,0x15,0xa6,0xf7,0xb5,0x9f,0x4b,0x99,0xfe,0x80,0x86,0xe5,0xfc,
        0x9e,0x31,0x83,0x3d,0x64,0x8a,0x04,0x02,0x68,0xaa,0xee,0x44,0x15,0xba,0x5d,0x6e,0x63,0x37,0x94,0x3e,0x5b,0x1e,0x45,0x17,0x4f,0xf8,0x39,0xb1,0x51,0xbe,0x74,0xe6,
        0x28,0xa0,0x22,0xff,0x44,0xcc,0xe0,0xb8,0x89,0x73,0xfe,0x5d,0x49,0xad,0x24,0xd5,0xee,0xd1,0x45,0xc9,0x73,0xb7,0xab,0x6f,0x76,0xea,0x12,0xb8,0x8f,0x6a,0xeb,0x6a,
        0xcc,0x0d,0xe7,0xeb,0xfe,0x1b,0x61,0x62,0x95,0x1c,0x41,0x0c,0xa5,0x4f,0x99,0x4e,0x13,0x64,0x4d,0xf4,0xc4,0x30,0xfb,0x73,0xae,0xfe,0x08,0xb0,0x93,0x0b,0x8d,0xb4,
        0xcf,0xaa,0xc1,0xda,0xed,0x1b,0xe1,0x5e,0xe2,0xa0,0xcd,0xc8,0xee,0x74,0x14,0xe4,0x6c,0xf7,0x99,0x3f,0x63,0x95,0x75,0xdd,0x62,0x98,0xd3,0xb9,0x17,0x37,0x88,0xcc,
        0xb2,0xf3,0xd8,0xa4,0x69,0xdf,0x22,0x05,0x18,0xfe,0x1e,0xf1,0xe5,0x6f,0x01,0x65,0x05,0x0a,0xa8,0xab,0x9c,0x26,0xfc,0x2a,0xec,0x12,0xf8,0xd9,0x26,0x0d,0x7c,0xb9,
        0x97,0x30,0x94,0x16,0x8d,0xa6,0xfe,0x2e,0xfc,0xe8,0x90,0x80,0x24,0xf1,0x79,0x05,0x67,0x26,0x6d,0xcc,0x54,0x08,0x8e,0xf5,0x1b,0x1d,0xae,0x62,0xe5,0xf2,0xa6,0x08,
        0x11,0xbb,0x30,0x59,0xbb,0xd5,0x3b,0x7d,0x50,0xf6,0xdd,0xfc,0xe3,0xed,0xef,0x59,0x1e,0xa3,0xd7,0xb8,0xc2,0xba,0x2f,0x82,0x13,0xa6,0x74,0x6b,0x53,0x8c,0xa2,0x13,
        0x63,0x04,0x07,0x7c,0x3b,0xd8,0x87,0x28,0xbf,0xf8,0xa4,0x4e,0x52,0xa0,0x32,0xc6,0xd8,0x8c,0xda,0x0f,0x24,0x14,0xc1,0xc8,0xd0,0xc8,0x69,0xc3,0x79,0xba,0xa3,0x33,
        0x81,0xcf,0x29,0x8d,0x4f,0x69,0x26,0xc3,0x47,0x89,0xe6,0x31,0x8b,0x4a,0x01,0x40,0x78,0xf9,0x14,0x64,0xe5,0xec,0x6c,0x4d,0x67,0x3e,0x34,0x50,0xa2,0x05,0xd0,0x38,
        0x60,0x91,0x46,0xb6,0x14,0xb1,0x44,0xeb,0x8a,0x42,0xa9,0x04,0x8b,0x11,0x6d,0x9e,0xe5,0xab,0xef,0x3f,0x53,0x96,0xd8,0xa4,0x96,0x70,0xd8,0xf3,0xcb,0x1b,0x01,0x51,
        0x66,0xe5,0xca,0xc3,0x77,0x3a,0x86,0x5a,0x25,0x2e,0x31,0xf7,0x85,0xbc,0x46,0x12,0x07,0x15,0xaf,0x39,0x1a,0x87,0x31,0x91,0x57,0x74,0xde,0x10,0x6e,0xbf,0xa4,0x25,
        0xd9,0x14,0x33,0xce,0x34,0x8b,0xe3,0xdf,0x57,0x78,0xbd,0xed,0x64,0x78,0x9d,0x29,0x63,0xdb,0xb5,0x15,0x83,0x42,0x2d,0x6e,0xfc,0xcc,0xf8,0x50,0x06,0x17,0x48,0xae,
        0xd3,0x38
    };

    const unsigned char g_rsa_pubFileKey1[258] = {
        0x00,0x04,0xc4,0x44,0x11,0x7f,0xd4,0x53,0x1b,0xc2,0x92,0x7e,0x4d,0xb2,0x8a,0x3a,0x9f,0xdc,0xde,0x7a,0x98,0x29,0x68,0xc5,0x24,0x7f,0x43,0x69,0x48,0x70,0x5e,0x9a,
        0x9d,0xc3,0x00,0x66,0x86,0xee,0xc7,0x52,0xd4,0xea,0xdc,0x1b,0xd5,0xaf,0xd5,0x49,0x22,0x3d,0x8a,0x9b,0x8c,0xd1,0xda,0x22,0x6b,0xb5,0x7d,0x17,0xcc,0x12,0x84,0xd2,
        0x45,0x8c,0x63,0x86,0xae,0x5b,0x23,0xd1,0x52,0xb0,0xe1,0x83,0x6e,0x2b,0xb3,0xa0,0xc1,0xdd,0xba,0xf3,0xb6,0x27,0x8f,0x49,0x8d,0xc1,0x2f,0x13,0x5f,0xb7,0x04,0x93,
        0x76,0x3a,0x6b,0xf0,0x2b,0x5b,0x0d,0x20,0x67,0x05,0x06,0x5e,0x27,0x40,0x02,0xbe,0x63,0xbe,0x4a,0x19,0xf3,0xd4,0x75,0xec,0xd4,0xeb,0xc7,0xca,0x30,0xcb,0xaf,0x00,
        0xda,0x81,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
        0x00,0x01

    };


    const unsigned char g_rsa_prvFileKey2[706] = {
        0x00,0x04,0xb0,0xe9,0xa4,0x51,0x8a,0xdc,0x0c,0x06,0x9c,0x8d,0xbc,0x22,0x9f,0x95,0x4d,0xdc,0xd3,0x3d,0x58,0xa5,0x4a,0x1a,0x68,0x27,0x90,0x5f,0x77,0xfa,0xc8,0x80,
        0x5e,0x3b,0xa9,0xd5,0x45,0x5c,0x47,0x62,0xb7,0x36,0x15,0x56,0xfc,0xf8,0x8a,0xc8,0x65,0x7b,0x1a,0xd6,0x25,0xbd,0x31,0x58,0xd9,0x76,0x60,0xd4,0xc5,0xce,0x5b,0x26,
        0x34,0xfa,0xc8,0x9c,0x0e,0x28,0x52,0x03,0xab,0x18,0x91,0x1f,0x95,0x10,0x59,0xec,0xed,0xa7,0x20,0x25,0x13,0xa9,0x9d,0xe9,0xdd,0xc6,0x26,0x17,0xfc,0x87,0x43,0x2d,
        0xae,0x86,0xfc,0x99,0xe0,0xf9,0x06,0x47,0x5e,0x05,0x7e,0x44,0x3a,0xe3,0xb5,0x37,0xec,0xac,0x4f,0xe5,0xe2,0x8d,0xc1,0x5c,0x7c,0xbd,0x07,0x91,0x9d,0xb6,0x6a,0x81,
        0xfc,0xe1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
        0x00,0x01,0x4c,0x7c,0x56,0xec,0x62,0x2a,0x8c,0x90,0x77,0x32,0xd7,0xf9,0xe8,0x20,0xb6,0xcf,0x18,0x0c,0xd7,0xbc,0x6e,0x73,0x48,0x40,0x82,0xac,0xa3,0xc4,0xaa,0xd6,
        0xc4,0xe6,0xea,0xa0,0x28,0x22,0xa3,0x42,0x4c,0x8f,0x77,0x5e,0x6a,0x74,0x52,0x1c,0x1d,0xb3,0xc1,0x80,0x80,0x44,0x71,0x87,0x46,0x4d,0x65,0xc3,0xd7,0xd9,0x45,0x93,
        0x19,0x27,0xf3,0x48,0x10,0x5f,0xf0,0xa4,0x5f,0xb8,0x29,0x4f,0x94,0xf4,0x5b,0x99,0xdf,0x44,0x1a,0x41,0xe3,0x2d,0x03,0x98,0x20,0x5b,0x9d,0x00,0xb1,0x24,0x00,0x28,
        0x8b,0xff,0x8a,0x21,0x49,0x9b,0x76,0x38,0x25,0x28,0xeb,0x2a,0xed,0x40,0xcb,0x74,0xe6,0xf0,0x46,0xa1,0xfc,0x98,0xbf,0x5f,0xf0,0x9a,0xda,0x56,0x73,0x86,0x3f,0xb1,
        0xbb,0xa1,0xde,0xf5,0xa2,0x79,0x32,0x14,0x62,0x30,0xf0,0xaa,0x4b,0x4e,0x1f,0x0f,0xf2,0xf5,0x16,0x03,0x97,0x01,0xc6,0xbf,0x8d,0x84,0x88,0x3d,0x17,0x04,0x47,0x2f,
        0x46,0x8d,0x01,0x31,0x21,0xe1,0xc1,0xc9,0x6c,0x8e,0x05,0x80,0x70,0x7b,0x25,0xad,0xe1,0x4c,0xbe,0xf5,0xbb,0x75,0x47,0xd5,0x62,0x59,0xee,0x91,0x85,0x8c,0x3d,0x89,
        0x1a,0x6d,0xcb,0x21,0x23,0x62,0xa4,0x83,0x3d,0x71,0x87,0x6a,0x4c,0x8f,0x64,0xbf,0x6d,0x3e,0x11,0xcf,0x8b,0x3a,0xe8,0x16,0x33,0xab,0xf2,0x7e,0x31,0xff,0x14,0x18,
        0x2c,0x3c,0x5a,0x45,0x21,0xed,0x8b,0x8c,0x22,0x48,0x5c,0x1f,0xff,0xb7,0xca,0xbd,0x65,0x71,0x42,0xbf,0x02,0x43,0x10,0xfc,0x53,0x0c,0x47,0xb3,0x30,0x63,0xf8,0x9a,
        0xe3,0xc5,0x4e,0xc5,0x55,0xbb,0x9f,0xd7,0xd7,0xc1,0x16,0xa5,0xf3,0xa9,0x0c,0x0c,0x64,0x77,0xf6,0x69,0x1a,0x46,0xc4,0x63,0x14,0xaa,0x88,0x3f,0x9d,0x0c,0x23,0xe2,
        0x2d,0xf8,0xcf,0x3c,0xfe,0x8d,0x76,0x1b,0xa5,0xc0,0x9f,0x6c,0xfb,0x50,0x66,0xc5,0x81,0xdd,0xcd,0x71,0x9f,0xe3,0x15,0x97,0xec,0x01,0x7d,0xe9,0xe7,0xd6,0xde,0x3e,
        0xbc,0x19,0x7e,0x85,0x21,0x79,0x2b,0x66,0xe3,0x3a,0x77,0x51,0xea,0x65,0xd6,0x26,0xb5,0x70,0x57,0xbf,0xa1,0xc5,0x0b,0x11,0xe9,0x15,0x4c,0x70,0xf5,0xcb,0x1f,0x44,
        0xff,0x28,0x88,0x41,0x4f,0x24,0x9a,0x2a,0xaa,0x9f,0xcf,0x61,0xce,0xf1,0x37,0x42,0x48,0x91,0x07,0x35,0xd9,0xbe,0xd4,0x31,0x95,0xb8,0xac,0x60,0x1b,0xdf,0xe8,0xd4,
        0xf0,0xd1,0xa2,0x51,0xe4,0xb7,0x8a,0x47,0x2e,0x58,0xaa,0x16,0x83,0xc3,0x49,0xf7,0xdb,0xa9,0x8b,0x91,0x5d,0xa7,0x65,0x01,0x0e,0xde,0xf9,0x20,0x58,0x81,0x04,0x71,
        0x88,0x2c,0xcd,0xdf,0x98,0xd0,0x79,0x37,0x15,0x1c,0x0b,0x87,0x62,0xa2,0x63,0x7b,0x81,0xab,0x49,0xaf,0x47,0x45,0x12,0xa8,0x54,0x87,0x88,0x80,0xe8,0x07,0x40,0x70,
        0x17,0x64
    };

    const unsigned char g_rsa_pubFileKey2[258] = {
        0x00,0x04,0xb0,0xe9,0xa4,0x51,0x8a,0xdc,0x0c,0x06,0x9c,0x8d,0xbc,0x22,0x9f,0x95,0x4d,0xdc,0xd3,0x3d,0x58,0xa5,0x4a,0x1a,0x68,0x27,0x90,0x5f,0x77,0xfa,0xc8,0x80,
        0x5e,0x3b,0xa9,0xd5,0x45,0x5c,0x47,0x62,0xb7,0x36,0x15,0x56,0xfc,0xf8,0x8a,0xc8,0x65,0x7b,0x1a,0xd6,0x25,0xbd,0x31,0x58,0xd9,0x76,0x60,0xd4,0xc5,0xce,0x5b,0x26,
        0x34,0xfa,0xc8,0x9c,0x0e,0x28,0x52,0x03,0xab,0x18,0x91,0x1f,0x95,0x10,0x59,0xec,0xed,0xa7,0x20,0x25,0x13,0xa9,0x9d,0xe9,0xdd,0xc6,0x26,0x17,0xfc,0x87,0x43,0x2d,
        0xae,0x86,0xfc,0x99,0xe0,0xf9,0x06,0x47,0x5e,0x05,0x7e,0x44,0x3a,0xe3,0xb5,0x37,0xec,0xac,0x4f,0xe5,0xe2,0x8d,0xc1,0x5c,0x7c,0xbd,0x07,0x91,0x9d,0xb6,0x6a,0x81,
        0xfc,0xe1,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
        0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,
        0x00,0x01
    };
    /*}}}*/



    bool support_plain = false;
    bool sign_pack     = true;
    bool verify_sign   = true;
    npp::NppConfig* npp_config = new npp::NppConfig(support_plain, sign_pack, verify_sign);
    npp::ext::auto_delete<npp::NppConfig> npp_config_auto_deleted(npp_config);

    npp_config->AddOpenSSLRSAKey(1, private_key1, private_key1_len, public_key1, public_key1_len);
    npp_config->AddOpenSSLRSAKey(2, private_key2, private_key2_len, public_key2, public_key2_len);

    npp_config->AddSimpleRSAKey(1, g_rsa_prvFileKey1, 706, g_rsa_pubFileKey1, 258);
    npp_config->AddSimpleRSAKey(2, g_rsa_prvFileKey2, 706, g_rsa_pubFileKey2, 258);


    for (int i = 1; i <= 2; ++i)
    {

        //OpenSSL RSA
        {
            const char * raw_data = "0047880d4a1cf095fa4b13f9cc9f06f8";
            size_t raw_data_len = strlen(raw_data);
            npp::OpenSSLRSA* rsa = npp_config->GetOpenSSLRSA(i);
            H_TEST_ASSERT(rsa);
            std::string sigret;
            H_TEST_ASSERT(rsa->sign(npp::OpenSSLRSA::ST_NID_sha1, raw_data, raw_data_len, sigret));
            H_TEST_ASSERT(rsa->verify(npp::OpenSSLRSA::ST_NID_sha1, raw_data, raw_data_len, sigret.data(), sigret.size()));
        }

        //Simple RSA
        {
            const char * raw_data = "0047880d4a1cf095fa4b13f9cc9f06f8";
            size_t raw_data_len = strlen(raw_data);
            npp::SimpleRSA* rsa = npp_config->GetSimpleRSA(i);
            H_TEST_ASSERT(rsa);
            std::string sigret;
            H_TEST_ASSERT(rsa->sign(raw_data, raw_data_len, sigret));
            H_TEST_ASSERT(rsa->verify(raw_data, raw_data_len, sigret.data(), sigret.size()));
        }
    }
}








// 
// virtual const unsigned char* GetIDEAKey(const int keyno) = 0;
// virtual bool GetOpenSSLRSAKey(const int keyno, std::string& private_key, std::string& public_key) = 0;
// virtual bool GetSimpleRSAKey(const int keyno, std::string& private_key, std::string& public_key) = 0;
// #define H_IF_IDEA_KEY_RETURN(no) {if (no == keyno) return idea_key##no;}
// #define H_IF_OPENSSL_RSA_KEY_RETURN(no) {\
//         if (no == keyno) {\
//             private_key = std::string((char*)g_server_rsa_private_key##no, sizeof(g_server_rsa_private_key##no)); \
//             public_key = std::string((char*)g_server_rsa_public_key##no, sizeof(g_server_rsa_public_key##no));\
//             return true;\
//         }\
//     }
// #define H_IF_SIMPLE_RSA_KEY_RETURN(no) {\
//         if (no == keyno) {\
//             private_key = std::string((char*)g_server_slrsa_private_key##no, sizeof(g_server_slrsa_private_key##no)); \
//             public_key = std::string((char*)g_server_slrsa_public_key##no, sizeof(g_server_slrsa_public_key##no));\
//             return true;\
//         }\
//     }
// 
// if (1 == keyno)
// {
//     key = std::string((char*)g_server_rsa_private_key1, sizeof(g_server_rsa_private_key1));
//     return true;
// }
// const unsigned char* GetIDEAKey(const int keyno)
// {
//     H_IF_IDEA_KEY_RETURN(1);
//     H_IF_IDEA_KEY_RETURN(2);
//     H_IF_IDEA_KEY_RETURN(3);
//     H_IF_IDEA_KEY_RETURN(4);
//     return NULL;
// }
// 
// bool GetOpenSSLRSAKey(const int keyno, std::string& private_key, std::string& pulic_key)
// {
//     if (1 == keyno)
//     {
//         private_key = std::string((char*)g_server_rsa_private_key1, sizeof(g_server_rsa_private_key1));
//         pulic_key = std::string((char*)g_server_rsa_public_key1, sizeof(g_server_rsa_public_key1));
//         return true;
//     }
//     
//     if (2 == keyno)
//     {
//         private_key = std::string((char*)g_server_rsa_private_key2, sizeof(g_server_rsa_private_key2));
//         pulic_key = std::string((char*)g_server_rsa_public_key2, sizeof(g_server_rsa_public_key2));
//         return true;
//     }
// 
//     fprintf(stderr, "%s keyno=%u error\n", __func__, keyno);
// 
//     return false;
// }
// 
// bool GetRSAPublicKey(const int keyno, std::string& key)
// {
//     if (1 == keyno)
//     {
//         key = std::string((char*)g_server_rsa_public_key1, sizeof(g_server_rsa_public_key1));
//         return true;
//     }
//     else if (2 == keyno)
//     {
//         key = std::string((char*)g_server_rsa_public_key2, sizeof(g_server_rsa_public_key2));
//         return true;
//     }
// 
//     fprintf(stderr, "%s keyno=%u error\n", __func__, keyno);
// 
//     return false;
// }
// 
// 
// bool GetSLRSAPrivateKey(const int keyno, std::string& key)
// {
//     if (1 == keyno)
//     {
//         key = std::string((char*)g_server_slrsa_private_key1, sizeof(g_server_slrsa_private_key1));
//         return true;
//     }
//     else if (2 == keyno)
//     {
//         key = std::string((char*)g_server_slrsa_private_key2, sizeof(g_server_slrsa_private_key2));
//         return true;
//     }
// 
//     fprintf(stderr, "%s keyno=%u error\n", __func__, keyno);
// 
//     return false;
// }
// 
// bool GetSLRSAPublicKey(const int keyno, std::string& key)
// {
//     if (1 == keyno)
//     {
//         key = std::string((char*)g_server_slrsa_public_key1, sizeof(g_server_slrsa_public_key1));
//         return true;
//     }
//     else if (2 == keyno)
//     {
//         key = std::string((char*)g_server_slrsa_public_key2, sizeof(g_server_slrsa_public_key2));
//         return true;
//     }
// 
//     fprintf(stderr, "%s keyno=%u error\n", __func__, keyno);
// 
//     return false;
// }