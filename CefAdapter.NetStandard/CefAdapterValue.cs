using System;
using System.Collections.Generic;
using System.Runtime.InteropServices;
using System.Text;

namespace CefAdapter
{
    enum CefAdapterValueType
    {
        Void = 0,
        Boolean,
        Byte,
        Int16,
        Int32,
        Int64,
        Double,
        String
    }

    [StructLayout(LayoutKind.Sequential)]
    struct CefAdapterValue
    {
        public CefAdapterValueType ValueType;
        public IntPtr Value;
    }
}
