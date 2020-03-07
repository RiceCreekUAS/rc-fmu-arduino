/*
  Copyright (c) 2016 - 2019 Regents of the University of Minnesota and Bolder Flight Systems Inc.
  MIT License; See LICENSE.md for complete details
  Author: Curtis Olson
*/

#pragma once

#include <Arduino.h>

#include <stdint.h>
#include <vector>
#include <memory>

using std::vector;
using std::shared_ptr;
using std::make_shared;

class Element;
typedef shared_ptr<Element> ElementPtr;

class Element {

private:

    // supported types
    enum { NONE, BOOL, INT, LONG, LONGLONG, FLOAT, DOUBLE } tag;

    union {
        bool b;
        int i;
        long l;
        long long ll;
        float f;
        double d;
    } x = {0};

public:

    String name;
    String description;

    Element() {}
    ~Element() {}

    void copyFrom( ElementPtr src ) {
        this->x = src->x;
        this->tag = src->tag;
    }

    void setBool( bool val ) { x.b = val; tag = BOOL; }
    void setInt( int val ) { x.i = val; tag = INT; }
    void setLong( long val ) { x.l = val; tag = LONG; }
    void setLongLong( long long val ) { x.ll = val; tag = LONGLONG; }
    void setFloat( float val ) { x.f = val; tag = FLOAT; }
    void setDouble( double val ) { x.d = val; tag = DOUBLE; }

    bool getBool() {
        switch(tag) {
        case BOOL: return x.b;
        case INT: return x.i;
        case LONG: return x.l;
        case LONGLONG: return x.ll;
        case FLOAT: return x.f;
        case DOUBLE: return x.d;
        default: return false;
        }
    }
    int getInt() {
        switch(tag) {
        case BOOL: return x.b;
        case INT: return x.i;
        case LONG: return x.l;
        case LONGLONG: return x.ll;
        case FLOAT: return x.f;
        case DOUBLE: return x.d;
        default: return 0;
        }
    }
    long getLong() {
        switch(tag) {
        case BOOL: return x.b;
        case INT: return x.i;
        case LONG: return x.l;
        case LONGLONG: return x.ll;
        case FLOAT: return x.f;
        case DOUBLE: return x.d;
        default: return 0;
        }
    }
    long long getLongLong() {
        switch(tag) {
        case BOOL: return x.b;
        case INT: return x.i;
        case LONG: return x.l;
        case LONGLONG: return x.ll;
        case FLOAT: return x.f;
        case DOUBLE: return x.d;
        default: return 0;
        }
    }
    float getFloat() {
        switch(tag) {
        case BOOL: return x.b;
        case INT: return x.i;
        case LONG: return x.l;
        case LONGLONG: return x.ll;
        case FLOAT: return x.f;
        case DOUBLE: return x.d;
        default: return 0.0;
        }
    }
    double getDouble() {
        switch(tag) {
        case BOOL: return x.b;
        case INT: return x.i;
        case LONG: return x.l;
        case LONGLONG: return x.ll;
        case FLOAT: return x.f;
        case DOUBLE: return x.d;
        default: return 0.0;
        }
    }

    String getType() {
        switch(tag) {
        case BOOL: return "bool";
        case INT: return "int";
        case LONG: return "long";
        case LONGLONG: return "longlong";
        case FLOAT: return "float";
        case DOUBLE: return "double";
        default: return "no type";
        }
    }

    // modified to use Arduino String class
    String getValueAsString() {
        String result = "";
        switch(tag) {
        case BOOL: result = x.b ? "true" : "false";
        case INT: result = String(x.i);
        case LONG: result = String((long unsigned int)x.l);
        case LONGLONG: result = String((long unsigned int)x.ll);
        case FLOAT: result = String(x.f);
        case DOUBLE: result = String(x.d);
        default: result = "no value";
        }
        return result;
    }

};


class DefinitionTree2 {

public:

    DefinitionTree2() {}
    ~DefinitionTree2() {}

    ElementPtr initElement(String name, String desc="");
    ElementPtr getElement(String name, bool create=true);

    int find(String Name);
    void GetKeys(String Name, vector<String> *KeysPtr);
    size_t Size(String Name);
    void PrettyPrint(String Prefix);

private:

    vector<ElementPtr> data;

};

// reference a global instance of the deftree
extern DefinitionTree2 deftree;