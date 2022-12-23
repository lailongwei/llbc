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

#include "llbc/common/Export.h"

#include "llbc/core/variant/Variant.h"
#include "llbc/core/config/Ini.h"
#include "llbc/core/tinyxml2/tinyxml2.h"
#include "llbc/core/utils/Util_Variant.h"

__LLBC_NS_BEGIN

const LLBC_String LLBC_XMLKeys::Name("__name__");
const LLBC_String LLBC_XMLKeys::Value("__value__");
const LLBC_String LLBC_XMLKeys::Children("__children__");
const LLBC_String LLBC_XMLKeys::Attrs("__attrs__");

void LLBC_VariantUtil::Ini2Variant(const LLBC_Ini &ini, LLBC_Variant &var)
{
    if (var.IsDict())
        var.Clear();
    else
        var.BecomeDict();

    auto &sections = ini.GetAllSections();
    for (auto &section : sections)
    {
        auto &sectionVar = var[section.first];
        sectionVar.AsDict();

        auto &sectionVals = section.second->GetAllValues();
        for (auto &sectionVal : sectionVals)
            sectionVar[sectionVal.first] = sectionVal.second;
    }
}

void LLBC_VariantUtil::Xml2Variant(const ::llbc::tinyxml2::XMLDocument &doc, LLBC_Variant &var)
{
    if (var.IsDict())
        var.Clear();
    else
        var.BecomeDict();

    auto &childrenVar = var[LLBC_XMLKeys::Children];
    childrenVar.BecomeSeq();

    auto child = doc.FirstChildElement();
    for (; child; child = child->NextSiblingElement())
    {
        LLBC_Variant childVar;
        Xml2Variant(*child, childVar);

        childrenVar.SeqPushBack(childVar);
        if (var.DictFind(childVar[LLBC_XMLKeys::Name]) == var.DictEnd())
            var.DictInsert(childVar[LLBC_XMLKeys::Name], childVar);
    }
}

void LLBC_VariantUtil::Xml2Variant(const ::llbc::tinyxml2::XMLElement &elem, LLBC_Variant &var)
{
    if (var.IsDict())
        var.Clear();
    else
        var.BecomeDict();

    // Elemment name/value.
    var[LLBC_XMLKeys::Name] = elem.Name();
    var[LLBC_XMLKeys::Value] = elem.GetText();

    // Element attrs.
    auto &attrsVar = var[LLBC_XMLKeys::Attrs];
    attrsVar.BecomeDict();

    auto attr = elem.FirstAttribute();
    for (; attr; attr = attr->Next())
        attrsVar[attr->Name()] = attr->Value();

    // Children elements.
    auto &childrenVar = var[LLBC_XMLKeys::Children];
    childrenVar.BecomeSeq();

    auto child = elem.FirstChildElement();
    for (; child; child = child->NextSiblingElement())
    {
        LLBC_Variant childVar;
        Xml2Variant(*child, childVar);
        childrenVar.SeqPushBack(childVar);
        if (var.DictFind(childVar[LLBC_XMLKeys::Name]) == var.DictEnd())
            var.DictInsert(childVar[LLBC_XMLKeys::Name], childVar);
    }
}

__LLBC_NS_END