package org.alljoyn.bus.core;

import org.alljoyn.bus.types.immutable.Immutable;

import java.lang.String;
import java.lang.StringBuffer;
import java.util.Vector;

/**
 * Created by Henrik on 16.11.2014.
 */
public final class Utils {
  private static final String hex = "0123456789ABCDEF";

  public static final Immutable[] copy(Immutable[] values) {
    Immutable[] result = new Immutable[values.length];
    for (int i = 0; i < values.length; i++) {
      result[i] = values[i];
    }
    return result;
  }
  public static final Immutable[] copy(Vector values) {
    Immutable[] result = new Immutable[values.size()];
    values.copyInto(result);
    return result;
  }

  public static final String dumpDec(byte[] bytes) {
    StringBuffer buffer = new StringBuffer();
    buffer.append('[');
    for (int i = 0; i < bytes.length; i++) {
      buffer.append(String.valueOf(bytes[i] & 0xFF));
      if (i != bytes.length - 1) {
        buffer.append(' ');
      }
    }
    buffer.append(']');
    return buffer.toString();
  }

  public static final String dumpHex(byte[] bytes) {
    StringBuffer buffer = new StringBuffer(bytes.length * 3);
    buffer.append('[');
    for (int i = 0; i < bytes.length; i++) {
      buffer.append(hex.charAt((bytes[i] >> 4) & 0xF));
      buffer.append(hex.charAt(bytes[i] & 0xF));
      if (i != bytes.length - 1) {
        buffer.append(' ');
      }
    }
    buffer.append(']');
    return buffer.toString();
  }

  public static String dump(String[] values) {
    StringBuffer buffer = new StringBuffer();
    buffer.append('[');
    for (int i = 0; i < values.length; i++) {
      buffer.append('"');
      buffer.append(values[i]);
      buffer.append('"');
      if (i != values.length - 1) {
        buffer.append(' ');
      }
    }
    buffer.append(']');
    return buffer.toString();
  }

  public static final String flatten(String[] values) {
    StringBuffer buffer = new StringBuffer();
    for (int i = 0; i < values.length; i++) {
      buffer.append(values[i]);
    }
    return buffer.toString();
  }

  public static byte[] getBytes(int value) {
    return new byte[]{
        (byte) (value >>> 24),
        (byte) (value >>> 16),
        (byte) (value >>> 8),
        (byte) value
    };
  }

  public static byte[] copy(byte[] bytes) {
    byte[] result = new byte[bytes.length];
    for (int i = 0; i < bytes.length; i++) {
      result[i] = bytes[i];
    }
    return result;
  }

  public static byte[] insert(byte[] bytes, int index, byte[] data) {
    byte[] result = new byte[bytes.length + data.length];
    for (int i = 0; i < index; i++) {
      result[i] = bytes[i];
    }
    for (int i = index; i < index + data.length; i++) {
      result[i] = data[i - index];
    }
    for (int i = index + data.length; i < index + data.length + bytes.length; i++) {
      result[i] = bytes[i - index - data.length];
    }
    return result;
  }

  public static byte[] reverse(byte[] bytes, int beginIndex, int endIndex) {
    byte[] result = copy(bytes);
    for (int i = beginIndex; i < endIndex; i++) {
      result[endIndex - i - 1] = bytes[i];
    }
    return result;
  }

  public static byte[] reverse(byte[] bytes) {
    return reverse(bytes, 0, bytes.length);
  }

  public static byte[] slice(byte[] bytes, int offset, int length) {
    byte[] result = new byte[length];
    for (int i = 0; i < length && i + offset < bytes.length; i++) {
      result[i] = bytes[i + offset];
    }
    return result;
  }

  public static byte[] cast(int[] values) {
    byte[] result = new byte[values.length];
    for (int i = 0; i < values.length; i++) {
      result[i] = (byte) values[i];
    }
    return result;
  }

  public static byte[] concat(byte[] a, byte[] b) {
    byte[] c = new byte[a.length+b.length];
    for (int i = 0; i < a.length; i++) {
      c[i] = a[i];
    }
    for (int i = 0; i < b.length; i++) {
      c[a.length + i] = b[i];
    }
    return c;
  }

  public static byte[] concat(byte[] a, byte[] b, byte[] c) {
    return concat(concat(a, b), c);
  }
}
