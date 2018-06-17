using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace CefAdapter.Native
{
    [StructLayout(LayoutKind.Sequential)]
    struct JavaScriptValue
    {
        public JavaScriptType ValueType;
        public bool BooleanValue;
        public double NumberValue;
        public IntPtr StringValue;        
    }
}
