#define AJ_MODULE LIGHTING

#include <aj_debug.h>
#include <aj_link_timeout.h>
#include <aj_peer.h>
#include <aj_target.h>
#include <alljoyn.h>

#include "tests.h"

uint8_t dbgLIGHTING = 0;

static uint8_t lightingState = FALSE;

static const char ServiceName[] = "org.alljoyn.lighting";
static const uint16_t ServicePort = 24000;
static const uint8_t CancelAdvertiseName = FALSE;
static const uint8_t ReflectSignal = FALSE;

static const char *const lightingInterface[] = {
    "org.alljoyn.lighting", "@Lighting=b",         "?TurnLightingOn",
    "?TurnLightingOff",     "!LightingChanged >b", NULL};

static const AJ_InterfaceDescription lightingInterfaces[] = {
    AJ_PropertiesIface, lightingInterface, NULL};

static AJ_Object AppObjects[] = {
    {"/org/alljoyn/lighting", lightingInterfaces, AJ_OBJ_FLAG_ANNOUNCED},
    {NULL}};

#define APP_GET_PROP AJ_APP_MESSAGE_ID(0, 0, AJ_PROP_GET)
#define APP_SET_PROP AJ_APP_MESSAGE_ID(0, 0, AJ_PROP_SET)
#define APP_LIGHT AJ_APP_PROPERTY_ID(0, 1, 0)
#define APP_LIGHT_ON AJ_APP_MESSAGE_ID(0, 1, 1)
#define APP_LIGHT_OFF AJ_APP_MESSAGE_ID(0, 1, 2)
#define APP_LIGHT_SIGNAL AJ_APP_MESSAGE_ID(0, 1, 3)

static AJ_Status AppLightOn(AJ_Message *msg) {
  lightingState = TRUE;
  AJ_AlwaysPrintf(("TurnLightingOn\n"));
  AJ_Message reply;
  AJ_MarshalReplyMsg(msg, &reply);
  return AJ_DeliverMsg(&reply);
}

static AJ_Status AppLightOff(AJ_Message *msg) {
  lightingState = FALSE;
  AJ_AlwaysPrintf(("TurnLightingOff\n"));
  AJ_Message reply;
  AJ_MarshalReplyMsg(msg, &reply);
  return AJ_DeliverMsg(&reply);
}

//! Handles a property GET request so marshals the property value to return
static AJ_Status PropGetHandler(AJ_Message *replyMsg, uint32_t propId,
                                void *context) {
  if (propId == APP_LIGHT) {
    AJ_AlwaysPrintf(("GetLighting\n"));
    return AJ_MarshalArgs(replyMsg, "b", lightingState);
  } else {
    return AJ_ERR_UNEXPECTED;
  }
}

//! Handles a property SET request so unmarshals the property value to apply.
static AJ_Status PropSetHandler(AJ_Message *replyMsg, uint32_t propId,
                                void *context) {
  if (propId == APP_LIGHT) {
    AJ_AlwaysPrintf(("SetLighting\n"));
    return AJ_UnmarshalArgs(replyMsg, "b", &lightingState);
  } else {
    return AJ_ERR_UNEXPECTED;
  }
}

#define CONNECT_TIMEOUT (1000 * 1000)
#define UNMARSHAL_TIMEOUT (1000 * 5)

int lighting() {
  AJ_Status status = AJ_OK;
  AJ_BusAttachment bus;
  uint8_t connected = FALSE;
  uint32_t sessionId = 0;

  //    AJ_PrintXML(AppObjects);
  AJ_RegisterObjects(AppObjects, NULL);

  while (TRUE) {
    AJ_Message msg;

    if (!connected) {
      status = AJ_StartService(&bus, NULL, CONNECT_TIMEOUT, FALSE, ServicePort,
                               ServiceName, AJ_NAME_REQ_DO_NOT_QUEUE, NULL);
      if (status != AJ_OK) {
        continue;
      }
      AJ_InfoPrintf(("StartService returned AJ_OK\n"));
      AJ_InfoPrintf(("Connected to Daemon:%s\n", AJ_GetUniqueName(&bus)));

      connected = TRUE;

      // Configure timeout for the link to the daemon bus
      AJ_SetBusLinkTimeout(&bus, 60); // 60 seconds
    }

    status = AJ_UnmarshalMsg(&bus, &msg, UNMARSHAL_TIMEOUT);
    if (AJ_ERR_TIMEOUT == status &&
        AJ_ERR_LINK_TIMEOUT == AJ_BusLinkStateProc(&bus)) {
      status = AJ_ERR_READ;
    }
    if (status != AJ_OK) {
      if (status == AJ_ERR_TIMEOUT) {
        continue;
      }
    }

    if (status == AJ_OK) {
      switch (msg.msgId) {

      case AJ_REPLY_ID(AJ_METHOD_ADD_MATCH):
        if (msg.hdr->msgType == AJ_MSG_ERROR) {
          AJ_InfoPrintf(("Failed to add match\n"));
          status = AJ_ERR_FAILURE;
        } else {
          status = AJ_OK;
        }
        break;

      case AJ_METHOD_ACCEPT_SESSION: {
        uint16_t port;
        char *joiner;
        AJ_UnmarshalArgs(&msg, "qus", &port, &sessionId, &joiner);
        if (port == ServicePort) {
          status = AJ_BusReplyAcceptSession(&msg, TRUE);
          AJ_InfoPrintf(("Accepted session session_id=%u joiner=%s\n",
                         sessionId, joiner));
        } else {
          status = AJ_BusReplyAcceptSession(&msg, FALSE);
          AJ_InfoPrintf(("Accepted rejected session_id=%u joiner=%s\n",
                         sessionId, joiner));
        }
      } break;

      case AJ_SIGNAL_SESSION_LOST_WITH_REASON: {
        uint32_t id, reason;
        AJ_UnmarshalArgs(&msg, "uu", &id, &reason);
        AJ_InfoPrintf(("Session lost. ID = %u, reason = %u", id, reason));
        if (CancelAdvertiseName) {
          status = AJ_BusAdvertiseName(&bus, ServiceName, AJ_TRANSPORT_ANY,
                                       AJ_BUS_START_ADVERTISING, 0);
        }
        status = AJ_ERR_SESSION_LOST;
      } break;

      case AJ_SIGNAL_SESSION_JOINED:
        if (CancelAdvertiseName) {
          status = AJ_BusAdvertiseName(&bus, ServiceName, AJ_TRANSPORT_ANY,
                                       AJ_BUS_STOP_ADVERTISING, 0);
        }
        break;

      case AJ_REPLY_ID(AJ_METHOD_CANCEL_ADVERTISE):
      case AJ_REPLY_ID(AJ_METHOD_ADVERTISE_NAME):
        if (msg.hdr->msgType == AJ_MSG_ERROR) {
          status = AJ_ERR_FAILURE;
        }
        break;

      case APP_GET_PROP:
        status = AJ_BusPropGet(&msg, PropGetHandler, NULL);
        break;

      case APP_SET_PROP:
        status = AJ_BusPropSet(&msg, PropSetHandler, NULL);
        if (status == AJ_OK) {
          AJ_InfoPrintf(("Property successfully set.\n"));
        } else {
          AJ_InfoPrintf((
              "Property set attempt unsuccessful. Status = 0x%04x.\n", status));
        }
        break;

      case APP_LIGHT_ON:
        status = AppLightOn(&msg);
        break;

      case APP_LIGHT_OFF:
        status = AppLightOff(&msg);
        break;

      case APP_LIGHT_SIGNAL:
        AJ_InfoPrintf(("Received light_signal\n"));
        status = AJ_OK;

        if (ReflectSignal) {
          AJ_Message out;
          AJ_MarshalSignal(&bus, &out, APP_LIGHT_SIGNAL, msg.destination,
                           msg.sessionId, 0, 0);
          AJ_MarshalArgs(&out, "b", 0, NULL);
          AJ_DeliverMsg(&out);
          AJ_CloseMsg(&out);
        }
        break;

      default:
        // Pass to the built-in bus message handlers
        status = AJ_BusHandleBusMessage(&msg);
        break;
      }

      // Any received packets indicates the link is active, so call to reinforce
      // the bus link state
      AJ_NotifyLinkActive();
    }

    // Unmarshalled messages must be closed to free resources
    AJ_CloseMsg(&msg);

    if ((status == AJ_ERR_READ) || (status == AJ_ERR_LINK_DEAD)) {
      AJ_InfoPrintf(("AllJoyn disconnect\n"));
      AJ_InfoPrintf(("Disconnected from Daemon:%s\n", AJ_GetUniqueName(&bus)));
      AJ_Disconnect(&bus);
      connected = FALSE;

      // Sleep a little while before trying to reconnect
      AJ_Sleep(10 * 1000);
    }
  }
  AJ_WarnPrintf(("lighting EXIT %d\n", status));

  return status;
}
