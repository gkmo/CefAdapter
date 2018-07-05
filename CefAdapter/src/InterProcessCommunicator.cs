using ZeroMQ;
using System.Threading;
using System;
using System.Threading.Tasks;

namespace CefAdapter
{
    class InterProcessCommunicator
    {
        private readonly ZContext _context;
        private readonly ZSocket _requestSocket;
        private readonly ZSocket _replySocket;
        private ManualResetEvent _connectedResetEvent;
        private Thread _thread;
        private bool _stop;        

        public InterProcessCommunicator()
        {
            _context = new ZContext();
            _requestSocket = new ZSocket(ZSocketType.REQ);            

            _replySocket = new ZSocket(ZSocketType.REP);
            _replySocket.Bind("tcp://*:5561");

            _thread = new Thread(ListenRequests);
            _thread.IsBackground = true;
            _thread.Start();    
        }

        public event EventHandler<MessageEventArgs> MessageReceived;

        public bool IsConnected { get; private set; }

        public bool Connect()
        {
            _connectedResetEvent = new ManualResetEvent(false);

            _requestSocket.Connect("tcp://localhost:5560");

            var reply = Invoke("CONNECT");

            IsConnected = reply == "CONNECT|SUCCESS";

            return IsConnected;
        }

        internal void ExecuteJavaScript(int id, string code)
        {
            Invoke("EXECUTE_JAVA_SCRIPT", id, code);
        }

        internal void ShowDeveloperTools(int id)
        {
            Invoke("SHOW_DEVELOPER_TOOLS", id);
        }

        public void Disconnect()
        {
            IsConnected = false;
        }

        private void ListenRequests(object obj)
        {
            while(!_stop)
            {
                var message = ReceiveMessage(_replySocket);
                var result = ProcessMessage(message);
                SendMessage(_replySocket, result);
            }
        }

        private string ProcessMessage(string message)
        {
            var splittedMessage = message.Split('|');
            var messageName = splittedMessage[0];
            var arguments = new string[splittedMessage.Length - 1];

            Array.Copy(splittedMessage, 1, arguments, 0, arguments.Length);

            Task.Run(() => MessageReceived?.Invoke(this, new MessageEventArgs(messageName, arguments)));

            return splittedMessage[0] + "|SUCCESS";
        }

        internal void OnQuerySuccess(long id, string message)
        {
            Invoke("QUERY_SUCCESS", id, message);
        }

        private string Invoke(string eventName, params object[] arguments)
        {
            SendMessage(_requestSocket, eventName, arguments);
            return ReceiveMessage(_requestSocket);
        }

        internal void OnQueryFailure(long queryId, int errorCode, string message)
        {
            Invoke("QUERY_FAILURE", queryId, errorCode, message);
        }

        private static string ReceiveMessage(ZSocket socket)
        {
            var frame = socket.ReceiveFrame();
            var message = frame.ReadString();
            Console.WriteLine("Received: {0}", message);
            return message;
        }

        private static void SendMessage(ZSocket socket, string eventName, params object[] arguments)
        {
            var message = eventName + "|" + string.Join("|", arguments);            
            socket.Send(new ZFrame(message));
            Console.WriteLine("Sent: {0}", message);
        }
    }
}   