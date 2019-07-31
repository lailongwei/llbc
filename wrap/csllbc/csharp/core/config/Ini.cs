// The MIT License (MIT)

// Copyright (c) 2013 lailongwei<lailongwei@126.com>
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of 
// this software and associated documentation files (the "Software"), to deal in 
// the Software without restriction, including without limitation the rights to 
// use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of 
// the Software, and to permit persons to whom the Software is furnished to do so, 
// subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all 
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR 
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS 
// FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR 
// COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER 
// IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN 
// CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.


using System;
using System.Collections.Generic;
using System.IO;
using System.Runtime.InteropServices;
using System.Text;

namespace llbc
{
    #region IniSection
    /// <summary>
    /// The ini section class encapsulation.
    /// </summary>
    public class IniSection : IDisposable
    {
        #region Constructor/Destructor
        /// <summary>
        /// Create new Ini section object
        /// </summary>
        /// <param name="sectionName">section name</param>
        /// <param name="values">section values</param>
        public IniSection(string sectionName, Dictionary<string, string> values)
        {
            if ((_sectionName = sectionName) == null)
                throw new LLBCException("Section name could not be null");
            if ((_values = values) == null)
                throw new LLBCException("Section values could not be null");
        }

        ~IniSection()
        {
            Dispose(false);
        }
        #endregion

        #region sectionName, values
        /// <summary>
        /// Get section name.
        /// </summary>
        public string sectionName
        {
            get { return _sectionName; }
        }

        /// <summary>
        /// Get all section values
        /// </summary>
        public Dictionary<string, string> values
        {
            get { return _values; }
        }
        #endregion

        #region Subscript operator
        /// <summary>
        /// Get specific key's value, if not found, will raise exception.
        /// </summary>
        /// <param name="key">config key</param>
        /// <returns>config value</returns>
        public string this[string key]
        {
            get
            {
                if (!_values.ContainsKey(key))
                    throw new LLBCException("Not found value in IniSection, key:{0}", key);

                return _values[key];
            }
        }
        #endregion

        #region HasKey/GetValue<T>
        /// <summary>
        /// Check ini section exist specific key config or not.
        /// </summary>
        /// <param name="key">the config key</param>
        /// <returns>return true if has specific key config, otherwise return false</returns>
        public bool HasKey(string key)
        {
            return _values.ContainsKey(key);
        }

        /// <summary>
        /// Get specific key's value(template method), if not found, will return dftValue.
        /// </summary>
        /// <typeparam name="T">the except return value type</typeparam>
        /// <param name="key">the config key</param>
        /// <param name="dftValue">default config value</param>
        /// <returns></returns>
        public T GetValue<T>(string key, T dftValue = default(T))
        {
            string value;
            if (!_values.TryGetValue(key, out value))
                return dftValue;

            return StringUtil.ToType<T>(value, dftValue);
        }
        #endregion

        #region Dispose implement
        protected virtual void Dispose(bool disposing)
        {
            if (_disposed)
                return;

            _sectionName = null;
            _values = null;

            _disposed = true;
            if (disposing)
                GC.SuppressFinalize(this);
        }

        public void Dispose()
        {
            Dispose(true);
        }
        #endregion

        private bool _disposed;

        private string _sectionName;
        private Dictionary<string, string> _values;
    }
    #endregion

    #region Ini
    /// <summary>
    /// The ini class encapsulation, use for parse ini config format file/content.
    /// </summary>
    public class Ini : IDisposable
    {
        #region Constructor/Destructor
        public Ini()
        {
            _sections = new Dictionary<string, IniSection>();
        }

        ~Ini()
        {
            Dispose(false);
        }
        #endregion

        #region sectionNames, sections
        public List<string> sectionNames
        {
            get
            {
                return new List<string>(_sections.Keys);
            }
        }

        public Dictionary<string, IniSection> sections
        {
            get
            {
                return _sections;
            }
        }
        #endregion

        #region Subscript operator implementation
        /// <summary>
        /// Get specific section name's ini section object, if not found will raise exception.
        /// </summary>
        /// <param name="sectionName">the section name</param>
        /// <returns>specific name's ini section object</returns>
        public IniSection this[string sectionName]
        {
            get
            {
                var section = GetSection(sectionName);
                if (section == null)
                    throw new LLBCException("Not found section[{0}] in Ini object", sectionName);

                return section;
            }
        }
        #endregion

        #region LoadFromContent/LoadFromFile
        /// <summary>
        /// Load ini config data from ini format string content.
        /// </summary>
        /// <param name="content">ini format string content</param>
        public void LoadFromContent(string content)
        {
            _sections.Clear();

            unsafe
            {
                int sectionCount = 0;
                IntPtr nativeContent = LibUtil.CreateNativeStr(content);

                int errDescLen = 0;
                IntPtr nativeSections = 
                    LLBCNative.csllbc_Ini_LoadFromContent(nativeContent, new IntPtr(&sectionCount), new IntPtr(&errDescLen));
                LibUtil.FreeNativePtr(ref nativeContent, false);

                if (sectionCount == -1)
                {
                    byte* nativeErrorDesc = (byte*)nativeSections;
                    string errorDesc = LibUtil.Ptr2Str(nativeErrorDesc, errDescLen);
                    LibUtil.FreeNativePtr(nativeErrorDesc);

                    throw new LLBCException("{0}", errorDesc);
                }
                else if (sectionCount == 0)
                    return;

                _LoadAllSections((_NativeIniSection*)nativeSections.ToPointer(), sectionCount);
                LLBCNative.csllbc_Ini_FreeNativeSections(nativeSections, sectionCount);
            }
        }

        /// <summary>
        /// Load ini config data from ini config file.
        /// </summary>
        /// <param name="file">ini config file</param>
        public void LoadFromFile(string file)
        {
            byte[] bytes = null;
            using (var fs = new FileStream(file, FileMode.Open, FileAccess.Read))
            {
                if (fs.Length == 0)
                {
                    _sections.Clear();
                    return;
                }

                bytes = new byte[fs.Length];
                fs.Read(bytes, 0, (int)fs.Length);
            }

            LoadFromContent(Encoding.UTF8.GetString(bytes));
        }
        #endregion

        #region HasSection/GetSection, HasKey/GetValue
        /// <summary>
        /// Check has specific section name's config or not.
        /// </summary>
        /// <param name="sectionName">the section name</param>
        /// <returns>true if exist specific section name's config, otherwise return false</returns>
        public bool HasSection(string sectionName)
        {
            return _sections.ContainsKey(sectionName);
        }

        /// <summary>
        /// Get specific name's ini section object, if not found, return null(no exception raise).
        /// </summary>
        /// <param name="sectionName">the section name</param>
        /// <returns>the ini section object, if not found, return null</returns>
        public IniSection GetSection(string sectionName)
        {
            IniSection section = null;
            _sections.TryGetValue(sectionName, out section);
            return section;
        }

        /// <summary>
        /// Check has specific key's config or not.
        /// </summary>
        /// <param name="sectionName">the section name</param>
        /// <param name="key">the config key</param>
        /// <returns>true if exist specific config key's config, otherwise return false</returns>
        public bool HasKey(string sectionName, string key)
        {
            IniSection section;
            if (!_sections.TryGetValue(sectionName, out section))
                return false;

            return section.HasKey(key);
        }

        /// <summary>
        /// Get specific key's config value, if not found, return default value(no exception raise).
        /// </summary>
        /// <typeparam name="T">the value type</typeparam>
        /// <param name="sectionName">the section name</param>
        /// <param name="key">the config key</param>
        /// <param name="dftValue">if not found, will return this default value</param>
        /// <returns>the specific key's config value, if not found will return dftValue parameter specified value</returns>
        public T GetValue<T>(string sectionName, string key, T dftValue = default(T))
        {
            IniSection section = GetSection(sectionName);
            return section != null ? section.GetValue<T>(key, dftValue) : dftValue;
        }
        #endregion

        #region Dispose implement
        public void Dispose()
        {
            Dispose(true);
        }

        virtual protected void Dispose(bool disposing)
        {
            if (_sections == null)
                return;

            _sections = null;
            if (disposing)
                GC.SuppressFinalize(this);
        }
        #endregion

        #region Internal implementation
        private unsafe void _LoadAllSections(_NativeIniSection* nativeSections, int sectionCount)
        {
            for (int sectionIdx = 0; sectionIdx < sectionCount; ++sectionIdx)
            {
                _NativeIniSection nativeSection = *(nativeSections + sectionIdx);
                string sectionName = LibUtil.Ptr2Str(nativeSection.sectionName, nativeSection.sectionNameLen);

                Dictionary<string, string> values = new Dictionary<string, string>(nativeSection.count);
                for (int valueIdx = 0; valueIdx < nativeSection.count; ++valueIdx)
                {
                    string key = LibUtil.Ptr2Str(nativeSection.keys[valueIdx], nativeSection.keysLen[valueIdx]);
                    string value = LibUtil.Ptr2Str(nativeSection.values[valueIdx], nativeSection.valuesLen[valueIdx]);
                    values.Add(key, value);
                }

                _sections.Add(sectionName, new IniSection(sectionName, values));
            }
        }
        #endregion
        
        private Dictionary<string, IniSection> _sections;
    }
    #endregion

    #region Internal structure encapsulation.
    /// <summary>
    /// Library internal structure define, use for swap manage and unmanaged memory data.
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    internal unsafe struct _NativeIniSection
    {
        public byte* sectionName;
        public int sectionNameLen;

        public byte** keys;
        public int* keysLen;
        public byte** values;
        public int* valuesLen;

        public int count;
    }
    #endregion
}
