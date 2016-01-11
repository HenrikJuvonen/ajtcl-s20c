package org.alljoyn.bus.messaging;

/**
 * Created by Henrik on 17.11.2014.
 */
public class Padder {
  public static byte[] padHeader(int padding) {
    return pad(8, padding);
  }

  public static byte[] pad(char tcode, int padding) {
    switch (tcode) {
      case 'y':
        return pad(1, padding);
      case 'b':
        return pad(4, padding);
      case 'n':
        return pad(2, padding);
      case 'q':
        return pad(2, padding);
      case 'i':
        return pad(4, padding);
      case 'u':
        return pad(4, padding);
      case 'x':
        return pad(8, padding);
      case 't':
        return pad(8, padding);
      case 'd':
        return pad(8, padding);
      case 's':
        return pad(4, padding);
      case 'o':
        return pad(4, padding);
      case 'g':
        return pad(1, padding);
      case 'a':
        return pad(4, padding);
      case '(':
        return pad(8, padding);
      case 'v':
        return pad(1, padding);
      case '{':
        return pad(8, padding);
      case 'h':
        return pad(4, padding);
    }
    return pad(0, 0);
  }

  public static byte[] pad(int align, int padding) {
    return new byte[align - padding & align - 1];
  }
}
