#include "aj_target.h"
#include "aj_util.h"
#include "tests.h"
#include "alljoyn_java.h"
#include "esp8266.h"
#include "esputil.h"
#include <socket.h>
  
void AJ_Main() {
//	Following tests run fast.					 	 
//	aestest();
//	base64();	
//	mutter();

//	Following tests run slowly.
//	aesbench();
//	certificate();

//	Following tests require real system.
//	nvramdump();
//	nvrampersistencetest();
//	nvramtest();

//	ajlite();
//	bastress2();
//	clientlite();
//	codisco();
//	sessions1();
//	sessionslite();
//	siglite();
//	sigtest();
//	svclite();
	lighting();

	AJ_Java("App");		 
}
