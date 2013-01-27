// Copyright (c) 2010 Gratian Lup. All rights reserved.
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are
// met:
//
// * Redistributions of source code must retain the above copyright
// notice, this list of conditions and the following disclaimer.
//
// * Redistributions in binary form must reproduce the above
// copyright notice, this list of conditions and the following
// disclaimer in the documentation and/or other materials provided
// with the distribution.
//
// * The name "ObjectExtrusion3D" must not be used to endorse or promote
// products derived from this software without prior written permission.
//
// * Products derived from this software may not be called "ObjectExtrusion3D" nor
// may "ObjectExtrusion3D" appear in their names without prior written
// permission of the author.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
// "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
// LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
// A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
// OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
// SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
// LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
// DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
// THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
// (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
// OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

#ifndef STREAM_HPP
#define STREAM_HPP

#include <Windows.h>

class Stream {
private:
    template <class T>
    struct Helper {
        static void Write(const T &value, Stream &stream) {
            value.Serialize(stream);
        }

        static void Read(T &value, Stream &stream) {
            value.Deserialize(stream);
        }
    };

    // Objects indicated by pointers are not serialized
    // (the user must find a way to serialize them).
    template <class T>
    struct Helper<T *> {
        static void Write(const T* value, Stream &stream) {}
        static void Read(T* value, Stream &stream) {}
    };

    template <>
    struct Helper<char> {
        static void Write(const char &value, Stream &stream) { stream.WriteChar(value); }
        static void Read(char &value, Stream &stream) { stream.ReadChar(value); }
    };

    template <>
    struct Helper<wchar_t> {
        static void Write(const wchar_t &value, Stream &stream) { stream.WriteWChar(value); }
        static void Read(wchar_t &value, Stream &stream) { stream.ReadWChar(value); }
    };

    template <>
    struct Helper<short int> {
        static void Write(const short int &value, Stream &stream) { stream.WriteShort(value); }
        static void Read(short int &value, Stream &stream) { stream.ReadShort(value); }
    };

    template <>
    struct Helper<int> {
        static void Write(const int &value, Stream &stream) { stream.WriteInt(value); }
        static void Read(int &value, Stream &stream) { stream.ReadInt(value); }
    };

    template <>
    struct Helper<float> {
        static void Write(const float &value, Stream &stream) { stream.WriteFloat(value); }
        static void Read(float &value, Stream &stream) { stream.ReadFloat(value); } 
    };

    template <>
    struct Helper<double> {
        static void Write(const double &value, Stream &stream) { stream.WriteDouble(value); }
        static void Read(double &value, Stream &stream) { stream.ReadDouble(value); }
    };

    template <>
    struct Helper<size_t> {
        static void Write(const size_t &value, Stream &stream) { stream.WriteSizeT(value); }
        static void Read(size_t &value, Stream &stream) { stream.ReadSizeT(value); }
    };

    template <>
    struct Helper<bool> {
        static void Write(const bool &value, Stream &stream) { stream.WriteBool(value); }
        static void Read(bool &value, Stream &stream) { stream.ReadBool(value); }
    };

    HANDLE stream_;

public:
    //
    // Constructors / destructor.
    //
    Stream(wchar_t *path, bool write = false) {
        Open(path, write);
    }

    ~Stream() {
        Close();
    }

    //
    // Public methods.
    //
    bool IsValid() { 
        return stream_ != INVALID_HANDLE_VALUE; 
    }

    bool Open(wchar_t *path, bool write = false) {
        unsigned long openMode = write ? CREATE_ALWAYS : OPEN_EXISTING;

        stream_ = CreateFile(path, GENERIC_READ | GENERIC_WRITE, 
                             FILE_SHARE_READ, NULL, openMode, 
                             FILE_ATTRIBUTE_NORMAL, NULL);
        return stream_ != INVALID_HANDLE_VALUE;
    }

    void Close() { 
        CloseHandle(stream_); 
    }

    void WriteChar(char value)        { WriteBytes(&value, sizeof(char));      }
    void WriteWChar(wchar_t value)    { WriteBytes(&value, sizeof(wchar_t));   }
    void WriteShort(short int value)  { WriteBytes(&value, sizeof(short int)); }
    void WriteInt(int value)          { WriteBytes(&value, sizeof(int));       }
    void WriteFloat(float value)      { WriteBytes(&value, sizeof(float));     }
    void WriteDouble(double value)    { WriteBytes(&value, sizeof(double));    }
    void WriteSizeT(size_t value)     { WriteBytes(&value, sizeof(size_t));    }
    void WriteBool(bool value)        { WriteBytes(&value, sizeof(bool)); }

    template<class T>
    void Write(const T &value) {
        Helper<T>::Write(value, *this);
    }

    void ReadChar(char &value)        { ReadBytes(&value, sizeof(char));      }
    void ReadWChar(wchar_t &value)    { ReadBytes(&value, sizeof(wchar_t));   }
    void ReadShort(short int &value)  { ReadBytes(&value, sizeof(short int)); }
    void ReadInt(int &value)          { ReadBytes(&value, sizeof(int));       }
    void ReadFloat(float &value)      { ReadBytes(&value, sizeof(float));     }
    void ReadDouble(double &value)    { ReadBytes(&value, sizeof(double));    }
    void ReadSizeT(size_t &value)     { ReadBytes(&value, sizeof(size_t));    }
    void ReadBool(bool &value)        { ReadBytes(&value, sizeof(bool));      }

    template<class T>
    void Read(T &value) {
        Helper<T>::Read(value, *this);
    }

protected:
    virtual void WriteBytes(void *data, size_t size) {
        unsigned long written;
        WriteFile(stream_, data, size, &written, NULL);
    }

    virtual void ReadBytes(void *data, size_t size) {
        unsigned long read;
        ReadFile(stream_, data, size, &read, NULL);
    }
};

#endif
