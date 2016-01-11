package org.alljoyn.bus;

import org.alljoyn.bus.Message;
import org.alljoyn.bus.types.immutable.Immutable;

public interface PropHandler {
  int handleGet(Message replyMsg, int propId, Object context);
  int handleSet(Immutable imm, int propId, Object context);
}
