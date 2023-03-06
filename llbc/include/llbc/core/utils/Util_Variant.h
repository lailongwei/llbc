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

#pragma once

#include "llbc/common/Common.h"

/**
 * Classes pre-declare.
 */
__LLBC_NS_BEGIN

class LLBC_Ini;
class LLBC_Variant;
class LLBC_Property;

namespace tinyxml2
{

class XMLElement;
class XMLDocument;

}
__LLBC_NS_END

__LLBC_NS_BEGIN

/**
 * \brief The XML keys (in variant) define.
 */
class LLBC_EXPORT LLBC_XMLKeys
{
public:
    // XML element/attribute name.
    static const LLBC_String Name;
    // XML element/attribute value.
    static const LLBC_String Value;

    // XML elements(sequence).
    static const LLBC_String Children;

    // XML attributes set.
    static const LLBC_String Attrs;
};

/**
 * \brief The variant helper encapsulation.
 */
class LLBC_EXPORT LLBC_VariantUtil
{
public:
    /**
     * Convert ini to variant.
     * @param[in] ini  - the ini object.
     * @param[out] var  - the variant object.
     */
    static void Ini2Variant(const LLBC_Ini &ini, LLBC_Variant &var);

    /**
     * Convert xml document to variant.
     * @param[in] doc  - the xml document.
     * @param[out] var - the variant object.
     */
    static void Xml2Variant(const ::llbc::tinyxml2::XMLDocument &doc, LLBC_Variant &var);
    /**
     * Convert xml element to variant.
     * @param[in] elem - the xml element.
     * @param[out] var - the variant object.
     */
    static void Xml2Variant(const ::llbc::tinyxml2::XMLElement &elem, LLBC_Variant &var);
};

__LLBC_NS_END

