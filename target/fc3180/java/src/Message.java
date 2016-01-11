package org.alljoyn.bus;

public class Message {
  public int ptr;
  public boolean collect;
  public int msgType;
  public int msgId;
  public String destination;
  public int sessionId;
  public int bodyBytes;
  public int totalLen;
}
