package org.alljoyn.bus.core;

import org.alljoyn.bus.types.immutable.Immutable;
import org.alljoyn.bus.types.immutable.ImmutableContainer;

/**
 * Created by Henrik on 5.11.2014.
 */
public interface Func extends Delegate {
  public static final Func None = new Func() {
    public Immutable execute(ImmutableContainer args) {
      return Immutable.None;
    }
  };

  Immutable execute(ImmutableContainer args) throws UnsupportedOperationException;
}
