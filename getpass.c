//**************************************************************************************************
// getpass.c - Universal Password Retrieval Utility 2.0 - 2010-07-02
//
// This program retrieves a user's Universal Password via an ldap extended operation.
//
// Usage: getpass (ldap server) (admin DN) (admin Pass) (target DN)
//
// "Wrapper" (this program) developed by Timothy Patterson (tim@pc-professionals.net)
//
// Uses edir_ldapext.c and a few lines of code that are property of the FreeRADIUS
// project and Novell, Inc.
//
// Feel free to modify and/or redistribute this code, however make sure that credit
// is given where credit is due...  Make sure to include the FreeRADIUS project and
// Novell, Inc.!
//
// To Compile, download and install the Novell, Inc. LDAP Libraries for C SDK, then
// cc -g -I../include -DUNIX    -c -o getpass.o getpass.c
// cc -o getpass getpass.o -L../lib -lldapsdk -lpthread -lresolv -ldl -L../lib -lldapssl -lcrypto
// Assuming that the relative paths point to the respective folders in the Novell SDK
//
//**************************************************************************************************

// Include Files...
#include	<sys/types.h>
#include	<sys/socket.h>
#include	<sys/time.h>
#include	<netinet/in.h>

#include	<stdio.h>
#include	<stdlib.h>
#include	<netdb.h>
#include	<pwd.h>
#include	<time.h>
#include	<ctype.h>
#include	<string.h>

#include	<lber.h>
#include        <ldap.h>
#include 	<ldap_ssl.h>

#include	<errno.h>
#include	<unistd.h>
#include	<pthread.h>

#include "edir_ldapext.c" // Again, many thanks to Novell and FreeRADIUS!

#define MY_PORT 636  /* #unsecure 389 */

#define UNIVERSAL_PASS_LEN 256

#define L_ERR                 4
#define L_CONS                128

// Function Definition Required to Use edir_ldapext.c - Taken from the FreeRADIUS project...
void *rad_malloc(size_t size);

int main( int argc, char **argv )
{
	char *ldapServer = argv[1];
	char *adminDN = argv[2];
	char *adminPass = argv[3];
	char *userDN = argv[4];

	if(ldapServer == NULL || adminDN == NULL || adminPass == NULL || userDN == NULL)
	{
		printf("Usage: getpass (ldap server) (admin DN) (admin Pass) (target DN)\n\n");
		exit(1);
	}

	int rc, version;
	LDAP *ld;

	/* Initializing SSL */
	rc = ldapssl_client_init(NULL,   /* Certificate File */
                         					NULL); /* reserved, just use NULL */

	if (LDAP_SUCCESS != rc)
	{
		printf("ldapssl_client_init error: %d\n", rc);
		exit(1);
	}

    rc = ldapssl_set_verify_mode(LDAPSSL_VERIFY_NONE);
    if (LDAP_SUCCESS != rc)
    {
    	printf("ldapssl_set_verify_mode error: %d\n", rc);
      ldapssl_client_deinit();
      exit(1);
    }

    /* Create an SSL enabled LDAP handle */
	ld = ldapssl_init( ldapServer,       /* host name */
										MY_PORT,       /* port number */
                       			1);             /* 0- clear text, 1-enable for ssl */

	if (NULL == ld)
   {
   	printf("ldapssl_init error\n");
      ldapssl_client_deinit();
      exit(1);
	}

	/* Set the LDAP protocol version supported by the client
	to 3. (By default, this is set to 2. Extended operations
	are part of version 3 of the LDAP protocol.) */

	ldap_get_option( ld, LDAP_OPT_PROTOCOL_VERSION, &version );
	version = LDAP_VERSION3;
	ldap_set_option( ld, LDAP_OPT_PROTOCOL_VERSION, &version );

	/* Authenticate to the directory as an Admin user */
	if ( ldap_simple_bind_s( ld, adminDN, adminPass ) != LDAP_SUCCESS )
	{
		ldap_perror( ld, "ldap_simple_bind_s" );
		ldap_unbind( ld );

		return( 1 );
	}

	/* Get Universal Password */
	char *universal_password = NULL;
	/* int universal_password_len = UNIVERSAL_PASS_LEN; */
	size_t universal_password_len = 256;
	char *passwd_val = NULL;
	int res = 0;

	universal_password = rad_malloc(universal_password_len);
	memset(universal_password, 0, universal_password_len);

	res = nmasldap_get_password(ld, userDN, &universal_password_len, universal_password);

	passwd_val = universal_password;
	printf("%s\n", passwd_val);

	/* Disconnect from the server. */
	ldap_unbind( ld );

	return 0;
} 

// This function was taken from the FreeRADIUS project...  Give all credit to them.
// I simply commented out one line...
void *rad_malloc(size_t size)
{
	void *ptr = malloc(size);

	if (ptr == NULL)
	{
   	 //radlog(L_ERR|L_CONS, "no memory");
      exit(1);
	}

	return ptr;
}

