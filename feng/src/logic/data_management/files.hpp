#pragma once

#include <string>
#include <fstream>
#include "../../logging/logging.h"


struct dbuf {
    uint32_t size;
    void* data;
};

namespace feng::data {

    class wfile {
    public:
        wfile(std::string filepath)
            : _out(filepath, std::ios::binary), _filepath(filepath) {
            chech_error();
        }

        ~wfile() {
            close();
        }

        void close() {
            _out.close();
        }

        void write(const char* data, size_t size) {
            _out.write(data, size);
            chech_error();
        }

        template<typename T>
        typename std::enable_if_t<std::is_fundamental_v<T>, void>
            write(T* data) {
            _out.write(data, sizeof(T));
            chech_error();
        }

        void chech_error() {
            if (!_out)
                LOG_ERROR("Error to open '" + _filepath + "' file in write mode.");
        }

    private:
        std::ofstream _out;
        std::string _filepath;
    };

    class rfile {
    public:
        rfile(std::string filepath)
            : _in(filepath, std::ios::binary), _filepath(filepath) {
            chech_error();
        }

        ~rfile() {
            close();
        }

        dbuf* read_all() {
            dbuf* s = new dbuf;
            _in.seekg(0, std::ios::end);
            std::streampos file_size = _in.tellg();
            _in.seekg(0, std::ios::beg);

            if (file_size == 0) {
                LOG_ERROR("File '" + _filepath + "' is empty.");
                delete s;
                return nullptr;
            }

            s->size = static_cast<uint64_t>(file_size);
            s->data = new char[s->size];
            if (!_in.read(s->data, s->size)) {
                LOG_ERROR("Error to read file '" + _filepath + "' data.");
                delete[] s->data;
                delete s;
                return nullptr;
            }

            return s;
        }

        void close() {
            _in.close();
        }

        void chech_error() {
            if (!_in)
                LOG_ERROR("Error to open '" + _filepath + "' file in read mode.");
        }

    private:
        std::ifstream _in;
        std::string _filepath;
    };

}