/*
	SLRSA.H - header file for slrsa.C

	Copyright (c) J.S.A.Kapp 1994 - 1995.

	RSAEURO - RSA Library compatible with RSAREF(tm) 2.0.

	All functions prototypes are the Same as for RSAREF(tm).
	To aid compatiblity the source and the files follow the
	same naming comventions that RSAREF(tm) uses.  This should aid
	direct importing to your applications.

	This library is legal everywhere outside the US.  And should
	NOT be imported to the US and used there.

	All Trademarks Acknowledged.

	RSA Routines Header File.

	Revision 1.00 - JSAK 23/6/95, Final Release Version
*/

#ifndef _SL_RSA_H_
#define _SL_RSA_H_

#ifdef __cplusplus
extern "C" {
#endif

    /**
     *  slrsa_sign() signs the message digest m of size m_len using the private
     *  key generated by 360KeyMgr. It stores the signature in sigret
     *  and the signature size in siglen. sigret must point to at least 128
     *  bytes of memory.
     *
     *  slrsa_verify() verifies that the signature sigbuf of size siglen matches
     *  a given message digest m of size m_len. publickey is the signer's
     *  public key.
     *
     *  lsrsa_sign() returns 1 on success, 0 otherwise.  
     *  slrsa_verify() returns 1 on successful verification, 0 otherwise.
     */
    int slrsa_sign( const unsigned char* m, 
                    const unsigned int   m_len,
                    const unsigned char* privatekey,
                    unsigned char*       sigret, 
                    unsigned int*        siglen );

    int slrsa_verify( const unsigned char* m, 
                      const unsigned int   m_len,
                      const unsigned char* publickey,
                      const unsigned char* sigbuf, 
                      const unsigned int   siglen );

#ifdef __cplusplus
}
#endif



#endif
