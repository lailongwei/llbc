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


#include "llbc/common/Export.h"

#include "llbc/core/tinyxml2/tinyxml2.h"

#include "llbc/core/log/LogConfigLoader.h"

__LLBC_NS_BEGIN

namespace
{

// Forward decl for mutual recursion.
static void __LLBC_XmlElementToCir(const LLBC_TINYXML2_NS XMLElement *elem,
                                   LLBC_Variant &outCir);

// Two-pass walk of `parent`'s child elements:
//   pass 1: count occurrences per tag name -- the count decides whether
//           the emitted CIR slot is a bare value (unique tag) or a Seq
//           (repeated tag).
//   pass 2: walk in document order, materialize each child's CIR and
//           attach to the slot (Seq slots grow lazily).
static void __LLBC_XmlChildrenToCir(const LLBC_TINYXML2_NS XMLElement *parent,
                                    LLBC_Variant &outDict)
{
    // Pass 1: tag counts.
    std::map<LLBC_String, int> tagCount;
    for (auto child = parent->FirstChildElement();
         child != nullptr;
         child = child->NextSiblingElement())
    {
        const char *name = child->Name();
        if (name == nullptr || *name == '\0')
            continue;
        ++tagCount[name];
    }

    // Pass 2: emit.
    for (auto child = parent->FirstChildElement();
         child != nullptr;
         child = child->NextSiblingElement())
    {
        const char *name = child->Name();
        if (name == nullptr || *name == '\0')
            continue;

        LLBC_Variant childCir;
        __LLBC_XmlElementToCir(child, childCir);

        const LLBC_String tag(name);
        if (tagCount[tag] <= 1)
        {
            outDict[tag] = std::move(childCir);
        }
        else
        {
            auto &slot = outDict[tag];
            if (!slot.Is<LLBC_Variant::Seq>())
                slot.Become<LLBC_Variant::Seq>();
            slot.SeqPushBack(std::move(childCir));
        }
    }
}

// Single-element dispatcher: leaf (no attrs & no children) -> Str scalar;
// compound (has attrs and/or children) -> Dict.
static void __LLBC_XmlElementToCir(const LLBC_TINYXML2_NS XMLElement *elem,
                                   LLBC_Variant &outCir)
{
    const auto *firstAttr = elem->FirstAttribute();
    const auto *firstChild = elem->FirstChildElement();

    // Pure leaf: expose inline text as a Str scalar. Empty text becomes "".
    if (firstAttr == nullptr && firstChild == nullptr)
    {
        const char *text = elem->GetText();
        outCir = (text != nullptr) ? LLBC_String(text) : LLBC_String();
        return;
    }

    // Compound: Dict; attrs written first, children second (child wins on
    // rare key collision -- documented in the header).
    outCir.Become<LLBC_Variant::Dict>();
    for (auto attr = firstAttr; attr != nullptr; attr = attr->Next())
    {
        const char *attrName = attr->Name();
        if (attrName == nullptr || *attrName == '\0')
            continue;
        const char *attrValue = attr->Value();
        outCir[attrName] = (attrValue != nullptr) ? LLBC_String(attrValue)
                                                  : LLBC_String();
    }
    if (firstChild != nullptr)
        __LLBC_XmlChildrenToCir(elem, outCir);
}

}

int LLBC_LogConfigLoader::LoadXmlFromFile(const LLBC_String &filePath,
                                          LLBC_Variant &cir)
{
    cir.Become<void>();

    LLBC_TINYXML2_NS XMLDocument xmlDoc;
    const auto xmlLoadRet = xmlDoc.LoadFile(filePath.c_str());
    if (xmlLoadRet != LLBC_TINYXML2_NS XML_SUCCESS)
    {
        LLBC_String detail;
        detail.format("load log config file failed(xml format), "
                      "file:%s, errno(tinyxml2:%d), error str:%s",
                      filePath.c_str(),
                      xmlLoadRet,
                      xmlDoc.ErrorStr() != nullptr ? xmlDoc.ErrorStr() : "");
        LLBC_SetLastError(LLBC_ERROR_FORMAT, detail.c_str());
        return LLBC_FAILED;
    }

    // Walk top-level elements of the document. In practice log configs have
    // a single <Log> root, but the loader stays structural: every top-level
    // element is exposed under `cir[<tag>]`.
    cir.Become<LLBC_Variant::Dict>();
    for (auto elem = xmlDoc.FirstChildElement();
         elem != nullptr;
         elem = elem->NextSiblingElement())
    {
        const char *name = elem->Name();
        if (name == nullptr || *name == '\0')
            continue;
        __LLBC_XmlElementToCir(elem, cir[LLBC_String(name)]);
    }

    return LLBC_OK;
}

__LLBC_NS_END
