package org.alljoyn.bus.messaging;

import org.alljoyn.bus.types.immutable.Immutable;
import org.alljoyn.bus.types.immutable.ImmutableContainer;
import org.alljoyn.bus.core.UnsupportedOperationException;
import org.alljoyn.bus.core.Utils;
import org.alljoyn.bus.types.immutable.*;

import java.io.ByteArrayOutputStream;
import java.io.DataOutputStream;
import java.io.IOException;
import java.lang.String;

/**
 * Created by Henrik on 15.11.2014.
 */
public final class Packer {

  private static byte[] marshalSimple(Immutable value, boolean lendian) throws IOException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(baos);
    if (Bool.class.isInstance(value)) {
      dos.writeInt(value.asBool().value ? 1 : 0);
    } else if (UInt8.class.isInstance(value)) {
      dos.writeByte(value.asInt8().value);
    } else if (Int16.class.isInstance(value)) {
      dos.writeShort(value.asInt16().value);
    } else if (Int32.class.isInstance(value)) {
      dos.writeInt(value.asInt32().value);
    } else if (Int64.class.isInstance(value)) {
      dos.writeLong(value.asInt64().value);
    } else if (UInt16.class.isInstance(value)) {
      dos.writeShort(value.asUInt16().value);
    } else if (UInt32.class.isInstance(value)) {
      dos.writeInt(value.asUInt32().value);
    } else if (UInt64.class.isInstance(value)) {
      dos.writeLong(value.asUInt64().value);
    } else if (Float64.class.isInstance(value)) {
      dos.writeDouble(value.asFloat64().value);
    }
    dos.close();
    byte[] bytes = baos.toByteArray();
    baos.close();
    return lendian ? Utils.reverse(bytes) : bytes;
  }

  private static byte[] marshalString(Immutable value, boolean lendian) throws UnsupportedOperationException, IOException {
    String string = value.toUTF();
    if (string.indexOf(0) != -1) {
      throw new UnsupportedOperationException();
    }
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(baos);
    dos.writeInt(string.length());
    dos.write(string.getBytes("UTF8"));
    dos.writeByte(0);
    dos.close();
    byte[] bytes = baos.toByteArray();
    baos.close();
    return lendian ? Utils.reverse(bytes, 0, 4) : bytes;
  }

  private static byte[] marshalSignature(String value, boolean lendian) throws IOException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(baos);
    dos.writeByte(value.length());
    dos.write(value.getBytes("ISO8859_1"));
    dos.writeByte(0);
    dos.close();
    byte[] bytes = baos.toByteArray();
    baos.close();
    return bytes;
  }

  private static byte[] marshalArray(Array value, int startByte, int totalLen, boolean lendian) throws IOException, UnsupportedOperationException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(baos);
    startByte += 4;
    String sig = value.sig().substring(1);
    byte[] initialPadding = Padder.pad(sig.charAt(0), totalLen + startByte);
    if (initialPadding.length > 0) {
      startByte += initialPadding.length;
      dos.write(initialPadding);
    }
    int dataLen = 0;
    for (int i = 0; i < value.values.length; i++) {
      byte[] padding = Padder.pad(value.values[i].align(), totalLen + startByte);
      if (padding.length > 0) {
        startByte += padding.length;
        dataLen += padding.length;
        dos.write(padding);
      }
      byte[] chunk = marshal(value.values[i], startByte, totalLen, lendian);
      startByte += chunk.length;
      dataLen += chunk.length;
      dos.write(chunk);
    }
    dos.close();
    byte[] bytes = baos.toByteArray();
    baos.close();
    byte[] len = Utils.getBytes(dataLen);
    len = lendian ? Utils.reverse(len) : len;
    bytes = Utils.insert(bytes, 0, len);
    return bytes;
  }

  private static byte[] marshalStruct(Struct value, int startByte, int totalLen, boolean lendian) throws IOException, UnsupportedOperationException {
    return marshal(ImmutableContainer.create(value.values), startByte, totalLen, lendian);
  }

  private static byte[] marshalDictEntry(DictEntry value, int startByte, int totalLen, boolean lendian) throws IOException, UnsupportedOperationException {
    return marshal(ImmutableContainer.create(new Immutable[]{value.key, value.value}), startByte, totalLen, lendian);
  }

  private static byte[] marshalVariant(Immutable value, int startByte, int totalLen, boolean lendian) throws IOException, UnsupportedOperationException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(baos);
    byte[] chunks = marshalSignature(value.sig(), lendian);
    dos.write(chunks);
    startByte += chunks.length;
    byte[] padding = Padder.pad(value.align(), totalLen + startByte);
    if (padding.length > 0) {
      startByte += padding.length;
      dos.write(padding);
    }
    dos.write(marshal(value, startByte, totalLen, lendian));
    dos.close();
    byte[] bytes = baos.toByteArray();
    baos.close();
    return bytes;
  }

  private static byte[] marshal(Immutable value, int startByte, int totalLen, boolean lendian) throws IOException, UnsupportedOperationException {
    ByteArrayOutputStream baos = new ByteArrayOutputStream();
    DataOutputStream dos = new DataOutputStream(baos);
    if (ImmutableContainer.class.isInstance(value)) {
      Immutable[] values = ((ImmutableContainer) value).values;
      for (int i = 0; i < values.length; i++) {
        byte[] padding = Padder.pad(values[i].align(), totalLen + startByte);
        if (padding.length > 0) {
          startByte += padding.length;
          dos.write(padding);
        }
        byte[] chunks = marshal(values[i], startByte, totalLen, lendian);
        startByte += chunks.length;
        dos.write(chunks);
      }
    } else {
      byte[] padding = Padder.pad(value.align(), totalLen + startByte);
      if (padding.length > 0) {
        startByte += padding.length;
        dos.write(padding);
      }
      byte[] chunks;
      if (WString.class.isInstance(value) || ObjectPath.class.isInstance(value)) {
        chunks = marshalString(value, lendian);
      } else if (Signature.class.isInstance(value)) {
        chunks = marshalSignature(value.asSignature().value, lendian);
      } else if (Array.class.isInstance(value)) {
        chunks = marshalArray(value.asArray(), startByte, totalLen, lendian);
      } else if (Struct.class.isInstance(value)) {
        chunks = marshalStruct(value.asStruct(), startByte, totalLen, lendian);
      } else if (DictEntry.class.isInstance(value)) {
        chunks = marshalDictEntry(value.asDictEntry(), startByte, totalLen, lendian);
      } else if (Variant.class.isInstance(value)) {
        chunks = marshalVariant(value.asVariant().value, startByte, totalLen, lendian);
      } else {
        chunks = marshalSimple(value, lendian);
      }
      dos.write(chunks);
    }
    dos.flush();
    dos.close();
    byte[] bytes = baos.toByteArray();
    baos.close();
    return bytes;
  }

  public static byte[] pack(Immutable value, int totalLen, boolean lendian) throws IOException, UnsupportedOperationException {
    return marshal(value, 0, totalLen, lendian);
  }
}
