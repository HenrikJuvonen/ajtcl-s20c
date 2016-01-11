public class LightingService extends BusObject {
  private boolean lightingOn;

  public LightingService() {
    super("org.alljoyn.lighting");

    property("Lighting", Sig.Bool, new Func() {
      public Immutable execute(ImmutableContainer args) throws UnsupportedOperationException {
        return Bool.create(lightingOn);
      }
    }, new Action() {
      public Immutable execute(ImmutableContainer args) throws UnsupportedOperationException {
        lightingOn = args.get(0).toBool();
      }
    });
    method("TurnLightingOn", new Action() {
      public void execute(ImmutableContainer args) throws UnsupportedOperationException {
        lightingOn = true;
      }
    });
    method("TurnLightingOff", new Action() {
      public void execute(ImmutableContainer args) throws UnsupportedOperationException {
        lightingOn = false;
      }
    });
    signal("LightingChanged", Sig.Bool);
  }
}
