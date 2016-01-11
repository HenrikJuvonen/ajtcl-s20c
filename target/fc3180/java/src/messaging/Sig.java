package org.alljoyn.bus.messaging;

import java.lang.String;

/**
 * Created by Henrik on 16.11.2014.
 */
public final class Sig {
  private Sig() {
  }

  public static final String Bool = "b";
  public static final String Int16 = "n";
  public static final String Int32 = "i";
  public static final String Int64 = "x";
  public static final String UInt8 = "y";
  public static final String UInt16 = "q";
  public static final String UInt32 = "u";
  public static final String UInt64 = "t";
  public static final String Float64 = "d";
  public static final String WString = "s";
  public static final String ObjectPath = "o";
  public static final String Signature = "g";
  public static final String Variant = "v";

  public static final String Array(String type) {
    return "a" + type;
  }

  public static final String Struct(String types) {
    return "(" + types + ")";
  }

  public static final String DictEntry(String keyType, String valueType) {
    return "{" + keyType + valueType + "}";
  }
}
