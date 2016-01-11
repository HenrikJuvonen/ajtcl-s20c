package org.alljoyn.bus.core;

import org.alljoyn.bus.types.immutable.ImmutableContainer;

/**
 * Created by Henrik on 5.11.2014.
 */
public interface Action extends Delegate {
  public static final Action None = new Action() {
    public void execute(ImmutableContainer args) {
    }
  };

  void execute(ImmutableContainer args) throws UnsupportedOperationException;
}
