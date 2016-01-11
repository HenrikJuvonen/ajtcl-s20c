package org.alljoyn.bus;

public class BusObject {
  public String path;
  public String[][] interfaces;
  public int flags;

  public BusObject(String path, String[][] interfaces, int flags) {
    this.path = path;
    this.interfaces = interfaces;
    this.flags = flags;
  }
}
