package org.alljoyn.bus.messaging;

import org.alljoyn.bus.core.UnsupportedOperationException;
import org.alljoyn.bus.messaging.messages.*;
import org.alljoyn.bus.types.immutable.Immutable;

import java.io.IOException;
import java.lang.String;

/**
 * Created by Henrik on 20.11.2014.
 */
public final class Serializer {
  Serializer() {}
  public static Message unpack(byte[] rawMessage) throws IOException, UnsupportedOperationException {
    // parses raw message and return subtype of message
    return null;
  }

  public static Message packMethodCall(String path, String member, String iface, String destination, Immutable body) throws IOException, UnsupportedOperationException {
    return new MethodCallMessage(path, member, iface, destination, body);
  }

  public static Message packMethodReturn(int replySerial, Immutable body, String destination) throws IOException, UnsupportedOperationException {
    return new MethodReturnMessage(replySerial, body, destination);
  }

  public static Message packError(String errorName, int replySerial, String destination, Immutable body, String sender) throws IOException, UnsupportedOperationException {
    return new ErrorMessage(errorName, replySerial, destination, body, sender);
  }

  public static Message packSignal(String path, String member, String iface, String destination, Immutable body) throws IOException, UnsupportedOperationException {
    return new SignalMessage(path, member, iface, destination, body);
  }
}
