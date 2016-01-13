#define AJ_MODULE JAVA

#include "alljoyn.h"
#include "aj_link_timeout.h"
#include "alljoyn_java.h"
#include <ish.h>
#include <kni.h>
#include <stdio.h>
#include <stdlib.h>

uint8_t dbgJAVA = 0;

void jctomb(char* buf, jstring jstr) {
	jsize len = KNI_GetStringLength(jstr);
	jchar *jcbuf = malloc(sizeof(jchar)*len);
	KNI_GetStringRegion(jstr, 0, len, jcbuf);
	int i, j = 0;
	for (i = 0; i < len; i++) {
		buf[j++] = (jcbuf[i] & 0x00FF);
		if (jcbuf[i] > 0x00FF) {
			buf[j++] = (jcbuf[i] & 0xFF00) >> 8;
		}
	}
	free(jcbuf);
	buf[j++] = 0;
}

uint32_t msglen(AJ_Message* msg) {
    return sizeof(AJ_MsgHeader) + ((msg->hdr->headerLen + 7) & 0xFFFFFFF8) + msg->hdr->bodyLen;
}

//--------- KNI METHODS ---------//

KNIEXPORT KNI_RETURNTYPE_VOID Java_AllJoyn_initialize() {
	AJ_Initialize();
	KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_startService() {
	KNI_StartHandles(6);
    KNI_DeclareHandle(bus);
    KNI_DeclareHandle(busclass);
    KNI_DeclareHandle(daemonName);
    KNI_DeclareHandle(name);
    KNI_DeclareHandle(opts);
    KNI_DeclareHandle(optsclass);
	KNI_GetParameterAsObject(1, bus);
	KNI_GetObjectClass(bus, busclass);
	KNI_GetParameterAsObject(2, daemonName);
	jint timeout = KNI_GetParameterAsInt(3);
	jboolean connected = KNI_GetParameterAsBoolean(4);
	jshort port = KNI_GetParameterAsShort(5);
	KNI_GetParameterAsObject(6, name);
	jint flags = KNI_GetParameterAsInt(7);
	KNI_GetParameterAsObject(8, opts);
	KNI_GetObjectClass(opts, optsclass);
	jint ptr = KNI_GetIntField(bus, KNI_GetFieldID(busclass, "ptr", "I"));
	if (ptr == 0) {
		AJ_BusAttachment* nativebus = malloc(sizeof(AJ_BusAttachment));
		ptr = (jint)nativebus;
		KNI_SetIntField(bus, KNI_GetFieldID(busclass, "ptr", "I"), ptr);
	}
//	jint len = KNI_GetStringLength(daemonName);
//	char *dnm = malloc(sizeof(char)*(len+1));
//	jctomb(dnm, daemonName);
	jint len2 = KNI_GetStringLength(name);
	char *nm = malloc(sizeof(char)*(len2+1));
	jctomb(nm, name);
	jint res = AJ_StartService((AJ_BusAttachment*)ptr, NULL, timeout, connected, port, nm, flags, NULL);
//	free(dnm);
	free(nm);
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_busAdvertiseName() {
	KNI_StartHandles(3);
    KNI_DeclareHandle(bus);
    KNI_DeclareHandle(busclass);
    KNI_DeclareHandle(name);
	KNI_GetParameterAsObject(1, bus);
	KNI_GetObjectClass(bus, busclass);
	KNI_GetParameterAsObject(2, name);
	jint transport = KNI_GetParameterAsInt(3);
	jint op = KNI_GetParameterAsInt(4);
	jint flags = KNI_GetParameterAsInt(5);
	jint ptr = KNI_GetIntField(bus, KNI_GetFieldID(busclass, "ptr", "I"));
	if (ptr == 0) {
		AJ_BusAttachment* nativebus = malloc(sizeof(AJ_BusAttachment));
		ptr = (jint)nativebus;
		KNI_SetIntField(bus, KNI_GetFieldID(busclass, "ptr", "I"), ptr);
	}
	jint len2 = KNI_GetStringLength(name);
	char *nm = malloc(sizeof(char)*(len2+1));
	jctomb(nm, name);
	jint res = AJ_BusAdvertiseName((AJ_BusAttachment*)ptr, nm, transport, op, flags);
	free(nm);
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_VOID Java_AllJoyn_disconnect() {
	KNI_StartHandles(2);
    KNI_DeclareHandle(bus);
    KNI_DeclareHandle(busclass);
	KNI_GetParameterAsObject(1, bus);
	KNI_GetObjectClass(bus, busclass);
	jint ptr = KNI_GetIntField(bus, KNI_GetFieldID(busclass, "ptr", "I"));
	AJ_Disconnect((AJ_BusAttachment*)ptr);
	KNI_EndHandles();
	KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_OBJECT Java_AllJoyn_getUniqueName() {
	KNI_StartHandles(3);
    KNI_DeclareHandle(bus);
    KNI_DeclareHandle(busclass);
    KNI_DeclareHandle(res);
	KNI_GetParameterAsObject(1, bus);
	KNI_GetObjectClass(bus, busclass);
	jint ptr = KNI_GetIntField(bus, KNI_GetFieldID(busclass, "ptr", "I"));
	const char *name = AJ_GetUniqueName((AJ_BusAttachment*)ptr);
	KNI_NewStringUTF(name, res);
	KNI_EndHandlesAndReturnObject(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_busLinkStateProc() {
	KNI_StartHandles(2);
    KNI_DeclareHandle(bus);
    KNI_DeclareHandle(busclass);
	KNI_GetParameterAsObject(1, bus);
	KNI_GetObjectClass(bus, busclass);
	jint ptr = KNI_GetIntField(bus, KNI_GetFieldID(busclass, "ptr", "I"));
	KNI_EndHandles();
	KNI_ReturnInt(AJ_BusLinkStateProc((AJ_BusAttachment*)ptr));
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_setBusLinkTimeout() {
	KNI_StartHandles(2);
    KNI_DeclareHandle(bus);
    KNI_DeclareHandle(busclass);
	KNI_GetParameterAsObject(1, bus);
	KNI_GetObjectClass(bus, busclass);
	jint ptr = KNI_GetIntField(bus, KNI_GetFieldID(busclass, "ptr", "I"));
	jint secs = KNI_GetParameterAsInt(2);
	KNI_EndHandles();
	KNI_ReturnInt(AJ_SetBusLinkTimeout((AJ_BusAttachment*)ptr, secs));
}

KNIEXPORT KNI_RETURNTYPE_VOID Java_AllJoyn_notifyLinkActive() {
	AJ_NotifyLinkActive();
	KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_deliverMsg() {
	KNI_StartHandles(2);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
	KNI_GetParameterAsObject(1, msg);
	KNI_GetObjectClass(msg, msgclass);
	jint ptr = KNI_GetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"));
	jboolean collect = KNI_GetBooleanField(msg, KNI_GetFieldID(msgclass, "collect", "B"));
	jint res = ptr ? AJ_DeliverMsg((AJ_Message*)ptr) : AJ_ERR_MARSHAL;
	if (collect == KNI_TRUE) {
		AJ_Message *nativemsg = (AJ_Message*)ptr;
		free((void*)nativemsg->destination);
		free(nativemsg);
		KNI_SetBooleanField(msg, KNI_GetFieldID(msgclass, "collect", "B"), KNI_FALSE);
	}
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_closeMsg() {
	KNI_StartHandles(2);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
	KNI_GetParameterAsObject(1, msg);
	KNI_GetObjectClass(msg, msgclass);
	jint ptr = KNI_GetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"));
	jboolean collect = KNI_GetBooleanField(msg, KNI_GetFieldID(msgclass, "collect", "B"));
	jint res = ptr ? AJ_CloseMsg((AJ_Message*)ptr) : AJ_OK;
	if (collect == KNI_TRUE) {
		AJ_Message *nativemsg = (AJ_Message*)ptr;
		free((void*)nativemsg->destination);
		free(nativemsg);
		KNI_SetBooleanField(msg, KNI_GetFieldID(msgclass, "collect", "B"), KNI_FALSE);
	}
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_busHandleBusMessage() {
	KNI_StartHandles(2);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
	KNI_GetParameterAsObject(1, msg);
	KNI_GetObjectClass(msg, msgclass);
	jint ptr = KNI_GetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"));
	KNI_EndHandles();
	KNI_ReturnInt(AJ_BusHandleBusMessage((AJ_Message*)ptr));
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_busReplyAcceptSession() {
	KNI_StartHandles(2);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
	KNI_GetParameterAsObject(1, msg);
	KNI_GetObjectClass(msg, msgclass);
	jint ptr = KNI_GetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"));
	jboolean accept = KNI_GetParameterAsBoolean(2);
	KNI_EndHandles();
	KNI_ReturnInt(AJ_BusReplyAcceptSession((AJ_Message*)ptr, accept));
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_marshalReplyMsg() {
	KNI_StartHandles(4);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
    KNI_DeclareHandle(reply);
    KNI_DeclareHandle(destination);
	KNI_GetParameterAsObject(1, msg);
	KNI_GetObjectClass(msg, msgclass);
	KNI_GetParameterAsObject(2, reply);
	jint ptr = KNI_GetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"));
	AJ_Message* nativereply = malloc(sizeof(AJ_Message));
	jint res = AJ_MarshalReplyMsg((AJ_Message *)ptr, nativereply);
	KNI_NewStringUTF(nativereply->destination, destination);
	KNI_SetIntField(reply, KNI_GetFieldID(msgclass, "ptr", "I"), (jint)nativereply);
	KNI_SetIntField(reply, KNI_GetFieldID(msgclass, "msgType", "I"), nativereply->hdr->msgType);
	KNI_SetIntField(reply, KNI_GetFieldID(msgclass, "msgId", "I"), nativereply->msgId);
	KNI_SetObjectField(reply, KNI_GetFieldID(msgclass, "destination", "Ljava/lang/String;"), destination);
	KNI_SetIntField(reply, KNI_GetFieldID(msgclass, "sessionId", "I"), nativereply->sessionId);
	KNI_SetIntField(reply, KNI_GetFieldID(msgclass, "totalLen", "I"), msglen(nativereply));
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_marshalStatusMsg() {
	KNI_StartHandles(4);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
    KNI_DeclareHandle(reply);
    KNI_DeclareHandle(destination);
	KNI_GetParameterAsObject(1, msg);
	KNI_GetObjectClass(msg, msgclass);
	KNI_GetParameterAsObject(2, reply);
	jint status = KNI_GetParameterAsInt(3);
	jint ptr = KNI_GetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"));
	AJ_Message* nativereply = malloc(sizeof(AJ_Message));
	jint res = AJ_MarshalStatusMsg((AJ_Message *)ptr, nativereply, status);
	KNI_NewStringUTF(nativereply->destination, destination);
	KNI_SetIntField(reply, KNI_GetFieldID(msgclass, "ptr", "I"), (jint)nativereply);
	KNI_SetIntField(reply, KNI_GetFieldID(msgclass, "msgType", "I"), nativereply->hdr->msgType);
	KNI_SetIntField(reply, KNI_GetFieldID(msgclass, "msgId", "I"), nativereply->msgId);
	KNI_SetObjectField(reply, KNI_GetFieldID(msgclass, "destination", "Ljava/lang/String;"), destination);
	KNI_SetIntField(reply, KNI_GetFieldID(msgclass, "sessionId", "I"), nativereply->sessionId);
	KNI_SetIntField(reply, KNI_GetFieldID(msgclass, "totalLen", "I"), msglen(nativereply));
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_deliverRaw() {
	KNI_StartHandles(3);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
    KNI_DeclareHandle(buf);
	KNI_GetParameterAsObject(1, msg);
	KNI_GetParameterAsObject(2, buf);
	jsize buflen = KNI_GetArrayLength(buf);
	char* data = malloc(sizeof(*data)*(buflen+1));
	int i;
	for (i = 0; i < buflen; i++) {
		data[i] = KNI_GetByteArrayElement(buf, i);
	}
	data[buflen] = 0;
	KNI_GetObjectClass(msg, msgclass);
	jint len = KNI_GetParameterAsInt(3);
	jint ptr = KNI_GetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"));
	jint sta = AJ_DeliverMsgPartial((AJ_Message *)ptr, len);
	jint res = AJ_MarshalRaw((AJ_Message *)ptr, data, len);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "totalLen", "I"), msglen((AJ_Message *)ptr));
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_marshalSignal() {
	KNI_StartHandles(5);
    KNI_DeclareHandle(bus);
    KNI_DeclareHandle(busclass);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
    KNI_DeclareHandle(destination);
	KNI_GetParameterAsObject(1, bus);
	KNI_GetObjectClass(bus, busclass);
	KNI_GetParameterAsObject(2, msg);
	KNI_GetObjectClass(msg, msgclass);
	jint msgId = KNI_GetParameterAsInt(3);
	KNI_GetParameterAsObject(4, destination);
	jint sessionId = KNI_GetParameterAsInt(5);
	jint flags = KNI_GetParameterAsInt(6);
	jint ttl = KNI_GetParameterAsInt(7);
	jint len = KNI_GetStringLength(destination);
	char *dest = malloc(sizeof(char)*(len+1));
	jctomb(dest, destination);
	jint ptr = KNI_GetIntField(bus, KNI_GetFieldID(busclass, "ptr", "I"));
	AJ_Message* nativemsg = malloc(sizeof(AJ_Message));
	jint res = AJ_MarshalSignal((AJ_BusAttachment *)ptr, nativemsg, msgId, dest, sessionId, flags, ttl);
	KNI_NewStringUTF(nativemsg->destination, destination);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"), (jint)nativemsg);
	KNI_SetBooleanField(msg, KNI_GetFieldID(msgclass, "collect", "B"), KNI_TRUE);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "msgType", "I"), nativemsg->hdr->msgType);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "msgId", "I"), nativemsg->msgId);
	KNI_SetObjectField(msg, KNI_GetFieldID(msgclass, "destination", "Ljava/lang/String;"), destination);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "sessionId", "I"), nativemsg->sessionId);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "totalLen", "I"), msglen(nativemsg));
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_unmarshalMsg() {
	KNI_StartHandles(5);
    KNI_DeclareHandle(bus);
    KNI_DeclareHandle(busclass);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
    KNI_DeclareHandle(destination);
	KNI_GetParameterAsObject(1, bus);
	KNI_GetObjectClass(bus, busclass);
	KNI_GetParameterAsObject(2, msg);
	KNI_GetObjectClass(msg, msgclass);
	jint timeout = KNI_GetParameterAsInt(3);
	jint ptr = KNI_GetIntField(bus, KNI_GetFieldID(busclass, "ptr", "I"));
	AJ_Message* nativemsg = malloc(sizeof(AJ_Message));
	jint res = AJ_UnmarshalMsg((AJ_BusAttachment *)ptr, nativemsg, timeout);
	KNI_NewStringUTF(nativemsg->destination, destination);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"), (jint)nativemsg);
	KNI_SetBooleanField(msg, KNI_GetFieldID(msgclass, "collect", "B"), KNI_TRUE);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "msgType", "I"), nativemsg->hdr->msgType);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "msgId", "I"), nativemsg->msgId);
	KNI_SetObjectField(msg, KNI_GetFieldID(msgclass, "destination", "Ljava/lang/String;"), destination);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "sessionId", "I"), nativemsg->sessionId);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "bodyBytes", "I"), nativemsg->bodyBytes);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "totalLen", "I"), msglen(nativemsg));
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_unmarshalPropertyArgs() {
	KNI_StartHandles(7);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
    KNI_DeclareHandle(propid);
    KNI_DeclareHandle(propidclass);
    KNI_DeclareHandle(sig);
    KNI_DeclareHandle(sigclass);
    KNI_DeclareHandle(sigvalue);
	KNI_GetParameterAsObject(1, msg);
	KNI_GetObjectClass(msg, msgclass);
	KNI_GetParameterAsObject(2, propid);
	KNI_GetObjectClass(propid, propidclass);
	KNI_GetParameterAsObject(3, sig);
	KNI_GetObjectClass(sig, sigclass);
	jint ptr = KNI_GetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"));
    uint32_t propidval;
    const char* sigval;
	jint res = AJ_UnmarshalPropertyArgs((AJ_Message *)ptr, &propidval, &sigval);
	KNI_NewStringUTF(sigval, sigvalue);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "bodyBytes", "I"), ((AJ_Message *)ptr)->bodyBytes);
	KNI_SetIntField(propid, KNI_GetFieldID(propidclass, "value", "I"), propidval);
	KNI_SetObjectField(sig, KNI_GetFieldID(sigclass, "value", "Ljava/lang/String;"), sigvalue);
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_unmarshalVariant() {
	KNI_StartHandles(5);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
    KNI_DeclareHandle(variant);
    KNI_DeclareHandle(variantclass);
    KNI_DeclareHandle(variantvalue);
	KNI_GetParameterAsObject(1, msg);
	KNI_GetObjectClass(msg, msgclass);
	KNI_GetParameterAsObject(2, variant);
	KNI_GetObjectClass(variant, variantclass);
	jint ptr = KNI_GetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"));
    const char* variantval;
	jint res = AJ_UnmarshalVariant((AJ_Message *)ptr, &variantval);
	KNI_NewStringUTF(variantval, variantvalue);
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "bodyBytes", "I"), ((AJ_Message *)ptr)->bodyBytes);
	KNI_SetObjectField(variant, KNI_GetFieldID(variantclass, "value", "Ljava/lang/String;"), variantvalue);
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_INT Java_AllJoyn_unmarshalRaw() {
	KNI_StartHandles(5);
    KNI_DeclareHandle(msg);
    KNI_DeclareHandle(msgclass);
    KNI_DeclareHandle(buf);
    KNI_DeclareHandle(len);
    KNI_DeclareHandle(lenclass);
	KNI_GetParameterAsObject(1, msg);
	KNI_GetParameterAsObject(2, buf);
	KNI_GetParameterAsObject(3, len);
	KNI_GetObjectClass(msg, msgclass);
	KNI_GetObjectClass(len, lenclass);
	jint ptr = KNI_GetIntField(msg, KNI_GetFieldID(msgclass, "ptr", "I"));
	jsize buflen = KNI_GetArrayLength(buf);
	size_t actual = 0;
	char* data = malloc(sizeof(*data)*(buflen+1));
	jint res = AJ_UnmarshalRaw((AJ_Message *)ptr, (const void**)&data, buflen, &actual);
	int i;
	for (i = 0; i < actual; i++) {
		KNI_SetByteArrayElement(buf, i, data[i]);
	}
	KNI_SetIntField(msg, KNI_GetFieldID(msgclass, "bodyBytes", "I"), ((AJ_Message *)ptr)->bodyBytes);
	KNI_SetIntField(len, KNI_GetFieldID(lenclass, "value", "I"), actual);
	KNI_EndHandles();
	KNI_ReturnInt(res);
}

KNIEXPORT KNI_RETURNTYPE_VOID Java_AllJoyn_delay() {
	jint msec = KNI_GetParameterAsInt(1);
	AJ_Sleep(msec);
	KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID Java_AllJoyn_print() {
	jint level = KNI_GetParameterAsInt(1);
	KNI_StartHandles(1);
    KNI_DeclareHandle(s);
	KNI_GetParameterAsObject(2, s);
	jsize len = KNI_GetStringLength(s);
	char *buf = malloc(sizeof(char)*(len+1));
	jctomb(buf, s);
	switch (level) {
		case 1: // WARN
			AJ_WarnPrintf((buf));
			break;
		case 2: // ERROR
			AJ_ErrPrintf((buf));
			break;
		case 3: // INFO
			AJ_InfoPrintf((buf));
			break;
		case 5: // ALWAYS
			AJ_AlwaysPrintf((buf));
			break;
	}
	free(buf);
	KNI_EndHandles();
	KNI_ReturnVoid();
}

AJ_Object* create_busobjects(int index) {
	KNI_StartHandles(7);
	KNI_DeclareHandle(busobjects);
	KNI_DeclareHandle(busobj);
	KNI_DeclareHandle(busobjclass);
	KNI_DeclareHandle(path);
	KNI_DeclareHandle(interfaces);
	KNI_DeclareHandle(strs);
	KNI_DeclareHandle(str);
	KNI_GetParameterAsObject(index, busobjects);

	jsize len = KNI_GetArrayLength(busobjects);
	AJ_Object* objs = malloc(sizeof(*objs)*(len+1));
	objs[len].path = NULL;

	int i, j, k;
	for (i = 0; i < len; i++) {
		KNI_GetObjectArrayElement(busobjects, i, busobj);
		KNI_GetObjectClass(busobj, busobjclass);
		KNI_GetObjectField(busobj, KNI_GetFieldID(busobjclass, "path", "Ljava/lang/String;"), path);

		int pathlen = KNI_GetStringLength(path);

		char *nativepath = malloc(sizeof(char)*(pathlen+1));
		jctomb(nativepath, path);

		jint flags = KNI_GetIntField(busobj, KNI_GetFieldID(busobjclass, "flags", "I"));

		KNI_GetObjectField(busobj, KNI_GetFieldID(busobjclass, "interfaces", "[[Ljava/lang/String;"), interfaces);

		jsize interfaceslen = KNI_GetArrayLength(interfaces);

		char ***nativeinterfaces = malloc(sizeof(char**)*(interfaceslen+1));
		nativeinterfaces[interfaceslen] = NULL;

		for (j = 0; j < interfaceslen; j++) {
			KNI_GetObjectArrayElement(interfaces, j, strs);
			jsize strslen = KNI_GetArrayLength(strs);

			char **nativestrs = malloc(sizeof(char*)*(strslen+1));
			nativestrs[strslen] = NULL;

			for (k = 0; k < strslen; k++) {
				KNI_GetObjectArrayElement(strs, k, str);
				int strlen = KNI_GetStringLength(str);

				char *nativestr = malloc(sizeof(char)*(strlen+1));
				jctomb(nativestr, str);
				nativestrs[k] = nativestr;
			}

			nativeinterfaces[j] = nativestrs;
		}

		objs[i].path = nativepath;
		objs[i].interfaces = nativeinterfaces;
		objs[i].flags = flags;
	}
	KNI_EndHandles();
	return objs;
}

void free_busobjects(AJ_Object* objs) {
	int j, k;
	AJ_Object* o = objs;
	while (o && o->path) {
		free((void*)o->path);
		j = 0;
		while(o->interfaces[j]) {
			k = 0;
			while(o->interfaces[j][k]) {
				free((void*)objs->interfaces[j][k]);
				k++;
			}
			free((void*)o->interfaces[j]);
			j++;
		}
		free((void*)o->interfaces);
		o++;
	}
	free(objs);
}

KNIEXPORT KNI_RETURNTYPE_VOID Java_AllJoyn_printXml() {
	AJ_Object *objs = create_busobjects(1);

	AJ_PrintXML(objs);
	free_busobjects(objs);
	
	KNI_ReturnVoid();
}

KNIEXPORT KNI_RETURNTYPE_VOID Java_AllJoyn_registerObjects() {
	AJ_Object *objs1 = create_busobjects(1);
	AJ_Object *objs2 = NULL;

	AJ_RegisterObjects(objs1, objs2);
	
	KNI_ReturnVoid();
}

NativeImplementationType Java_AllJoyn_natives[] = {
	{"initialize",   	      			(NativeFunctionPtr)Java_AllJoyn_initialize}, 
    {"startService",         			(NativeFunctionPtr)Java_AllJoyn_startService}, 
    {"busAdvertiseName",         		(NativeFunctionPtr)Java_AllJoyn_busAdvertiseName}, 
    {"disconnect",         				(NativeFunctionPtr)Java_AllJoyn_disconnect}, 
    {"getUniqueName",         			(NativeFunctionPtr)Java_AllJoyn_getUniqueName}, 
    {"busLinkStateProc",         		(NativeFunctionPtr)Java_AllJoyn_busLinkStateProc}, 
    {"setBusLinkTimeout",         		(NativeFunctionPtr)Java_AllJoyn_setBusLinkTimeout}, 
    {"notifyLinkActive",         		(NativeFunctionPtr)Java_AllJoyn_notifyLinkActive}, 
    {"marshalReplyMsg",         		(NativeFunctionPtr)Java_AllJoyn_marshalReplyMsg},
    {"marshalStatusMsg",				(NativeFunctionPtr)Java_AllJoyn_marshalStatusMsg},
    {"deliverMsg",         				(NativeFunctionPtr)Java_AllJoyn_deliverMsg}, 
    {"closeMsg",         				(NativeFunctionPtr)Java_AllJoyn_closeMsg}, 
    {"busHandleBusMessage",         	(NativeFunctionPtr)Java_AllJoyn_busHandleBusMessage}, 
    {"busReplyAcceptSession",         	(NativeFunctionPtr)Java_AllJoyn_busReplyAcceptSession}, 
    {"deliverRaw",  	       			(NativeFunctionPtr)Java_AllJoyn_deliverRaw},
    {"marshalSignal",         			(NativeFunctionPtr)Java_AllJoyn_marshalSignal}, 
    {"unmarshalRaw",         			(NativeFunctionPtr)Java_AllJoyn_unmarshalRaw}, 
    {"unmarshalMsg",         			(NativeFunctionPtr)Java_AllJoyn_unmarshalMsg},
    {"unmarshalPropertyArgs",			(NativeFunctionPtr)Java_AllJoyn_unmarshalPropertyArgs},
    {"unmarshalVariant",				(NativeFunctionPtr)Java_AllJoyn_unmarshalVariant},
    {"delay",         					(NativeFunctionPtr)Java_AllJoyn_delay}, 
    {"print",         					(NativeFunctionPtr)Java_AllJoyn_print}, 
    {"printXml",         				(NativeFunctionPtr)Java_AllJoyn_printXml}, 
    {"registerObjects",         		(NativeFunctionPtr)Java_AllJoyn_registerObjects},
    NATIVE_END_OF_LIST
};

ClassNativeImplementationType nativeImpl[] = {
    {"org/alljoyn/bus/AllJoyn", 		Java_AllJoyn_natives},
	NATIVE_END_OF_LIST
};

void AJ_Java(const char* className) {
	KNI_RegisterNatives(nativeImpl);
	char* argv[2] = { "ajtcl", "-l" };
	char command[128];
	sprintf(command, "java -w -classpath b: %s\0", className);
	ish(2, argv, "java");
	ish_execute(command);
}
