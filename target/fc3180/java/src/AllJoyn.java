package org.alljoyn.bus;

import org.alljoyn.bus.BusObject;
import org.alljoyn.bus.Message;
import org.alljoyn.bus.PropHandler;
import org.alljoyn.bus.Result;
import org.alljoyn.bus.SessionOpts;
import org.alljoyn.bus.messaging.Packer;
import org.alljoyn.bus.messaging.Unpacker;
import org.alljoyn.bus.types.Container;
import org.alljoyn.bus.types.immutable.*;
import java.lang.String;

//! Interop layer
public class AllJoyn {
  private AllJoyn() {
    preload();
  }

  private void preload() {
    Container container = Container.create()
      .addBool(false)
      .addUInt8(0)
      .addUInt16((short)0)
      .addUInt32(0)
      .addUInt64(0)
      .addInt16((short)0)
      .addInt32(0)
      .addInt64(0)
      .addObjectPath("")
      .addSignature("")
      .addWString("")
      .addVariant(Bool.create(true))
      .add(Bool.create(false), Bool.create(false));
    Array array = container.toArray();
    Struct struct = container.toStruct();
    try {
      Unpacker.unpack("b", Packer.pack(Bool.create(false), 0, true), 0, true);
    } catch (Throwable t) {
      // ignore
    }
  }

  public static final AllJoyn instance = new AllJoyn();

  public native void initialize();

  public native int startService(BusAttachment bus, String daemonName, int timeout, boolean connected, short port, String name, int flags, SessionOpts opts);
  public native int busAdvertiseName(BusAttachment bus, String name, int transportMask, int op, int flags);
  public native void disconnect(BusAttachment bus);
  public native String getUniqueName(BusAttachment bus);
  public native int busLinkStateProc(BusAttachment bus);
  public native int setBusLinkTimeout(BusAttachment bus, int secs);
  public native void notifyLinkActive();

  public native int deliverMsg(Message msg);
  public native int closeMsg(Message msg);
  public native int busHandleBusMessage(Message msg);
  public native int busReplyAcceptSession(Message msg, boolean accept);

  private int propAccess(Message msg, PropHandler handler, Object ctx, int op) {
    Message reply = new Message();
    IntRef propId = new IntRef();
    StrRef sig = new StrRef();

    print(INFO, "PropAccess(msg=" + msg.ptr + ", op=" + (op == PROP_GET ? "get" : "set") + ")\n");

    // Find out which property is being accessed and whether the access is a GET or SET
    int status = unmarshalPropertyArgs(msg, propId, sig);

    if (status == OK) {
      marshalReplyMsg(msg, reply);

      // Callback to let the application marshal or unmarshal the value
      if (op == PROP_GET) { 
        status = handler.handleGet(reply, propId.value, ctx);
      } else {
        ImcRef ref = new ImcRef();
        status = unmarshal(msg, "v", ref);
        if (status == OK) {
          Immutable imm = ref.value.get(0).asVariant().value;
          // Check that the value has the expected signature
          if (sig.value.equals(imm.sig())) {
            status = handler.handleSet(imm, propId.value, ctx);
          } else {
            print(INFO, "PropAccess(): AJ_ERR_SIGNATURE\n");
            status = ERR_SIGNATURE;
          }
        }
      }
    }
    if (status != OK) {
      marshalStatusMsg(msg, reply, status);
    }
    return deliverMsg(reply);
  }

  public int busPropGet(Message msg, PropHandler handler, Object ctx) {
    print(INFO, "AJ_BusPropGet(msg=" + msg.ptr + ")\n");
    return propAccess(msg, handler, ctx, PROP_GET);
  }
  public int busPropSet(Message msg, PropHandler handler, Object ctx) {
    print(INFO, "AJ_BusPropSet(msg=" + msg.ptr + ")\n");
    return propAccess(msg, handler, ctx, PROP_SET);
  }

  public int deliverMsgPartial(Message msg, Immutable imm) {
    try {
      byte[] buf = Packer.pack(imm, msg.totalLen, true);
      return deliverRaw(msg, buf, buf.length);
    } catch (Throwable e) {
      print(ERROR, e.getMessage());
      return ERR_MARSHAL;
    }
  }

  public int unmarshal(Message msg, String sig, ImcRef ref) {
    try {
      byte[] buf = new byte[msg.bodyBytes];
      IntRef len = new IntRef();
      int status = unmarshalRaw(msg, buf, len);
      ref.value = (ImmutableContainer)Unpacker.unpack(sig, buf, msg.totalLen, true);
      return status;
    } catch (Throwable e) {
      print(ERROR, e.getClass().getName() + ": " + e.getMessage() + "\n");
      ref.value = null;
      return ERR_UNMARSHAL;
    }
  }

  public native int marshalReplyMsg(Message msg, Message replyMsg);
  public native int marshalStatusMsg(Message msg, Message replyMsg, int status);
  public native int deliverRaw(Message msg, byte[] buf, int len);
  public native int marshalSignal(BusAttachment bus, Message msg, int msgId, String destination, int sessionId, int flags, int ttl);

  public native int unmarshalMsg(BusAttachment bus, Message msg, int timeout);
  public native int unmarshalPropertyArgs(Message msg, IntRef propId, StrRef sig);
  public native int unmarshalRaw(Message msg, byte[] buf, IntRef len);
  public native int unmarshalVariant(Message msg, StrRef variant);

  public native void delay(int msec);
  public native void print(int level, String s);

  public native void printXml(BusObject[] busObjects);
  public native void registerObjects(BusObject[] localObjects, BusObject[] proxyObjects);

  public final String[] propertiesInterface = new String[] {
    "#org.freedesktop.DBus.Properties",
    "?Get <s <s >v",
    "?Set <s <s <v",
    "?GetAll <s >a{sv}"
  };

  public final int BUS_ID_FLAG = 0x00;
  public final int APP_ID_FLAG = 0x01;
  public final int PRX_ID_FLAG = 0x02;
  public final int REP_ID_FLAG = 0x80;

  public final int PROP_GET = 0;
  public final int PROP_SET = 1;
  public final int PROP_GET_ALL = 2;

  public final int WARN = 1;
  public final int ERROR = 2;
  public final int INFO = 3;
  public final int ALWAYS = 5;

  public final int NAME_REQ_ALLOW_REPLACEMENT = 0x01;
  public final int NAME_REQ_REPLACE_EXISTING  = 0x02;
  public final int NAME_REQ_DO_NOT_QUEUE      = 0x04;

  public final int MSG_INVALID      = 0;
  public final int MSG_METHOD_CALL  = 1;
  public final int MSG_METHOD_RET   = 2;
  public final int MSG_ERROR        = 3;
  public final int MSG_SIGNAL       = 4;

  public final int
    OK               = 0,  /**< Success status */
    ERR_NULL         = 1,  /**< Unexpected NULL pointer */
    ERR_UNEXPECTED   = 2,  /**< An operation was unexpected at this time */
    ERR_INVALID      = 3,  /**< A value was invalid */
    ERR_IO_BUFFER    = 4,  /**< An I/O buffer was invalid or in the wrong state */
    ERR_READ         = 5,  /**< An error while reading data from the network */
    ERR_WRITE        = 6,  /**< An error while writing data to the network */
    ERR_TIMEOUT      = 7,  /**< A timeout occurred */
    ERR_MARSHAL      = 8,  /**< Marshaling failed due to badly constructed message argument */
    ERR_UNMARSHAL    = 9,  /**< Unmarshaling failed due to a corrupt or invalid message */
    ERR_END_OF_DATA  = 10, /**< Not enough data */
    ERR_RESOURCES    = 11, /**< Insufficient memory to perform the operation */
    ERR_NO_MORE      = 12, /**< Attempt to unmarshal off the end of an array */
    ERR_SECURITY     = 13, /**< Authentication or decryption failed */
    ERR_CONNECT      = 14, /**< Network connect failed */
    ERR_UNKNOWN      = 15, /**< A unknown value */
    ERR_NO_MATCH     = 16, /**< Something didn't match */
    ERR_SIGNATURE    = 17, /**< Signature is not what was expected */
    ERR_DISALLOWED   = 18, /**< An operation was not allowed */
    ERR_FAILURE      = 19, /**< A failure has occurred */
    ERR_RESTART      = 20, /**< The OEM event loop must restart */
    ERR_LINK_TIMEOUT = 21, /**< The bus link is inactive too long */
    ERR_DRIVER       = 22, /**< An error communicating with a lower-layer driver */
    ERR_OBJECT_PATH  = 23, /**< Object path was not specified */
    ERR_BUSY         = 24, /**< An operation failed and should be retried later */
    ERR_DHCP         = 25, /**< A DHCP operation has failed */
    ERR_ACCESS       = 26, /**< The operation specified is not allowed */
    ERR_SESSION_LOST = 27, /**< The session was lost */
    ERR_LINK_DEAD    = 28, /**< The network link is now dead */
    ERR_HDR_CORRUPT  = 29, /**< The message header was corrupt */
    ERR_RESTART_APP  = 30, /**< The application must cleanup and restart */
    ERR_INTERRUPTED  = 31,
    ERR_REJECTED     = 32, /**< The connection was rejected */
    ERR_RANGE        = 33, /**< Value provided was out of range */
    ERR_ACCESS_ROUTING_NODE = 34, /**< Access defined by routing node */
    ERR_KEY_EXPIRED  = 35, /**< The key has expired */
    ERR_SPI_NO_SPACE = 36, /**< Out of space error */
    ERR_SPI_READ     = 37, /**< Read error */
    ERR_SPI_WRITE    = 38, /**< Write error */
    ERR_OLD_VERSION  = 39, /**< Router you connected to is old and unsupported */
    ERR_NVRAM_READ   = 40, /**< Error while reading from NVRAM */
    ERR_NVRAM_WRITE  = 41; /**< Error while writing to NVRAM */

  public int ENCODE_MESSAGE_ID(int o, int p, int i, int m) {
    return (o << 24) | (p << 16) | (i << 8) | m;
  }

  public int ENCODE_PROPERTY_ID(int o, int p, int i, int m) {
    return (o << 24) | (p << 16) | (i << 8) | m;
  }

  public int BUS_MESSAGE_ID(int p, int i, int m) {
    return ENCODE_MESSAGE_ID(BUS_ID_FLAG, p, i, m);
  }

  public int APP_MESSAGE_ID(int p, int i, int m) {
    return ENCODE_MESSAGE_ID(APP_ID_FLAG, p, i, m);
  }

  public int PRX_MESSAGE_ID(int p, int i, int m) {
    return ENCODE_MESSAGE_ID(PRX_ID_FLAG, p, i, m);
  }

  public int BUS_PROPERTY_ID(int p, int i, int m) {
    return ENCODE_MESSAGE_ID(BUS_ID_FLAG, p, i, m);
  }

  public int APP_PROPERTY_ID(int p, int i, int m) {
    return ENCODE_MESSAGE_ID(APP_ID_FLAG, p, i, m);
  }

  public int PRX_PROPERTY_ID(int p, int i, int m) {
    return ENCODE_MESSAGE_ID(PRX_ID_FLAG, p, i, m);
  }

  public int REPLY_ID(int id) {
    return id | (REP_ID_FLAG << 24);
  }

  public final int METHOD_HELLO                = BUS_MESSAGE_ID(0, 0, 0);
  public final int SIGNAL_NAME_OWNER_CHANGED   = BUS_MESSAGE_ID(0, 0, 1);
  public final int SIGNAL_NAME_ACQUIRED        = BUS_MESSAGE_ID(0, 0, 2);
  public final int SIGNAL_NAME_LOST            = BUS_MESSAGE_ID(0, 0, 3);
  public final int SIGNAL_PROPS_CHANGED        = BUS_MESSAGE_ID(0, 0, 4);
  public final int METHOD_REQUEST_NAME         = BUS_MESSAGE_ID(0, 0, 5);
  public final int METHOD_ADD_MATCH            = BUS_MESSAGE_ID(0, 0, 6);
  public final int METHOD_REMOVE_MATCH         = BUS_MESSAGE_ID(0, 0, 7);
  public final int METHOD_RELEASE_NAME         = BUS_MESSAGE_ID(0, 0, 8);
  public final int SIGNAL_SESSION_LOST                  = BUS_MESSAGE_ID(1, 0, 0);
  public final int SIGNAL_FOUND_ADV_NAME                = BUS_MESSAGE_ID(1, 0, 1);
  public final int SIGNAL_LOST_ADV_NAME                 = BUS_MESSAGE_ID(1, 0, 2);
  public final int SIGNAL_MP_SESSION_CHANGED            = BUS_MESSAGE_ID(1, 0, 3);
  public final int METHOD_ADVERTISE_NAME                = BUS_MESSAGE_ID(1, 0, 4);
  public final int METHOD_CANCEL_ADVERTISE              = BUS_MESSAGE_ID(1, 0, 5);
  public final int METHOD_FIND_NAME                     = BUS_MESSAGE_ID(1, 0, 6);
  public final int METHOD_CANCEL_FIND_NAME              = BUS_MESSAGE_ID(1, 0, 7);
  public final int METHOD_BIND_SESSION_PORT             = BUS_MESSAGE_ID(1, 0, 8);
  public final int METHOD_UNBIND_SESSION                = BUS_MESSAGE_ID(1, 0, 9);
  public final int METHOD_JOIN_SESSION                  = BUS_MESSAGE_ID(1, 0, 10);
  public final int METHOD_LEAVE_SESSION                 = BUS_MESSAGE_ID(1, 0, 11);
  public final int METHOD_CANCEL_SESSIONLESS            = BUS_MESSAGE_ID(1, 0, 12);
  public final int METHOD_FIND_NAME_BY_TRANSPORT        = BUS_MESSAGE_ID(1, 0, 13);
  public final int METHOD_CANCEL_FIND_NAME_BY_TRANSPORT = BUS_MESSAGE_ID(1, 0, 14);
  public final int METHOD_SET_LINK_TIMEOUT              = BUS_MESSAGE_ID(1, 0, 15);
  public final int METHOD_REMOVE_SESSION_MEMBER         = BUS_MESSAGE_ID(1, 0, 16);
  public final int SIGNAL_SESSION_LOST_WITH_REASON      = BUS_MESSAGE_ID(1, 0, 17);
  public final int METHOD_BUS_PING                      = BUS_MESSAGE_ID(1, 0, 18);
  public final int METHOD_ACCEPT_SESSION       = BUS_MESSAGE_ID(2, 0, 0);
  public final int SIGNAL_SESSION_JOINED       = BUS_MESSAGE_ID(2, 0, 1);
  public final int METHOD_EXCHANGE_GUIDS       = BUS_MESSAGE_ID(2, 1, 0);
  public final int METHOD_GEN_SESSION_KEY      = BUS_MESSAGE_ID(2, 1, 1);
  public final int METHOD_EXCHANGE_GROUP_KEYS  = BUS_MESSAGE_ID(2, 1, 2);
  public final int METHOD_AUTH_CHALLENGE       = BUS_MESSAGE_ID(2, 1, 3);
  public final int METHOD_EXCHANGE_SUITES      = BUS_MESSAGE_ID(2, 1, 4);
  public final int METHOD_KEY_EXCHANGE         = BUS_MESSAGE_ID(2, 1, 5);
  public final int METHOD_KEY_AUTHENTICATION   = BUS_MESSAGE_ID(2, 1, 6);
  public final int METHOD_INTROSPECT           = BUS_MESSAGE_ID(3, 0, 0);
  public final int METHOD_PING                 = BUS_MESSAGE_ID(3, 1, 0);
  public final int METHOD_GET_MACHINE_ID       = BUS_MESSAGE_ID(3, 1, 1);
  public final int METHOD_GET_DESCRIPTION_LANG = BUS_MESSAGE_ID(3, 2, 0);
  public final int METHOD_INTROSPECT_WITH_DESC = BUS_MESSAGE_ID(3, 2, 1);
  public final int SIGNAL_PROBE_REQ            = BUS_MESSAGE_ID(4, 0, 0);
  public final int SIGNAL_PROBE_ACK            = BUS_MESSAGE_ID(4, 0, 1);
  public final int METHOD_ABOUT_GET_PROP                = BUS_MESSAGE_ID(5, 0, PROP_GET);
  public final int METHOD_ABOUT_SET_PROP                = BUS_MESSAGE_ID(5, 0, PROP_SET);
  public final int PROPERTY_ABOUT_VERSION               = BUS_PROPERTY_ID(5, 1, 0);
  public final int METHOD_ABOUT_GET_ABOUT_DATA          = BUS_MESSAGE_ID(5, 1, 1);
  public final int METHOD_ABOUT_GET_OBJECT_DESCRIPTION  = BUS_MESSAGE_ID(5, 1, 2);
  public final int SIGNAL_ABOUT_ANNOUNCE                = BUS_MESSAGE_ID(5, 1, 3);
  public final int METHOD_ABOUT_ICON_GET_PROP          = BUS_MESSAGE_ID(6, 0, PROP_GET);
  public final int METHOD_ABOUT_ICON_SET_PROP          = BUS_MESSAGE_ID(6, 0, PROP_SET);
  public final int PROPERTY_ABOUT_ICON_VERSION_PROP    = BUS_PROPERTY_ID(6, 1, 0);
  public final int PROPERTY_ABOUT_ICON_MIMETYPE_PROP   = BUS_PROPERTY_ID(6, 1, 1);
  public final int PROPERTY_ABOUT_ICON_SIZE_PROP       = BUS_PROPERTY_ID(6, 1, 2);
  public final int METHOD_ABOUT_ICON_GET_URL           = BUS_MESSAGE_ID(6, 1, 3);
  public final int METHOD_ABOUT_ICON_GET_CONTENT       = BUS_MESSAGE_ID(6, 1, 4);
  public final int INVALID_MSG_ID   = 0xFFFFFFFF;
  public final int INVALID_PROP_ID  = 0xFFFFFFFF;

  public final int TRANSPORT_NONE      = 0x0000;
  public final int TRANSPORT_ALL       = 0xFFFF;
  public final int TRANSPORT_LOCAL     = 0x0001;
  public final int TRANSPORT_BLUETOOTH = 0x0002;
  public final int TRANSPORT_WLAN      = 0x0004;
  public final int TRANSPORT_WWAN      = 0x0008;
  public final int TRANSPORT_LAN       = 0x0010;
  public final int TRANSPORT_PROXIMITY = 0x0040;

  public final int TRANSPORT_TCP       = 0x0004;
  public final int TRANSPORT_UDP       = 0x0100;
  public final int TRANSPORT_IP        = (TRANSPORT_TCP | TRANSPORT_UDP);

  public final int TRANSPORT_ANY       = (TRANSPORT_ALL & ~TRANSPORT_UDP);

  public final int BUS_START_ADVERTISING = 0;
  public final int BUS_STOP_ADVERTISING  = 1;
}
