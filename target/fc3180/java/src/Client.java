import org.alljoyn.bus.*;
import org.alljoyn.bus.types.Container;
import org.alljoyn.bus.types.immutable.*;
import java.lang.String;

public class Client
{
  AllJoyn aj = AllJoyn.instance;

  private boolean lightingState = false;

  private final String serviceName = "org.alljoyn.lighting";
  private final short servicePort = 24000;
  private final boolean cancelAdvertiseName = false;
  private final boolean reflectSignal = false;

  private final int connectTimeout = 1000 * 20;
  private final int unmarshalTimeout = 1000 * 5;

  private final String[] lightingInterface = new String[]
  {
    "org.alljoyn.lighting",
    "@Lighting=b",
    "?TurnLightingOn",
    "?TurnLightingOff",
    "!LightingChanged >b"
  };

  private final String[][] lightingInterfaces = new String[][]
  {
    aj.propertiesInterface,
    lightingInterface
  };

  private final BusObject[] appObjects = new BusObject[]
  {
    new BusObject("/org/alljoyn/lighting", lightingInterfaces, 0x08)
  };

  private int APP_GET_PROP     = aj.APP_MESSAGE_ID(0, 0, aj.PROP_GET);
  private int APP_SET_PROP     = aj.APP_MESSAGE_ID(0, 0, aj.PROP_SET);
  private int APP_LIGHT        = aj.APP_PROPERTY_ID(0, 1, 0);
  private int APP_LIGHT_ON     = aj.APP_MESSAGE_ID(0, 1, 1);
  private int APP_LIGHT_OFF    = aj.APP_MESSAGE_ID(0, 1, 2);
  private int APP_LIGHT_SIGNAL = aj.APP_MESSAGE_ID(0, 1, 3);

  private int appLightOn(Message msg)
  {
      lightingState = true;
      aj.print(aj.ALWAYS, "TurnLightingOn\n");
      Message reply = new Message();
      aj.marshalReplyMsg(msg, reply);
      return aj.deliverMsg(reply);
  }

  private int appLightOff(Message msg)
  {
      lightingState = false;
      aj.print(aj.ALWAYS, "TurnLightingOff\n");
      Message reply = new Message();
      aj.marshalReplyMsg(msg, reply);
      return aj.deliverMsg(reply);
  }

  public class MyPropHandler implements PropHandler
  {
    public int handleGet(Message replyMsg, int propId, Object context)
    {
        if (propId == APP_LIGHT)
        {
          aj.print(aj.ALWAYS, "GetLighting\n");
          return aj.deliverMsgPartial(replyMsg, Variant.create(Bool.create(lightingState)));
        }
        else
        {
          return aj.ERR_UNEXPECTED;
        }
    }
    public int handleSet(Immutable imm, int propId, Object context)
    {
        if (propId == APP_LIGHT)
        {
          aj.print(aj.ALWAYS, "SetLighting sig=" + imm.sig() + " class=" + imm.getClass().getName() + " before=" + (lightingState ? "true" : "false") +  "\n");
          lightingState = imm.asBool().value;
          aj.print(aj.ALWAYS, "SetLighting after=" + (lightingState ? "true" : "false") +  "\n");
          return aj.OK;
        }
        else
        {
          return aj.ERR_UNEXPECTED;
        }
    }
  }

  private final PropHandler propHandler = new MyPropHandler();

  public void run()
  {
    int status = aj.OK;
    BusAttachment bus = new BusAttachment();
    boolean connected = false;
    int sessionId = 0;

	aj.initialize();
    aj.printXml(appObjects);
    aj.registerObjects(appObjects, null);

    while (true)
    {
      Message msg = new Message();

      if (!connected)
      {
        status = aj.startService(bus, null, connectTimeout, false, servicePort, serviceName, aj.NAME_REQ_DO_NOT_QUEUE, null);
        if (status != aj.OK)
        {
          continue;
        }
        aj.print(aj.INFO, "StartService returned aj.OK\n");
        aj.print(aj.INFO, "Connected to Daemon:" + aj.getUniqueName(bus) + "\n");

        connected = true;

        aj.setBusLinkTimeout(bus, 60);
      }

      status = aj.unmarshalMsg(bus, msg, unmarshalTimeout);

      if (status == aj.ERR_TIMEOUT)
      {
        if (aj.busLinkStateProc(bus) == aj.ERR_LINK_TIMEOUT)
        {
          status = aj.ERR_READ;
        }
        else
        {
          continue;
        }
      }

      if (status == aj.OK)
      {
        if (msg.msgId == aj.REPLY_ID(aj.METHOD_ADD_MATCH))
        {
          if (msg.msgType == aj.MSG_ERROR) {
            aj.print(aj.INFO, "Failed to add match\n");
            status = aj.ERR_FAILURE;
          }
          else
          {
            status = aj.OK;
          }
        }
        else if (msg.msgId == aj.METHOD_ACCEPT_SESSION)
        {
          ImcRef ref = new ImcRef();
          aj.unmarshal(msg, "qus", ref);
          short port = ref.value.get(0).asUInt16().value;
          sessionId = ref.value.get(1).asUInt32().value;
          String joiner = ref.value.get(2).asWString().value;

          if (port == servicePort)
          {
              status = aj.busReplyAcceptSession(msg, true);
              aj.print(aj.INFO, "Accepted session session_id=" + sessionId + " joiner=" + joiner + "\n");
          }
          else
          {
              status = aj.busReplyAcceptSession(msg, false);
              aj.print(aj.INFO, "Accepted rejected session_id=" + sessionId + " joiner=" + joiner + "\n");
          }
        }
        else if (msg.msgId == aj.SIGNAL_SESSION_LOST_WITH_REASON)
        {
          ImcRef ref = new ImcRef();
          aj.unmarshal(msg, "uu", ref);
          int id = ref.value.get(0).asUInt32().value;
          int reason = ref.value.get(1).asUInt32().value;
          aj.print(aj.INFO, "Session lost. ID = " + id + ", reason = " + reason);
          if (cancelAdvertiseName)
          {
              status = aj.busAdvertiseName(bus, serviceName, aj.TRANSPORT_ANY, aj.BUS_START_ADVERTISING, 0);
          }
          status = aj.ERR_SESSION_LOST;
        }
        else if (msg.msgId == aj.SIGNAL_SESSION_JOINED)
        {
          if (cancelAdvertiseName)
          {
              status = aj.busAdvertiseName(bus, serviceName, aj.TRANSPORT_ANY, aj.BUS_STOP_ADVERTISING, 0);
          }
        }
        else if (msg.msgId == aj.REPLY_ID(aj.METHOD_CANCEL_ADVERTISE) || msg.msgId == aj.REPLY_ID(aj.METHOD_ADVERTISE_NAME))
        {
          if (msg.msgType == aj.MSG_ERROR)
          {
              status = aj.ERR_FAILURE;
          }
        }
        else if (msg.msgId == APP_GET_PROP)
        {
          status = aj.busPropGet(msg, propHandler, null);
        }
        else if (msg.msgId == APP_SET_PROP)
        {
          status = aj.busPropSet(msg, propHandler, null);
          if (status == aj.OK)
          {
            aj.print(aj.INFO, "Property successfully set.\n");
          }
          else
          {
            aj.print(aj.INFO, "Property set attempt unsuccessful. Status = " + status + "\n");
          }
        }
        else if (msg.msgId == APP_LIGHT_ON)
        {
          status = appLightOn(msg);
        }
        else if (msg.msgId == APP_LIGHT_OFF)
        {
          status = appLightOff(msg);
        }
        else if (msg.msgId == APP_LIGHT_SIGNAL)
        {
          aj.print(aj.INFO, "Received light_signal\n");
          status = aj.OK;
          if (reflectSignal)
          {
            Message out = new Message();
            aj.marshalSignal(bus, out, APP_LIGHT_SIGNAL, msg.destination, msg.sessionId, 0, 0);
            aj.deliverMsgPartial(out, Bool.create(lightingState));
            aj.deliverMsg(out);
			aj.closeMsg(out);
          }
        }
        else
        {
          status = aj.busHandleBusMessage(msg);
        }

        aj.notifyLinkActive();
      }

      aj.closeMsg(msg);

      if (status == aj.ERR_READ || status == aj.ERR_LINK_DEAD)
      {
        aj.print(aj.INFO, "AllJoyn disconnect\n");
        aj.print(aj.INFO, "Disconnected from Daemon:" + aj.getUniqueName(bus) + "\n");
        aj.disconnect(bus);
        connected = false;

        aj.delay(10 * 1000);
      }
    }
  }
}
